#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/blkdev.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/core.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio.h>
#include <linux/dma-mapping.h>

#include <mach/dma.h>
#include <mach/board.h> /* FIXME */
#include <mach/mt6577_clock_manager.h>
#include <mach/mt6577_reg_base.h>
#include "mach/mt6577_clkmgr_internal.h"
//#include <asm/tcm.h>

#include "mt6577_sd.h"
#include <sd_misc.h>
#include "board-custom.h"
#include "../../../../../../kernel/drivers/mmc/card/queue.h"


#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#ifdef MTK_EMMC_SUPPORT
#include "partition_define.h"

//extern struct excel_info PartInfoEmmc[PART_NUM];
extern int simple_sd_request(struct mmc_host*mmc, struct mmc_request*mrq);
#endif

#define DRV_NAME_MISC            "misc-sd"

#define DEBUG_MMC_IOCTL   0
#define DEBUG_MSDC_SSC    1
/*
 * For simple_sd_ioctl
 */
#define FORCE_IN_DMA (0x11)
#define FORCE_IN_PIO (0x10)
#define FORCE_NOTHING (0x0)
static int    dma_force[4] = //used for sd ioctrol
{
    FORCE_NOTHING,
    FORCE_NOTHING,
    FORCE_NOTHING,
    FORCE_NOTHING
};
#define dma_is_forced(host_id)     (dma_force[host_id] & 0x10)
#define get_forced_transfer_mode(host_id)  (dma_force[host_id] & 0x01)

//#ifdef MTK_EMMC_SUPPORT
#define EMMC_HOST_NUM    (0)
//#endif

static u32* sg_msdc_multi_buffer = NULL;

static int simple_sd_open(struct inode *inode, struct file *file)
{
    return 0;
}

extern int mmc_switch(struct mmc_card *card, u8 set, u8 index, u8 value,
           unsigned int timeout_ms);
extern int mmc_send_ext_csd(struct mmc_card *card, u8 *ext_csd);
extern struct msdc_host *msdc_6577_host[];
#ifdef MTK_SD_REINIT_SUPPORT
extern int msdc_reinit(struct msdc_host *host);

static int sd_ioctl_reinit(struct msdc_ioctl* msdc_ctl)
{
	struct msdc_host *host = msdc_6577_host[SD_HOST_ID];
	return msdc_reinit(host);
}
#endif
static void simple_sd_ioctl_clksrc_env(struct msdc_ioctl*);
static int simple_sd_ioctl_set_hop(struct msdc_ioctl*);
static int simple_sd_ioctl_get_hop(struct msdc_ioctl*);

static int simple_sd_ioctl_single_rw(struct msdc_ioctl* msdc_ctl)
{
    char l_buf[512];
    struct scatterlist msdc_sg;
    struct mmc_data    msdc_data;
    struct mmc_command msdc_cmd;
    struct mmc_request msdc_mrq;
    struct msdc_host *host_ctl;
   
    host_ctl = msdc_6577_host[msdc_ctl->host_num];
    BUG_ON(!host_ctl);
    BUG_ON(!host_ctl->mmc);
    BUG_ON(!host_ctl->mmc->card);

    mmc_claim_host(host_ctl->mmc);

#if DEBUG_MMC_IOCTL
    printk("user want access %d partition\n",msdc_ctl->partition);
#endif

    mmc_send_ext_csd(host_ctl->mmc->card, l_buf);
    switch (msdc_ctl->partition){
        case BOOT_PARTITION_1:
            if (0x1 != (l_buf[179] & 0x7)){
                /* change to access boot partition 1 */
                l_buf[179] &= ~0x7;
                l_buf[179] |= 0x1;
            }
            break;
        case BOOT_PARTITION_2:
            if (0x2 != (l_buf[179] & 0x7)){
                /* change to access boot partition 2 */
                l_buf[179] &= ~0x7;
                l_buf[179] |= 0x1;
            }
            break;
        default:
            /* make sure access partition is user data area */
            if (0 != (l_buf[179] & 0x7)){
                /* set back to access user area */
                l_buf[179] &= ~0x7;
                l_buf[179] |= 0x0;
            }
            break;
    }
    mmc_switch(host_ctl->mmc->card, 0, 179, l_buf[179], 1000);

    if(msdc_ctl->total_size > 512){
        msdc_ctl->result = -1;
        return  msdc_ctl->result;
    }

#if DEBUG_MMC_IOCTL
    printk("start MSDC_SINGLE_READ_WRITE !!\n");
#endif    
    memset(&msdc_data, 0, sizeof(struct mmc_data));
    memset(&msdc_mrq, 0, sizeof(struct mmc_request));
    memset(&msdc_cmd, 0, sizeof(struct mmc_command));

    msdc_mrq.cmd = &msdc_cmd;
    msdc_mrq.data = &msdc_data;

    if(msdc_ctl->trans_type)
        dma_force[host_ctl->id] = FORCE_IN_DMA;
    else
        dma_force[host_ctl->id] = FORCE_IN_PIO;

    if (msdc_ctl->iswrite){
        msdc_data.flags = MMC_DATA_WRITE;
        msdc_cmd.opcode = MMC_WRITE_BLOCK;
        msdc_data.blocks = msdc_ctl->total_size / 512;
        if (MSDC_CARD_DUNM_FUNC != msdc_ctl->opcode) {
            if (copy_from_user(sg_msdc_multi_buffer, msdc_ctl->buffer, 512)){
                dma_force[host_ctl->id] = FORCE_NOTHING;
                return -EFAULT;
            }
        } else {
            /* called from other kernel module */
            memcpy(sg_msdc_multi_buffer, msdc_ctl->buffer, 512);
        }
    } else {
        msdc_data.flags = MMC_DATA_READ;
        msdc_cmd.opcode = MMC_READ_SINGLE_BLOCK;
        msdc_data.blocks = msdc_ctl->total_size / 512;

        memset(sg_msdc_multi_buffer, 0 , 512);
    }

    msdc_cmd.arg = msdc_ctl->address;

    if (!mmc_card_blockaddr(host_ctl->mmc->card)){
        printk("the device is used byte address!\n");
        msdc_cmd.arg <<= 9;
    }

    msdc_cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;

    msdc_data.stop = NULL;
    msdc_data.blksz = 512;
    msdc_data.sg = &msdc_sg;
    msdc_data.sg_len = 1;

#if DEBUG_MMC_IOCTL
    printk("single block: ueser buf address is 0x%p!\n",msdc_ctl->buffer);
#endif    
    sg_init_one(&msdc_sg, sg_msdc_multi_buffer, msdc_ctl->total_size);
    mmc_set_data_timeout(&msdc_data, host_ctl->mmc->card);

    mmc_wait_for_req(host_ctl->mmc, &msdc_mrq);

    if (!msdc_ctl->iswrite){
        if (MSDC_CARD_DUNM_FUNC != msdc_ctl->opcode) {
            if (copy_to_user(msdc_ctl->buffer,sg_msdc_multi_buffer,512)){
                dma_force[host_ctl->id] = FORCE_NOTHING;
                return -EFAULT;
                }
        } else {
            /* called from other kernel module */
            memcpy(msdc_ctl->buffer,sg_msdc_multi_buffer,512);
        }
    }

    if (msdc_ctl->partition){
        mmc_send_ext_csd(host_ctl->mmc->card,l_buf);

        if (l_buf[179] & 0x7) {
            /* set back to access user area */
            l_buf[179] &= ~0x7;
            l_buf[179] |= 0x0;
            mmc_switch(host_ctl->mmc->card, 0, 179, l_buf[179], 1000);
        }
    }

    mmc_release_host(host_ctl->mmc);

    if (msdc_cmd.error)
        msdc_ctl->result= msdc_cmd.error;

    if (msdc_data.error)
        msdc_ctl->result= msdc_data.error;
    else
        msdc_ctl->result= 0;

        dma_force[host_ctl->id] = FORCE_NOTHING;
    return msdc_ctl->result;
}

int simple_sd_ioctl_multi_rw(struct msdc_ioctl* msdc_ctl)
{
    char l_buf[512];
    struct scatterlist msdc_sg;
    struct mmc_data  msdc_data;
    struct mmc_command msdc_cmd;
    struct mmc_command msdc_stop;
    struct mmc_request  msdc_mrq;
    struct msdc_host *host_ctl;
   
    host_ctl = msdc_6577_host[msdc_ctl->host_num];
    BUG_ON(!host_ctl);
    BUG_ON(!host_ctl->mmc);
    BUG_ON(!host_ctl->mmc->card);

    mmc_claim_host(host_ctl->mmc);

#if DEBUG_MMC_IOCTL
    printk("user want access %d partition\n",msdc_ctl->partition);
#endif

    mmc_send_ext_csd(host_ctl->mmc->card, l_buf);
    switch (msdc_ctl->partition){
        case BOOT_PARTITION_1:
            if (0x1 != (l_buf[179] & 0x7)){
                /* change to access boot partition 1 */
                l_buf[179] &= ~0x7;
                l_buf[179] |= 0x1;
            }
            break;
        case BOOT_PARTITION_2:
            if (0x2 != (l_buf[179] & 0x7)){
                /* change to access boot partition 2 */
                l_buf[179] &= ~0x7;
                l_buf[179] |= 0x1;
            }
            break;
        default:
            /* make sure access partition is user data area */
            if (0 != (l_buf[179] & 0x7)){
                /* set back to access user area */
                l_buf[179] &= ~0x7;
                l_buf[179] |= 0x0;
            }
            break;
    }
    mmc_switch(host_ctl->mmc->card, 0, 179, l_buf[179], 1000);

    if(msdc_ctl->total_size > 64*1024){
        msdc_ctl->result = -1;
        return  msdc_ctl->result;
    }

    memset(&msdc_data, 0, sizeof(struct mmc_data));
    memset(&msdc_mrq, 0, sizeof(struct mmc_request));
    memset(&msdc_cmd, 0, sizeof(struct mmc_command));
    memset(&msdc_stop, 0, sizeof(struct mmc_command));

    msdc_mrq.cmd = &msdc_cmd;
    msdc_mrq.data = &msdc_data;

    if(msdc_ctl->trans_type)
        dma_force[host_ctl->id] = FORCE_IN_DMA;
    else
        dma_force[host_ctl->id] = FORCE_IN_PIO;

    if (msdc_ctl->iswrite){
        msdc_data.flags = MMC_DATA_WRITE;
        msdc_cmd.opcode = MMC_WRITE_MULTIPLE_BLOCK;
        msdc_data.blocks = msdc_ctl->total_size / 512;
        if (MSDC_CARD_DUNM_FUNC != msdc_ctl->opcode) {
            if (copy_from_user(sg_msdc_multi_buffer, msdc_ctl->buffer, msdc_ctl->total_size)){
                dma_force[host_ctl->id] = FORCE_NOTHING;
                return -EFAULT; 
            }
        } else {
            /* called from other kernel module */
            memcpy(sg_msdc_multi_buffer, msdc_ctl->buffer, msdc_ctl->total_size);
        }
    } else {
        msdc_data.flags = MMC_DATA_READ;
        msdc_cmd.opcode = MMC_READ_MULTIPLE_BLOCK;
        msdc_data.blocks = msdc_ctl->total_size / 512;
        memset(sg_msdc_multi_buffer, 0 , msdc_ctl->total_size);
    }

    msdc_cmd.arg = msdc_ctl->address;

    if (!mmc_card_blockaddr(host_ctl->mmc->card)){
        printk("this device use byte address!!\n");
        msdc_cmd.arg <<= 9;
    }
    msdc_cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;

    msdc_stop.opcode = MMC_STOP_TRANSMISSION;
    msdc_stop.arg = 0;
    msdc_stop.flags = MMC_RSP_SPI_R1B | MMC_RSP_R1B | MMC_CMD_AC;

    msdc_data.stop = &msdc_stop;
    msdc_data.blksz = 512;
    msdc_data.sg = &msdc_sg;
    msdc_data.sg_len = 1;

#if DEBUG_MMC_IOCTL
    printk("total size is %d\n",msdc_ctl->total_size);
#endif
    sg_init_one(&msdc_sg, sg_msdc_multi_buffer, msdc_ctl->total_size);
    mmc_set_data_timeout(&msdc_data, host_ctl->mmc->card);
    mmc_wait_for_req(host_ctl->mmc, &msdc_mrq);

    if (!msdc_ctl->iswrite){
        if (MSDC_CARD_DUNM_FUNC != msdc_ctl->opcode) {
            if (copy_to_user(msdc_ctl->buffer, sg_msdc_multi_buffer, msdc_ctl->total_size)){
                dma_force[host_ctl->id] = FORCE_NOTHING;
                return -EFAULT;
                }
        } else {
            /* called from other kernel module */
            memcpy(msdc_ctl->buffer, sg_msdc_multi_buffer, msdc_ctl->total_size);
        }
    }

    if (msdc_ctl->partition){
        mmc_send_ext_csd(host_ctl->mmc->card,l_buf);

        if (l_buf[179] & 0x7) {
            /* set back to access user area */
            l_buf[179] &= ~0x7;
            l_buf[179] |= 0x0;
            mmc_switch(host_ctl->mmc->card, 0, 179, l_buf[179], 1000);
        }
    }

    mmc_release_host(host_ctl->mmc);

    if (msdc_cmd.error)
        msdc_ctl->result = msdc_cmd.error;

    if (msdc_data.error){
        msdc_ctl->result = msdc_data.error;
    } else {
        msdc_ctl->result = 0;
    }
        dma_force[host_ctl->id] = FORCE_NOTHING;
    return msdc_ctl->result;

}
EXPORT_SYMBOL(simple_sd_ioctl_multi_rw);
static int simple_sd_ioctl_get_cid(struct msdc_ioctl* msdc_ctl)
{
    struct msdc_host *host_ctl;
   
    host_ctl = msdc_6577_host[msdc_ctl->host_num];

    BUG_ON(!host_ctl);
    BUG_ON(!host_ctl->mmc);
    BUG_ON(!host_ctl->mmc->card);

#if DEBUG_MMC_IOCTL
    printk("user want the cid in msdc slot%d \n",msdc_ctl->host_num);
#endif

    if (copy_to_user(msdc_ctl->buffer, &host_ctl->mmc->card->raw_cid, 16))
        return -EFAULT;

#if DEBUG_MMC_IOCTL
    printk("cid:0x%x,0x%x,0x%x,0x%x\n",host_ctl->mmc->card->raw_cid[0],
            host_ctl->mmc->card->raw_cid[1],
            host_ctl->mmc->card->raw_cid[2],
            host_ctl->mmc->card->raw_cid[3]);
#endif
    return 0;

}

static int simple_sd_ioctl_get_csd(struct msdc_ioctl* msdc_ctl)
{
    struct msdc_host *host_ctl;
   
    host_ctl = msdc_6577_host[msdc_ctl->host_num];

    BUG_ON(!host_ctl);
    BUG_ON(!host_ctl->mmc);
    BUG_ON(!host_ctl->mmc->card);

#if DEBUG_MMC_IOCTL
    printk("user want the csd in msdc slot%d \n",msdc_ctl->host_num);
#endif

    if (copy_to_user(msdc_ctl->buffer, &host_ctl->mmc->card->raw_csd, 16))
        return -EFAULT;

#if DEBUG_MMC_IOCTL
    printk("csd:0x%x,0x%x,0x%x,0x%x\n",host_ctl->mmc->card->raw_csd[0],
            host_ctl->mmc->card->raw_csd[1],
            host_ctl->mmc->card->raw_csd[2],
            host_ctl->mmc->card->raw_csd[3]);
#endif
    return 0;

}

static int simple_sd_ioctl_get_excsd(struct msdc_ioctl* msdc_ctl)
{
    char l_buf[512];
    struct msdc_host *host_ctl;

#if DEBUG_MMC_IOCTL
    int i;
#endif
   
    host_ctl = msdc_6577_host[msdc_ctl->host_num];

    BUG_ON(!host_ctl);
    BUG_ON(!host_ctl->mmc);
    BUG_ON(!host_ctl->mmc->card);

    mmc_claim_host(host_ctl->mmc);

#if DEBUG_MMC_IOCTL
    printk("user want the extend csd in msdc slot%d \n",msdc_ctl->host_num);
#endif
    mmc_send_ext_csd(host_ctl->mmc->card,l_buf);
    if (copy_to_user(msdc_ctl->buffer, l_buf, 512))
        return -EFAULT;

#if DEBUG_MMC_IOCTL
    for (i = 0; i < 512; i++)
    {
        printk("%x", l_buf[i]);
        if (0 == ((i + 1) % 16)){
            printk("\n");
        }
    }
#endif

    if (copy_to_user(msdc_ctl->buffer, l_buf, 512))
        return -EFAULT;

    mmc_release_host(host_ctl->mmc);

    return 0;

}

int __simple_sd_ioctl_get_excsd(struct msdc_ioctl* msdc_ctl,u8 *excsd)
{
    char l_buf[512];
    struct msdc_host *host_ctl;
 
#if DEBUG_MMC_IOCTL
    int i;
#endif
  
    host_ctl = msdc_6577_host[msdc_ctl->host_num];

    BUG_ON(!host_ctl);
    BUG_ON(!host_ctl->mmc);
    BUG_ON(!host_ctl->mmc->card);

    mmc_claim_host(host_ctl->mmc);

#if DEBUG_MMC_IOCTL
    printk("user want the extend csd in msdc slot%d \n",msdc_ctl->host_num);
#endif
    mmc_send_ext_csd(host_ctl->mmc->card,l_buf);
    
    memcpy(excsd, l_buf, 512);

#if DEBUG_MMC_IOCTL

    for (i = 0; i < 512; i++)
    {
        printk("%x", l_buf[i]);
        if (0 == ((i + 1) % 16)){
            printk("\n");
        }
    }
#endif

    mmc_release_host(host_ctl->mmc);

    return 0;
}
EXPORT_SYMBOL(__simple_sd_ioctl_get_excsd);

static int simple_sd_ioctl_set_driving(struct msdc_ioctl* msdc_ctl)
{
    u32 base;
#if DEBUG_MMC_IOCTL
    unsigned int l_value;
#endif

    if (msdc_ctl->host_num == 0){
#ifndef CFG_DEV_MSDC0
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 1) {
#ifndef CFG_DEV_MSDC1
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 2) {
#ifndef CFG_DEV_MSDC2
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 3) {
#ifndef CFG_DEV_MSDC3
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    }

    base = msdc_6577_host[msdc_ctl->host_num]->base;
#ifndef FPGA_PLATFORM
   enable_clock(15 + msdc_6577_host[msdc_ctl->host_num]->id, "SD"); 
#endif
#if DEBUG_MMC_IOCTL
    printk("set: clk pull down driving is 0x%x\n", msdc_ctl->clk_pd_driving);
    printk("set: clk pull up driving is 0x%x\n", msdc_ctl->clk_pu_driving);
    printk("set: cmd pull down driving is 0x%x\n", msdc_ctl->cmd_pd_driving);
    printk("set: cmd pull up driving is 0x%x\n", msdc_ctl->cmd_pu_driving);
    printk("set: dat pull down driving is 0x%x\n", msdc_ctl->dat_pd_driving);
    printk("set: dat pull up driving is 0x%x\n", msdc_ctl->dat_pu_driving);
#endif

    sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVN, msdc_ctl->clk_pd_driving);
    sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVP, msdc_ctl->clk_pu_driving);
    sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVN, msdc_ctl->cmd_pd_driving);
    sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVP, msdc_ctl->cmd_pu_driving);
    sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVN, msdc_ctl->dat_pd_driving);
    sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVP, msdc_ctl->dat_pu_driving);

#if DEBUG_MMC_IOCTL
    l_value = 0;
    sdr_get_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVN, l_value);
    printk("read: clk pull down driving is 0x%x\n", l_value);
    l_value = 0;
    sdr_get_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVP, l_value);
    printk("read: clk pull up driving is 0x%x\n", l_value);
    l_value = 0;
    sdr_get_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVN, l_value);
    printk("read: cmd pull down driving is 0x%x\n", l_value);
    l_value = 0;
    sdr_get_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVP, l_value);
    printk("read: cmd pull up driving is 0x%x\n", l_value);
    l_value = 0;
    sdr_get_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVN, l_value);
    printk("read: dat pull down driving is 0x%x\n", l_value);
    l_value = 0;
    sdr_get_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVP, l_value);
    printk("read: dat pull up driving is 0x%x\n", l_value);
#endif

    return 0;
}

static int simple_sd_ioctl_get_driving(struct msdc_ioctl* msdc_ctl)
{
    u32 base;
    unsigned int l_value;

    
    if (msdc_ctl->host_num == 0){
#ifndef CFG_DEV_MSDC0
          printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 1) {
#ifndef CFG_DEV_MSDC1
          printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 2) {
#ifndef CFG_DEV_MSDC2
          printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 3) {
#ifndef CFG_DEV_MSDC3
          printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    }

    base = msdc_6577_host[msdc_ctl->host_num]->base;
#ifndef FPGA_PLATFORM   
    enable_clock(15 + msdc_6577_host[msdc_ctl->host_num]->id, "SD"); 
#endif
    sdr_get_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVN, l_value);
    msdc_ctl->clk_pd_driving = l_value;

    sdr_get_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVP, l_value);
    msdc_ctl->clk_pu_driving = l_value;

    sdr_get_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVN, l_value);
    msdc_ctl->cmd_pd_driving = l_value;

    sdr_get_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVP, l_value);
    msdc_ctl->cmd_pu_driving = l_value;

    sdr_get_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVN, l_value);
    msdc_ctl->dat_pd_driving = l_value;

    sdr_get_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVP, l_value);
    msdc_ctl->dat_pu_driving = l_value;


#if DEBUG_MMC_IOCTL
    printk("read: clk pull down driving is 0x%x\n", msdc_ctl->clk_pd_driving);
    printk("read: clk pull up driving is 0x%x\n", msdc_ctl->clk_pu_driving);
    printk("read: cmd pull down driving is 0x%x\n", msdc_ctl->cmd_pd_driving);
    printk("read: cmd pull up driving is 0x%x\n", msdc_ctl->cmd_pu_driving);
    printk("read: dat pull down driving is 0x%x\n", msdc_ctl->dat_pd_driving);
    printk("read: dat pull up driving is 0x%x\n", msdc_ctl->dat_pu_driving);
#endif

    return 0;
}

/*  to ensure format operate is clean the emmc device fully(partition erase) */
typedef struct mbr_part_info {
    unsigned int start_sector;
    unsigned int nr_sects;
    unsigned int part_no;
    unsigned char* part_name;
}MBR_PART_INFO_T;

#define MBR_PART_NUM  6
#define __MMC_ERASE_ARG        0x00000000
#define __MMC_TRIM_ARG         0x00000001

struct __mmc_blk_data {
    spinlock_t    lock;
    struct gendisk    *disk;
    struct mmc_queue queue;

    unsigned int    usage;
    unsigned int    read_only;
};

#ifdef MTK_EMMC_SUPPORT
int mt65xx_mmc_change_disk_info(unsigned int px, unsigned int addr, unsigned int size)
{
#ifdef MTK_EMMC_SUPPORT

    struct disk_part_iter piter;
	struct hd_struct *part;
	struct msdc_host *host;
	struct gendisk *disk;
    struct __mmc_blk_data *md;
    int i;
    /* emmc always in slot0 */
	host = msdc_6577_host[EMMC_HOST_NUM];
	BUG_ON(!host);
	BUG_ON(!host->mmc);
	BUG_ON(!host->mmc->card);

	md = mmc_get_drvdata(host->mmc->card);
	BUG_ON(!md);
	BUG_ON(!md->disk);
	disk = md->disk;
    disk_part_iter_init(&piter, disk, 0);

	for(i=0;i<PART_NUM;i++){
		if((PartInfo[i].partition_idx == px)&&((!strncmp(PartInfo[i].name,"usrdata",7))||(!strncmp(PartInfo[i].name,"sec_ro",6))||(!strncmp(PartInfo[i].name,"android",7))||(!strncmp(PartInfo[i].name,"cache",5)))){
			printk("update %s,need reduce 1MB in block device\n",PartInfo[i].name);
			size -= (0x100000)/512;
		}
	}
    while ((part = disk_part_iter_next(&piter))){
      
            if (px != 0 && px == part->partno) {
//#if DEBUG_MMC_IOCTL
                printk("[mt65xx_mmc_change_disk_info]px = %d size %llx -> %x offset %llx -> %x\n",px,part->nr_sects,size,part->start_sect,addr);
//#endif                
               
                    part->start_sect = addr;           
                    part->nr_sects = size;           

                    disk_part_iter_exit(&piter);
                    return 0;
                

             
            }
        
    }
    disk_part_iter_exit(&piter);

    return 1;
#else
    return 0;
#endif
}
EXPORT_SYMBOL(mt65xx_mmc_change_disk_info);

static int simple_mmc_get_disk_info(struct mbr_part_info* mpi, unsigned char* name)
{
    int i = 0;
    char* no_partition_name = "n/a";
    struct disk_part_iter piter;
    struct hd_struct *part;
    struct msdc_host *host;
    struct gendisk *disk;
    struct __mmc_blk_data *md;
    
    /* emmc always in slot0 */
    host = msdc_6577_host[EMMC_HOST_NUM];
    BUG_ON(!host);
    BUG_ON(!host->mmc);
    BUG_ON(!host->mmc->card);

    md = mmc_get_drvdata(host->mmc->card);
    BUG_ON(!md);
    BUG_ON(!md->disk);

    disk = md->disk;
    
    /* use this way to find partition info is to avoid handle addr transfer in scatter file 
     * and 64bit address calculate */
    disk_part_iter_init(&piter, disk, 0);
    while ((part = disk_part_iter_next(&piter))){
        for (i = 0; i < PART_NUM; i++) {
            if (PartInfo[i].partition_idx != 0 && PartInfo[i].partition_idx == part->partno) {
#if DEBUG_MMC_IOCTL
                printk("part_name = %s    name = %s\n", PartInfo[i].name, name);
#endif                
                if (!strncmp(PartInfo[i].name, name, 25)){
                    mpi->start_sector = part->start_sect;           
                    mpi->nr_sects = part->nr_sects;           
                    mpi->part_no = part->partno; 
                    if (i < PART_NUM){
                        mpi->part_name = PartInfo[i].name;
                    } else {
                        mpi->part_name = no_partition_name;
                    }

                    disk_part_iter_exit(&piter);
                    return 0;
                }

                break;
            }
        }
    }
    disk_part_iter_exit(&piter);

    return 1;
}


/* call mmc block layer interface for userspace to do erase operate */
static int simple_mmc_erase_func(unsigned int start, unsigned int size)
{
    struct msdc_host *host;
    
    /* emmc always in slot0 */
    host = msdc_6577_host[EMMC_HOST_NUM];
    BUG_ON(!host);
    BUG_ON(!host->mmc);
    BUG_ON(!host->mmc->card);
    
    mmc_claim_host(host->mmc);

    if (!mmc_can_trim(host->mmc->card)){
        printk("emmc card can't support trim\n");
        return 0;
    }
    
    mmc_erase(host->mmc->card, start, size,
             __MMC_TRIM_ARG);

#if DEBUG_MMC_IOCTL
    printk("erase done....\n");
#endif

    mmc_release_host(host->mmc);
    
    return 0;
}
#endif

int simple_mmc_erase_partition(unsigned char* name)
{
#ifdef MTK_EMMC_SUPPORT
    struct mbr_part_info mbr_part;
    int l_ret = -1;

    BUG_ON(!name);
    /* just support erase cache & data partition now */
    if (('u' == *name && 's' == *(name + 1) && 'r' == *(name + 2) && 'd' == *(name + 3) && 
         'a' == *(name + 4) && 't' == *(name + 5) && 'a' == *(name + 6)) || 
        ('c' == *name && 'a' == *(name + 1) && 'c' == *(name + 2) && 'h' == *(name + 3) && 'e' == *(name + 4))) {
        /* find erase start address and erase size, just support high capacity emmc card now */
        l_ret = simple_mmc_get_disk_info(&mbr_part, name);


        if (l_ret == 0){
            /* do erase */
            printk("erase %s start sector: 0x%x size: 0x%x \n", mbr_part.part_name, mbr_part.start_sector, mbr_part.nr_sects);
            simple_mmc_erase_func(mbr_part.start_sector, mbr_part.nr_sects);
        }
    }

    return 0;
#else
    return 0;
#endif

}

static int simple_mmc_erase_partition_wrap(struct msdc_ioctl* msdc_ctl)
{
    unsigned char name[25];

    if (copy_from_user(name, (unsigned char*)msdc_ctl->buffer, msdc_ctl->total_size))
        return -EFAULT;

    return simple_mmc_erase_partition(name);
}

static long simple_sd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct msdc_ioctl *msdc_ctl = (struct msdc_ioctl *)arg;
	int ret;
	if(msdc_ctl == NULL){
		switch(cmd){
			#ifdef MTK_SD_REINIT_SUPPORT
			case MSDC_REINIT_SDCARD:
				ret = sd_ioctl_reinit(msdc_ctl);
				break;
			#endif
        	default:
            printk("mt6575_sd_ioctl:this opcode value is illegal!!\n");
            return -EINVAL;
			}
		return ret;		
	}
	else{	
    switch (msdc_ctl->opcode){
        case MSDC_SINGLE_READ_WRITE:
            msdc_ctl->result = simple_sd_ioctl_single_rw(msdc_ctl);
            break;
        case MSDC_MULTIPLE_READ_WRITE:
            msdc_ctl->result = simple_sd_ioctl_multi_rw(msdc_ctl);
            break;
        case MSDC_GET_CID:
            msdc_ctl->result = simple_sd_ioctl_get_cid(msdc_ctl);
            break;
        case MSDC_GET_CSD:
            msdc_ctl->result = simple_sd_ioctl_get_csd(msdc_ctl);
            break;
        case MSDC_GET_EXCSD:
            msdc_ctl->result = simple_sd_ioctl_get_excsd(msdc_ctl);
            break;
        case MSDC_DRIVING_SETTING:
            printk("in ioctl to change driving\n");
            if (1 == msdc_ctl->iswrite){
                msdc_ctl->result = simple_sd_ioctl_set_driving(msdc_ctl);
            } else {
                msdc_ctl->result = simple_sd_ioctl_get_driving(msdc_ctl);
            }
            break;
        case MSDC_HOPPING_SETTING:
            printk("in ioctl to get/set hopping bit/time.\n");
            if (1 == msdc_ctl->iswrite){
                msdc_ctl->result = simple_sd_ioctl_set_hop(msdc_ctl);
            }else {
                msdc_ctl->result = simple_sd_ioctl_get_hop(msdc_ctl);
            }
            break;
        case MSDC_ERASE_PARTITION:
            msdc_ctl->result = simple_mmc_erase_partition_wrap(msdc_ctl);
            break;
        default:
            printk("simple_sd_ioctl:this opcode value is illegal!!\n");
            return -EINVAL;
    }

    return msdc_ctl->result;
		}
}

static void simple_sd_ioctl_clksrc_env(struct msdc_ioctl* msdc_ctl)
{    
    unsigned int temp;    

    //set AUDPLL clock frequency as 183.15MHz(F00071E0 = 0x5001)                
    pll_fsel(MT65XX_AUDPLL, 0x5001);               
    enable_pll(MT65XX_AUDPLL, "msdc_em");

#if DEBUG_MSDC_SSC
    printk(KERN_WARNING "--AUDPLL--> set env clock reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG01));
#endif

    switch (msdc_ctl->host_num)
    {
        case 0:
            //switch MSDC0 clock source of to AUDPLL(F100000C bit1~0 = 2'b10)
            temp = sdr_read32(AUDPLL_CTL_REG02);
            temp &= (~(1 << 1)) & (~(1 << 0));//bit 1,0 set 0;    
            temp |= (1 << 1); //bit 1 set 1;
            sdr_write32(AUDPLL_CTL_REG02, temp);
            break;
        case 1:
            //switch MSDC1 clock source of to AUDPLL(F100000C bit4~3 = 2'b10)
            temp = sdr_read32(AUDPLL_CTL_REG02);
            temp &= (~(1 << 4)) & (~(1 << 3));//bit 4,3 set 0;    
            temp |= (1 << 4); //bit 4 set 1;
            sdr_write32(AUDPLL_CTL_REG02, temp);
            break;
        case 2:
            //switch MSDC2 clock source of to AUDPLL(F100000C bit6~5 = 2'b10)
            temp = sdr_read32(AUDPLL_CTL_REG02);
            temp &= (~(1 << 6)) & (~(1 << 5));//bit 6,5 set 0;    
            temp |= (1 << 6); //bit 6 set 1;
            sdr_write32(AUDPLL_CTL_REG02, temp);
            break;
        case 3:
            //switch MSDC3 clock source of to AUDPLL(F100000C bit8~7 = 2'b10)
            temp = sdr_read32(AUDPLL_CTL_REG02);
            temp &= (~(1 << 8)) & (~(1 << 7));//bit 8,7 set 0;    
            temp |= (1 << 8); //bit 8 set 1;
            sdr_write32(AUDPLL_CTL_REG02, temp);
            break;
        default:
            printk("simple_sd_ioctl_clksrc_env: host_num is illegal!\n");
            break;
    }

    mdelay(50);        

#if DEBUG_MSDC_SSC
    freq_meter(0x11, 0);
    printk(KERN_WARNING "--AUDPLL--> set env switch src reg = 0x%x\n", sdr_read32(AUDPLL_CTL_REG02));
#endif
}

static int simple_sd_ioctl_set_hop(struct msdc_ioctl* msdc_ctl)
{
    struct msdc_host *host_ctl;
    unsigned short temp_1;
    unsigned short temp_2; 

    if (msdc_ctl->host_num == 0){
#ifndef CFG_DEV_MSDC0
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 1) {
#ifndef CFG_DEV_MSDC1
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 2) {
#ifndef CFG_DEV_MSDC2
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 3) {
#ifndef CFG_DEV_MSDC3
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    }


#if DEBUG_MSDC_SSC
    printk(KERN_WARNING "---->Prepear in set hopping bit , msdc_ctl->hopping_bit = %d", msdc_ctl->hopping_bit);
#endif

    host_ctl = msdc_6577_host[msdc_ctl->host_num];
    BUG_ON(!host_ctl);
    BUG_ON(!host_ctl->mmc);

    mmc_claim_host(host_ctl->mmc);    
    simple_sd_ioctl_clksrc_env(msdc_ctl);   
    mmc_release_host(host_ctl->mmc);

    //set hopping bit
    do{        
        temp_1 = sdr_read16(AUDPLL_CTL_REG12);
        temp_1 &= (~(1 << 0)) & (~(1 << 1)) & (~(1 << 2)); //bit 0,1,2 set 0,  (BSEL = 000);

        switch (msdc_ctl->hopping_bit)
        {
            case SET_HOP_BIT_NONE:
                temp_1 &= (~(1 << 0));                                 //bit 0     set 0,  (BSEL = 000);
                sdr_write16(AUDPLL_CTL_REG12, temp_1);
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> set frq hopping bit msdc_ctl->hopping_bit = %d\n", msdc_ctl->hopping_bit);
                printk(KERN_WARNING "--AUDPLL--> set frq hopping bit reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> set delta frq = None!\n");
#endif
                break;
            case SET_HOP_BIT1:
                temp_1 |= (1 << 0);                                 //bit 0     set 1,  (BSEL = 001);
                sdr_write16(AUDPLL_CTL_REG12, temp_1);
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> set frq hopping bit msdc_ctl->hopping_bit = %d\n", msdc_ctl->hopping_bit);
                printk(KERN_WARNING "--AUDPLL--> set frq hopping bit reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> set delta frq = 2.26 MHz!\n");
#endif
                break;

            case SET_HOP_BIT2:    
                temp_1 |= (1 << 1);                                 //bit 1     set 1,  (BSEL = 010);
                sdr_write16(AUDPLL_CTL_REG12, temp_1);
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> set frq hopping bit msdc_ctl->hopping_bit = %d\n", msdc_ctl->hopping_bit);
                printk(KERN_WARNING "--AUDPLL--> set frq hopping bit reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> set delta frq = 4.52 MHz!\n");
#endif
                break;

            case SET_HOP_BIT3:
                temp_1 |= (1 << 2);                                 //bit 2     set 1,  (BSEL = 100);
                sdr_write16(AUDPLL_CTL_REG12, temp_1);
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> set frq hopping bit msdc_ctl->hopping_bit = %d\n", msdc_ctl->hopping_bit);
                printk(KERN_WARNING "--AUDPLL--> set frq hopping bit reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> set delta frq = 9.04 MHz!\n");
#endif
                break;

            default:
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> set illega delta frq!\n");
#endif
                return -1;
        }
    }while(0);

    //set hopping time
    do{
        #if DEBUG_MSDC_SSC
            printk(KERN_WARNING "---->Prepear in set hopping time , msdc_ctl->hopping_time = %d", msdc_ctl->hopping_time);
        #endif           
        temp_2 = sdr_read16(AUDPLL_CTL_REG12);
        temp_2 &= (~(1 << 8)) & (~(1 << 9 )) & (~(1 << 10)); //bit 8,9,10 set 0,    (TSEL = 000);
        
            switch (msdc_ctl->hopping_time)
            {
                case SET_HOP_TIME0:
                    temp_2 &= (~(1 << 8)) & (~(1 <<9 )) & (~(1 << 10)); //bit 8,9,10 set 0,  (TSEL = 000);
                    sdr_write16(AUDPLL_CTL_REG12, temp_2);
                    #if DEBUG_MSDC_SSC
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time = 30.5 us!\n");
                    #endif
                    break;
                case SET_HOP_TIME1:
                    temp_2 |= (1 << 8);                                  //bit 8       set 1,  (TSEL = 001);
                    sdr_write16(AUDPLL_CTL_REG12, temp_2);
                    #if DEBUG_MSDC_SSC
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time = 61.0 us!\n");
                    #endif
                    break;
                case SET_HOP_TIME2:
                    temp_2 |= (1 << 9);                                  //bit 9       set 1,  (TSEL = 010);
                    sdr_write16(AUDPLL_CTL_REG12, temp_2);
                    #if DEBUG_MSDC_SSC
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time = 122.1 us!\n");
                    #endif
                    break;
                case SET_HOP_TIME3:
                    temp_2 |= (1 << 8) | (1 << 9);                      //bit 8,9    set 1,  (TSEL = 011);
                    sdr_write16(AUDPLL_CTL_REG12, temp_2);
                    #if DEBUG_MSDC_SSC
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time = 244.1 us!\n");
                    #endif
                    break;
                case SET_HOP_TIME4:
                    temp_2 |= (1 << 10);                                  //bit 10       set 1,  (TSEL = 100);
                    sdr_write16(AUDPLL_CTL_REG12, temp_2);
                    #if DEBUG_MSDC_SSC
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                        printk(KERN_WARNING "--AUDPLL--> set frq hopping time = 488.3 us!\n");
                    #endif
                    break;
                default:
                    #if DEBUG_MSDC_SSC
                        printk(KERN_WARNING "--AUDPLL--> set illega hopping time!\n");
                    #endif
                    return -1;
            }
        }while(0);
    
    return 0;
}

static int simple_sd_ioctl_get_hop(struct msdc_ioctl* msdc_ctl)
{

    if (msdc_ctl->host_num == 0){
#ifndef CFG_DEV_MSDC0
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 1) {
#ifndef CFG_DEV_MSDC1
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 2) {
#ifndef CFG_DEV_MSDC2
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    } else if (msdc_ctl->host_num == 3) {
#ifndef CFG_DEV_MSDC3
        printk("host%d is not config\n", msdc_ctl->host_num);
        return -1;
#endif
    }


    //get hopping bit
    do{
#if DEBUG_MSDC_SSC
        printk(KERN_WARNING "---->Prepear in get hopping bit , reg = 0x%x", sdr_read16(AUDPLL_CTL_REG12));
#endif
        switch((sdr_read16(AUDPLL_CTL_REG12) & AUDPLL_BSEL_MASK))
        {

            case AUDPLL_BSEL_RESULT0:
                put_user(0, &(msdc_ctl->hopping_bit));
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get frq hopping bit msdc_ctl->hopping_bit = %d\n", msdc_ctl->hopping_bit);
                printk(KERN_WARNING "--AUDPLL--> get frq hopping bit reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> get delta frq = None!\n");
#endif
                break;

            case AUDPLL_BSEL_RESULT1:
                put_user(1, &(msdc_ctl->hopping_bit));
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get frq hopping bit msdc_ctl->hopping_bit = %d\n", msdc_ctl->hopping_bit);
                printk(KERN_WARNING "--AUDPLL--> get frq hopping bit reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> get delta frq = 2.26 MHz!\n");
#endif
                break;

            case AUDPLL_BSEL_RESULT2:
                put_user(2, &(msdc_ctl->hopping_bit));
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get frq hopping bit msdc_ctl->hopping_bit = %d\n", msdc_ctl->hopping_bit);
                printk(KERN_WARNING "--AUDPLL--> get frq hopping bit reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> get delta frq = 4.52 MHz!\n");
#endif
                break;

            case AUDPLL_BSEL_RESULT3:
                put_user(3, &(msdc_ctl->hopping_bit));
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get frq hopping bit msdc_ctl->hopping_bit = %d\n", msdc_ctl->hopping_bit);
                printk(KERN_WARNING "--AUDPLL--> get frq hopping bit reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> get delta frq = 9.04 MHz!\n");
#endif
                break;

            default:
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get illegal delta frq!\n");
#endif
                return -1;
        }
    }while(0);

    //get hopping time
    do{
#if DEBUG_MSDC_SSC
        printk(KERN_WARNING "---->Prepear in get hopping time , reg = 0x%x", sdr_read16(AUDPLL_CTL_REG12));
#endif
        switch((sdr_read16(AUDPLL_CTL_REG12) & AUDPLL_TSEL_MASK))
        {
            case AUDPLL_TSEL_RESULT1:
                put_user(0, &(msdc_ctl->hopping_time));
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time = 30.5 us!\n");
#endif
                break;

            case AUDPLL_TSEL_RESULT2:
                put_user(1, &(msdc_ctl->hopping_time));
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time = 61.0 us!\n");
#endif
                break;

            case AUDPLL_TSEL_RESULT3:
                put_user(2, &(msdc_ctl->hopping_time));
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time = 122.1 us!\n");
#endif
                break;

            case AUDPLL_TSEL_RESULT4:
                put_user(3, &(msdc_ctl->hopping_time));
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time = 244.1 us!\n");
#endif
                break;

            case AUDPLL_TSEL_RESULT5:
                put_user(4, &(msdc_ctl->hopping_time));
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time msdc_ctl->hopping_time = %d\n", msdc_ctl->hopping_time);
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time reg = 0x%x\n", sdr_read16(AUDPLL_CTL_REG12));
                printk(KERN_WARNING "--AUDPLL--> get frq hopping time = 488.3 us!\n");
#endif
                break;

            default:
#if DEBUG_MSDC_SSC
                printk(KERN_WARNING "--AUDPLL--> get illegal hopping time!\n");
#endif
                return -1;
        }
    }while(0);
    return 0;
}

static struct file_operations simple_msdc_em_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl    = simple_sd_ioctl,
    .open           = simple_sd_open,
};

static struct miscdevice simple_msdc_em_dev[] = {
    {
        .minor    = MISC_DYNAMIC_MINOR,
        .name    = "misc-sd",
        .fops    = &simple_msdc_em_fops,
    }
};

static int simple_sd_probe(struct platform_device *pdev)
{
    int ret = 0;

    printk("in misc_sd_probe function\n");

    return ret;
}

static int simple_sd_remove(struct platform_device *pdev)
{
    return 0;
}

static struct platform_driver simple_sd_driver = {
    .probe   = simple_sd_probe,
    .remove  = simple_sd_remove,

    .driver  = {
        .name  = DRV_NAME_MISC,
        .owner = THIS_MODULE,
    },
};

static int __init simple_sd_init(void)
{
    int ret;

    sg_msdc_multi_buffer =(u32*)kmalloc(64*1024,GFP_KERNEL);
    if( sg_msdc_multi_buffer ==NULL)
        printk("allock 64KB memory failed\n");


    ret = platform_driver_register(&simple_sd_driver);
    if (ret) {
        printk(KERN_ERR DRV_NAME_MISC ": Can't register driver\n");
        return ret;
    }
    printk(KERN_INFO DRV_NAME_MISC ": MediaTek simple SD/MMC Card Driver\n");

    /*msdc0 is for emmc only, just for emmc*/
    //ret = misc_register(&simple_msdc_em_dev[host->id]);
    ret = misc_register(&simple_msdc_em_dev[0]);
    if (ret) {
        printk("register MSDC Slot[0] misc driver failed (%d)\n",ret);
        return ret;
    }

    return 0;
}

static void __exit simple_sd_exit(void)
{
    if (sg_msdc_multi_buffer != NULL){
        kfree(sg_msdc_multi_buffer);
        sg_msdc_multi_buffer = NULL;
    }

    misc_deregister(&simple_msdc_em_dev[0]);
    
    platform_driver_unregister(&simple_sd_driver);
}

#ifdef MTK_EMMC_SUPPORT
static int mmc_claim_host_simple(struct mmc_host *host, char *abort)
{
    host->claimed = 1;
    host->claimer = current;
    host->claim_cnt += 1;

    return 0;
}

static void mmc_do_release_host_simple(struct mmc_host *host)
{
    if (--host->claim_cnt) {
    } else {
        host->claimed = 0;
        host->claimer = NULL;
    }
}

static void
mmc_start_request_simple(struct mmc_host *host, struct mmc_request *mrq)
{
    mrq->cmd->error = 0;
    mrq->cmd->mrq = mrq;
    if (mrq->data) {

        mrq->cmd->data = mrq->data;
        mrq->data->error = 0;
        mrq->data->mrq = mrq;
        if (mrq->stop) {
            mrq->data->stop = mrq->stop;
            mrq->stop->error = 0;
            mrq->stop->mrq = mrq;
        }
    }

    simple_sd_request(host, mrq);
}

static int simple_sd_stop(struct msdc_ioctl* msdc_ctl)
{
    struct mmc_command msdc_cmd;
    struct mmc_request msdc_mrq;
    struct msdc_host *host_ctl;

    host_ctl = msdc_6577_host[msdc_ctl->host_num];

    mmc_claim_host_simple(host_ctl->mmc, NULL);

    if(msdc_ctl->total_size > 512){
        msdc_ctl->result = -1;
        return  msdc_ctl->result;
    }

    memset(&msdc_mrq, 0, sizeof(struct mmc_request));
    memset(&msdc_cmd, 0, sizeof(struct mmc_command));

    msdc_mrq.cmd = &msdc_cmd;
    msdc_mrq.data = NULL;

    dma_force[host_ctl->id] = FORCE_IN_PIO;

    msdc_cmd.opcode = MMC_STOP_TRANSMISSION;
    msdc_cmd.arg = 0;

    BUG_ON(!host_ctl->mmc->card);

    msdc_cmd.flags = MMC_RSP_SPI_R2 | MMC_RSP_R1B | MMC_CMD_AC;

    mmc_start_request_simple(host_ctl->mmc, &msdc_mrq);

    mmc_do_release_host_simple(host_ctl->mmc);
    
    if (msdc_cmd.error)
        msdc_ctl->result= msdc_cmd.error;

    return msdc_ctl->result;
}


static int simple_sd_status(struct msdc_ioctl* msdc_ctl)
{
    struct mmc_command msdc_cmd;
    struct mmc_request msdc_mrq;
    struct msdc_host *host_ctl;

    host_ctl = msdc_6577_host[msdc_ctl->host_num];

    mmc_claim_host_simple(host_ctl->mmc, NULL);


    if(msdc_ctl->total_size > 512){
        msdc_ctl->result = -1;
        return  msdc_ctl->result;
    }

    memset(&msdc_mrq, 0, sizeof(struct mmc_request));
    memset(&msdc_cmd, 0, sizeof(struct mmc_command));

    msdc_mrq.cmd = &msdc_cmd;
    msdc_mrq.data = NULL;

    dma_force[host_ctl->id] = FORCE_IN_PIO;

    msdc_cmd.opcode = MMC_SEND_STATUS;
    msdc_cmd.arg = host_ctl->mmc->card->rca << 16;

    BUG_ON(!host_ctl->mmc->card);

    msdc_cmd.flags = MMC_RSP_SPI_R2 | MMC_RSP_R1 | MMC_CMD_AC;

    mmc_start_request_simple(host_ctl->mmc, &msdc_mrq);

    mmc_do_release_host_simple(host_ctl->mmc);
    
    //if (msdc_cmd.error)
    //    msdc_ctl->result= msdc_cmd.error;
    msdc_ctl->result= msdc_cmd.resp[0];

    return msdc_ctl->result;
}


static int simple_sd_panic_dump_single_w(struct msdc_ioctl* msdc_ctl)
{
    struct scatterlist msdc_sg;
    struct mmc_data    msdc_data;
    struct mmc_command msdc_cmd;
    struct mmc_request msdc_mrq;
    struct msdc_host *host_ctl;

    host_ctl = msdc_6577_host[msdc_ctl->host_num];

    mmc_claim_host_simple(host_ctl->mmc, NULL);


    if(msdc_ctl->total_size > 512){
        msdc_ctl->result = -1;
        return  msdc_ctl->result;
    }

    memset(&msdc_data, 0, sizeof(struct mmc_data));
    memset(&msdc_mrq, 0, sizeof(struct mmc_request));
    memset(&msdc_cmd, 0, sizeof(struct mmc_command));

    msdc_mrq.cmd = &msdc_cmd;
    msdc_mrq.data = &msdc_data;

    dma_force[host_ctl->id] = FORCE_IN_PIO;

    msdc_data.flags = MMC_DATA_WRITE;
    msdc_cmd.opcode = MMC_WRITE_BLOCK;
    msdc_data.blocks = msdc_ctl->total_size / 512;
    if (MSDC_CARD_DUNM_FUNC != msdc_ctl->opcode) {
        if (copy_from_user(sg_msdc_multi_buffer, msdc_ctl->buffer, 512))
            return -EFAULT;
    } else {
        /* called from other kernel module */
        memcpy(sg_msdc_multi_buffer, msdc_ctl->buffer, 512);
    }

    msdc_cmd.arg = msdc_ctl->address;

    BUG_ON(!host_ctl->mmc->card);
    if (!mmc_card_blockaddr(host_ctl->mmc->card)){
        printk("the device is used byte address!\n");
        msdc_cmd.arg <<= 9;
    }

    msdc_cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;

    msdc_data.stop = NULL;
    msdc_data.blksz = 512;
    msdc_data.sg = &msdc_sg;
    msdc_data.sg_len = 1;

    sg_init_one(&msdc_sg, sg_msdc_multi_buffer, msdc_ctl->total_size);

    mmc_start_request_simple(host_ctl->mmc, &msdc_mrq);

    mmc_do_release_host_simple(host_ctl->mmc);
    
    if (msdc_cmd.error)
        msdc_ctl->result= msdc_cmd.error;

    if (msdc_data.error)
        msdc_ctl->result= msdc_data.error;
    else
        msdc_ctl->result= 0;

    return msdc_ctl->result;
}

//#define PART_NUM  (18)
#define DUMP_INTO_CARD_TYPE_MASK     (0xFFFF0000)
#define DUMP_INTO_SLOT_NUM_MASK      (0xFFFF)
#define DUMP_INTO_SD_CARD            (0x00000000)
#define DUMP_INTO_MMC_CARD           (0x00010000)
#define SD_FALSE             -1
#define SD_TRUE               0


static int emmc_dump_read(unsigned char* buf, unsigned int len, unsigned int offset,unsigned int slot)
{
    /* maybe delete in furture */
    struct msdc_ioctl     msdc_ctl;
    unsigned int i;
    unsigned int l_user_begin_num;
    unsigned int l_dest_num;
    unsigned long long l_start_offset;
    unsigned int ret = SD_FALSE;

    if ((0 != slot) || (0 != offset % 512) || (0 != len % 512)) {
        /* emmc always in slot0 */
        printk("debug: slot is not use for emmc!\n");
        return ret;
    }

    /* find the offset in emmc */
    for (i = 0; i < PART_NUM; i++) {
    //for (i = 0; i < 1; i++) {
        if ('m' == *(PartInfo[i].name) && 'b' == *(PartInfo[i].name + 1) &&
                'r' == *(PartInfo[i].name + 2)){
            l_user_begin_num = i;
        }

        if ('e' == *(PartInfo[i].name) && 'x' == *(PartInfo[i].name + 1) &&
                'p' == *(PartInfo[i].name + 2) && 'd' == *(PartInfo[i].name + 3) &&
                'b' == *(PartInfo[i].name + 4)){
            l_dest_num = i;
        }
    }

#if DEBUG_MMC_IOCTL
    printk("l_user_begin_num = %d l_dest_num = %d\n",l_user_begin_num,l_dest_num);
#endif

    if (l_user_begin_num >= PART_NUM && l_dest_num >= PART_NUM) {
        printk("not find in scatter file error!\n");
        return ret;
    }

    if (PartInfo[l_dest_num].size < (len + offset)) {
        printk("read operation oversize!\n");
        return ret;
    }

#if DEBUG_MMC_IOCTL
    printk("read start address=0x%x\n", PartInfo[l_dest_num].start_address - PartInfo[l_user_begin_num].start_address);
#endif 
    l_start_offset = offset + PartInfo[l_dest_num].start_address - PartInfo[l_user_begin_num].start_address;

    msdc_ctl.partition = 0;
    msdc_ctl.iswrite = 0;
    msdc_ctl.host_num = slot;
    msdc_ctl.opcode = MSDC_CARD_DUNM_FUNC;
    msdc_ctl.total_size = 512;
    msdc_ctl.trans_type = 0;
    for (i = 0; i < (len/512); i++) {
        /* code */
        msdc_ctl.address = (l_start_offset >> 9) + i; //blk address
        msdc_ctl.buffer =(u32*)(buf + i * 512);

#if DEBUG_MMC_IOCTL
        printk("l_start_offset = %d\n", msdc_ctl.address);
#endif        
        msdc_ctl.result = simple_sd_ioctl_single_rw(&msdc_ctl);
    }
    
#if DEBUG_MMC_IOCTL
    printk("read data:");
    for (i = 0; i < 32; i++) {
        printk("0x%x", buf[i]);
        if (0 == (i+1)%32)
            printk("\n");
    }
#endif
    return 0;
}

static int simple_emmc_dump_write(unsigned char* buf, unsigned int len, unsigned int offset,unsigned int slot)
{
    /* maybe delete in furture */
    struct msdc_ioctl     msdc_ctl;
    unsigned int i;
    unsigned int l_user_begin_num;
    unsigned int l_dest_num;
    unsigned long long l_start_offset;
    unsigned int ret = SD_FALSE;

    static int sg_test = 0;

    if ((0 != slot) || (0 != offset % 512) || (0 != len % 512)) {
        /* emmc always in slot0 */
        printk("debug: slot is not use for emmc!\n");
        return ret;
    }

#if DEBUG_MMC_IOCTL
    printk("write data:");
    for (i = 0; i < 32; i++) {
        printk("0x%x", buf[i]);
        if (0 == (i+1)%32)
            printk("\n");
    }
#endif


    /* find the offset in emmc */
    for (i = 0; i < PART_NUM; i++) {
        if ('m' == *(PartInfo[i].name) && 'b' == *(PartInfo[i].name + 1) &&
                'r' == *(PartInfo[i].name + 2)){
            l_user_begin_num = i;
        }

        if ('e' == *(PartInfo[i].name) && 'x' == *(PartInfo[i].name + 1) &&
                'p' == *(PartInfo[i].name + 2) && 'd' == *(PartInfo[i].name + 3) &&
                'b' == *(PartInfo[i].name + 4)){
            l_dest_num = i;
        }
    }

    if (l_user_begin_num >= PART_NUM && l_dest_num >= PART_NUM) {
        printk("not find in scatter file error!\n");
        return ret;
    }

    if (PartInfo[l_dest_num].size < (len + offset)) {
        printk("write operation oversize!\n");
        return ret;
    }

#if DEBUG_MMC_IOCTL
    printk("write start address=0x%x\n", PartInfo[l_dest_num].start_address - PartInfo[l_user_begin_num].start_address);
#endif 

    l_start_offset = offset + PartInfo[l_dest_num].start_address - PartInfo[l_user_begin_num].start_address;
    msdc_ctl.partition = 0;
    msdc_ctl.iswrite = 1;
    msdc_ctl.host_num = slot;
    msdc_ctl.opcode = MSDC_CARD_DUNM_FUNC;
    msdc_ctl.total_size = 512;
    msdc_ctl.trans_type = 0;
    if (sg_test == 0) {
        sg_test = 1;

        msdc_ctl.result = simple_sd_stop(&msdc_ctl);
        do{
            msdc_ctl.result = simple_sd_status(&msdc_ctl);
            printk("card state: %d\n", (msdc_ctl.result & 0x1f00)>>9);
        } while( ((msdc_ctl.result & 0x1f00)>>9) != 4);
    }
    for (i = 0; i < (len/512); i++) {
        /* code */
        msdc_ctl.address = (l_start_offset >> 9) + i; //blk address
        msdc_ctl.buffer = (u32*)(buf + i * 512);

#if DEBUG_MMC_IOCTL
        printk("l_start_offset = %d\n", msdc_ctl.address);
#endif        
        msdc_ctl.result = simple_sd_panic_dump_single_w(&msdc_ctl);
    }

    return 0;
}
#endif

int card_dump_func_read(unsigned char* buf, unsigned int len, unsigned int offset, unsigned int dev)
{
#ifdef MTK_EMMC_SUPPORT
    unsigned int l_slot;
    unsigned int ret = SD_FALSE;

    l_slot = dev & DUMP_INTO_SLOT_NUM_MASK;
    if (l_slot > 3){
        printk("big slot number, error!\n");
        return ret;
    }

    if (DUMP_INTO_MMC_CARD == (dev & DUMP_INTO_CARD_TYPE_MASK)) {
        /* emmc card */
        ret = emmc_dump_read(buf, len, offset, l_slot);
    } else if(DUMP_INTO_SD_CARD == (dev & DUMP_INTO_CARD_TYPE_MASK)) {
        /* not support now */
    } else {
        printk("unknown card type, error!\n");
        return ret;
    }
    return SD_TRUE;
#else
    return 0;
#endif
}

int card_dump_func_write(unsigned char* buf, unsigned int len, unsigned int offset, unsigned int dev)
{
#ifdef MTK_EMMC_SUPPORT
    unsigned int l_slot;
    unsigned int ret = SD_FALSE;

    l_slot = dev & DUMP_INTO_SLOT_NUM_MASK;
    if (l_slot > 3){
        printk("big slot number, error!\n");
        return ret;
    }

    if (DUMP_INTO_MMC_CARD == (dev & DUMP_INTO_CARD_TYPE_MASK)) {
        /* emmc card */
        //printk("in card_dump_func_write func!\n");
        ret = simple_emmc_dump_write(buf, len, offset, l_slot);
    } else if(DUMP_INTO_SD_CARD == (dev & DUMP_INTO_CARD_TYPE_MASK)) {
        /* not support now */
    } else {
        printk("unknown card type, error!\n");
        return ret;
    }
    return SD_TRUE;
#else
    return 0;
#endif
}

#if 0
static void card_dump_func_test(void)
{
    u32 i;
    char l_test_buf[1024];
    memset(l_test_buf, 0, 1024);
    //buf, len, dev, offset
    printk("l_test_buf = 0x%p\n",l_test_buf);
    card_dump_func_read(l_test_buf, 1024, 0, 0x10000);

    printk("read as follow:\n");
    for (i = 0; i < 1024; i++) {
        printk("0x%x", l_test_buf[i]);

        if (((i + 1) % 64) == 0){
            printk("\n");
        }
    }

    //write test
    memset(l_test_buf, 0x5a, 1024);
    card_dump_func_write(l_test_buf, 1024, 0, 0x10000);

    memset(l_test_buf, 0, 1024);
    card_dump_func_read(l_test_buf, 1024, 0, 0x10000);
    for (i = 0; i < 1024; i++) {
        printk("0x%x", l_test_buf[i]);

        if (((i + 1) % 64) == 0){
            printk("\n");
        }
    }

    /* read size not aligned */
    card_dump_func_read(l_test_buf, 1004, 0, 0x10000);

    /* read addterss not aligned */
    card_dump_func_read(l_test_buf, 1024, 3, 0x10000);

    /* read size beyond the partition size */
    card_dump_func_read(l_test_buf, 1004, 3, 0x10000);

    /* write size not aligned */
    card_dump_func_write(l_test_buf, 1004, 0, 0x10000);

    /* write addterss not aligned */
    card_dump_func_write(l_test_buf, 1024, 3, 0x10000);

    /* write size beyond the partition size */
    card_dump_func_write(l_test_buf, 1004, 3, 0x10000);
}
#endif
EXPORT_SYMBOL(card_dump_func_read);
EXPORT_SYMBOL(card_dump_func_write);


module_init(simple_sd_init);
module_exit(simple_sd_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("simple MediaTek SD/MMC Card Driver");
MODULE_AUTHOR("feifei.wang <feifei.wang@mediatek.com>");
