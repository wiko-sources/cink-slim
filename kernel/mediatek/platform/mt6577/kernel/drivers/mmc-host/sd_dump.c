#include <stdarg.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <mach/board.h>
//#include <mach/mt6577_devs.h>
#include <mach/mt6577_typedefs.h>
//#include <mach/mt6577_clock_manager.h>
#include <mach/mt6577_pm_ldo.h>		/* hwPowerOn */
#include <asm/io.h>		/* __raw_readl */
#include "sd_dump.h"

#define MSDC0_BASE 0xF1220000
#define MSDC1_BASE 0xF1230000
#define MSDC2_BASE 0xF1250000
#define MSDC3_BASE 0xF1240000

/* the base address of sd card slot */
#define MSDC_REG_BASE    (MSDC1_BASE)// [Fix me] EVB and Phone 
#define MSDC_ID          (1)         // using msdc[1],  0~3 
#define MSDC_CLKSRC      (MSDC_CLKSRC_197M)
static  u32 clks[] = {26000000, 197000000, 208000000, 0}; 

//#define CMD_RETRIES        (5)
//#define CMD_TIMEOUT        (100)        /* 100ms */
#define BLK_LEN            (512)
#define MAX_SCLK           (52000000)
#define NORMAL_SCLK        (25000000)
#define MIN_SCLK           (260000)

enum {
    RESP_NONE = 0,
    RESP_R1   = 1,
    RESP_R2   = 2,
    RESP_R3   = 3,
    RESP_R4   = 4,
    RESP_R5   = 5,
    RESP_R6   = 6,
    RESP_R7   = 7,
    RESP_R1B  = 8
};

static int msdc_rsp[] = {
    0,  /* RESP_NONE */
    1,  /* RESP_R1 */
    2,  /* RESP_R2 */
    3,  /* RESP_R3 */
    4,  /* RESP_R4 */
    1,  /* RESP_R5 */
    1,  /* RESP_R6 */
    1,  /* RESP_R7 */
    7,  /* RESP_R1b */
};

#define msdc_retry(expr,retry,cnt) \
    do { \
        int backup = cnt; \
        while (retry) { \
            if (!(expr)) break; \
            if (cnt-- == 0) { \
                retry--; mdelay(1); cnt = backup; \
            } \
        } \
        WARN_ON(retry == 0); \
    } while(0)

#define msdc_reset() \
    do { \
        int retry = 3, cnt = 1000; \
        sdr_set_bits(MSDC_CFG, MSDC_CFG_RST); \
        dsb(); \
        msdc_retry(sdr_read32(MSDC_CFG) & MSDC_CFG_RST, retry, cnt); \
    } while(0)

#define msdc_clr_fifo() \
    do { \
        int retry = 3, cnt = 1000; \
        sdr_set_bits(MSDC_FIFOCS, MSDC_FIFOCS_CLR); \
        msdc_retry(sdr_read32(MSDC_FIFOCS) & MSDC_FIFOCS_CLR, retry, cnt); \
    } while(0)

#define msdc_clr_int() \
    do { \
        volatile u32 val = sdr_read32(MSDC_INT); \
        sdr_write32(MSDC_INT, val); \
    } while(0)
    
static struct msdc_host g_host;
static struct msdc_host *host = &g_host;
static struct msdc_card card;

static u32 Response;  /* u32 is enough [Fix me] */
 
/* command define */
#define CMD0     (0)    /* GO_IDLE_STATE        */
#define CMD8     (8)    /* SEND_IF_COND         */
#define CMD55    (55)   /* APP_CMD              */
#define ACMD41   (41)   /* SD_SEND_OP_COND      */
#define CMD2     (2)    /* ALL_SEND_CID         */
#define CMD3     (3)    /* SEND_RELATIVE_ADDR   */
#define CMD9     (9)    /* SEND_CSD             */
#define CMD13    (13)   /* SEND_STATUS          */
#define CMD7     (7)    /* SELECT/DESELECT_CARD */
#define ACMD42   (42)   /* SET_CLR_CARD_DETECT  */
#define ACMD6    (6)    /* SET_BUS_WIDTH        */

//#define CMD16    (16) /* don't need CMD16 [Fix me] how to confirm block_len is 512 bytes */ 
#define CMD17    (17)
#define CMD18    (18)
#define CMD24    (24)
#define CMD25    (25)
#define CMD12    (12)  

/* command argument */
#define CMD0_ARG             (0)

#define CMD8_ARG_VOL_27_36   (0x100)
#define CMD8_ARG_PATTERN     (0x5a)   /* or 0xAA */
#define CMD8_ARG             (CMD8_ARG_VOL_27_36 | CMD8_ARG_PATTERN)

#define CMD55_ARG            (host->card->rca) 

#define ACMD41_ARG_HCS       (1 << 30)
#define ACMD41_ARG_VOL_27_36 (0xff8000)
#define ACMD41_ARG_20        (ACMD41_ARG_VOL_27_36 | ACMD41_ARG_HCS) 
#define ACMD41_ARG_10        (ACMD41_ARG_VOL_27_36) 

#define CMD2_ARG             (0)
#define CMD3_ARG             (0)
#define CMD9_ARG             (host->card->rca) 
#define CMD13_ARG            (host->card->rca) 
#define CMD7_ARG             (host->card->rca) 

#define ACMD42_ARG_CLR_CD    (0)
#define ACMD42_ARG_SET_CD    (1)
#define ACMD42_ARG           (ACMD42_ARG_CLR_CD) 
 
#define ACMD6_ARG_BUS_WIDTH_4 (0x2)
#define ACMD6_ARG             (ACMD6_ARG_BUS_WIDTH_4)   

#define CMD17_ARG    (data_address)  /* in bytes units in a SDSC */
#define CMD18_ARG    (data_address)  /* in block units in a SDHC (512 bytes)*/
#define CMD24_ARG    (data_address)   
#define CMD25_ARG    (data_address)   
#define CMD12_ARG    (0) 

/*  SDC_CMD =
    CMD | RESPTYP << 7 | DTYPE << 11 | RW << 13 | STOP << 14 | LEN << 16 
    BREAK << 6 |  GO_IRQ << 15 | AUTO_CMD << 28 | VOL_SWTH 
*/
#if 1
#define CMD_RAW(cmd, rspt, dtyp, rw, len, stop) \
          (cmd) | (rspt << 7) | \
          (dtyp << 11) | (rw << 13) | (len << 16) | \
          (stop << 14)  
#else
static int CMD_RAW(cmd, rspt, dtyp, rw, len, stop)
{
    return (cmd | rspt << 7 | dtyp << 11 | rw << 13 | len << 16 | stop << 14 );	
}

#endif           
/* compare the value with mt6573 [Fix me]*/          
#define CMD0_RAW     CMD_RAW(0 , msdc_rsp[RESP_NONE], 0, 0,   0, 0)
#define CMD8_RAW     CMD_RAW(8 , msdc_rsp[RESP_R7]  , 0, 0,   0, 0)  /* 0x88 -> R1 */
#define CMD55_RAW    CMD_RAW(55, msdc_rsp[RESP_R1]  , 0, 0,   0, 0)  /* R1 not R3! */
#define ACMD41_RAW   CMD_RAW(41, msdc_rsp[RESP_R3]  , 0, 0,   0, 0)
#define CMD2_RAW     CMD_RAW(2 , msdc_rsp[RESP_R2]  , 0, 0,   0, 0)
#define CMD3_RAW     CMD_RAW(3 , msdc_rsp[RESP_R6]  , 0, 0,   0, 0)
#define CMD9_RAW     CMD_RAW(9 , msdc_rsp[RESP_R2]  , 0, 0,   0, 0)
#define CMD13_RAW    CMD_RAW(13, msdc_rsp[RESP_R1]  , 0, 0,   0, 0)
#define CMD7_RAW     CMD_RAW(7 , msdc_rsp[RESP_R1]  , 0, 0,   0, 0)  /* R1 or R1b [Fix me] */
#define ACMD42_RAW   CMD_RAW(42, msdc_rsp[RESP_R1]  , 0, 0,   0, 0)
#define ACMD6_RAW    CMD_RAW(6 , msdc_rsp[RESP_R1]  , 0, 0,   0, 0)
/* block size always 512 [Fix me] */
#define CMD17_RAW    CMD_RAW(17, msdc_rsp[RESP_R1]  , 1, 0, 512, 0)  /* single   + read  +  */    
#define CMD18_RAW    CMD_RAW(18, msdc_rsp[RESP_R1]  , 2, 0, 512, 0)  /* multiple + read  +  */ 
#define CMD24_RAW    CMD_RAW(24, msdc_rsp[RESP_R1]  , 1, 1, 512, 0)  /* single   + write +  */ 
#define CMD25_RAW    CMD_RAW(25, msdc_rsp[RESP_R1]  , 2, 1, 512, 0)  /* multiple + write +  */  
#define CMD12_RAW    CMD_RAW(12, msdc_rsp[RESP_R1B] , 0, 0,   0, 1)  

/* command response */
#define R3_OCR_POWER_UP_BIT        (1 << 31)
#define R3_OCR_CARD_CAPACITY_BIT   (1 << 30)

#define PERI_MSDC_SRCSEL   (0xF100000c)
#define PDN_REG            (0xF1000010)     
static void msdc_config_clksrc(struct msdc_host *host, clk_source_t clksrc)
{
    u32 tmp;

    /* Enable PDN MSDC */ 
    sdr_set_bits(PDN_REG, 1 << (15 + host->id)); 

    /* 2 msdc clock cycles is required before gating. Use 1ms delay to avoid
     * glitch in switching clock source
     */
    mdelay(1);

    tmp = sdr_read32(PERI_MSDC_SRCSEL);
    tmp &= ~0x3; tmp |= clksrc;
    sdr_write32(PERI_MSDC_SRCSEL, tmp);
    
    host->clksrc = clksrc;
    host->clk  = clks[clksrc];

    /* Disable PDN MSDC */
    sdr_clr_bits(PDN_REG, 1 << (15 + host->id)); 
}

static void msdc_config_clock(struct msdc_host *host, u32 hz)  /* no ddr */
{
    // struct msdc_hw *hw = host->priv;
    u32 base = host->base;
    u32 mode;  /* use divisor or not */
    u32 div = 0;           
    u32 sclk;
    u32 hclk = host->clk;
    u32 orig_clksrc = host->clksrc;
  
    if (hz >= hclk) {
        mode = 0x1; /* no divisor */
        sclk = hclk; 
    } else {
        mode = 0x0; /* use divisor */
        if (hz >= (hclk >> 1)) {
        	div  = 0;         /* mean div = 1/2 */
        	sclk = hclk >> 1; /* sclk = clk / 2 */
        } else {
        	div  = (hclk + ((hz << 2) - 1)) / (hz << 2);
        	sclk = (hclk >> 2) / div;
        }
    }
    host->sclk  = sclk;

    /* set clock mode and divisor */
    msdc_config_clksrc(host, MSDC_CLKSRC_NONE);

    sdr_set_field(MSDC_CFG, MSDC_CFG_CKMOD, mode);
    sdr_set_field(MSDC_CFG, MSDC_CFG_CKDIV, div);

    msdc_config_clksrc(host, orig_clksrc);
 
    /* wait clock stable */
    while (!(sdr_read32(MSDC_CFG) & MSDC_CFG_CKSTB));
}

static void msdc_set_timeout(struct msdc_host *host, u32 ns, u32 clks)
{
    u32 base = host->base;
    u32 timeout, clk_ns;

    clk_ns  = 1000000000UL / host->sclk;
    timeout = ns / clk_ns + clks;
    timeout = timeout >> 16; /* in 65536 sclk cycle unit */
    timeout = timeout > 1 ? timeout - 1 : 0;
    timeout = timeout > 255 ? 255 : timeout;

    sdr_set_field(SDC_CFG, SDC_CFG_DTOC, timeout);
}

static void msdc_set_host_pwr(int on)
{    
/*    if (on) {
        hwPowerOn(MT65XX_POWER_LDO_VMC, VOL_3300, "SD");	
    } else {
        hwPowerDown(MT65XX_POWER_LDO_VMC, "SD");	
    }      	
*/    
}

static void msdc_set_card_pwr(int on)
{
    /* VMCH */	
    if (on) {
        //hwPowerOn(MT65XX_POWER_LDO_VMC, VOL_3300, "SD");	
    } else {
        //hwPowerDown(MT65XX_POWER_LDO_VMC, "SD");	
    } 	
	
}

#define MSDC_PIN_PULL_NONE      (0)
#define MSDC_PIN_PULL_DOWN      (1)
#define MSDC_PIN_PULL_UP        (2)
#define MSDC_PIN_KEEP           (3)


static void msdc_config_pin(struct msdc_host *host, int mode)
{
    u32 base = host->base;

    switch (mode) {
    case MSDC_PIN_PULL_UP:
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPU, 1);
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPD, 0);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPU, 1);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPD, 0);
        break;
    case MSDC_PIN_PULL_DOWN:
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPU, 0);
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPD, 1);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPU, 0);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPD, 1);
        break;
    case MSDC_PIN_PULL_NONE:
    default:
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPU, 0);
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPD, 0);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPU, 0);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPD, 0);
        break;
    }
}



static void msdc_host_power(struct msdc_host *host, int on)
{
    if (on) {
        msdc_config_pin(host, MSDC_PIN_PULL_UP);      
        msdc_set_host_pwr(1);
        // msdc_clock(host, 1);
    } else {
        // msdc_clock(host, 0);
        msdc_set_host_pwr(0);
        msdc_config_pin(host, MSDC_PIN_PULL_DOWN);
    }
}

static void msdc_card_power(struct msdc_host *host, int on)
{
    if (on) {
        msdc_set_card_pwr(1);
    } else {
        msdc_set_card_pwr(0);
    }
}

static void msdc_power(struct msdc_host *host, u8 on)
{
    if (on) {
        msdc_host_power(host, 1); // VMC
        msdc_card_power(host, 1); // VMCH_EN
    } else {
        msdc_card_power(host, 0);
        msdc_host_power(host, 0);
    }
}

#define CMD_WAIT_RETRY  (0x8FFFFFFF)
#define sdc_is_busy()          (sdr_read32(SDC_STS) & SDC_STS_SDCBUSY)
#define sdc_is_cmd_busy()      (sdr_read32(SDC_STS) & SDC_STS_CMDBUSY)
#define sdc_send_cmd(cmd,arg) \
    do { \
        sdr_write32(SDC_ARG, (arg)); \
        sdr_write32(SDC_CMD, (cmd)); \
    } while(0)
static int msdc_cmd(struct msdc_host *host, u32 cmd, u32 raw, u32 arg, int rsptyp, u32*resp)
{
    int retry = CMD_WAIT_RETRY; 
    u32 base   = host->base;	  
    u32 error = 0 ;     
    u32 intsts = 0; 
    u32 cmdsts = MSDC_INT_CMDRDY |MSDC_INT_CMDTMO |MSDC_INT_RSPCRCERR ;
        
    /* wait before send command */ 
    if (cmd == CMD13) {
        while (retry--) {
            if (!sdc_is_cmd_busy()) 
                break; 		
        }
        if (retry == 0) {
            error = 1;        	
            goto end;
        }
    } else {
        while (retry--) {
            if (!sdc_is_busy())
                break; 		
        }	
        if (retry == 0) {
            error = 1;        	
            goto end;
        }        
    }  

    sdc_send_cmd(raw, arg);

    /* polling to check the interrupt */
    retry = CMD_WAIT_RETRY; 
    while( (intsts & cmdsts) == 0) {     
        intsts = sdr_read32(MSDC_INT); 
        retry--; 
        if (retry == 0) {
            error = 1; 
            goto end; 	
        }
    }      
    intsts &= cmdsts ; 
    sdr_write32(MSDC_INT, intsts); /* clear interrupts */    

    if (intsts & MSDC_INT_CMDRDY) {
        //printk("msdc cmd<%d> Ready \r\n", cmd);
        /* get the response */
        switch (rsptyp) {
        case RESP_NONE:
            break;        	 	     
        case RESP_R2: 
            break; 
        default: /* Response types 1, 3, 4, 5, 6, 7(1b) */
            *resp = sdr_read32(SDC_RESP0);   		
        }	   	
    } else {
        error = 1;  
        goto end; 	   	
    } 

    if (rsptyp == RESP_R1B) {
        while ((sdr_read32(MSDC_PS) & MSDC_PS_DAT0) != MSDC_PS_DAT0);
    }
    
end:
    return error;               	
}

#define MSDC_FIFO_SZ            (128)
#define MSDC_FIFO_THD           (64)  // (128)
#define msdc_txfifocnt()   ((sdr_read32(MSDC_FIFOCS) & MSDC_FIFOCS_TXCNT) >> 16)
#define msdc_rxfifocnt()   ((sdr_read32(MSDC_FIFOCS) & MSDC_FIFOCS_RXCNT) >> 0)
#define msdc_fifo_write32(v)   sdr_write32(MSDC_TXDATA, (v))
#define msdc_fifo_read32()     sdr_read32(MSDC_RXDATA)
static int msdc_pio_write(u32 *ptr, u32 size)
{
    u32  base = host->base;	
    u32  left = size;    
    
    /* don't need to card 4 bytes issues */
    while (left) {
        if ((left >= MSDC_FIFO_SZ) && (msdc_txfifocnt() == 0)) {
            int count = MSDC_FIFO_SZ >> 2;
            do {
                msdc_fifo_write32(*ptr); ptr++;
            } while (--count);
            left -= MSDC_FIFO_SZ;      	
        } else if (left < MSDC_FIFO_SZ && msdc_txfifocnt() == 0) {
            while (left) {
                msdc_fifo_write32(*ptr); ptr++;
                left -= 4;
            }    
            //left = 0;      	
        }	
    }	
    
    return 0; 		
}

static int msdc_pio_read(u32 *ptr, u32 size)
{
    u32  base = host->base;	
    u32  left = size;    
    
    /* don't need to card 4 bytes issues */
    while (left) {
        if ((left >=  MSDC_FIFO_THD) && (msdc_rxfifocnt() >= MSDC_FIFO_THD)) {	
            int count = MSDC_FIFO_THD >> 2; 
            do {
                *ptr++ = msdc_fifo_read32();
            } while (--count);
            left -= MSDC_FIFO_THD;            	
        } else if ((left < MSDC_FIFO_THD) && msdc_rxfifocnt() >= left) {
            while (left) {
                *ptr++ = msdc_fifo_read32();
                left -= 4;
            }  
            //left = 0;       	
        }
    }    
    
    return 0; 	        	
}

static void msdc_set_blknum(struct msdc_host *host, u32 blknum)
{
    u32 base = host->base;
    sdr_write32(SDC_BLK_NUM, blknum);
}

int msdc_write_sectors(unsigned char* dst, u32 blk_num, u32 nblks)
{
    int status = 0;
    u32 totalLen  = BLK_LEN * nblks;   /* 512 * nblks */                     
    u32 base = host->base;   
    u32 data_address; 
    u32 intsts = 0;

    if(host->card->card_cap == standard_capacity) {
        data_address = blk_num * BLK_LEN ;	
    } else {
        data_address = blk_num; 
    }   

    /* [Fix me] retry 3 times */	

    msdc_set_blknum(host, nblks);
    // msdc_clr_fifo();  

    /* send command */
    if(nblks == 1) {
        if (msdc_cmd(host, CMD24, CMD24_RAW, data_address, RESP_R1,  &Response)) {
            ; 	
        }
    } else {
        if (msdc_cmd(host, CMD25, CMD25_RAW, data_address, RESP_R1,  &Response)) {
            ; 	
        }        	
    } 

    /* write the data to FIFO */
    msdc_pio_write((u32 *)dst, totalLen); 

    /* make sure contents in fifo flushed to device */
    while (msdc_txfifocnt());

    /* check and clear interrupt */
    while( (intsts & MSDC_INT_XFER_COMPL) == 0 ){
        intsts = sdr_read32(MSDC_INT);	                
    }
    sdr_set_bits(MSDC_INT, MSDC_INT_XFER_COMPL);	

    /* stop command */ 
    if (nblks > 1) {
        if (msdc_cmd(host, CMD12, CMD12_RAW, CMD12_ARG, RESP_R1B, &Response)) {
            ; 	
        }         	
    } 

    return !status;              
}

int msdc_read_sectors(unsigned char* dst, u32 blk_num, u32 nblks)
{
    int status = 0;
    u32 totalLen  = BLK_LEN * nblks;   /* 512 * nblks */                     
    u32 base = host->base;   
    u32 data_address; 
    u32 intsts = 0;

    if(host->card->card_cap == standard_capacity) {
        data_address = blk_num * BLK_LEN ;	
    } else {
        data_address = blk_num; 
    }   

    /* [Fix me] retry 3 times */

    // msdc_set_timeout(host, data->timeout_ns, data->timeout_clks);   
    
    msdc_set_blknum(host, nblks);
    // msdc_clr_fifo();  

    /* send command */
    if(nblks == 1) {
        if (msdc_cmd(host, CMD17, CMD17_RAW, CMD17_ARG, RESP_R1,  &Response)) {
            ; 	
        }
    } else {
        if (msdc_cmd(host, CMD18, CMD18_RAW, CMD18_ARG, RESP_R1,  &Response)) {
            ; 	
        }        	
    } 

    /* read the data out*/
    msdc_pio_read((u32 *)dst, totalLen); 	
    
    /* check and clear interrupt */
    while( (intsts & MSDC_INT_XFER_COMPL) == 0 ){
        intsts = sdr_read32(MSDC_INT);	                
    }
    sdr_set_bits(MSDC_INT, MSDC_INT_XFER_COMPL);	

    /* stop command */ 
    if (nblks > 1) {
        if (msdc_cmd(host, CMD12, CMD12_RAW, CMD12_ARG, RESP_R1B, &Response)) {
            ; 	
        }         	
    } 

    return !status;     
}

void msdc_soft_reset(void)
{
    u32 base = host->base;

    msdc_reset();
    msdc_clr_fifo();
}

#define ACMD41_RETRY   (20)
static int SD_initialize(struct msdc_host *host)
{
    u32 ACMD41_ARG; 	
    u8  retry; 
    u32 base = host->base;
    int bRet = 0;
    	
    if (msdc_cmd(host, CMD0, CMD0_RAW, CMD0_ARG, RESP_NONE, &Response)) goto EXIT;

    if (msdc_cmd(host, CMD8, CMD8_RAW, CMD8_ARG, RESP_R7,   &Response)) goto EXIT;
    if (Response == CMD8_ARG) {
        ACMD41_ARG = ACMD41_ARG_20;         	
    } else {
        ACMD41_ARG = ACMD41_ARG_10;   	
    }

    retry = ACMD41_RETRY; 
    while (retry--) {
        if (msdc_cmd(host,  CMD55,  CMD55_RAW,  CMD55_ARG,  RESP_R1, &Response)) goto EXIT;				
        if (msdc_cmd(host, ACMD41, ACMD41_RAW, ACMD41_ARG,  RESP_R3, &Response)) goto EXIT;
        if (Response & R3_OCR_POWER_UP_BIT) {
            host->card->card_cap = ((Response & R3_OCR_CARD_CAPACITY_BIT) ? high_capacity : standard_capacity);
            if(host->card->card_cap == standard_capacity){ 
                printk("just standard_capacity card!!\r\n");    
            }
            break;    		  				
        }
        mdelay(1000 / ACMD41_RETRY);        
    }

    if (msdc_cmd(host,  CMD2,    CMD2_RAW,   CMD2_ARG, RESP_R2, &Response)) goto EXIT;    

    if (msdc_cmd(host,  CMD3,    CMD3_RAW,   CMD3_ARG, RESP_R6, &Response)) goto EXIT;
    /* save the rca */	
    host->card->rca = (Response & 0xffff0000); 	/* RCA[31:16]*/

    if (msdc_cmd(host,  CMD9,    CMD9_RAW,   CMD9_ARG, RESP_R2, &Response)) goto EXIT;	
			
    if (msdc_cmd(host,  CMD13,  CMD13_RAW,  CMD13_ARG, RESP_R1, &Response)) goto EXIT;			
 
    if (msdc_cmd(host,  CMD7,    CMD7_RAW,   CMD7_ARG, RESP_R1, &Response)) goto EXIT;		    
    if (msdc_cmd(host,  CMD55,  CMD55_RAW,  CMD55_ARG, RESP_R1, &Response)) goto EXIT;	

    if (msdc_cmd(host, ACMD42, ACMD42_RAW, ACMD42_ARG, RESP_R1, &Response)) goto EXIT;	
	
    if (msdc_cmd(host,  CMD55,  CMD55_RAW,  CMD55_ARG, RESP_R1, &Response)) goto EXIT;	
	
    if (msdc_cmd(host, ACMD6,   ACMD6_RAW,  ACMD6_ARG, RESP_R1, &Response)) goto EXIT;
    /* set host bus width to 4 */    	
    sdr_set_field(SDC_CFG, SDC_CFG_BUSWIDTH, 1);  /* 1: 4 bits mode */ 	
    	
    bRet = 1;

EXIT:
    return bRet;    	
}

int msdc_init(void)
{
    u32 ret = 0;
    u32 l_tmp;
    u32 base = MSDC_REG_BASE;
    
    //struct msdc_hw *hw;    	
    /* Todo1: struct msdc_hw in board.c */

    memset(host, 0, sizeof(struct msdc_host));  
    host->id     = MSDC_ID;
    host->base   = MSDC_REG_BASE; 
    //host->priv   = (void*)hw;
    host->clksrc   = MSDC_CLKSRC; 
    host->clk      = clks[host->clksrc]; 

    memset(&card, 0, sizeof(struct msdc_card));  
    card.type = MMC_TYPE_SD;  /*now only support SD*/
    card.file_system = FAT32; 
    host->card = &card;

    /* Power off,Power On */    
    msdc_power(host, 0);
    msdc_power(host, 1);

    /* open clk */
    /* step1: open pll */
    l_tmp = 0;
    l_tmp = sdr_read16(0xF00071c0);
    l_tmp |= 0x3; //bit0-bit1 3G pll enable
    sdr_write16(0xF00071c0, l_tmp);
    l_tmp = sdr_read16(0xF00071c0);
    printk("l_tmp = 0x%x\n", l_tmp);    

   
    /* step2: enable clock */
    l_tmp = sdr_read32(0xF1000010);
    l_tmp &= (~(1<<16)); //bit16 is control msdc1's clock
    sdr_write32(0xF1000010, l_tmp);
    l_tmp = sdr_read32(0xF1000010);
    printk("l_tmp = 0x%x\n", l_tmp);

    /* set to SD/MMC mode, the first step while operation msdc */
    sdr_set_field(MSDC_CFG, MSDC_CFG_MODE, 1); // MSDC_SDMMC

    /* reset controller */
    msdc_reset(); 

    /* clear FIFO */
    msdc_clr_fifo();     

    /* Disable & Clear all interrupts */    
    msdc_clr_int();  
    sdr_write32(MSDC_INTEN, 0);

    /* reset tuning parameter */
    sdr_write32(MSDC_PAD_CTL0,   0x0090000);
    sdr_write32(MSDC_PAD_CTL1,   0x00A0000);
    sdr_write32(MSDC_PAD_CTL2,   0x00A0000);
    sdr_write32(MSDC_PAD_TUNE,   0x0000000);
    sdr_write32(MSDC_DAT_RDDLY0, 0x00000000);
    sdr_write32(MSDC_DAT_RDDLY1, 0x00000000);
    sdr_write32(MSDC_IOCON,      0x00000000);
    sdr_write32(MSDC_PATCH_BIT,  0x003C000F);
    
    /* PIO mode */    
    sdr_set_bits(MSDC_CFG, MSDC_CFG_PIO);

    /* sdio + inswkup*/
    sdr_set_bits(SDC_CFG, SDC_CFG_SDIO);
    sdr_set_bits(SDC_CFG, SDC_CFG_INSWKUP);
        
    /* eneable SMT for glitch filter */
    sdr_set_bits(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKSMT);
    sdr_set_bits(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDSMT);
    sdr_set_bits(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATSMT);

    /* set clk, cmd, dat pad driving */
    sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVN, 4); 
    sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVP, 4); 
    sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVN, 4); 
    sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVP, 4); 
    sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVN, 4); 
    sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVP, 4); 

    /* set sampling edge */
    sdr_set_field(MSDC_IOCON, MSDC_IOCON_RSPL, 0); // rising: 0
    sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, 0);

    /* write crc timeout detection */
    sdr_set_field(MSDC_PATCH_BIT, 1 << 30, 1);

    /* Clock source select*/
    msdc_config_clksrc(host, host->clksrc);
    
    /* Bus width to 1 bit*/
    sdr_set_field(SDC_CFG, SDC_CFG_BUSWIDTH, 0);

    /* make sure the clock is 260K */
    msdc_config_clock(host, MIN_SCLK);
        
    /* Set Timeout 100ms*/ 
    msdc_set_timeout(host, 100000000, 0); 
        
    sdr_clr_bits(MSDC_PS, MSDC_PS_CDEN); 
    /* detect card */
    /* need to check card is insert [Fix me] */
    
    /* check write protection [Fix me] */

    ret = SD_initialize(host);

    if (ret) {
#if 0        
        msdc_config_clock(host, NORMAL_SCLK);  /* [Fix me] why not enable High-Speed */
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_RSPL, 1);
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, 1);
#else 
        msdc_config_clock(host, NORMAL_SCLK/5);         
        printk ("freq down\n") ;
#endif        
    }
    
    return ret;                                      	
}

/* Interface with AEE_KDUMP */
EXPORT_SYMBOL(msdc_read_sectors) ;
EXPORT_SYMBOL(msdc_write_sectors) ;
EXPORT_SYMBOL(msdc_soft_reset) ;
EXPORT_SYMBOL(msdc_init) ;
