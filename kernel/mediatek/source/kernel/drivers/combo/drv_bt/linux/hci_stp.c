/*
 *
 *  Bluetooth HCI UART driver
 *
 *  Copyright (C) 2000-2001  Qualcomm Incorporated
 *  Copyright (C) 2002-2003  Maxim Krasnyansky <maxk@qualcomm.com>
 *  Copyright (C) 2004-2005  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <linux/module.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ptrace.h>
#include <linux/poll.h>

#include <linux/slab.h>
#include <linux/tty.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/signal.h>
#include <linux/ioctl.h>
#include <linux/skbuff.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

/* debugging */
#include <linux/time.h>
#include <linux/delay.h>

/* constant of kernel version */
#include <linux/version.h>


#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>

#include "hci_stp.h"
#include "stp_exp.h"

#include "stp_drv.h"

#include "wmt_exp.h"

#ifndef CONFIG_BT_HCIUART_DEBUG
#undef  BT_DBG
#define BT_DBG 
#endif

#define LOG(fmt, arg...) printk("%s:%d " fmt " \n", __FUNCTION__, __LINE__, ## arg)

/* 
	Debugging Purpose 
*/

#define PFX                         "[BT] "
#define BT_LOG_DBG                  3
#define BT_LOG_INFO                 2
#define BT_LOG_WARN                 1
#define BT_LOG_ERR                  0

unsigned int gDbgLevel = BT_LOG_INFO;

#define BT_DBG_FUNC(fmt, arg...)    if(gDbgLevel >= BT_LOG_DBG){ printk(PFX "%s: "  fmt, __FUNCTION__ ,##arg);}
#define BT_INFO_FUNC(fmt, arg...)   if(gDbgLevel >= BT_LOG_INFO){ printk(PFX "%s: "  fmt, __FUNCTION__ ,##arg);}
#define BT_WARN_FUNC(fmt, arg...)   if(gDbgLevel >= BT_LOG_WARN){ printk(PFX "%s: "  fmt, __FUNCTION__ ,##arg);}
#define BT_ERR_FUNC(fmt, arg...)    if(gDbgLevel >= BT_LOG_ERR){ printk(PFX "%s: "   fmt, __FUNCTION__ ,##arg);}
#define BT_TRC_FUNC(f)              if(gDbgLevel >= BT_LOG_DBG){printk(PFX "<%s> <%d>\n", __FUNCTION__, __LINE__);}

#define VERSION "1.0"
#define BT_NVRAM_CUSTOM_NAME "/data/BT_Addr"

/* H4 receiver States */
#define H4_W4_PACKET_TYPE	0
#define H4_W4_EVENT_HDR		1
#define H4_W4_ACL_HDR		2
#define H4_W4_SCO_HDR		3
#define H4_W4_DATA		4

static int reset = 0;


extern INT32 mtk_wcn_stp_receive_data(UINT8 *buffer, UINT32 length, UINT8 type);
extern INT32 mtk_wcn_stp_send_data(UINT8 *buffer, UINT32 length, UINT8 type);
extern int mtk_wcn_stp_register_tx_event_cb(int type, MTK_WCN_STP_EVENT_CB func);
int hci_stp_tx_wakeup(struct hci_stp *hu);
static void hci_tx_tasklet(unsigned long data);
static DEFINE_RWLOCK(hci_stptx_task_lock);

struct tasklet_struct hci_tx_task_queue;
unsigned char g_nvram_btdata[8];


/* Allow one BT driver */
static struct hci_dev *hdev = NULL;

static int nvram_read(char *filename, char *buf, ssize_t len, int offset)
{	
    struct file *fd;
    //ssize_t ret;
    int retLen = -1;
    
    mm_segment_t old_fs = get_fs();
    set_fs(KERNEL_DS);
    
    fd = filp_open(filename, O_WRONLY|O_CREAT, 0644);
    
    if(IS_ERR(fd)) {
        BT_ERR_FUNC("failed to open!!\n");
        return -1;
    }
    do{
        if ((fd->f_op == NULL) || (fd->f_op->read == NULL))
        {
            BT_ERR_FUNC("file can not be read!!\n");
            break;
        } 
    		
        if (fd->f_pos != offset) {
            if (fd->f_op->llseek) {
                if(fd->f_op->llseek(fd, offset, 0) != offset) {
                    BT_ERR_FUNC("[nvram_read] : failed to seek!!\n");
                    break;
                }
            } else {
                fd->f_pos = offset;
            }
        }    		
        
        retLen = fd->f_op->read(fd,
                                buf,
                                len,
                                &fd->f_pos);			
    		
    }while(false);
    
    filp_close(fd, NULL);
    
    set_fs(old_fs);
    
    return retLen;
}


int platform_load_nvram_data( char * filename, char * buf, int len)
{
    //int ret;
    BT_INFO_FUNC("platform_load_nvram_data ++ BDADDR\n");

    return nvram_read( filename, buf, len, 0);
}

/* Invoked when there is ONE received BT packet */
void stp_tx_event_cb(void)
{
    struct hci_stp *hu;
    
    BUG_ON(hdev == NULL);

    if (!hdev) {
        return;
    }

    hu = (struct hci_stp *) hdev->driver_data;

    hci_stp_tx_wakeup(hu);
}

#if 1

/*
    Direct delivery of bluez not changed hands through the stp buffer
*/
void stp_rx_event_cb_directly(unsigned char *data, int count)
{
    register char *ptr;
    struct hci_event_hdr *eh;
    struct hci_acl_hdr   *ah;
    struct hci_sco_hdr   *sh;
    register int len, type, dlen;
    int while_count = 0;
    static unsigned long rx_count;
    static unsigned long rx_state;
    struct  sk_buff *rx_skb= NULL;
    register int    room;

    BT_DBG_FUNC("count %d rx_state %ld rx_count %ld\n", count, rx_state, rx_count);

    if(data == NULL){
        BT_ERR_FUNC("Data is Null\n");
        return;
    }
    
    ptr = data;

    /*Add statistic*/
    hdev->stat.byte_rx += count;

    while (count) {
        while_count ++;

        if(while_count > 5000){
            BT_ERR_FUNC("h4_recv while_count %d\n", while_count);
        }

        if (rx_count) {
            len = min_t(unsigned int, rx_count, count);
            memcpy(skb_put(rx_skb, len), ptr, len);
            rx_count -= len; count -= len; ptr += len;

            /*rx_count =0 , ready to indicate to hci_core*/
            if (rx_count)
                continue;

            switch (rx_state) {
               case H4_W4_DATA:
                    BT_DBG_FUNC("Complete data\n");
                    hci_recv_frame(rx_skb);
                    rx_state = H4_W4_PACKET_TYPE;
                    rx_skb = NULL;
                    continue;
                            
               case H4_W4_EVENT_HDR:
                    eh = hci_event_hdr(rx_skb);
                    
                    BT_DBG_FUNC("Event header: evt 0x%2.2x plen %d\n", eh->evt, eh->plen);
                    
                    room = skb_tailroom(rx_skb);
                    BT_DBG_FUNC("len %d room %d\n", eh->plen, room);
                    if (!eh->plen) {
                        hci_recv_frame(rx_skb);
                        rx_state = H4_W4_PACKET_TYPE;
                        rx_skb   = NULL;
                        rx_count = 0;
                    } else if (eh->plen > room) {
                        BT_ERR("Data length is too large\n");
                        kfree_skb(rx_skb);
                        rx_state = H4_W4_PACKET_TYPE;
                        rx_skb   = NULL;
                        rx_count = 0;    
                    } else {
                        rx_state = H4_W4_DATA;
                        rx_count = eh->plen;
                    }
                    continue;
                    
               case H4_W4_ACL_HDR:
                    ah = hci_acl_hdr(rx_skb);
                    dlen = __le16_to_cpu(ah->dlen);
                    
                    room = skb_tailroom(rx_skb);
                    BT_DBG_FUNC("ACL header: dlen %d\n", dlen);
                    if (!dlen) {
                        hci_recv_frame(rx_skb);
                        rx_state = H4_W4_PACKET_TYPE;
                        rx_skb   = NULL;
                        rx_count = 0;
                    } else if (dlen > room) {
                        BT_ERR_FUNC("Data length is too large\n");
                        kfree_skb(rx_skb);
                        rx_state = H4_W4_PACKET_TYPE;
                        rx_skb   = NULL;
                        rx_count = 0;
                    } else {
                        rx_state = H4_W4_DATA;
                        rx_count = dlen;
                    }
                    continue;
                    
               case H4_W4_SCO_HDR:
                    sh = hci_sco_hdr(rx_skb);
                    
                    room = skb_tailroom(rx_skb);
                    BT_DBG_FUNC("SCO header: dlen %d\n", sh->dlen);
                    if (!sh->dlen) {
                        hci_recv_frame(rx_skb);
                        rx_state = H4_W4_PACKET_TYPE;
                        rx_skb   = NULL;
                        rx_count = 0;
                    } else if (sh->dlen > room) {
                        BT_ERR_FUNC("Data length is too large\n");
                        kfree_skb(rx_skb);
                        rx_state = H4_W4_PACKET_TYPE;
                        rx_skb   = NULL;
                        rx_count = 0;
                    } else {
                        rx_state = H4_W4_DATA;
                        rx_count = sh->dlen;
                    }
                    continue;
            }
        }

        /* H4_W4_PACKET_TYPE */
        switch (*ptr) {
        	 case HCI_EVENT_PKT:
        	 	    BT_DBG_FUNC("Event packet\n");
        	 	    rx_state = H4_W4_EVENT_HDR;
        	 	    rx_count = HCI_EVENT_HDR_SIZE;
        	 	    type = HCI_EVENT_PKT;
        	 	    break;
        	 	    
        	 case HCI_ACLDATA_PKT:
        	 	    BT_DBG_FUNC("ACL packet\n");
        	 	    rx_state = H4_W4_ACL_HDR;
        	 	    rx_count = HCI_ACL_HDR_SIZE;
        	 	    type = HCI_ACLDATA_PKT;
        	 	    break;
        	 	    
        	 case HCI_SCODATA_PKT:
        	 	    BT_DBG_FUNC("SCO packet\n");
        	 	    rx_state = H4_W4_SCO_HDR;
        	 	    rx_count = HCI_SCO_HDR_SIZE;
        	 	    type = HCI_SCODATA_PKT;
        	 	    break;
        	 	    
        	 default:
        	 	    BT_ERR_FUNC("Unknown HCI packet type %2.2x\n", (__u8)*ptr);
        	 	    hdev->stat.err_rx++;
        	 	    ptr++; count--;
        	 	    continue;
        };
        
        ptr++; count--;

        /* Allocate packet */
        rx_skb = bt_skb_alloc(HCI_MAX_FRAME_SIZE, GFP_ATOMIC);
        if (!rx_skb) {
        	  BT_ERR_FUNC("Can't allocate mem for new packet\n");
        	  rx_state = H4_W4_PACKET_TYPE;
        	  rx_count = 0;
        	  return;
        }
        
        rx_skb->dev = (void *) hdev;
        bt_cb(rx_skb)->pkt_type = type;
    }

    return;
}
#else 
/*
    Direct delivery of bluez not changed hands through the stp buffer
*/
void stp_rx_event_cb_directly(unsigned char *stp_buf, int stp_buf_len)
{
    char 	  *ptr = NULL;
	  int 	  rx_state, rx_count = 0;
	  struct  hci_event_hdr *eh;
	  struct  hci_acl_hdr   *ah;
	  struct  hci_sco_hdr   *sh;
	  struct  sk_buff *	  rx_skb= NULL;
	  int     type, dlen;
	  char    pkyType = 0;
	  int 	  nav_index = 0;

    /*Add statistic*/
    hdev->stat.byte_rx += stp_buf_len;

    /* 
		  Allocate packet 
		*/
		rx_skb = bt_skb_alloc(HCI_MAX_FRAME_SIZE, GFP_ATOMIC);
		if (!rx_skb) {
			  BT_ERR("Can't allocate mem for new packet");
			  rx_state = H4_W4_PACKET_TYPE;
			  rx_count = 0;
			  return;
		}
		
		rx_skb->dev = (void *)hdev;
		ptr = &pkyType;

    /* 
    	read HCI packet type 
    */
    if((stp_buf_len - nav_index) < 1){
        BT_ERR("Invalid length %d", stp_buf_len);
        kfree_skb(rx_skb);
        rx_skb = NULL;
        return;
    }
    else
    {
    	  *ptr = stp_buf[nav_index];
    	  nav_index++;
    }


    /* 
		  read HCI packet header 
		*/
		switch (*ptr) {
			 case HCI_EVENT_PKT:
			 	    BT_DBG_FUNC("Event packet");
			 	    rx_state = H4_W4_EVENT_HDR;
			      rx_count = HCI_EVENT_HDR_SIZE;
			      type = HCI_EVENT_PKT;
			      break;
			      
			 case HCI_ACLDATA_PKT:
			 	    BT_DBG_FUNC("ACL packet");
			 	    rx_state = H4_W4_ACL_HDR;
			 	    rx_count = HCI_ACL_HDR_SIZE;
			 	    type = HCI_ACLDATA_PKT;
			 	    break;
			 	    
			 case HCI_SCODATA_PKT:
			 	    BT_DBG_FUNC("SCO packet");
			 	    rx_state = H4_W4_SCO_HDR;
			 	    rx_count = HCI_SCO_HDR_SIZE;
			 	    type = HCI_SCODATA_PKT;
			 	    break;
			 	    
			 default:
			 	    BT_ERR("Unknown HCI packet type %2.2x", (__u8)*ptr);
			 	    return;
		};

    bt_cb(rx_skb)->pkt_type = type;

    /*
		  prepare header's socket buffer
	  */
    ptr = skb_put(rx_skb, rx_count);
    
    /*
		  put stp buffer to socket buffer
		*/
		if((stp_buf_len - nav_index)< rx_count)
		{
			  BT_ERR("Invalid length %d", rx_count);
        LOG("Error type %d", type);
    }
    else
    {
    	  memcpy(ptr, &stp_buf[nav_index], rx_count);
    	  nav_index += rx_count;
    }

    /*
		  read HCI data length
		*/
    switch(rx_state){
       case H4_W4_EVENT_HDR:
            rx_state = H4_W4_DATA;
            eh = hci_event_hdr(rx_skb);
            rx_count = eh->plen;
            break;
            
       case H4_W4_ACL_HDR:
            rx_state = H4_W4_DATA;
            ah = hci_acl_hdr(rx_skb);
            dlen = __le16_to_cpu(ah->dlen);
            rx_count = dlen;
            break;
            
        case H4_W4_SCO_HDR:
            rx_state = H4_W4_DATA;
            sh = hci_sco_hdr(rx_skb);
            rx_count = sh->dlen;
            break;
    }

    /*
		  prepare header's socket buffer
		*/
    ptr = skb_put(rx_skb, rx_count);

	  /*
		  put stp buffer to socket buffer
	  */
	  if((stp_buf_len - nav_index) < rx_count)
	  {
	  	  BT_ERR_FUNC("Invalid length %d", rx_count);
	  	  LOG("Error type %d", type);
	  }
	  else
	  {
	  	  memcpy(ptr, &stp_buf[nav_index], rx_count);
	  	  nav_index += rx_count;
	  }

    /*
		  Indicate to HCI_CORE
		*/
		hci_recv_frame(rx_skb);
}
#endif


/* 
	Invoked when there is ONE received BT packet 
*/
void stp_rx_event_cb(void)
{
    char    *ptr = NULL;
    int     rx_state, rx_count = 0;
    struct  hci_event_hdr *eh;
    struct  hci_acl_hdr   *ah;
    struct  hci_sco_hdr   *sh;
    struct  sk_buff *	rx_skb	= NULL;
    int     len = 0, type, dlen;
    int     retval;
    char    pkyType = 0;

    /* Allocate packet */
    rx_skb = bt_skb_alloc(HCI_MAX_FRAME_SIZE, GFP_ATOMIC);
    if (!rx_skb) {
    	  BT_ERR_FUNC("Can't allocate mem for new packet\n");
    	  rx_state = H4_W4_PACKET_TYPE;
    	  rx_count = 0;
    	  return;
    }
    
    rx_skb->dev = (void *)hdev;

    ptr = &pkyType;

    /* read HCI packet type */
    if(mtk_wcn_stp_receive_data(ptr, 1, BT_TASK_INDX) != 1){
        BT_ERR_FUNC("Invalid length %d\n", rx_count);
        /* ASSERT? */
        kfree_skb(rx_skb);
        rx_skb = NULL;
        return;
    }

    /* read HCI packet header */
    switch (*ptr) {
    	 case HCI_EVENT_PKT:
    	 	    BT_DBG_FUNC("Event packet");
    	 	    rx_state = H4_W4_EVENT_HDR;
    	 	    rx_count = HCI_EVENT_HDR_SIZE;
    	 	    type = HCI_EVENT_PKT;
    	 	    break;
    	 	    
    	 case HCI_ACLDATA_PKT:
    	 	    BT_DBG_FUNC("ACL packet");
    	 	    rx_state = H4_W4_ACL_HDR;
    	 	    rx_count = HCI_ACL_HDR_SIZE;
    	 	    type = HCI_ACLDATA_PKT;
    	 	    break;
    	 	    
    	 case HCI_SCODATA_PKT:
    	 	    BT_DBG_FUNC("SCO packet");
    	 	    rx_state = H4_W4_SCO_HDR;
    	 	    rx_count = HCI_SCO_HDR_SIZE;
    	 	    type = HCI_SCODATA_PKT;
    	 	    break;
    	 	    
    	 default:
    	 	    BT_ERR_FUNC("Unknown HCI packet type %2.2x", (__u8)*ptr);
//			      hu->hdev->stat.err_rx++;
//			      ptr++; count--;
            return;
		};

//    LOG("BT type %d %d", type, rx_count);

//    rx_skb->dev = (void *) hu->hdev;
    bt_cb(rx_skb)->pkt_type = type;

//    if(mtk_wcn_stp_receive_data(ptr, rx_count, BT_TASK_INDX) != rx_count){
    /* extend data area */
    ptr = skb_put(rx_skb, rx_count);
    if(mtk_wcn_stp_receive_data(ptr, rx_count, BT_TASK_INDX) != rx_count){
        BT_ERR_FUNC("Invalid length %d\n", rx_count);
        BT_ERR_FUNC("Error type %d\n", type);
        /* ASSERT? */
    }

    len += rx_count;
//    ptr += rx_count;
    switch(rx_state){
       case H4_W4_EVENT_HDR:
            rx_state = H4_W4_DATA;
            eh = hci_event_hdr(rx_skb);
            rx_count = eh->plen;
            break;
            
       case H4_W4_ACL_HDR:
            rx_state = H4_W4_DATA;
            ah = hci_acl_hdr(rx_skb);
            dlen = __le16_to_cpu(ah->dlen);
            rx_count = dlen;
            break;
            
       case H4_W4_SCO_HDR:
            rx_state = H4_W4_DATA;
            sh = hci_sco_hdr(rx_skb);
            rx_count = sh->dlen;
            break;
    }

    ptr = skb_put(rx_skb, rx_count);
    retval = mtk_wcn_stp_receive_data(ptr, rx_count, BT_TASK_INDX);
    if(retval != rx_count){
        BT_ERR_FUNC("Invalid length %d %s %d\n", rx_count, __FUNCTION__, __LINE__);
        /* ASSERT? */
    }

    len += rx_count;

    hci_recv_frame(rx_skb);

    return;
}


static int h4_enqueue(struct hci_stp *hu, struct sk_buff *skb)
{
    BT_DBG_FUNC("hu %p skb %p\n", hu, skb);
    
    /* Prepend skb with frame type */
    memcpy(skb_push(skb, 1), &bt_cb(skb)->pkt_type, 1);
    skb_queue_tail(&hu->txq, skb);
    return 0;
}

static inline void hci_stp_tx_complete(struct hci_stp *hu, int pkt_type)
{
	  struct hci_dev *hdev = hu->hdev;

    /* Update HCI stat counters */
    switch (pkt_type) {
    	 case HCI_COMMAND_PKT:
    	 	    hdev->stat.cmd_tx++;
    	 	    break;
    	 	    
    	 case HCI_ACLDATA_PKT:
    	 	    hdev->stat.acl_tx++;
    	 	    break;
    	 	    
    	 case HCI_SCODATA_PKT:
    	 	    hdev->stat.cmd_tx++;
    	 	    break;
    }
}

static struct sk_buff *hci_stp_dequeue(struct hci_stp *hu)
{
    struct sk_buff *skb = NULL;

    BT_DBG_FUNC("hu->txq = %p\n", &hu->txq);
    skb = skb_dequeue(&hu->txq);
    BT_DBG_FUNC("skb = %p\n", skb);
    
    return skb;
}

int hci_stp_tx_wakeup(struct hci_stp *hu)
{
    struct hci_dev *hdev = hu->hdev;
    struct sk_buff *skb;
    int j=0;
    
    BT_TRC_FUNC();
    
    if (test_and_set_bit(HCI_UART_SENDING, &hu->tx_state)) {
    	  set_bit(HCI_UART_TX_WAKEUP, &hu->tx_state);
    	  printk("[BT] enqueue and return\n");
    	  return 0;
    }
    
    BT_DBG_FUNC("hci_stp_tx_wakeup %d\n", __LINE__);

restart:
	  clear_bit(HCI_UART_TX_WAKEUP, &hu->tx_state);

	  while ((skb = hci_stp_dequeue(hu))) {
	  	  int len;
	  	  BT_DBG_FUNC("dqueue times = %d\n", ++j);
	  	  
	  	  if((len = mtk_wcn_stp_send_data(skb->data, skb->len, BT_TASK_INDX)) == 0 ){
            /* can not send */
            BT_ERR_FUNC("mtk_wcn_stp_send_data can not send\n");
            BT_ERR_FUNC("Error %s %d\n", __FUNCTION__, __LINE__);

            skb_queue_head(&hu->txq, skb);//Put back to queue head
            goto END;
        }
        hdev->stat.byte_tx += len;
        hci_stp_tx_complete(hu, bt_cb(skb)->pkt_type);
        kfree_skb(skb);
    }

END:
    if (test_bit(HCI_UART_TX_WAKEUP, &hu->tx_state))
    	  goto restart;

    clear_bit(HCI_UART_SENDING, &hu->tx_state);
    
    return 0;
}

/* ------- Interface to HCI layer ------ */
/* Initialize device */
static int hci_stp_open(struct hci_dev *hdev)
{
    /*set PCM*/	
    /*oringinal code path*/
    /*
    system/bluetooth/bluedroid/bluetooth.c
    external/bluetooth/bluez/tools/hciattach.c
    mtk/src/custom/mt6516_evb/kernel/core/src/board.c // set power gpio configuration
    */
    unsigned char bt_bd_addr[10]=
                  {0x01,0x1a,0xfc,0x06,0x00,0x00,0x46,0x66,0x20,0x01};
    unsigned char bt_link_key_type[5]=
                  {0x01,0x1b,0xfc,0x01,0x01};
    unsigned char bt_unit_key[20]=
                  {0x01,0x75,0xfc,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    unsigned char bt_encrypt[7]=
                  {0x01,0x76,0xfc,0x03,0x00,0x02,0x10};
    unsigned char bt_pin_code_type[5]=
                  {0x01,0x0a,0x0c,0x01,0x00};
    unsigned char bt_voice[6]=
                  {0x01,0x26,0x0c,0x02,0x60,0x00};
    unsigned char bt_codec[8]=
                  {0x01,0x72,0xfc,0x04,0x23,0x10,0x00,0x00};
    unsigned char bt_radio[10]=
                  {0x01,0x79,0xfc,0x06,0x06,0x80,0x00,0x06,0x03,0x06};
    unsigned char bt_tx_pw_offset[7]=
                  {0x01,0x93,0xfc,0x03,0xff,0xff,0xff};
    unsigned char bt_sleep[11]=
                  {0x01,0x7a,0xfc,0x07,0x03,0x40,0x1f,0x40,0x1f,0x00,0x04};   
    unsigned char bt_FTR[6]=
                  {0x01,0x7d,0xfc,0x02,0x80,0x0};
    unsigned char bt_OSC[9]=
                  {0x01,0x7b,0xfc,0x05,0x01,0x01,0x14,0x0a,0x05};
    unsigned char bt_LPO[14]=
                  {0x01,0x7c,0xfc,0x0a,0x01,0xfa,0x0a,0x02,0x00,0xa6,0x0e,0x00,0x40,0x00};
    unsigned char bt_legacy_PTA[14]=
                  {0x01,0x74,0xfc,0x0a,0xc9,0x8b,0xbf,0x00,0x00,0x52,0x0e,0x0e,0x07,0x1f};
    unsigned char bt_BLE_PTA[9]=
                  {0x01,0xfc,0xfc,0x05,0x16,0x0e,0x0e,0x00,0x05};
    unsigned char bt_rf_desence[10]=
                  {0x01,0x20,0xfc,0x06,0x01,0x00,0x01,0x00,0x01,0x00};
    unsigned char bt_reset[4]=
                  {0x01,0x03,0x0c,0x0};
    unsigned char bt_intern_PTA_1[19]=
                  {0x01,0xfb,0xfc,0x0f,0x00,0x01,0x0f,0x0f,0x01,0x0f,0x0f,0x01,0x0f,0x0f,0x01,0x0f,0x0f,0x02,0x01};
    unsigned char bt_intern_PTA_2[11]=
                  {0x01,0xfb,0xfc,0x07,0x01,0x19,0x19,0x07,0xd0,0x00,0x01};
    unsigned char bt_sleep_reg[12]=
                  {0x01,0xd0,0xfc,0x08,0x74,0x00,0x01,0x81,0xe2,0x29,0x00,0x00};
    unsigned char bt_SLP_LDOD_reg[12]=
                  {0x01,0xd0,0xfc,0x08,0x1c,0x00,0x02,0x81,0x79,0x08,0x00,0x00};
    unsigned char bt_rf_reg[10]=
                  {0x01,0xb0,0xfc,0x06,0x64,0x01,0x02,0x00,0x00,0x00};
	
    BT_DBG_FUNC("%d", __LINE__);
    BT_DBG_FUNC("%s %p", hdev->name, hdev);

    /* turn on BT */
    if (MTK_WCN_BOOL_FALSE == mtk_wcn_wmt_func_on(WMTDRV_TYPE_BT)) {
        BT_WARN_FUNC("WMT turn on BT fail!\n");
        return -ENODEV;
    }
    BT_INFO_FUNC("WMT turn on BT OK!\n");

    if (mtk_wcn_stp_is_ready()) {
        
        BT_INFO_FUNC("STP is ready!\n");

		    /*In sdio, stanalone to enable BT function without WMT*/
        #if 0
		    if (mtk_wcn_stp_is_sdio_mode()){
			      /*wmt bt on command*/
			      unsigned char bt_on_cmd[12]={0x80,0x40,0x06,0x00,0x01,0x06,0x02,0x00,0x00,0x01,0x00,0x00};
			      
			      /*set BD_ADDR*/
			      unsigned char bt_bd_addr[10]={0x01,0x1a,0xfc,0x06,0x00,0xba,0x0c,0x46,0xa8,0x00};
			      
			      /*set PCM*/
			      unsigned char bt_pcm[5]={0x01,0x72,0xfc,0x01,0x23};
			      
			      /*set eSCO feature*/
			      //unsigned char bt_esco_feature[12]={0x01,0x1e,0xfc,0x08,0xff,0xff,0x8d,0xfe,0x98,0x1f,0x79,0x83};
			       
			      mtk_wcn_stp_set_bluez(0);
			      
			      BT_INFO_FUNC("STP run under sdio mode!\n");
			      
			      BT_INFO_FUNC("BT Func On=>\n");
			      mtk_wcn_stp_send_data_raw(bt_on_cmd, 12, 4);
			      msleep(500);
			      BT_INFO_FUNC("BT Func On<=\n");
			      
			      /*Re-resetting BD_ADDR*/
			      BT_INFO_FUNC("Reset bd_addr =>\n");
			      mtk_wcn_stp_send_data(bt_bd_addr,10,0);
			      msleep(500);
			      BT_INFO_FUNC("Reset bd_addr <=\n");
			      
			      BT_INFO_FUNC("Set PCM =>\n");
			      mtk_wcn_stp_send_data(bt_pcm,5,0);
			      msleep(500);
			      BT_INFO_FUNC("Set PCM <=\n");
			  }
        #endif

        if (!mtk_wcn_stp_is_sdio_mode()){
            /*platform_load_nvram_data(BT_NVRAM_CUSTOM_NAME, (char *)&g_nvram_btdata, sizeof(g_nvram_btdata));
            BT_INFO_FUNC("Read NVRAM : BD address %02x%02x%02x%02x%02x%02x Cap 0x%02x Codec 0x%02x\n",
                g_nvram_btdata[0], g_nvram_btdata[1], g_nvram_btdata[2],         
                g_nvram_btdata[3], g_nvram_btdata[4], g_nvram_btdata[5],         
                g_nvram_btdata[6], g_nvram_btdata[7]);    
        
            memcpy(&g_bt_bd_addr[4], &g_nvram_btdata[0], 6);*/
        
            BT_INFO_FUNC("Reset bd_addr =>\n");
            mtk_wcn_stp_send_data(bt_bd_addr, 10, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bd_addr <=\n");
            
            BT_INFO_FUNC("Reset bt_link_key_type =>\n");
            mtk_wcn_stp_send_data(bt_link_key_type, 5, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_link_key_type <=\n");
            
            BT_INFO_FUNC("Reset bt_unit_key =>\n");
            mtk_wcn_stp_send_data(bt_unit_key, 20, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_unit_key <=\n");
            
            BT_INFO_FUNC("Reset bt_encrypt =>\n");
            mtk_wcn_stp_send_data(bt_encrypt, 7, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_encrypt <=\n");
            
            BT_INFO_FUNC("Reset bt_pin_code_type =>\n");
            mtk_wcn_stp_send_data(bt_pin_code_type, 5, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_pin_code_type <=\n");
            
            BT_INFO_FUNC("Reset bt_voice =>\n");
            mtk_wcn_stp_send_data(bt_voice, 6, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_voice <=\n");
            
            BT_INFO_FUNC("Reset bt_codec =>\n");
            mtk_wcn_stp_send_data(bt_codec, 8, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_codec <=\n");
            
            BT_INFO_FUNC("Reset bt_radio =>\n");
            mtk_wcn_stp_send_data(bt_radio, 10, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_radio <=\n");
            
            BT_INFO_FUNC("Reset bt_tx_pw_offset =>\n");
            mtk_wcn_stp_send_data(bt_tx_pw_offset, 7, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_tx_pw_offset <=\n");
            
            BT_INFO_FUNC("Reset bt_sleep =>\n");
            mtk_wcn_stp_send_data(bt_sleep, 11, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_sleep <=\n");
            
            BT_INFO_FUNC("Reset bt_FTR =>\n");
            mtk_wcn_stp_send_data(bt_FTR, 6, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_FTR <=\n");
            
            BT_INFO_FUNC("Reset bt_OSC =>\n");
            mtk_wcn_stp_send_data(bt_OSC, 9, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_OSC <=\n");
            
            BT_INFO_FUNC("Reset bt_LPO =>\n");
            mtk_wcn_stp_send_data(bt_LPO, 14, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_LPO <=\n");
            
            BT_INFO_FUNC("Reset bt_legacy_PTA =>\n");
            mtk_wcn_stp_send_data(bt_legacy_PTA, 14, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_legacy_PTA <=\n");
            
            BT_INFO_FUNC("Reset bt_BLE_PTA =>\n");
            mtk_wcn_stp_send_data(bt_BLE_PTA, 9, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_BLE_PTA <=\n");
            
            BT_INFO_FUNC("Reset bt_rf_desence =>\n");
            mtk_wcn_stp_send_data(bt_rf_desence, 10, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_rf_desence <=\n");
            
            BT_INFO_FUNC("Reset bt_reset =>\n");
            mtk_wcn_stp_send_data(bt_reset, 4, BT_TASK_INDX);		
            msleep(100);		
            BT_INFO_FUNC("Reset bt_reset <=\n");
            
            BT_INFO_FUNC("Reset bt_intern_PTA_1 =>\n");
            mtk_wcn_stp_send_data(bt_intern_PTA_1, 19, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_intern_PTA_1 <=\n");
            
            BT_INFO_FUNC("Reset bt_intern_PTA_2 =>\n");
            mtk_wcn_stp_send_data(bt_intern_PTA_2, 11, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_intern_PTA_2 <=\n");
            
            BT_INFO_FUNC("Reset bt_sleep_reg =>\n");
            mtk_wcn_stp_send_data(bt_sleep_reg, 12, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_sleep_reg <=\n");
            
            BT_INFO_FUNC("Reset bt_SLP_LDOD_reg =>\n");
            mtk_wcn_stp_send_data(bt_SLP_LDOD_reg, 12, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_SLP_LDOD_reg <=\n");
            
            BT_INFO_FUNC("Reset bt_rf_reg =>\n");
            mtk_wcn_stp_send_data(bt_rf_reg, 10, BT_TASK_INDX);
            msleep(100);
            BT_INFO_FUNC("Reset bt_rf_reg <=\n");
			
        }

        set_bit(HCI_RUNNING, &hdev->flags);
        
        /*registered tx/rx path*/
        mtk_wcn_stp_register_if_rx(stp_rx_event_cb_directly);
        
        mtk_wcn_stp_register_event_cb(BT_TASK_INDX, NULL);
        mtk_wcn_stp_register_tx_event_cb(BT_TASK_INDX, stp_tx_event_cb);

        /*use bluez*/
        mtk_wcn_stp_set_bluez(1);
        
        return 0;
    }
    else {
   	    BT_WARN_FUNC("STP is not ready!\n");
   	    return -ENODEV;
   	}
}

/* Reset device */
static int hci_stp_flush(struct hci_dev *hdev)
{
	  BT_TRC_FUNC();
	  BT_DBG_FUNC("%d", __LINE__);
	  
	  return 0;
}

/* Close device */
static int hci_stp_close(struct hci_dev *hdev)
{
	  BT_DBG_FUNC("hdev %p\n", hdev);
    BT_INFO_FUNC("%s %d", __FUNCTION__, __LINE__);

    if (!test_and_clear_bit(HCI_RUNNING, &hdev->flags))
    	  return 0;

    /*In sdio, stanalone to disable BT function without WMT*/
    #if 0
    if(mtk_wcn_stp_is_sdio_mode()){
    	
    	unsigned char bt_off_cmd[12]={0x80,0x40,0x06,0x00,0x01,0x06,0x02,0x00,0x00,0x00,0x00,0x00};
    	
    	BT_INFO_FUNC("BT Func Off=>\n");
    	mtk_wcn_stp_send_data_raw(bt_off_cmd, 12, 4);
    	msleep(500);
    	BT_INFO_FUNC("BT Func Off<=\n");
    }
    #endif
    
    hci_stp_flush(hdev);
    hdev->flush = NULL;

    /*unregistered tx/rx path*/
    mtk_wcn_stp_register_if_rx(NULL);
    
    mtk_wcn_stp_register_event_cb(BT_TASK_INDX, NULL);
    mtk_wcn_stp_register_tx_event_cb(BT_TASK_INDX, NULL);
	
    /*not use bluez*/
    mtk_wcn_stp_set_bluez(0);

    if (MTK_WCN_BOOL_FALSE == mtk_wcn_wmt_func_off(WMTDRV_TYPE_BT)) {
        BT_WARN_FUNC("WMT turn off BT fail!\n");
    }
    else {
        BT_INFO_FUNC("WMT turn off BT OK!\n");
    }
    
    return 0;
}

/* Send frames from HCI layer */
static int hci_stp_send_frame(struct sk_buff *skb)
{
	  struct hci_dev* hdev = (struct hci_dev *) skb->dev;
	  struct hci_stp *hu;

	  BT_TRC_FUNC();
	  if (!hdev) {
	  	  BT_ERR_FUNC("Frame for uknown device (hdev=NULL)\n");
	  	  return -ENODEV;
	  }

    if (!test_bit(HCI_RUNNING, &hdev->flags))
    	  return -EBUSY;

    hu = (struct hci_stp *) hdev->driver_data;
    
    BT_DBG_FUNC("%s: type %d len %d\n", hdev->name, bt_cb(skb)->pkt_type, skb->len);

    if (gDbgLevel >= BT_LOG_DBG)
    {
        struct timeval now;
    
        do_gettimeofday(&now);
        
        printk("%s:  sec = %d, --> usec --> %d\n", 
             __FUNCTION__, now.tv_sec, now.tv_usec);
    }
    
    h4_enqueue(hu, skb);
    
    /*tasklet_hi_schedule(&hci_tx_task_queue);*/
    tasklet_schedule(&hci_tx_task_queue);
    
    return 0;
}

static void hci_tx_tasklet(unsigned long data){

    struct hci_stp *hu = (struct hci_stp *)data;
        
    read_lock(&hci_stptx_task_lock);
    
    hci_stp_tx_wakeup(hu);
    
    read_unlock(&hci_stptx_task_lock);	
}


static void hci_stp_destruct(struct hci_dev *hdev)
{
    /*BT_DBG_FUNC("%d", __LINE__);*/
    if (!hdev)
    	  return;
    	  
    BT_DBG_FUNC("%s\n", hdev->name);
    kfree(hdev->driver_data);
}

static int __init hci_stp_init(void)
{
    struct hci_stp *hu = NULL;
    struct timeval now;
    
    BT_INFO_FUNC("HCI STP driver ver %s\n", VERSION);
    
    if (!(hu = kzalloc(sizeof(struct hci_stp), /*GFP_KERNEL*/GFP_ATOMIC))) {
    	  BT_ERR("Can't allocate control structure");
        LOG("%s %d", __FUNCTION__, __LINE__);
        return -ENOMEM;
    }
    
    BT_INFO_FUNC("hu 0x%08x\n", (int)hu);
    
    /* 
		  used to stored pending skb
	*/
    skb_queue_head_init(&hu->txq);

    /* 
		  Initialize and register HCI device
		*/
		hdev = hci_alloc_dev();
		if (!hdev) {
        if(hu){
            kfree(hu);
            hu = NULL;
        }
     
        BT_ERR_FUNC("Can't allocate HCI device\n");
        return -ENOMEM;
     }
     
     hu->hdev = hdev;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
     hdev->type = HCI_UART;
#else
     hdev->bus = HCI_UART;
     hdev->dev_type =HCI_BREDR;
#endif
     hdev->driver_data = hu;
     
     BT_INFO_FUNC("hdev->driver_data 0x%08x\n", (int)hdev->driver_data);
     hdev->open  = hci_stp_open;
     hdev->close = hci_stp_close;
     hdev->flush = hci_stp_flush;
     hdev->send  = hci_stp_send_frame;
     hdev->destruct = hci_stp_destruct;
     
     hdev->owner = THIS_MODULE;
     
     BT_INFO_FUNC("HCI_QUIRK_NO_RESET\n");
     
     set_bit(HCI_QUIRK_NO_RESET, &hdev->quirks);

    	if (hci_register_dev(hdev) < 0) {
     	   BT_ERR_FUNC("Can't register HCI device\n");
     	   kfree(hu);
     	   hu = NULL;
     	   hci_free_dev(hdev);
     	   hdev = NULL;
     	   return -ENODEV;
     }
     
     tasklet_init(&hci_tx_task_queue, hci_tx_tasklet, (unsigned long)hu);
     //tasklet_schedule(&hci_tx_task_queue);
     mtk_wcn_stp_register_if_rx(NULL);
     mtk_wcn_stp_register_event_cb(BT_TASK_INDX, NULL);
     mtk_wcn_stp_register_tx_event_cb(BT_TASK_INDX, NULL);
     
     //BT_DBG_FUNC("=======> hu addr = %p\n", hu);
     //BT_DBG_FUNC("%s %d\n", __FUNCTION__, __LINE__);
     return 0;
}

static void __exit hci_stp_exit(void)
{
    struct hci_stp *hu = (struct hci_stp *)hdev->driver_data;
    BT_INFO_FUNC("%s %d\n", __FUNCTION__, __LINE__);
    
    mtk_wcn_stp_register_event_cb(BT_TASK_INDX, NULL);
    mtk_wcn_stp_register_tx_event_cb(BT_TASK_INDX, NULL);
    
    hci_unregister_dev(hdev);
    
    tasklet_kill(&hci_tx_task_queue);

    skb_queue_purge(&hu->txq);

    /* hci_stp_destruct does this */
    /*kfree(hdev->driver_data);*/
    hci_free_dev(hdev);
	
    hdev = NULL;
}

module_init(hci_stp_init);
module_exit(hci_stp_exit);

module_param(reset, bool, 0644);
MODULE_PARM_DESC(reset, "Send HCI reset command on initialization");

MODULE_AUTHOR("Mediatek Inc.");
MODULE_DESCRIPTION("Bluetooth HCI STP driver ver " VERSION);
MODULE_VERSION(VERSION);
MODULE_LICENSE("GPL");
MODULE_ALIAS_LDISC(N_HCI);
