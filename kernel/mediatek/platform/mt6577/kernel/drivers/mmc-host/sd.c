#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/blkdev.h>
#include <linux/slab.h>
#include <linux/wakelock.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/core.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/sdio.h>
#include <linux/dma-mapping.h>
#include <linux/irq.h>

#include <mach/dma.h>
#include <mach/board.h>
#include <mach/mt6577_devs.h>
#include <mach/mt6577_typedefs.h>
#include <mach/mt6577_clock_manager.h>
#include <mach/mt6577_pm_ldo.h>
#include <mach/irqs.h>
//#include <mach/mt6577_pll.h>
#include <mach/mt6577_gpio.h>
//#include <mach/mt6577_gpt_sw.h>
//#include <asm/tcm.h>
//#include <mach/mt6577_gpt.h>

#include "mt6577_sd.h"
#include "dbg.h"

#include <linux/proc_fs.h>
#include "../../../../../../kernel/drivers/mmc/card/queue.h"
#include "partition_define.h"
#include "mach/mt6577_clkmgr_internal.h"
#include <mach/mt_storage_logger.h>

//Ivan #define RESUME_POLLING_MODE

#ifdef MTK_EMMC_SUPPORT
//#include "part_info_emmc.h"
//extern struct excel_info PartInfo[PART_NUM];
#endif
#include "partition_define.h"

#define EXT_CSD_BOOT_SIZE_MULT          226 /* R */
#define EXT_CSD_RPMB_SIZE_MULT          168 /* R */
#define EXT_CSD_GP1_SIZE_MULT           143 /* R/W 3 bytes */
#define EXT_CSD_GP2_SIZE_MULT           146 /* R/W 3 bytes */
#define EXT_CSD_GP3_SIZE_MULT           149 /* R/W 3 bytes */
#define EXT_CSD_GP4_SIZE_MULT           152 /* R/W 3 bytes */
#define EXT_CSD_PART_CFG                179 /* R/W/E & R/W/E_P */
#define CAPACITY_2G						(2 * 1024 * 1024 * 1024ULL)

#ifdef MTK_EMMC_SUPPORT
#define EMMC_HOST_NUM    (0)
#define MTK_EMMC_ETT_TO_DRIVER  /* for eMMC off-line apply to driver */
#endif

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);			\
		const int __shft = (start) & 31;			\
		u32 __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	}) 
	
#ifdef MTK_EMMC_ETT_TO_DRIVER
#include "emmc_device_list.h"
static  u8   m_id = 0;           // Manufacturer ID
static char pro_name[8] = {0};  // Product name
#endif 

struct mmc_blk_data {
	spinlock_t	lock;
	struct gendisk	*disk;
	struct mmc_queue queue;

	unsigned int	usage;
	unsigned int	read_only;
};

extern void mt6577_irq_mask(struct irq_data *data);
extern void mt6577_irq_unmask(struct irq_data *data);

#define DRV_NAME            "mtk-sd"


#define HOST_MAX_MCLK       (197000000) //(104000000)
#define HOST_MIN_MCLK       (260000)

#define HOST_MAX_BLKSZ      (2048)

#define MSDC_OCR_AVAIL      (MMC_VDD_28_29 | MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_32_33)
//#define MSDC_OCR_AVAIL      (MMC_VDD_32_33 | MMC_VDD_33_34)

#define GPIO_PULL_DOWN      (0)
#define GPIO_PULL_UP        (1)

#define MSDC1_IRQ_SEL       (1 << 9)
#define PDN_REG             (0xF1000010) 

#define DEFAULT_DEBOUNCE    (8)       /* 8 cycles */
#define DEFAULT_DTOC        (40)      /* data timeout counter. 65536x40 sclk. */

#define CMD_TIMEOUT         (HZ/10 * 5)   /* 100ms x5 */
#define DAT_TIMEOUT         (HZ    * 5)   /* 1000ms x5 */
#define CLK_TIMEOUT         (HZ    * 5 )  /* 5s    */ 

#define MAX_DMA_CNT         (64 * 1024 - 512)   /* a single transaction for WIFI may be 50K*/

#define MAX_HW_SGMTS        (MAX_BD_NUM)
#define MAX_PHY_SGMTS       (MAX_BD_NUM)
#define MAX_SGMT_SZ         (MAX_DMA_CNT)
#define MAX_REQ_SZ          (512*1024)  

#define CMD_TUNE_UHS_MAX_TIME 	(2*32*8*8)
#define CMD_TUNE_HS_MAX_TIME 	(2*32)

#define READ_TUNE_UHS_CLKMOD1_MAX_TIME 	(2*32*32*8)
#define READ_TUNE_UHS_MAX_TIME 	(2*32*32)
#define READ_TUNE_HS_MAX_TIME 	(2*32)

#define WRITE_TUNE_HS_MAX_TIME 	(2*32*32)
#define WRITE_TUNE_UHS_MAX_TIME (2*32*32*8)

#ifdef MT6577_SD_DEBUG
static struct msdc_regs *msdc_reg[HOST_MAX_NUM];
#endif 

//=================================

#define MSDC_LOWER_FREQ 
#define MSDC_MAX_FREQ_DIV   (2)  /* 197 / (4 * 2) */

#ifdef FPGA_PLATFORM
#define PWR_GPIO        (0xf100c000)
#define PWR_GPIO_EO     (0xf100c008)
#define PWR_MASK_VOL_33 (0x4)
#define PWR_MASK_EN     (0x1)

bool hwPowerOn_fpga(){
    //u32 addr = ioremap(PWR_GPIO, 4);
    sdr_write8(PWR_GPIO, (PWR_MASK_VOL_33 | PWR_MASK_EN));
    //sdr_write8(PWR_GPIO_EO, (PWR_MASK_VOL_33 | PWR_MASK_EN));

    printk("[%s]: done\n", __func__);
    return true; 
}

bool hwPowerDown_fpga(){
    //u32 addr = ioremap(PWR_GPIO, 4);
    sdr_write8(PWR_GPIO, 0x0);
    //sdr_write8(PWR_GPIO_EO, (PWR_MASK_VOL_33 | PWR_MASK_EN));

    printk("[%s]: done\n", __func__);
    return true; 
}
#endif
 
struct msdc_host *msdc_6577_host[] = {NULL, NULL, NULL, NULL};
static int g_intsts[] = {0, 0, 0, 0};
int g_dma_debug[HOST_MAX_NUM] = {0, 0, 0, 0};
transfer_mode msdc_latest_transfer_mode[HOST_MAX_NUM] = // 0 for PIO; 1 for DMA; 2 for nothing
{
   TRAN_MOD_NUM,
   TRAN_MOD_NUM,
   TRAN_MOD_NUM,
   TRAN_MOD_NUM
};

operation_type msdc_latest_operation_type[HOST_MAX_NUM] = // 0 for read; 1 for write; 2 for nothing
{
   OPER_TYPE_NUM,
   OPER_TYPE_NUM,
   OPER_TYPE_NUM,
   OPER_TYPE_NUM
};

struct dma_addr msdc_latest_dma_address[MAX_BD_PER_GPD];

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

/* For Inhanced DMA */
#define msdc_init_gpd_ex(gpd,extlen,cmd,arg,blknum) \
    do { \
        ((gpd_t*)gpd)->extlen = extlen; \
        ((gpd_t*)gpd)->cmd    = cmd; \
        ((gpd_t*)gpd)->arg    = arg; \
        ((gpd_t*)gpd)->blknum = blknum; \
    }while(0)
    
#define msdc_init_bd(bd, blkpad, dwpad, dptr, dlen) \
    do { \
        BUG_ON(dlen > 0xFFFFUL); \
        ((bd_t*)bd)->blkpad = blkpad; \
        ((bd_t*)bd)->dwpad  = dwpad; \
        ((bd_t*)bd)->ptr    = (void*)dptr; \
        ((bd_t*)bd)->buflen = dlen; \
    }while(0)

#define msdc_txfifocnt()   ((sdr_read32(MSDC_FIFOCS) & MSDC_FIFOCS_TXCNT) >> 16)
#define msdc_rxfifocnt()   ((sdr_read32(MSDC_FIFOCS) & MSDC_FIFOCS_RXCNT) >> 0)
#define msdc_fifo_write32(v)   sdr_write32(MSDC_TXDATA, (v))
#define msdc_fifo_write8(v)    sdr_write8(MSDC_TXDATA, (v))
#define msdc_fifo_read32()   sdr_read32(MSDC_RXDATA)
#define msdc_fifo_read8()    sdr_read8(MSDC_RXDATA)	

#define msdc_dma_on()        sdr_clr_bits(MSDC_CFG, MSDC_CFG_PIO)
#define msdc_dma_off()       sdr_set_bits(MSDC_CFG, MSDC_CFG_PIO)

static void msdc_dump_register(struct msdc_host *host)
{
    u32 base = host->base; 
    //u32 off = 0; 

    INIT_MSG("Reg[00] MSDC_CFG       = 0x%.8x", sdr_read32(base + 0x00));
    INIT_MSG("Reg[04] MSDC_IOCON     = 0x%.8x", sdr_read32(base + 0x04));
    INIT_MSG("Reg[08] MSDC_PS        = 0x%.8x", sdr_read32(base + 0x08));
    INIT_MSG("Reg[0C] MSDC_INT       = 0x%.8x", sdr_read32(base + 0x0C));
    INIT_MSG("Reg[10] MSDC_INTEN     = 0x%.8x", sdr_read32(base + 0x10));    
    INIT_MSG("Reg[14] MSDC_FIFOCS    = 0x%.8x", sdr_read32(base + 0x14));
    INIT_MSG("Reg[18] MSDC_TXDATA    = not read");                    	
    INIT_MSG("Reg[1C] MSDC_RXDATA    = not read");
    INIT_MSG("Reg[30] SDC_CFG        = 0x%.8x", sdr_read32(base + 0x30));
    INIT_MSG("Reg[34] SDC_CMD        = 0x%.8x", sdr_read32(base + 0x34));
    INIT_MSG("Reg[38] SDC_ARG        = 0x%.8x", sdr_read32(base + 0x38));
    INIT_MSG("Reg[3C] SDC_STS        = 0x%.8x", sdr_read32(base + 0x3C));
    INIT_MSG("Reg[40] SDC_RESP0      = 0x%.8x", sdr_read32(base + 0x40));
    INIT_MSG("Reg[44] SDC_RESP1      = 0x%.8x", sdr_read32(base + 0x44));
    INIT_MSG("Reg[48] SDC_RESP2      = 0x%.8x", sdr_read32(base + 0x48));                                
    INIT_MSG("Reg[4C] SDC_RESP3      = 0x%.8x", sdr_read32(base + 0x4C));
    INIT_MSG("Reg[50] SDC_BLK_NUM    = 0x%.8x", sdr_read32(base + 0x50));
    INIT_MSG("Reg[58] SDC_CSTS       = 0x%.8x", sdr_read32(base + 0x58));
    INIT_MSG("Reg[5C] SDC_CSTS_EN    = 0x%.8x", sdr_read32(base + 0x5C));
    INIT_MSG("Reg[60] SDC_DATCRC_STS = 0x%.8x", sdr_read32(base + 0x60));
    INIT_MSG("Reg[70] EMMC_CFG0      = 0x%.8x", sdr_read32(base + 0x70));                        
    INIT_MSG("Reg[74] EMMC_CFG1      = 0x%.8x", sdr_read32(base + 0x74));
    INIT_MSG("Reg[78] EMMC_STS       = 0x%.8x", sdr_read32(base + 0x78));
    INIT_MSG("Reg[7C] EMMC_IOCON     = 0x%.8x", sdr_read32(base + 0x7C));            
    INIT_MSG("Reg[80] SD_ACMD_RESP   = 0x%.8x", sdr_read32(base + 0x80));
    INIT_MSG("Reg[84] SD_ACMD19_TRG  = 0x%.8x", sdr_read32(base + 0x84));      
    INIT_MSG("Reg[88] SD_ACMD19_STS  = 0x%.8x", sdr_read32(base + 0x88));
    INIT_MSG("Reg[90] DMA_SA         = 0x%.8x", sdr_read32(base + 0x90));
    INIT_MSG("Reg[94] DMA_CA         = 0x%.8x", sdr_read32(base + 0x94));
    INIT_MSG("Reg[98] DMA_CTRL       = 0x%.8x", sdr_read32(base + 0x98));
    INIT_MSG("Reg[9C] DMA_CFG        = 0x%.8x", sdr_read32(base + 0x9C));                        
    INIT_MSG("Reg[A0] SW_DBG_SEL     = 0x%.8x", sdr_read32(base + 0xA0));
    INIT_MSG("Reg[A4] SW_DBG_OUT     = 0x%.8x", sdr_read32(base + 0xA4));
    INIT_MSG("Reg[B0] PATCH_BIT0     = 0x%.8x", sdr_read32(base + 0xB0));            
    INIT_MSG("Reg[B4] PATCH_BIT1     = 0x%.8x", sdr_read32(base + 0xB4));
    INIT_MSG("Reg[E0] SD20_PAD_CTL0  = 0x%.8x", sdr_read32(base + 0xE0));        
    INIT_MSG("Reg[E4] SD20_PAD_CTL1  = 0x%.8x", sdr_read32(base + 0xE4));
    INIT_MSG("Reg[E8] SD20_PAD_CTL2  = 0x%.8x", sdr_read32(base + 0xE8));
    INIT_MSG("Reg[EC] PAD_TUNE       = 0x%.8x", sdr_read32(base + 0xEC));
    INIT_MSG("Reg[F0] DAT_RD_DLY0    = 0x%.8x", sdr_read32(base + 0xF0));                        
    INIT_MSG("Reg[F4] DAT_RD_DLY1    = 0x%.8x", sdr_read32(base + 0xF4));
    INIT_MSG("Reg[F8] HW_DBG_SEL     = 0x%.8x", sdr_read32(base + 0xF8));
    INIT_MSG("Rg[100] MAIN_VER       = 0x%.8x", sdr_read32(base + 0x100));     
    INIT_MSG("Rg[104] ECO_VER        = 0x%.8x", sdr_read32(base + 0x104));                   
} 

u32 msdc_a0[8] = {
    1, 2, 7, 12, 13, 14, 15, 16  	
};
#if 0
static void msdc_debug_reg(struct msdc_host *host)
{
    u32 base = host->base;	
    u32 i; 

    for (i=0; i < 8; i++) {
        sdr_write32(base + 0xA0, msdc_a0[i]);    
        INIT_MSG("SW_DBG_SEL: write reg[%x] to %d", base + 0xA0, msdc_a0[i]);      
        INIT_MSG("SW_DBG_OUT: read  reg[%x] to %d", base + 0xA4, sdr_read32(base + 0xA4));  	
    }  	
    
    sdr_write32(base + 0xA0, 0);    
}
#endif

/*
 * for AHB read / write debug
 * return DMA status. 
 */
int msdc_get_dma_status(int host_id)
{
   int result = -1;

   if(host_id < 0 || host_id >= HOST_MAX_NUM)
   {
      printk("[%s] failed to get dma status, bad host_id %d\n", __func__, host_id);
      return result;
   }

   if(msdc_latest_transfer_mode[host_id] == TRAN_MOD_DMA)
   {
      switch(msdc_latest_operation_type[host_id])
      {
         case OPER_TYPE_READ:
             result = 1; // DMA read
             break;
         case OPER_TYPE_WRITE:
             result = 2; // DMA write
             break;
         default:
             break;
      }
   }else if(msdc_latest_transfer_mode[host_id] == TRAN_MOD_PIO){
      result = 0; // PIO mode
   }
  
   return result;
}
EXPORT_SYMBOL(msdc_get_dma_status);

struct dma_addr* msdc_get_dma_address(int host_id)
{
   bd_t* bd; 
   int i;
   struct msdc_host *host;

   if(host_id < 0 || host_id >= HOST_MAX_NUM)
   {
      printk("[%s] failed to get dma status, bad host_id %d\n", __func__, host_id);
      return NULL;
   }
 
   if(!msdc_6577_host[host_id]) 
   {
      printk("[%s] failed to get dma status, msdc%d is not exist\n", __func__, host_id);
      return NULL;
   }
   
   host = msdc_6577_host[host_id];
   //spin_lock(&host->lock);

   bd = host->dma.bd;
   i = 0;
   while(i < MAX_BD_PER_GPD){
       msdc_latest_dma_address[i].start_address = (u32)bd[i].ptr;
       msdc_latest_dma_address[i].size = bd[i].buflen; 
       msdc_latest_dma_address[i].end = bd[i].eol;
       if(i>0)
          msdc_latest_dma_address[i-1].next = &msdc_latest_dma_address[i];

       if(bd[i].eol){
          break;
       }
       i++;
   }

   //spin_unlock(&host->lock);

   return msdc_latest_dma_address;

}
EXPORT_SYMBOL(msdc_get_dma_address);

static void msdc_dump_clock_sts(struct msdc_host* host)
{
    INIT_MSG("3G pll status: Reg[0xF00071C0] = 0x%.8x\r\n", sdr_read32(0xF00071C0));	    
    freq_meter(0xf, 0); 
    INIT_MSG("clock select reg[%x] = 0x%.8x",  MSDC_CLKSRC_REG, sdr_read32(MSDC_CLKSRC_REG));    	
    INIT_MSG("clock gate status reg[F1000010] = 0x%.8x", sdr_read32(0xF1000010));
    INIT_MSG("clock mgr.clock_state[0] = 0x%x",clk_mgr.clock_state[0]);
}
static void msdc_dump_info(u32 id)
{
    struct msdc_host *host = msdc_6577_host[id];
    u32 base;
    u32 temp;

    //return;
        
    if(host == NULL) {
        printk("msdc host<%d> null\r\n", id);	
        return;
    }             
    base = host->base;     	

    // 1: dump msdc hw register 	
    msdc_dump_register(host);
    
    // 2: check msdc clock gate and clock source
    msdc_dump_clock_sts(host);

    // 3: check the register read_write 
    temp = sdr_read32(base + 0xB0);
    INIT_MSG("patch reg[%x] = 0x%.8x", (base + 0xB0), temp);  
      
    temp = (~temp);
    sdr_write32(base + 0xB0, temp);    
    temp = sdr_read32(base + 0xB0);
    INIT_MSG("patch reg[%x] = 0x%.8x second time", (base + 0xB0), temp);          
 
    temp = (~temp);
    sdr_write32(base + 0xB0, temp);    
    temp = sdr_read32(base + 0xB0);
    INIT_MSG("patch reg[%x] = 0x%.8x Third time", (base + 0xB0), temp);   

    // 4: For YD 
    //msdc_debug_reg(host);
        
    //sd_debug_zone[id] = 0x3ff;        
}

#define msdc_retry(expr, retry, cnt,id) \
    do { \
        int backup = cnt; \
        while (retry) { \
            if (!(expr)) break; \
            if (cnt-- == 0) { \
                retry--; mdelay(1); cnt = backup; \
            } \
        } \
        if (retry == 0) { \
            msdc_dump_info(id); \
        } \
        WARN_ON(retry == 0); \
    } while(0)

#define msdc_reset(id) \
    do { \
        int retry = 3, cnt = 1000; \
        sdr_set_bits(MSDC_CFG, MSDC_CFG_RST); \
        mb(); \
        msdc_retry(sdr_read32(MSDC_CFG) & MSDC_CFG_RST, retry, cnt,id); \
    } while(0)

#define msdc_clr_int() \
    do { \
        volatile u32 val = sdr_read32(MSDC_INT); \
        sdr_write32(MSDC_INT, val); \
    } while(0)

#define msdc_clr_fifo(id) \
    do { \
        int retry = 3, cnt = 1000; \
        sdr_set_bits(MSDC_FIFOCS, MSDC_FIFOCS_CLR); \
        msdc_retry(sdr_read32(MSDC_FIFOCS) & MSDC_FIFOCS_CLR, retry, cnt,id); \
    } while(0)

#define msdc_reset_hw(id) \
		msdc_reset(id); \
		msdc_clr_fifo(id); \
		msdc_clr_int(); 

static int msdc_clk_stable(struct msdc_host *host,u32 mode, u32 div){
		u32 base = host->base;
        int retry = 0;
		int cnt = 1000; 
		int retry_cnt = 1;
		do{
			retry = 3;
        	sdr_set_field(MSDC_CFG, MSDC_CFG_CKDIV,(div + retry_cnt) % 0xff); 
        	sdr_set_field(MSDC_CFG, MSDC_CFG_CKMOD, mode); 
        	msdc_retry(!(sdr_read32(MSDC_CFG) & MSDC_CFG_CKSTB), retry, cnt,host->id);
			if(retry == 0){
					printk(KERN_ERR "msdc%d host->onclock(%d)\n",host->id,host->core_clkon);
					printk(KERN_ERR "msdc%d on clock failed ===> retry twice\n",host->id);
					disable_clock(PERI_MSDC0_PDN + host->id, "SD");
					enable_clock(PERI_MSDC0_PDN + host->id, "SD");
					msdc_dump_info(host->id);
				}
			retry = 3;
        	sdr_set_field(MSDC_CFG, MSDC_CFG_CKDIV, div); 
        	msdc_retry(!(sdr_read32(MSDC_CFG) & MSDC_CFG_CKSTB), retry, cnt,host->id);
			if(retry == 0)
				msdc_dump_info(host->id);
        	msdc_reset_hw(host->id);
			if(retry_cnt == 2)
				break;
			retry_cnt += 1;
		}while(!retry);
		return 0;
}

#define msdc_irq_save(val) \
    do { \
        val = sdr_read32(MSDC_INTEN); \
        sdr_clr_bits(MSDC_INTEN, val); \
    } while(0)
	
#define msdc_irq_restore(val) \
    do { \
        sdr_set_bits(MSDC_INTEN, val); \
    } while(0)

/* clock source for host: global */
//static u32 hclks[] = {26000000, 197000000, 208000000, 0};
// 50M -> 26/4 = 6.25 MHz
static u32 hclks[] = {197000001, 197000000, 192000000, 0};

/* VMCH is for T-card main power.
 * VMC for T-card when no emmc, for eMMC when has emmc. 
 * VGP for T-card when has emmc.
 */
u32 g_vmc  = 0;
u32 g_vmch = 0;
u32 g_vgp  = 0; 

static u32 msdc_ldo_power(u32 on, MT65XX_POWER powerId, MT65XX_POWER_VOLTAGE powerVolt, u32 *status)
{
    if (on) { // want to power on
        if (*status == 0) {  // can power on 
            printk(KERN_ERR "msdc LDO<%d> power on<%d>\n", powerId, powerVolt); 	
#ifdef FPGA_PLATFORM
            hwPowerOn_fpga(); //powerId, powerVolt, "msdc");
#else
            hwPowerOn(powerId, powerVolt, "msdc");
#endif
            *status = powerVolt;             
        } else if (*status == powerVolt) {
            printk(KERN_ERR "LDO<%d><%d> power on again!\n", powerId, powerVolt);	
        } else { // for sd3.0 later
            printk(KERN_ERR "LDO<%d> change<%d> to <%d>\n", powerId, *status, powerVolt);
#ifdef FPGA_PLATFORM
            hwPowerDown_fpga(); //(powerId, "msdc");
            hwPowerOn_fpga(); //powerId, powerVolt, "msdc");
#else
            hwPowerDown(powerId, "msdc");
            hwPowerOn(powerId, powerVolt, "msdc");
#endif
            *status = powerVolt;	
        }
    } else {  // want to power off
        if (*status != 0) {  // has been powerred on
            printk(KERN_ERR "msdc LDO<%d> power off\n", powerId);   
#ifdef FPGA_PLATFORM     	
            hwPowerDown_fpga(); //powerId, "msdc");
#else
            hwPowerDown(powerId, "msdc");
#endif
            *status = 0;
        } else {
            printk(KERN_ERR "LDO<%d> not power on\n", powerId);	
        }            	
    }  
    
    return 0;      	
}

// maintain the Power ID internal   
static u32 msdc_sd1_power(u32 on, MT65XX_POWER_VOLTAGE powerVolt)
{     
#ifdef FPGA_PLATFORM
	   if(on)
	   	  hwPowerOn_fpga();
	   else
	   	  hwPowerDown_fpga();
#else
    msdc_ldo_power(on, MT65XX_POWER_LDO_VGP, powerVolt, &g_vgp);
#ifdef HW_LAYOUT_EVB
#ifndef MTK_EMMC_SUPPORT
    msdc_ldo_power(on, MT65XX_POWER_LDO_VMC, powerVolt, &g_vmc);
#endif
#endif 
  
    msdc_ldo_power(on, MT65XX_POWER_LDO_VMCH, powerVolt, &g_vmch);
#endif   	  
    return 0;     
}

static u32 msdc_sd1_power_switch(u32 on, MT65XX_POWER_VOLTAGE powerVolt)
{
    msdc_ldo_power(on, MT65XX_POWER_LDO_VGP, powerVolt, &g_vgp);
#ifdef HW_LAYOUT_EVB
#ifndef MTK_EMMC_SUPPORT
    msdc_ldo_power(on, MT65XX_POWER_LDO_VMC, powerVolt, &g_vmc);
#endif
#endif   
    return 0;     
}

static u32 msdc_sd0_power(u32 on, MT65XX_POWER_VOLTAGE powerVolt)
{
#ifdef FPGA_PLATFORM	
	   if(on)
	   	  hwPowerOn_fpga();
	   else
	   	  hwPowerDown_fpga();
#else
#ifdef MTK_EMMC_SUPPORT
    // msdc_ldo_power(on, MT65XX_POWER_LDO_VMC, powerVolt, &g_vmc); 
    // VMC can't power off. 
#else 
    
#endif     

#endif	   	  
    return 0;
}

#define sdc_is_busy()          (sdr_read32(SDC_STS) & SDC_STS_SDCBUSY)
#define sdc_is_cmd_busy()      (sdr_read32(SDC_STS) & SDC_STS_CMDBUSY)

#define sdc_send_cmd(cmd,arg) \
    do { \
        sdr_write32(SDC_ARG, (arg)); \
        mb(); \
        sdr_write32(SDC_CMD, (cmd)); \
    } while(0)

// can modify to read h/w register.
//#define is_card_present(h)   ((sdr_read32(MSDC_PS) & MSDC_PS_CDSTS) ? 0 : 1);
#define is_card_present(h)     (((struct msdc_host*)(h))->card_inserted)
//Ivan
//#define is_card_sdio(h)        (((struct msdc_host*)(h))->hw->register_pm)     
#define is_card_sdio(h)        (((struct msdc_host*)(h))->id == 3)

static unsigned int msdc_do_command(struct msdc_host   *host, 
                                      struct mmc_command *cmd,
                                      int                 tune,
                                      unsigned long       timeout);  
                                     
static int msdc_tune_cmdrsp(struct msdc_host *host);
static int msdc_get_card_status(struct mmc_host *mmc, struct msdc_host *host, u32 *status);
static void msdc_clksrc_onoff(struct msdc_host *host, u32 on);

//host doesn't need the clock on
static void msdc_gate_clock(struct msdc_host* host, int delay) 
{
    unsigned long flags; 

    spin_lock_irqsave(&host->clk_gate_lock, flags);
    if(host->clk_gate_count > 0)
       host->clk_gate_count--; 
    if(delay) 
    {
       mod_timer(&host->timer, jiffies + CLK_TIMEOUT);                    
       N_MSG(CLK, "[%s]: msdc%d, clk_gate_count=%d, delay=%d\n", __func__, host->id, host->clk_gate_count, delay);
    }
    else if(host->clk_gate_count == 0)
    {
       del_timer(&host->timer);
       msdc_clksrc_onoff(host, 0);
       N_MSG(CLK, "[%s]: msdc%d, successfully gate clock, clk_gate_count=%d, delay=%d\n", __func__, host->id, host->clk_gate_count, delay);
    }
    else 
    {
       ERR_MSG("[%s]: msdc%d, failed to gate clock, the clock is still needed by host, clk_gate_count=%d, delay=%d\n", __func__, host->id, host->clk_gate_count, delay);
    }
    spin_unlock_irqrestore(&host->clk_gate_lock, flags); 
}

static void msdc_suspend_clock(struct msdc_host* host) 
{
    unsigned long flags; 

    spin_lock_irqsave(&host->clk_gate_lock, flags);
    if(host->clk_gate_count == 0)
    {
       del_timer(&host->timer);
       msdc_clksrc_onoff(host, 0);
       N_MSG(CLK, "[%s]: msdc%d, successfully gate clock, clk_gate_count=%d\n", __func__, host->id, host->clk_gate_count);
    }
    else 
    {
       ERR_MSG("[%s]: msdc%d, the clock is still needed by host, clk_gate_count=%d\n", __func__, host->id, host->clk_gate_count);
    }
    spin_unlock_irqrestore(&host->clk_gate_lock, flags); 
}

//host does need the clock on
static void msdc_ungate_clock(struct msdc_host* host) 
{ 
    unsigned long flags;
    spin_lock_irqsave(&host->clk_gate_lock, flags);
    host->clk_gate_count++; 
    N_MSG(CLK, "[%s]: msdc%d, clk_gate_count=%d\n", __func__, host->id, host->clk_gate_count);
    if(host->clk_gate_count == 1)
       msdc_clksrc_onoff(host, 1);        
    spin_unlock_irqrestore(&host->clk_gate_lock, flags); 
}  

// do we need sync object or not 
void msdc_clk_status(int * status)
{
    int g_clk_gate = 0;
    int i; 
    unsigned long flags; 

    for(i=0; i<HOST_MAX_NUM; i++)
    {
       if(!msdc_6577_host[i])
         continue;
         
       spin_lock_irqsave(&msdc_6577_host[i]->clk_gate_lock, flags);
       if(msdc_6577_host[i]->clk_gate_count > 0)
         g_clk_gate |= 1 << ((i) + PERI_MSDC0_PDN);
       spin_unlock_irqrestore(&msdc_6577_host[i]->clk_gate_lock, flags);
    }
    *status = g_clk_gate;    	
}

#if 0
static void msdc_dump_card_status(struct msdc_host *host, u32 status)
{
    static char *state[] = {
        "Idle",			/* 0 */
        "Ready",		/* 1 */
        "Ident",		/* 2 */
        "Stby",			/* 3 */
        "Tran",			/* 4 */
        "Data",			/* 5 */
        "Rcv",			/* 6 */
        "Prg",			/* 7 */
        "Dis",			/* 8 */
        "Reserved",		/* 9 */
        "Reserved",		/* 10 */
        "Reserved",		/* 11 */
        "Reserved",		/* 12 */
        "Reserved",		/* 13 */
        "Reserved",		/* 14 */
        "I/O mode",		/* 15 */
    };
    if (status & R1_OUT_OF_RANGE)
        N_MSG(RSP, "[CARD_STATUS] Out of Range");
    if (status & R1_ADDRESS_ERROR)
        N_MSG(RSP, "[CARD_STATUS] Address Error");
    if (status & R1_BLOCK_LEN_ERROR)
        N_MSG(RSP, "[CARD_STATUS] Block Len Error");
    if (status & R1_ERASE_SEQ_ERROR)
        N_MSG(RSP, "[CARD_STATUS] Erase Seq Error");
    if (status & R1_ERASE_PARAM)
        N_MSG(RSP, "[CARD_STATUS] Erase Param");
    if (status & R1_WP_VIOLATION)
        N_MSG(RSP, "[CARD_STATUS] WP Violation");
    if (status & R1_CARD_IS_LOCKED)
        N_MSG(RSP, "[CARD_STATUS] Card is Locked");
    if (status & R1_LOCK_UNLOCK_FAILED)
        N_MSG(RSP, "[CARD_STATUS] Lock/Unlock Failed");
    if (status & R1_COM_CRC_ERROR)
        N_MSG(RSP, "[CARD_STATUS] Command CRC Error");
    if (status & R1_ILLEGAL_COMMAND)
        N_MSG(RSP, "[CARD_STATUS] Illegal Command");
    if (status & R1_CARD_ECC_FAILED)
        N_MSG(RSP, "[CARD_STATUS] Card ECC Failed");
    if (status & R1_CC_ERROR)
        N_MSG(RSP, "[CARD_STATUS] CC Error");
    if (status & R1_ERROR)
        N_MSG(RSP, "[CARD_STATUS] Error");
    if (status & R1_UNDERRUN)
        N_MSG(RSP, "[CARD_STATUS] Underrun");
    if (status & R1_OVERRUN)
        N_MSG(RSP, "[CARD_STATUS] Overrun");
    if (status & R1_CID_CSD_OVERWRITE)
        N_MSG(RSP, "[CARD_STATUS] CID/CSD Overwrite");
    if (status & R1_WP_ERASE_SKIP)
        N_MSG(RSP, "[CARD_STATUS] WP Eraser Skip");
    if (status & R1_CARD_ECC_DISABLED)
        N_MSG(RSP, "[CARD_STATUS] Card ECC Disabled");
    if (status & R1_ERASE_RESET)
        N_MSG(RSP, "[CARD_STATUS] Erase Reset");
    if ((status & R1_READY_FOR_DATA) == 0)
        N_MSG(RSP, "[CARD_STATUS] Not Ready for Data");
    if (status & R1_SWITCH_ERROR)
        N_MSG(RSP, "[CARD_STATUS] Switch error");
    if (status & R1_APP_CMD)
        N_MSG(RSP, "[CARD_STATUS] App Command");
    
    N_MSG(RSP, "[CARD_STATUS] '%s' State", state[R1_CURRENT_STATE(status)]);
} 
#endif

static void msdc_set_timeout(struct msdc_host *host, u32 ns, u32 clks)
{
    u32 base = host->base;
    u32 timeout, clk_ns;

    host->timeout_ns   = ns;
    host->timeout_clks = clks;

    clk_ns  = 1000000000UL / host->sclk;
    timeout = ns / clk_ns + clks;
    timeout = timeout >> 16; /* in 65536 sclk cycle unit */
    timeout = timeout > 1 ? timeout - 1 : 0;
    timeout = timeout > 255 ? 255 : timeout;

    sdr_set_field(SDC_CFG, SDC_CFG_DTOC, timeout);

    N_MSG(OPS, "Set read data timeout: %dns %dclks -> %d x 65536 cycles",
        ns, clks, timeout + 1);
}

/* msdc_eirq_sdio() will be called when EIRQ(for WIFI) */
static void msdc_eirq_sdio(void *data)
{
    struct msdc_host *host = (struct msdc_host *)data;

    N_MSG(INT, "SDIO EINT");
    mmc_signal_sdio_irq(host->mmc);
}

/* msdc_eirq_cd will not be used!  We not using EINT for card detection. */
static void msdc_eirq_cd(void *data)
{
    struct msdc_host *host = (struct msdc_host *)data;

    N_MSG(INT, "CD EINT");

    tasklet_hi_schedule(&host->card_tasklet);
}

/* detect cd interrupt */
static void msdc_tasklet_card(unsigned long arg)
{
    struct msdc_host *host = (struct msdc_host *)arg;
    struct msdc_hw *hw = host->hw;
    //unsigned long flags;
    u32 base = host->base;
    u32 inserted;	
    u32 status = 0;

    //spin_lock_irqsave(&host->lock, flags);
    msdc_ungate_clock(host);
    
    if (hw->get_cd_status) { // NULL
        inserted = hw->get_cd_status();
    } else {
        status = sdr_read32(MSDC_PS);
        inserted = (status & MSDC_PS_CDSTS) ? 0 : 1;
    }
    IRQ_MSG("card found<%s>", inserted ? "inserted" : "removed");  

    host->card_inserted = inserted;    
    host->mmc->f_max = HOST_MAX_MCLK; 
    host->hw->cmd_edge  = 0; // new card tuning from 0
    host->hw->data_edge = 0;

    // [Fix me] if card remove during a request
    msdc_gate_clock(host, 1); 
    //spin_unlock_irqrestore(&host->lock, flags);

    if (!host->suspend) { 
        mmc_detect_change(host->mmc, msecs_to_jiffies(200)); 
    }
}

/* For E2 */
static u8 clk_src_bit[4] = {
   0, 3, 5, 7    	
};
static void msdc_select_clksrc(struct msdc_host* host, int clksrc)
{
    u32 val; 
    u32 base = host->base;
        
    BUG_ON(clksrc > 3);	
    INIT_MSG("set clock source to <%d>", clksrc);    	

    val = sdr_read32(MSDC_CLKSRC_REG);      
    if (sdr_read32(MSDC_ECO_VER) >= 4) {
        val &= ~(0x3  << clk_src_bit[host->id]); 
        val |= clksrc << clk_src_bit[host->id];                   	
    } else {        
        val &= ~0x3; val |= clksrc;
    }    
    sdr_write32(MSDC_CLKSRC_REG, val);
            
    host->hclk = hclks[clksrc];     
    host->hw->clk_src = clksrc;
}

static void msdc_set_mclk(struct msdc_host *host, int ddr, u32 hz)
{
    //struct msdc_hw *hw = host->hw;
    u32 base = host->base;
    u32 mode;
    u32 flags;
    u32 div;
    u32 sclk;
    u32 hclk = host->hclk;
    //u8  clksrc = hw->clk_src;

    if (!hz) { // set mmc system clock to 0 
        ERR_MSG("set mclk to 0");  // fix me: need to set to 0
        host->mclk = 0;        
        msdc_reset_hw(host->id);       
        return;
    }

    msdc_irq_save(flags);
    
    if (ddr) { /* may need to modify later */
        mode = 0x2; /* ddr mode and use divisor */
        if (hz >= (hclk >> 2)) {
        	div  = 0;         /* mean div = 1/4 */
        	sclk = hclk >> 2; /* sclk = clk / 4 */
        } else {
        	div  = (hclk + ((hz << 2) - 1)) / (hz << 2);
        	sclk = (hclk >> 2) / div;
			div  = (div >> 1); 
        }
    } else if (hz >= hclk) {
        mode = 0x1; /* no divisor */
        div  = 0;
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

    msdc_clk_stable(host,mode, div);
    
    host->sclk = sclk;
    host->mclk = hz;

#if 0
    if (host->sclk > 100000000) {
        sdr_clr_bits(MSDC_PATCH_BIT0, CKGEN_RX_SDClKO_SEL);	
    } else {
        sdr_set_bits(MSDC_PATCH_BIT0, CKGEN_RX_SDClKO_SEL);	     	
    }     
#endif

    msdc_set_timeout(host, host->timeout_ns, host->timeout_clks); // need because clk changed.

    INIT_MSG("================");  
    INIT_MSG("!!! Set<%dKHz> Source<%dKHz> -> sclk<%dKHz> DDR<%d> mode<%d> div<%d>" , 
		                  hz/1000, hclk/1000, sclk/1000, ddr, mode, div); 
    INIT_MSG("================");

    msdc_irq_restore(flags);
}
extern int mmc_sd_power_cycle(struct mmc_host *host, u32 ocr, struct mmc_card *card);

/* 0 means pass */
static u32 msdc_power_tuning(struct msdc_host *host)
{
    struct mmc_host *mmc = host->mmc;
    struct mmc_card *card;
	struct mmc_request *mrq;
	u32 power_cycle = 0;
	u32 read_timeout_tune = 0;
	u32 write_timeout_tune = 0;
	u32 sw_timeout = 0;
    u32 ret = 1;
    
    if (!mmc) return 1;            

    card = mmc->card;
    if (card == NULL) {
        ERR_MSG("mmc->card is NULL");
        return 1;        
    }

    // eMMC first 
    #ifdef MTK_EMMC_SUPPORT
    if (mmc_card_mmc(card) && (host->id == 0)) { 
        /* Fixme: */
        return 1;        
    }
    #endif

    if (mmc_card_sd(card) && (host->id == 1) && (host->power_cycle < 3)) {
        // power cycle 
        ERR_MSG("Power cycle start");
        spin_unlock(&host->lock);
        msdc_sd1_power(0, VOL_DEFAULT);
        msleep(10);
        msdc_sd1_power(1, VOL_3300);
        spin_lock(&host->lock);

        // clock should set to 260K 
        mmc->ios.clock = HOST_MIN_MCLK;  
        mmc->ios.bus_width = MMC_BUS_WIDTH_1;
        mmc->ios.timing = MMC_TIMING_LEGACY;          
        msdc_set_mclk(host, 0, HOST_MIN_MCLK);                 

        //zone_temp = sd_debug_zone[1]; 
        //sd_debug_zone[1] |= (DBG_EVT_NRW | DBG_EVT_RW);
        
        // re-init the card! 
        mrq = host->mrq;
		host->mrq = NULL;
		power_cycle = host->power_cycle;
		host->power_cycle = 3; 
		read_timeout_tune = host->read_time_tune;
		write_timeout_tune = host->write_time_tune;
		sw_timeout = host->sw_timeout;
        spin_unlock(&host->lock);
        ret = mmc_sd_power_cycle(mmc, mmc->ocr, card);
        spin_lock(&host->lock);
		host->mrq = mrq;
		host->power_cycle = power_cycle;
		host->read_time_tune = read_timeout_tune;
		host->write_time_tune = write_timeout_tune;
		host->sw_timeout = sw_timeout;
		(host->power_cycle)++;
		ERR_MSG("Power cycle Done");   
    }

    return ret;
}


void msdc_send_stop(struct msdc_host *host)
{
    struct mmc_command stop = {0};    
    struct mmc_request mrq = {0};
    u32 err = -1; 
  
    stop.opcode = MMC_STOP_TRANSMISSION;    
    stop.arg = 0;  
    stop.flags = MMC_RSP_R1B | MMC_CMD_AC;

    mrq.cmd = &stop; stop.mrq = &mrq;
    stop.data = NULL;        
     
    err = msdc_do_command(host, &stop, 0, CMD_TIMEOUT);
}

typedef enum{
	cmd_counter = 0,
	read_counter,
	write_counter,
	all_counter,
}TUNE_COUNTER;
static void msdc_reset_tune_counter(struct msdc_host *host,TUNE_COUNTER index)
{
	if(index >= 0 && index <= all_counter){
	switch (index){
		case cmd_counter   : if(host->t_counter.time_cmd != 0)
									ERR_MSG("TUNE CMD Times(%d)", host->t_counter.time_cmd);
							 host->t_counter.time_cmd = 0;break;
		case read_counter  : if(host->t_counter.time_read != 0)
									ERR_MSG("TUNE READ Times(%d)", host->t_counter.time_read);
							 host->t_counter.time_read = 0;break;
		case write_counter : if(host->t_counter.time_write != 0)
									ERR_MSG("TUNE WRITE Times(%d)", host->t_counter.time_write);
							 host->t_counter.time_write = 0;break;
		case all_counter   : if(host->t_counter.time_cmd != 0)
									ERR_MSG("TUNE CMD Times(%d)", host->t_counter.time_cmd);
							 if(host->t_counter.time_read != 0)
									ERR_MSG("TUNE READ Times(%d)", host->t_counter.time_read);
							 if(host->t_counter.time_write != 0)
									ERR_MSG("TUNE WRITE Times(%d)", host->t_counter.time_write);
							 host->t_counter.time_cmd = 0;
						     host->t_counter.time_read = 0;
						     host->t_counter.time_write = 0;
						     break;
		default : break;
		}
	}
	else{
		 ERR_MSG("msdc%d ==> reset counter index(%d) error!\n",host->id,index);
		}
}

#ifdef MTK_SD_REINIT_SUPPORT
extern void mmc_remove_card(struct mmc_card *card);
extern void mmc_detach_bus(struct mmc_host *host);
extern void mmc_power_off(struct mmc_host *host);

int msdc_reinit(struct msdc_host *host)
{
    struct mmc_host *mmc = host->mmc;
    struct mmc_card *card;
	//struct mmc_request *mrq; 
    int ret = -1;
	u32 err = 0;
	u32 status = 0;
	unsigned long tmo = 12;
	//u32 state = 0;
    if(!host){
		ERR_MSG("msdc_host is NULL");
		return -1;
		}
    if (!mmc) {
		ERR_MSG("mmc is NULL");
		return -1;
		}

    card = mmc->card;
    if (card == NULL) 
        ERR_MSG("mmc->card is NULL");
      

    // eMMC first 
    #ifdef MTK_EMMC_SUPPORT
    if (host->id == 0) { 
        /* Fixme: */
        return -1;        
    }
    #endif

    if (host->id == SD_HOST_ID) {
        // power cycle 
        ERR_MSG("SD card Re-Init!");
        mmc_claim_host(host->mmc);
		ERR_MSG("SD card Re-Init get host!");
		spin_lock(&host->lock);
		ERR_MSG("SD card Re-Init get lock!");
		msdc_clksrc_onoff(host, 1); 
		if(host->app_cmd_arg){
			while((err = msdc_get_card_status(mmc, host, &status))) {
					ERR_MSG("SD card Re-Init in get card status!err(%d)",err);
					if(err == (unsigned int)-EIO){
            			if (msdc_tune_cmdrsp(host)) {
                			ERR_MSG("update cmd para failed");	
                			break;
            			}
					}
					else 
						break;
				}					
        if(err == 0){
				msdc_clksrc_onoff(host, 0); 
				spin_unlock(&host->lock);  
				mmc_release_host(host->mmc);
				ERR_MSG("SD Card is ready.");
				return 0;
			}
		}
		msdc_clksrc_onoff(host, 0);
		//spin_unlock(&host->lock);  
        ERR_MSG("Reinit start..");
        //msdc_sd1_power(0, VOL_DEFAULT);
        //mdelay(10);
        //msdc_sd1_power(1, VOL_3300);
 
        // clock should set to 260K 
        //spin_lock(&host->lock); 
        mmc->ios.clock = HOST_MIN_MCLK;  
        mmc->ios.bus_width = MMC_BUS_WIDTH_1;
        mmc->ios.timing = MMC_TIMING_LEGACY;
		host->card_inserted = 1;
		msdc_clksrc_onoff(host, 1); 
        msdc_set_mclk(host, 0, HOST_MIN_MCLK);
		msdc_clksrc_onoff(host, 0);
		spin_unlock(&host->lock);
		mmc_release_host(host->mmc);
		if(host->mmc->card){
			mmc_remove_card(host->mmc->card);
			host->mmc->card = NULL;
			mmc_claim_host(host->mmc);
			mmc_detach_bus(host->mmc);
			mmc_release_host(host->mmc);
		}
		mmc_power_off(host->mmc);
		mmc_detect_change(host->mmc,0);
		while(tmo){
				if(host->mmc->card && mmc_card_present(host->mmc->card)){
					ret = 0;
					break;
				}
				msleep(50);
				tmo--;
			}
		ERR_MSG("Reinit %s",ret == 0 ? "success" : "fail");
		
    }
    return ret;
}
#endif
/* Fix me. when need to abort */
static u32 msdc_abort_data(struct msdc_host *host)
{
    struct mmc_host *mmc = host->mmc;     
    u32 base = host->base;    
    u32 status = 0;    
    u32 state = 0;
    u32 err = 0; 	
    unsigned long tmo = jiffies + DAT_TIMEOUT;
          
    while (state != 4) { // until status to "tran"
        msdc_reset_hw(host->id);
        while ((err = msdc_get_card_status(mmc, host, &status))) {
            if (err != (unsigned int)-EIO) {
                return msdc_power_tuning(host); /* if failed, will set no-card flag */                
            } else if (msdc_tune_cmdrsp(host)) {        
                ERR_MSG("update cmd para failed");	
                return 1;
            }   
        } 

        state = R1_CURRENT_STATE(status);
        ERR_MSG("check card state<%d>", state);
        if (state == 5 || state == 6) {
            ERR_MSG("state<%d> need cmd12 to stop", state);	
            msdc_send_stop(host); // don't tuning
        } else if (state == 7) {  // busy in programing        	
            ERR_MSG("state<%d> card is busy", state);	
            spin_unlock(&host->lock);             
            msleep(100);
            spin_lock(&host->lock);
        } else if (state != 4) {
            ERR_MSG("state<%d> ??? ", state);
            return 1;     	
        }
        
        if (time_after(jiffies, tmo)) {
            ERR_MSG("abort timeout. Do power cycle");
			return msdc_power_tuning(host);
        }
    }
    
    msdc_reset_hw(host->id); 
    return 0;   
}

static void msdc_pin_config(struct msdc_host *host, int mode)
{
    struct msdc_hw *hw = host->hw;
    u32 base = host->base;
    int pull = (mode == MSDC_PIN_PULL_UP) ? GPIO_PULL_UP : GPIO_PULL_DOWN;

    /* Config WP pin */
    if (hw->flags & MSDC_WP_PIN_EN) {
        if (hw->config_gpio_pin) /* NULL */
            hw->config_gpio_pin(MSDC_WP_PIN, pull);
    }

    switch (mode) {
    case MSDC_PIN_PULL_UP:
        //sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKPU, 1); /* Check & FIXME */
        //sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKPD, 0); /* Check & FIXME */
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPU, 1);
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPD, 0);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPU, 1);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPD, 0);
        break;
    case MSDC_PIN_PULL_DOWN:
        //sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKPU, 0); /* Check & FIXME */
        //sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKPD, 1); /* Check & FIXME */
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPU, 0);
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPD, 1);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPU, 0);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPD, 1);
        break;
    case MSDC_PIN_PULL_NONE:
    default:
        //sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKPU, 0); /* Check & FIXME */
        //sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKPD, 0); /* Check & FIXME */
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPU, 0);
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDPD, 0);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPU, 0);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATPD, 0);
        break;
    }
    
    N_MSG(CFG, "Pins mode(%d), down(%d), up(%d)", 
        mode, MSDC_PIN_PULL_DOWN, MSDC_PIN_PULL_UP);
}

void msdc_pin_reset(struct msdc_host *host, int mode)
{
    struct msdc_hw *hw = (struct msdc_hw *)host->hw;
    u32 base = host->base;
    int pull = (mode == MSDC_PIN_PULL_UP) ? GPIO_PULL_UP : GPIO_PULL_DOWN;

    /* Config reset pin */
    if (hw->flags & MSDC_RST_PIN_EN) {
        if (hw->config_gpio_pin) /* NULL */
            hw->config_gpio_pin(MSDC_RST_PIN, pull);

        if (mode == MSDC_PIN_PULL_UP) {
            sdr_clr_bits(EMMC_IOCON, EMMC_IOCON_BOOTRST);
        } else {
            sdr_set_bits(EMMC_IOCON, EMMC_IOCON_BOOTRST);
        }
    }
}

static void msdc_set_power_mode(struct msdc_host *host, u8 mode)
{
    N_MSG(CFG, "Set power mode(%d)", mode);

    if (host->power_mode == MMC_POWER_OFF && mode != MMC_POWER_OFF) {
        msdc_pin_reset (host, MSDC_PIN_PULL_UP);
        msdc_pin_config(host, MSDC_PIN_PULL_UP);
		if (host->id == 0) {
            msdc_sd0_power(1, VOL_3300);                    	
        } else if (host->id == 1) {
            msdc_sd1_power(1, VOL_3300);	
        }	
#if 0
        if (host->hw->ext_power_on) {
            host->hw->ext_power_on();
        }
#endif        
        mdelay(1);
    } else if (host->power_mode != MMC_POWER_OFF && mode == MMC_POWER_OFF) {
#if 0
        if (host->hw->ext_power_off) {
            host->hw->ext_power_off();
        }
#endif        
        if (is_card_sdio(host)) {
            msdc_pin_config(host, MSDC_PIN_PULL_UP);
        }else {
		if (host->id == 0) {
            msdc_sd0_power(0, VOL_DEFAULT);                    	
        } else if (host->id == 1) {
            msdc_sd1_power(0, VOL_DEFAULT);	
        }	
            msdc_pin_config(host, MSDC_PIN_PULL_DOWN);
        }
        mdelay(1);
        msdc_pin_reset (host, MSDC_PIN_PULL_DOWN);
    }
    host->power_mode = mode;
}

#ifdef MTK_EMMC_ETT_TO_DRIVER
static int msdc_ett_offline_to_driver(struct msdc_host *host)
{
    int ret = 1;  // 1 means failed
    int size = sizeof(g_mmcTable) / sizeof(mmcdev_info);
    int i, temp; 
    u32 base = host->base;
    
    //printk(KERN_ERR "msdc_ett_offline_to_driver size<%d> \n", size);

    for (i = 0; i < size; i++) {
        //printk(KERN_ERR"msdc <%d> <%s> <%s>\n", i, g_mmcTable[i].pro_name, pro_name); 
                    	
        if ((g_mmcTable[i].m_id == m_id) && (!strncmp(g_mmcTable[i].pro_name, pro_name, 6))) {
            printk(KERN_ERR "msdc ett index<%d>: <%d> <%d> <0x%x> <0x%x> <0x%x>\n", i, 
                g_mmcTable[i].r_smpl, g_mmcTable[i].d_smpl, 
                g_mmcTable[i].cmd_rxdly, g_mmcTable[i].rd_rxdly, g_mmcTable[i].wr_rxdly);  

            // set to msdc0 
            sdr_set_field(MSDC_IOCON, MSDC_IOCON_RSPL, g_mmcTable[i].r_smpl); 
            sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, g_mmcTable[i].d_smpl);
          
            sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, g_mmcTable[i].cmd_rxdly);
            sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATRRDLY, g_mmcTable[i].rd_rxdly); 
            sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, g_mmcTable[i].wr_rxdly); 

            temp = g_mmcTable[i].rd_rxdly; temp &= 0x1F;             
            sdr_write32(MSDC_DAT_RDDLY0, (temp<<0 | temp<<8 | temp<<16 | temp<<24)); 
            sdr_write32(MSDC_DAT_RDDLY1, (temp<<0 | temp<<8 | temp<<16 | temp<<24));
                                 
            ret = 0;
            break;
        }
    }
    
    //if (ret) printk(KERN_ERR "msdc failed to find\n");
    return ret;        	
}
#endif


extern int mmc_card_sleepawake(struct mmc_host *host, int sleep);
//extern int mmc_send_status(struct mmc_card *card, u32 *status);  
extern int mmc_go_idle(struct mmc_host *host);
extern int mmc_send_op_cond(struct mmc_host *host, u32 ocr, u32 *rocr);
extern int mmc_all_send_cid(struct mmc_host *host, u32 *cid);
//extern int mmc_attach_mmc(struct mmc_host *host, u32 ocr);

typedef enum MMC_STATE_TAG
{
    MMC_IDLE_STATE,
    MMC_READ_STATE,
    MMC_IDENT_STATE,
    MMC_STBY_STATE,
    MMC_TRAN_STATE,
    MMC_DATA_STATE,
    MMC_RCV_STATE,
    MMC_PRG_STATE,
    MMC_DIS_STATE,
    MMC_BTST_STATE,
    MMC_SLP_STATE,
    MMC_RESERVED1_STATE,
    MMC_RESERVED2_STATE,
    MMC_RESERVED3_STATE,
    MMC_RESERVED4_STATE,
    MMC_RESERVED5_STATE,
}MMC_STATE_T;

typedef enum EMMC_CHIP_TAG{
    SAMSUNG_EMMC_CHIP = 0x15,
    SANDISK_EMMC_CHIP = 0x45,
    HYNIX_EMMC_CHIP   = 0x90,
} EMMC_VENDOR_T;

#ifdef MTK_EMMC_SUPPORT
unsigned int sg_emmc_sleep = 0;
static void msdc_config_emmc_pad(int padEmmc)
{
    static int sg_gpio164_mode;
    static int sg_gpio165_mode;
    static int sg_gpio166_mode;
    static int sg_gpio168_mode;
    static int sg_gpio169_mode;
    static int sg_gpio170_mode;
    static int sg_gpio171_mode;
    static int sg_gpio173_mode;
    static int sg_gpio175_mode;
    
    if (padEmmc == 0){
        sg_gpio164_mode = mt_get_gpio_mode(GPIO164);
        sg_gpio165_mode = mt_get_gpio_mode(GPIO165);
        sg_gpio166_mode = mt_get_gpio_mode(GPIO166);
        sg_gpio168_mode = mt_get_gpio_mode(GPIO168);
        sg_gpio169_mode = mt_get_gpio_mode(GPIO169);
        sg_gpio170_mode = mt_get_gpio_mode(GPIO170);
        sg_gpio171_mode = mt_get_gpio_mode(GPIO171);
        sg_gpio173_mode = mt_get_gpio_mode(GPIO173);
        sg_gpio175_mode = mt_get_gpio_mode(GPIO175);

        mt_set_gpio_mode(GPIO164, GPIO_MODE_GPIO);
        mt_set_gpio_mode(GPIO165, GPIO_MODE_GPIO);
        mt_set_gpio_mode(GPIO166, GPIO_MODE_GPIO);
        mt_set_gpio_mode(GPIO168, GPIO_MODE_GPIO);
        mt_set_gpio_mode(GPIO169, GPIO_MODE_GPIO);
        mt_set_gpio_mode(GPIO170, GPIO_MODE_GPIO);
        mt_set_gpio_mode(GPIO171, GPIO_MODE_GPIO);
        mt_set_gpio_mode(GPIO173, GPIO_MODE_GPIO);
        mt_set_gpio_mode(GPIO175, GPIO_MODE_GPIO);

        mt_set_gpio_dir(GPIO164, GPIO_DIR_IN);
        mt_set_gpio_dir(GPIO165, GPIO_DIR_IN);
        mt_set_gpio_dir(GPIO166, GPIO_DIR_IN);
        mt_set_gpio_dir(GPIO168, GPIO_DIR_IN);
        mt_set_gpio_dir(GPIO169, GPIO_DIR_IN);
        mt_set_gpio_dir(GPIO170, GPIO_DIR_IN);
        mt_set_gpio_dir(GPIO171, GPIO_DIR_IN);
        mt_set_gpio_dir(GPIO173, GPIO_DIR_IN);
        mt_set_gpio_dir(GPIO175, GPIO_DIR_IN);

        mt_set_gpio_pull_enable(GPIO164, GPIO_PULL_DISABLE);
        mt_set_gpio_pull_enable(GPIO165, GPIO_PULL_DISABLE);
        mt_set_gpio_pull_enable(GPIO166, GPIO_PULL_DISABLE);
        mt_set_gpio_pull_enable(GPIO168, GPIO_PULL_DISABLE);
        mt_set_gpio_pull_enable(GPIO169, GPIO_PULL_DISABLE);
        mt_set_gpio_pull_enable(GPIO170, GPIO_PULL_DISABLE);
        mt_set_gpio_pull_enable(GPIO171, GPIO_PULL_DISABLE);
        mt_set_gpio_pull_enable(GPIO173, GPIO_PULL_DISABLE);
        mt_set_gpio_pull_enable(GPIO175, GPIO_PULL_DISABLE);
    } else { 
        mt_set_gpio_mode(GPIO164, sg_gpio164_mode);
        mt_set_gpio_mode(GPIO165, sg_gpio165_mode);
        mt_set_gpio_mode(GPIO166, sg_gpio166_mode);
        mt_set_gpio_mode(GPIO168, sg_gpio168_mode);
        mt_set_gpio_mode(GPIO169, sg_gpio169_mode);
        mt_set_gpio_mode(GPIO170, sg_gpio170_mode);
        mt_set_gpio_mode(GPIO171, sg_gpio171_mode);
        mt_set_gpio_mode(GPIO173, sg_gpio173_mode);
        mt_set_gpio_mode(GPIO175, sg_gpio175_mode);
    }
}

static void msdc_sleep_enter(struct msdc_host *host)
{
    //u32 l_status = MMC_IDLE_STATE;
    //unsigned long tmo;
    struct mmc_host *mmc;
    struct mmc_card *card;

    BUG_ON(!host->mmc);
    BUG_ON(!host->mmc->card);
	
    mmc = host->mmc;
    card = host->mmc->card;

    /* check card type */
    if (MMC_TYPE_MMC != card->type) {
        printk(KERN_WARNING"[EMMC] not a mmc card, pls check it before sleep\n");
        return ;
    }

    mmc_claim_host(mmc);
    mmc_go_idle(mmc); // Infinity: Ask eMMC into open-drain mode
    
    // add for hynix emcp chip 
    if (host->mmc->card->cid.manfid == HYNIX_EMMC_CHIP){
        {
            u32 l_ocr = mmc->ocr;
            u32 l_cid[4];
            u32 l_rocr;
            u32 l_ret;

            // clk freq down, 26kHz for emmc card init
            msdc_set_mclk(host, 0, 400000);

            //send CMD1, will loop for card's busy state  
            l_ret = mmc_send_op_cond(mmc, l_ocr | (1 << 30), &l_rocr);
            if (l_ret != 0){
                ERR_MSG("send cmd1 error while emmc card enter low power state\n");   	/* won't happen. */
            }

            //send CMD2
            l_cid[0] = 0;
            l_cid[1] = 0;
            l_cid[2] = 0;
            l_cid[3] = 0;
            l_ret = mmc_all_send_cid(mmc, l_cid);
            if (l_ret != 0){
                ERR_MSG("send cmd2 error while emmc card enter low power state\n");   	/* won't happen. */
            }
        }
    }

    msdc_config_emmc_pad(0);

    mmc_release_host(mmc);
}

static void msdc_sleep_out(struct msdc_host *host)
{
    struct mmc_host *mmc;
    struct mmc_card *card;

	BUG_ON(!host->mmc);
	BUG_ON(!host->mmc->card);
	
    mmc = host->mmc;
    card = host->mmc->card;
 
    
    /* check card type */
    if (MMC_TYPE_MMC != card->type) {
        printk(KERN_WARNING"[EMMC] not a mmc card, pls check it before sleep\n");
        return ;
    }

    mmc_claim_host(mmc);
    msdc_config_emmc_pad(1);
    mmc_release_host(mmc);
}

static void msdc_emmc_sleepawake(struct msdc_host *host, u32 awake)
{
    /* slot0 for emmc, sd/sdio do not support sleep state */
    if (host->id != EMMC_HOST_ID) {
        return;
    }

    /* for emmc card need to go sleep state, while suspend.
     * because we need emmc power always on to guarantee brom can 
     * boot from emmc */
    if ((awake == 1) && (sg_emmc_sleep == 1)) {
        msdc_sleep_out(host);
        sg_emmc_sleep = 0;
    } else if((awake == 0) && (sg_emmc_sleep == 0)) {
        msdc_sleep_enter(host);
        sg_emmc_sleep = 1;
    }      	
}
#endif

static void msdc_clksrc_onoff(struct msdc_host *host, u32 on)
{
    u32 base = host->base; 
    u32 div, mode;     	
    if (on) {
        if (0 == host->core_clkon) {
#ifndef FPGA_PLATFORM
            if(enable_clock(PERI_MSDC0_PDN + host->id, "SD")){
					printk(KERN_ERR "msdc%d on clock failed ===> retry once\n",host->id);
                disable_clock(PERI_MSDC0_PDN + host->id, "SD");
                enable_clock(PERI_MSDC0_PDN + host->id, "SD");
            }				
#endif
            host->core_clkon = 1;      
            udelay(10);
            if ((SD_HOST_ID == host->id) && sdr_read32(MSDC_ECO_VER) >= 4) {              	
                sdr_set_field(MSDC_CFG, MSDC_CFG_MODE, MSDC_SDMMC);  /* E2 */
                
                //INIT_MSG("set to normal level, intr<0x%x>", sdr_read32(MSDC_INT));
                sdr_set_field(MSDC_CLKSRC_REG, MSDC1_IRQ_SEL, 0);
                mt65xx_irq_set_sens(host->irq, MT65xx_LEVEL_SENSITIVE);
            }
                       
            sdr_get_field(MSDC_CFG, MSDC_CFG_CKMOD, mode);                
            sdr_get_field(MSDC_CFG, MSDC_CFG_CKDIV, div); 
            msdc_clk_stable(host,mode, div);  

            if (is_card_sdio(host)) { 
//Ivan		
                printk("Ivan SD3 clock on!\n");

//                mdelay(1000);  // wait for WIFI stable.                                     	 
            }
                          
            //INIT_MSG("3G pll = 0x%x when clk<on>", sdr_read32(0xF00071C0));
#ifndef FPGA_PLATFORM 
            freq_meter(0xf, 0); 
#endif              
        }    	
    } else {
        if (1 == host->core_clkon) {	
            if ((SD_HOST_ID == host->id) && (sdr_read32(MSDC_ECO_VER) >= 4)) {           	 
                sdr_set_field(MSDC_CFG, MSDC_CFG_MODE, MSDC_MS);  /* E2 */
                
                sdr_set_field(MSDC_CLKSRC_REG, MSDC1_IRQ_SEL, 1);
                mt65xx_irq_set_sens(host->irq, MT65xx_EDGE_SENSITIVE);
                //INIT_MSG("set to 32K edge, intr_en<0x%x>", sdr_read32(MSDC_INTEN));                        	   
            }
#ifndef FPGA_PLATFORM             	
            disable_clock(PERI_MSDC0_PDN + host->id, "SD");  
#endif           
            host->core_clkon = 0;   
                     
            //INIT_MSG("3G pll = 0x%x when clk<off>", sdr_read32(0xF00071C0));
#ifndef FPGA_PLATFORM 
            freq_meter(0xf, 0);      
#endif   
            if (is_card_sdio(host))
                printk("Ivan SD3 clock off!\n");
	    
        }
    }      	
}


/*
   register as callback function of WIFI(combo_sdio_register_pm) .    
   can called by msdc_drv_suspend/resume too. 
*/
static void msdc_pm(pm_message_t state, void *data)
{
    struct msdc_host *host = (struct msdc_host *)data;
    int evt = state.event;

    // Un-gate clock first when resume. 
    #if 0
    if ((evt == PM_EVENT_RESUME) || (evt == PM_EVENT_USER_RESUME)) {
        if (host->id == EMMC_HOST_ID) {
            msdc_sd0_power(1, VOL_3300);                    	
        } else if (host->id == SD_HOST_ID) {
            msdc_sd1_power(1, VOL_3300);	
        }	      	
    }
	#endif
    msdc_ungate_clock(host);
//Ivan added
    printk("Ivan MTK msdc_pm SD = %d \n",host->id);
    
//Ivan	if (host->id == 3)
//Ivan	    host->mmc->rescan_disable = 1;

    if (evt == PM_EVENT_SUSPEND || evt == PM_EVENT_USER_SUSPEND) {
        if (host->suspend) /* already suspend */  /* default 0*/
            goto end;

        /* for memory card. already power off by mmc */
        if (evt == PM_EVENT_SUSPEND && host->power_mode == MMC_POWER_OFF)  
            goto end;

        host->suspend = 1;
        host->pm_state = state;  /* default PMSG_RESUME */
        
        INIT_MSG("%s Suspend", evt == PM_EVENT_SUSPEND ? "PM" : "USR");                  	
        if(host->hw->flags & MSDC_SYS_SUSPEND){ /* set for card */
#ifdef MTK_EMMC_SUPPORT
            msdc_emmc_sleepawake(host, 0);
#endif
	    printk("Ivan MTK msdc_pm1 SD = %d \n",host->id);
	    
            (void)mmc_suspend_host(host->mmc);
	    printk("Ivan MTK msdc_pm2 SD = %d \n",host->id);
	    
        } else { 
            host->mmc->pm_flags |= MMC_PM_IGNORE_PM_NOTIFY;  /* just for double confirm */       
            mmc_remove_host(host->mmc);
        }
    } else if (evt == PM_EVENT_RESUME || evt == PM_EVENT_USER_RESUME) {
        if (!host->suspend){
            //ERR_MSG("warning: already resume");   	
            goto end;
        }

        /* No PM resume when USR suspend */
        if (evt == PM_EVENT_RESUME && host->pm_state.event == PM_EVENT_USER_SUSPEND) {
            ERR_MSG("PM Resume when in USR Suspend");   	/* won't happen. */
            goto end;
        }
        
        host->suspend = 0;
        host->pm_state = state;
        
        INIT_MSG("%s Resume", evt == PM_EVENT_RESUME ? "PM" : "USR");                
        if(host->hw->flags & MSDC_SYS_SUSPEND) { /* will not set for WIFI */
#ifdef RESUME_POLLING_MODE
            host->polling = 1;
#endif

#ifdef MTK_EMMC_SUPPORT
            msdc_emmc_sleepawake(host, 1);
#endif
            (void)mmc_resume_host(host->mmc);

#ifdef RESUME_POLLING_MODE
            host->polling = 0;
#endif
        } else { 
            host->mmc->pm_flags |= MMC_PM_IGNORE_PM_NOTIFY;         
            mmc_add_host(host->mmc);
        }
    }

end:
        	    
    // gate clock at the last step when suspend.
    if ((evt == PM_EVENT_SUSPEND) || (evt == PM_EVENT_USER_SUSPEND)) {
        msdc_gate_clock(host, 0);
        
    #if 0    
        if (host->id == EMMC_HOST_ID) {
            msdc_sd0_power(0, VOL_DEFAULT);                    	
        } else if (host->id == SD_HOST_ID) {
            msdc_sd1_power(0, VOL_DEFAULT);
        }	 
		#endif
    }else {
        msdc_gate_clock(host, 1);
    }           
          
}

/*--------------------------------------------------------------------------*/
/* mmc_host_ops members                                                      */
/*--------------------------------------------------------------------------*/
static u32 wints_cmd = MSDC_INT_CMDRDY  | MSDC_INT_RSPCRCERR  | MSDC_INT_CMDTMO  |  
                       MSDC_INT_ACMDRDY | MSDC_INT_ACMDCRCERR | MSDC_INT_ACMDTMO | 
                       MSDC_INT_ACMD19_DONE;  
static unsigned int msdc_command_start(struct msdc_host   *host, 
                                      struct mmc_command *cmd,
                                      int                 tune,   /* not used */
                                      unsigned long       timeout)
{
    u32 base = host->base;
    u32 opcode = cmd->opcode;
    u32 rawcmd;
    u32 resp;  
    unsigned long tmo;

    /* Protocol layer does not provide response type, but our hardware needs 
     * to know exact type, not just size!
     */
    if (opcode == MMC_SEND_OP_COND || opcode == SD_APP_OP_COND)
        resp = RESP_R3;
    else if (opcode == MMC_SET_RELATIVE_ADDR || opcode == SD_SEND_RELATIVE_ADDR)
        resp = (mmc_cmd_type(cmd) == MMC_CMD_BCR) ? RESP_R6 : RESP_R1;
    else if (opcode == MMC_FAST_IO)
        resp = RESP_R4;
    else if (opcode == MMC_GO_IRQ_STATE)
        resp = RESP_R5;
    else if (opcode == MMC_SELECT_CARD) {
        resp = (cmd->arg != 0) ? RESP_R1B : RESP_NONE;
        host->app_cmd_arg = cmd->arg;
        INIT_MSG("select card<0x%.8x>", cmd->arg);  // select and de-select                 	
    } else if (opcode == SD_IO_RW_DIRECT || opcode == SD_IO_RW_EXTENDED)
        resp = RESP_R1; /* SDIO workaround. */
    else if (opcode == SD_SEND_IF_COND && (mmc_cmd_type(cmd) == MMC_CMD_BCR))
        resp = RESP_R1;
    else {
        switch (mmc_resp_type(cmd)) {
        case MMC_RSP_R1:
            resp = RESP_R1;
            break;
        case MMC_RSP_R1B:
            resp = RESP_R1B;
            break;
        case MMC_RSP_R2:
            resp = RESP_R2;
            break;
        case MMC_RSP_R3:
            resp = RESP_R3;
            break;
        case MMC_RSP_NONE:
        default:
            resp = RESP_NONE;              
            break;
        }
    }

    cmd->error = 0;
    /* rawcmd :
     * vol_swt << 30 | auto_cmd << 28 | blklen << 16 | go_irq << 15 | 
     * stop << 14 | rw << 13 | dtype << 11 | rsptyp << 7 | brk << 6 | opcode
     */    
    rawcmd = opcode | msdc_rsp[resp] << 7 | host->blksz << 16;
    
    if (opcode == MMC_READ_MULTIPLE_BLOCK) {
        rawcmd |= (2 << 11);
		if (host->autocmd & MSDC_AUTOCMD12)
            rawcmd |= (1 << 28);
    } else if (opcode == MMC_READ_SINGLE_BLOCK) {
        rawcmd |= (1 << 11);
    } else if (opcode == MMC_WRITE_MULTIPLE_BLOCK) {
        rawcmd |= ((2 << 11) | (1 << 13));
		if (host->autocmd & MSDC_AUTOCMD12)
            rawcmd |= (1 << 28);
    } else if (opcode == MMC_WRITE_BLOCK) {
        rawcmd |= ((1 << 11) | (1 << 13));
    } else if (opcode == SD_IO_RW_EXTENDED) {
        if (cmd->data->flags & MMC_DATA_WRITE)
            rawcmd |= (1 << 13);
        if (cmd->data->blocks > 1)
            rawcmd |= (2 << 11);
        else
            rawcmd |= (1 << 11);
    } else if (opcode == SD_IO_RW_DIRECT && cmd->flags == (unsigned int)-1) {
        rawcmd |= (1 << 14);
    } else if (opcode == SD_SWITCH_VOLTAGE) {
        rawcmd |= (1 << 30);
    } else if ((opcode == SD_APP_SEND_SCR) || 
        (opcode == SD_APP_SEND_NUM_WR_BLKS) ||
        (opcode == SD_SWITCH && (mmc_cmd_type(cmd) == MMC_CMD_ADTC)) ||
        (opcode == SD_APP_SD_STATUS && (mmc_cmd_type(cmd) == MMC_CMD_ADTC)) ||
        (opcode == MMC_SEND_EXT_CSD && (mmc_cmd_type(cmd) == MMC_CMD_ADTC))) {
        rawcmd |= (1 << 11);
    } else if (opcode == MMC_STOP_TRANSMISSION) {
        rawcmd |= (1 << 14);
        rawcmd &= ~(0x0FFF << 16);
    }

    N_MSG(CMD, "CMD<%d><0x%.8x> Arg<0x%.8x>", opcode , rawcmd, cmd->arg);

    tmo = jiffies + timeout;

    if (opcode == MMC_SEND_STATUS) {
        for (;;) {
            if (!sdc_is_cmd_busy())
                break;
                
            if (time_after(jiffies, tmo)) {
                ERR_MSG("XXX cmd_busy timeout: before CMD<%d>", opcode);	
                cmd->error = (unsigned int)-ETIMEDOUT;
                msdc_reset_hw(host->id);
                return cmd->error;  /* Fix me: error handling */
            } 
        }
    }else {
        for (;;) {	 
            if (!sdc_is_busy())
                break;
            if (time_after(jiffies, tmo)) {
                ERR_MSG("XXX sdc_busy timeout: before CMD<%d>", opcode);	
                cmd->error = (unsigned int)-ETIMEDOUT;
                msdc_reset_hw(host->id);
                return cmd->error;    
            }   
        }    
    }   
    
    //BUG_ON(in_interrupt());
    host->cmd     = cmd;
    host->cmd_rsp = resp;		
    
#ifdef RESUME_POLLING_MODE
    if(host->polling)
       g_intsts[host->id] = 0;
    else
       init_completion(&host->cmd_done);     
#else
    init_completion(&host->cmd_done);     
#endif

    sdr_set_bits(MSDC_INTEN, wints_cmd);          
    sdc_send_cmd(rawcmd, cmd->arg);        
      
//end:    	
    return 0;  // irq too fast, then cmd->error has value, and don't call msdc_command_resp, don't tune. 
}

#ifdef RESUME_POLLING_MODE
static u32 polling_interrupt(struct msdc_host *host, unsigned long timeout)
{
   unsigned long tmo;
   //u32 base = host->base;

   tmo = jiffies + timeout; //CMD_TIMEOUT;
   for(;;){
      mdelay(1);
      if(g_intsts[host->id] != 0){
          break; 
      }
      if(time_after(jiffies, tmo)){
          printk(KERN_ERR "[SD%d] %s: cmd[0x%x] polling timeout\n", host->id, __func__, host->cmd->arg);
          break;
      }
   }

   return g_intsts[host->id];

}
#endif

static unsigned int msdc_command_resp(struct msdc_host   *host, 
                                      struct mmc_command *cmd,
                                      int                 tune,
                                      unsigned long       timeout)
{
    u32 base = host->base;
    u32 opcode = cmd->opcode;
    //u32 resp = host->cmd_rsp;    
    u32 tmo;
    u32 intsts;
  
#ifdef RESUME_POLLING_MODE
    if (host->polling) {
       tmo = (!host->data)? CMD_TIMEOUT : DAT_TIMEOUT;
       intsts = polling_interrupt(host, tmo);//intsts = sdr_read32(MSDC_INT);
       if (!intsts) { //intsts & cmdsts){
          ERR_MSG("XXX CMD<%d> polling timeout ARG<0x%.8x>", opcode, cmd->arg);
          msdc_dump_info(host->id);          
          cmd->error = (unsigned int)-ETIMEDOUT;
          msdc_reset_hw(host->id);
       }
    }
    else{ 
       spin_unlock(&host->lock);   
       if(!wait_for_completion_timeout(&host->cmd_done, 10*timeout)){ 
          ERR_MSG("XXX CMD<%d> wait_for_completion timeout ARG<0x%.8x>", opcode, cmd->arg);                
		  host->sw_timeout++;
          msdc_dump_info(host->id);          
          cmd->error = (unsigned int)-ETIMEDOUT;
          msdc_reset_hw(host->id);
       }    
       spin_lock(&host->lock);
    }
#else
    spin_unlock(&host->lock);   
    if(!wait_for_completion_timeout(&host->cmd_done, 10*timeout)){       
        ERR_MSG("XXX CMD<%d> wait_for_completion timeout ARG<0x%.8x>", opcode, cmd->arg);
		host->sw_timeout++;
        msdc_dump_info(host->id);          
        cmd->error = (unsigned int)-ETIMEDOUT;
        msdc_reset_hw(host->id);
    }    
    spin_lock(&host->lock);
#endif

    sdr_clr_bits(MSDC_INTEN, wints_cmd);
    host->cmd = NULL;
    /* if (resp == RESP_R1B) {
        while ((sdr_read32(MSDC_PS) & 0x10000) != 0x10000);       
    } */ 
                    	
    return cmd->error;
}                                   

static unsigned int msdc_do_command(struct msdc_host   *host, 
                                      struct mmc_command *cmd,
                                      int                 tune,
                                      unsigned long       timeout)
{
    if (msdc_command_start(host, cmd, tune, timeout)) 
        goto end;      

    if (msdc_command_resp(host, cmd, tune, timeout)) 
        goto end;          
           	    
end:	

    N_MSG(CMD, "        return<%d> resp<0x%.8x>", cmd->error, cmd->resp[0]); 	
    return cmd->error;
}
    
/* The abort condition when PIO read/write 
   tmo: 
*/
static int msdc_pio_abort(struct msdc_host *host, struct mmc_data *data, unsigned long tmo)
{
    int  ret = 0; 	
    u32  base = host->base;
    
    if (atomic_read(&host->abort)) {	
        ret = 1;
    }    

    if (time_after(jiffies, tmo)) {
        data->error = (unsigned int)-ETIMEDOUT;
        ERR_MSG("XXX PIO Data Timeout: CMD<%d>", host->mrq->cmd->opcode);
        msdc_dump_info(host->id);          
        ret = 1;		
    }

    if(ret) {
        msdc_reset_hw(host->id);     	
        ERR_MSG("msdc pio find abort");      
    }
    return ret; 
}

/*
   Need to add a timeout, or WDT timeout, system reboot.      
*/
// pio mode data read/write
static int msdc_pio_read(struct msdc_host *host, struct mmc_data *data)
{
    struct scatterlist *sg = data->sg;
    u32  base = host->base;
    u32  num = data->sg_len;
    u32 *ptr;
    u8  *u8ptr;
    u32  left;
    u32  count, size = 0;
    u32  wints = MSDC_INTEN_DATTMO | MSDC_INTEN_DATCRCERR ;     
    unsigned long tmo = jiffies + DAT_TIMEOUT;  
          
    sdr_set_bits(MSDC_INTEN, wints);
    while (num) {
        left = sg_dma_len(sg);
        ptr = sg_virt(sg);
        while (left) {
            if ((left >=  MSDC_FIFO_THD) && (msdc_rxfifocnt() >= MSDC_FIFO_THD)) {
                count = MSDC_FIFO_THD >> 2;
                do {
                    *ptr++ = msdc_fifo_read32();
                } while (--count);
                left -= MSDC_FIFO_THD;
            } else if ((left < MSDC_FIFO_THD) && msdc_rxfifocnt() >= left) {
                while (left > 3) {
                    *ptr++ = msdc_fifo_read32();
                    left -= 4;
                }
                 
                u8ptr = (u8 *)ptr; 
                while(left) {
                    * u8ptr++ = msdc_fifo_read8();
                    left--; 	  
                }
            }
            
            if (msdc_pio_abort(host, data, tmo)) {
                goto end; 	
            }
        }
        size += sg_dma_len(sg);
        sg = sg_next(sg); num--;
    }
end:
    data->bytes_xfered += size;
    N_MSG(FIO, "        PIO Read<%d>bytes", size);
        
    sdr_clr_bits(MSDC_INTEN, wints);    
    if(data->error) ERR_MSG("read pio data->error<%d> left<%d> size<%d>", data->error, left, size);
    return data->error;
}

/* please make sure won't using PIO when size >= 512 
   which means, memory card block read/write won't using pio
   then don't need to handle the CMD12 when data error. 
*/
static int msdc_pio_write(struct msdc_host* host, struct mmc_data *data)
{
    u32  base = host->base;
    struct scatterlist *sg = data->sg;
    u32  num = data->sg_len;
    u32 *ptr;
    u8  *u8ptr;
    u32  left;
    u32  count, size = 0;
    u32  wints = MSDC_INTEN_DATTMO | MSDC_INTEN_DATCRCERR ;      
    unsigned long tmo = jiffies + DAT_TIMEOUT;  
    
    sdr_set_bits(MSDC_INTEN, wints);    
    while (num) {
        left = sg_dma_len(sg);
        ptr = sg_virt(sg);

        while (left) {
            if (left >= MSDC_FIFO_SZ && msdc_txfifocnt() == 0) {
                count = MSDC_FIFO_SZ >> 2;
                do {
                    msdc_fifo_write32(*ptr); ptr++;
                } while (--count);
                left -= MSDC_FIFO_SZ;
            } else if (left < MSDC_FIFO_SZ && msdc_txfifocnt() == 0) {
                while (left > 3) {
                    msdc_fifo_write32(*ptr); ptr++;
                    left -= 4;
                } 
                
                u8ptr = (u8*)ptr; 
                while(left){
                    msdc_fifo_write8(*u8ptr);	u8ptr++;
                    left--;
                }
            }
            
            if (msdc_pio_abort(host, data, tmo)) {
                goto end; 	
            }                   
        }
        size += sg_dma_len(sg);
        sg = sg_next(sg); num--;
    }
end:    
    data->bytes_xfered += size;
    N_MSG(FIO, "        PIO Write<%d>bytes", size);
    if(data->error) ERR_MSG("write pio data->error<%d>", data->error);
    	
    sdr_clr_bits(MSDC_INTEN, wints);  
    return data->error;	
}

#if 0
// DMA resume / start / stop 
static void msdc_dma_resume(struct msdc_host *host)
{
    u32 base = host->base;

    sdr_set_field(MSDC_DMA_CTRL, MSDC_DMA_CTRL_RESUME, 1);

    N_MSG(DMA, "DMA resume");
}
#endif

static void msdc_dma_start(struct msdc_host *host)
{
    u32 base = host->base;
    u32 wints = MSDC_INTEN_XFER_COMPL | MSDC_INTEN_DATTMO | MSDC_INTEN_DATCRCERR ; 
           
    sdr_set_bits(MSDC_INTEN, wints);
    mb();
    sdr_set_field(MSDC_DMA_CTRL, MSDC_DMA_CTRL_START, 1);

    N_MSG(DMA, "DMA start");
}

static void msdc_dma_stop(struct msdc_host *host)
{
    u32 base = host->base;
    //u32 retries=500;
    u32 wints = MSDC_INTEN_XFER_COMPL | MSDC_INTEN_DATTMO | MSDC_INTEN_DATCRCERR ; 
    
    N_MSG(DMA, "DMA status: 0x%.8x",sdr_read32(MSDC_DMA_CFG));
    //while (sdr_read32(MSDC_DMA_CFG) & MSDC_DMA_CFG_STS);

    sdr_set_field(MSDC_DMA_CTRL, MSDC_DMA_CTRL_STOP, 1);
    while (sdr_read32(MSDC_DMA_CFG) & MSDC_DMA_CFG_STS);

    mb();
    sdr_clr_bits(MSDC_INTEN, wints); /* Not just xfer_comp */

    N_MSG(DMA, "DMA stop");
}

#if 0
/* dump a gpd list */
static void msdc_dma_dump(struct msdc_host *host, struct msdc_dma *dma)
{
    gpd_t *gpd = dma->gpd; 
    bd_t   *bd = dma->bd; 	 	
    bd_t   *ptr; 
    int i = 0; 
    int p_to_v; 
    
    if (dma->mode != MSDC_MODE_DMA_DESC) {
        return; 	
    }    

    ERR_MSG("try to dump gpd and bd");

    /* dump gpd */
    ERR_MSG(".gpd<0x%.8x> gpd_phy<0x%.8x>", (int)gpd, (int)dma->gpd_addr);
    ERR_MSG("...hwo   <%d>", gpd->hwo );
    ERR_MSG("...bdp   <%d>", gpd->bdp );
    ERR_MSG("...chksum<0x%.8x>", gpd->chksum );
    //ERR_MSG("...intr  <0x%.8x>", gpd->intr );
    ERR_MSG("...next  <0x%.8x>", (int)gpd->next );
    ERR_MSG("...ptr   <0x%.8x>", (int)gpd->ptr );
    ERR_MSG("...buflen<0x%.8x>", gpd->buflen );
    //ERR_MSG("...extlen<0x%.8x>", gpd->extlen );
    //ERR_MSG("...arg   <0x%.8x>", gpd->arg );
    //ERR_MSG("...blknum<0x%.8x>", gpd->blknum );    
    //ERR_MSG("...cmd   <0x%.8x>", gpd->cmd );      

    /* dump bd */
    ERR_MSG(".bd<0x%.8x> bd_phy<0x%.8x> gpd_ptr<0x%.8x>", (int)bd, (int)dma->bd_addr, (int)gpd->ptr);  
    ptr = bd; 
    p_to_v = ((u32)bd - (u32)dma->bd_addr);
    while (1) {
        ERR_MSG(".bd[%d]", i); i++;          	
        ERR_MSG("...eol   <%d>", ptr->eol );
        ERR_MSG("...chksum<0x%.8x>", ptr->chksum );
        //ERR_MSG("...blkpad<0x%.8x>", ptr->blkpad );
        //ERR_MSG("...dwpad <0x%.8x>", ptr->dwpad );
        ERR_MSG("...next  <0x%.8x>", (int)ptr->next );
        ERR_MSG("...ptr   <0x%.8x>", (int)ptr->ptr );
        ERR_MSG("...buflen<0x%.8x>", (int)ptr->buflen );
        
        if (ptr->eol == 1) {
            break; 	
        }
        	             
        /* find the next bd, virtual address of ptr->next */
        /* don't need to enable when use malloc */
        //BUG_ON( (ptr->next + p_to_v)!=(ptr+1) );     	
        //ERR_MSG(".next bd<0x%.8x><0x%.8x>", (ptr->next + p_to_v), (ptr+1));
        ptr++;               
    }    
    
    ERR_MSG("dump gpd and bd finished");
}
#endif

/* calc checksum */
static u8 msdc_dma_calcs(u8 *buf, u32 len)
{
    u32 i, sum = 0;
    for (i = 0; i < len; i++) {
        sum += buf[i];
    }
    return 0xFF - (u8)sum;
}

/* gpd bd setup + dma registers */
static int msdc_dma_config(struct msdc_host *host, struct msdc_dma *dma)
{
    u32 base = host->base;
    u32 sglen = dma->sglen;
    //u32 i, j, num, bdlen, arg, xfersz;
    u32 j, num, bdlen;
    u32 dma_address, dma_len;
    u8  blkpad, dwpad, chksum;
    struct scatterlist *sg = dma->sg;
    gpd_t *gpd;
    bd_t *bd;
    
    switch (dma->mode) {
    case MSDC_MODE_DMA_BASIC:
        BUG_ON(dma->xfersz > 65535);
        BUG_ON(dma->sglen != 1);
        dma_address = sg_dma_address(sg);
        dma_len = sg_dma_len(sg);
        sdr_write32(MSDC_DMA_SA, dma_address);

        sdr_set_field(MSDC_DMA_CTRL, MSDC_DMA_CTRL_LASTBUF, 1);
        sdr_set_field(MSDC_DMA_CTRL, MSDC_DMA_CTRL_XFERSZ, dma_len);
        sdr_set_field(MSDC_DMA_CTRL, MSDC_DMA_CTRL_BRUSTSZ, dma->burstsz);
        sdr_set_field(MSDC_DMA_CTRL, MSDC_DMA_CTRL_MODE, 0);
        break;
    case MSDC_MODE_DMA_DESC:
        blkpad = (dma->flags & DMA_FLAG_PAD_BLOCK) ? 1 : 0;
        dwpad  = (dma->flags & DMA_FLAG_PAD_DWORD) ? 1 : 0;
        chksum = (dma->flags & DMA_FLAG_EN_CHKSUM) ? 1 : 0;

        /* calculate the required number of gpd */
        num = (sglen + MAX_BD_PER_GPD - 1) / MAX_BD_PER_GPD;        
        BUG_ON(num !=1 );        
        
        gpd = dma->gpd; 
        bd  = dma->bd; 
        bdlen = sglen; 

        /* modify gpd*/
        //gpd->intr = 0; 
        gpd->hwo = 1;  /* hw will clear it */
        gpd->bdp = 1;     
        gpd->chksum = 0;  /* need to clear first. */   
        gpd->chksum = (chksum ? msdc_dma_calcs((u8 *)gpd, 16) : 0);
        
        /* modify bd*/          
        for (j = 0; j < bdlen; j++) {
#ifdef MSDC_DMA_VIOLATION_DEBUG
            if(g_dma_debug[host->id] && (msdc_latest_operation_type[host->id] == OPER_TYPE_READ)){
               printk("[%s] msdc%d do write 0x10000\n", __func__, host->id);
               dma_address = 0x10000;
            }else{
               dma_address = sg_dma_address(sg);
            }
#else
            dma_address = sg_dma_address(sg);
#endif
            dma_len = sg_dma_len(sg);
            msdc_init_bd(&bd[j], blkpad, dwpad, dma_address, dma_len);            

            if(j == bdlen - 1) {
            bd[j].eol = 1;     	/* the last bd */
            } else {
                bd[j].eol = 0; 	
            }
            bd[j].chksum = 0; /* checksume need to clear first */
            bd[j].chksum = (chksum ? msdc_dma_calcs((u8 *)(&bd[j]), 16) : 0);         
            sg++;
        }
#ifdef MSDC_DMA_VIOLATION_DEBUG
        if(g_dma_debug[host->id] && (msdc_latest_operation_type[host->id] == OPER_TYPE_READ))
             g_dma_debug[host->id] = 0;
#endif

        dma->used_gpd += 2;
        dma->used_bd += bdlen;  

        sdr_set_field(MSDC_DMA_CFG, MSDC_DMA_CFG_DECSEN, chksum);
        sdr_set_field(MSDC_DMA_CTRL, MSDC_DMA_CTRL_BRUSTSZ, dma->burstsz);
        sdr_set_field(MSDC_DMA_CTRL, MSDC_DMA_CTRL_MODE, 1);

        sdr_write32(MSDC_DMA_SA, (u32)dma->gpd_addr);               
        break;

    default:
        break;
    }
    
    N_MSG(DMA, "DMA_CTRL = 0x%x", sdr_read32(MSDC_DMA_CTRL));
    N_MSG(DMA, "DMA_CFG  = 0x%x", sdr_read32(MSDC_DMA_CFG));
    N_MSG(DMA, "DMA_SA   = 0x%x", sdr_read32(MSDC_DMA_SA));

    return 0;
} 

static void msdc_dma_setup(struct msdc_host *host, struct msdc_dma *dma, 
    struct scatterlist *sg, unsigned int sglen)
{ 
    BUG_ON(sglen > MAX_BD_NUM); /* not support currently */

    dma->sg = sg;
    dma->flags = DMA_FLAG_EN_CHKSUM;
    //dma->flags = DMA_FLAG_NONE; /* CHECKME */
    dma->sglen = sglen;
    dma->xfersz = host->xfer_size;
    dma->burstsz = MSDC_BRUST_64B;
    
    //if (sglen == 1 && sg_dma_len(sg) <= MAX_DMA_CNT)
        //dma->mode = MSDC_MODE_DMA_BASIC;
    //else
    dma->mode = MSDC_MODE_DMA_DESC;

    N_MSG(DMA, "DMA mode<%d> sglen<%d> xfersz<%d>", dma->mode, dma->sglen, dma->xfersz);

    msdc_dma_config(host, dma);
}

/* set block number before send command */
static void msdc_set_blknum(struct msdc_host *host, u32 blknum)
{
    u32 base = host->base;

    sdr_write32(SDC_BLK_NUM, blknum);
}


#define REQ_CMD_ERR  (0x001)
#define REQ_DAT_ERR  (0x010)
#define REQ_STOP_ERR (0x100)

#ifdef MTK_EMMC_SUPPORT
u8 ext_csd[512];
u32 offset;
char parition_access = 0;

static int msdc_get_data(u8* dst,struct mmc_data *data)
{
	int left;
	u8* ptr;
	struct scatterlist *sg = data->sg;
	int num = data->sg_len;
        while (num) {
			left = sg_dma_len(sg);
   			ptr = (u8*)sg_virt(sg);
			memcpy(dst,ptr,left);
            sg = sg_next(sg); 
			dst+=left;
			num--;	
        }
	return 0;
}

/* parse part_info struct, support otp & mtk reserve */
static struct excel_info* msdc_reserve_part_info(unsigned char* name)
{
    int i;

    /* find reserve partition */
    for (i = 0; i < PART_NUM; i++) {
        printk("name = %s\n", PartInfo[i].name);  //====================debug
        if (0 == strcmp(name, PartInfo[i].name)){
            printk("size = %llu\n", PartInfo[i].size);//=======================debug
            return &PartInfo[i];
        }
    }

    return NULL;
}

int msdc_get_offset(void)
{
    u32 l_offset;

    l_offset =  MBR_START_ADDRESS_BYTE - (ext_csd[EXT_CSD_BOOT_SIZE_MULT]* 128 * 1024
            + ext_csd[EXT_CSD_BOOT_SIZE_MULT] * 128 * 1024
            + ext_csd[EXT_CSD_RPMB_SIZE_MULT] * 128 * 1024
            + ext_csd[EXT_CSD_GP1_SIZE_MULT + 2] * 256 * 256 
            + ext_csd[EXT_CSD_GP1_SIZE_MULT + 1] * 256 
            + ext_csd[EXT_CSD_GP1_SIZE_MULT + 0]
            + ext_csd[EXT_CSD_GP2_SIZE_MULT + 2] * 256 * 256 
            + ext_csd[EXT_CSD_GP2_SIZE_MULT + 1] * 256 
            + ext_csd[EXT_CSD_GP2_SIZE_MULT + 0]
            + ext_csd[EXT_CSD_GP3_SIZE_MULT + 2] * 256 * 256 
            + ext_csd[EXT_CSD_GP3_SIZE_MULT + 1] * 256 
            + ext_csd[EXT_CSD_GP3_SIZE_MULT + 0]
            + ext_csd[EXT_CSD_GP4_SIZE_MULT + 2] * 256 * 256 
            + ext_csd[EXT_CSD_GP4_SIZE_MULT + 1] * 256 
            + ext_csd[EXT_CSD_GP4_SIZE_MULT + 0]);

    return (l_offset >> 9);
}
EXPORT_SYMBOL(msdc_get_offset);

int msdc_get_reserve(void)
{
    u32 l_offset;
    struct excel_info* lp_excel_info;
    u32 l_otp_reserve = 0;
    u32 l_mtk_reserve = 0;

    l_offset = msdc_get_offset(); //==========check me

    lp_excel_info = msdc_reserve_part_info("bmtpool");
    if (NULL == lp_excel_info) {
        printk("can't get otp info from part_info struct\n");
        return -1;
    }

    l_mtk_reserve = (unsigned int)(lp_excel_info->start_address & 0xFFFFUL) << 8; /* unit is 512B */

    printk("mtk reserve: start address = %llu\n", lp_excel_info->start_address); //============================debug
#ifdef MTK_EMMC_SUPPORT_OTP
    lp_excel_info = msdc_reserve_part_info("otp");
    if (NULL == lp_excel_info) {
        printk("can't get otp info from part_info struct\n");
        return -1;
    }

    l_otp_reserve = (unsigned int)(lp_excel_info->start_address & 0xFFFFUL) << 8; /* unit is 512B */

printk("otp reserve: start address = %llu\n", lp_excel_info->start_address);//========================debug
    l_otp_reserve -= l_mtk_reserve;  /* the size info stored with total reserved size */
#endif 

    printk("total reserve: l_otp_reserve = 0x%x blocks, l_mtk_reserve = 0x%x blocks, l_offset = 0x%x blocks\n", 
             l_otp_reserve, l_mtk_reserve, l_offset);

    return (l_offset + l_otp_reserve + l_mtk_reserve);
}
EXPORT_SYMBOL(msdc_get_reserve);
#endif

static int msdc_do_request(struct mmc_host*mmc, struct mmc_request*mrq)
{
    struct msdc_host *host = mmc_priv(mmc);
    struct mmc_command *cmd;
    struct mmc_data *data;	
#ifdef MTK_EMMC_SUPPORT
    u64 device_capacity = 0;
    u32 device_legacy_capacity = 0;
#endif
    u32 base = host->base;
    //u32 intsts = 0;     
	  unsigned int left=0;
    int dma = 0, read = 1, dir = DMA_FROM_DEVICE, send_type=0;
    u32 map_sg = 0;  /* Fix the bug of dma_map_sg and dma_unmap_sg not match issue */
    u32 bus_mode = 0;    
    #define SND_DAT 0
    #define SND_CMD 1

    BUG_ON(mmc == NULL);
    BUG_ON(mrq == NULL);    

    host->error = 0;
    atomic_set(&host->abort, 0);
    
    cmd  = mrq->cmd;
    data = mrq->cmd->data;

    /* check msdc is work ok. rule is RX/TX fifocnt must be zero after last request 
     * if find abnormal, try to reset msdc first
     */
    if (msdc_txfifocnt() || msdc_rxfifocnt()) {
        printk("[SD%d] register abnormal,please check!\n",host->id);
        msdc_reset_hw(host->id);
	if (is_card_sdio(host))
	    udelay(30);			//Ivan add for testing
	
    }

//    if (host->id == 3)
//	printk("Ivan SDIO msdc_do_request! \n");
   		
    if (!data) {
        send_type=SND_CMD;	
#ifdef MTK_EMMC_SUPPORT
        if (host->id == EMMC_HOST_ID && 
           (cmd->opcode == MMC_ERASE_GROUP_START || cmd->opcode == MMC_ERASE_GROUP_END) &&
            parition_access == 0){

            cmd->arg  += offset;
        }
#endif
        if (msdc_do_command(host, cmd, 0, CMD_TIMEOUT) != 0) {
            goto done;         
        }
        
#ifdef MTK_EMMC_SUPPORT
        if(host->id == EMMC_HOST_ID && cmd->opcode == MMC_SWITCH && ((cmd->arg >> 16) & 0xFF) == EXT_CSD_PART_CFG){
            parition_access = (char)((cmd->arg >> 8) & 0x07);
        }
#endif

#ifdef MTK_EMMC_ETT_TO_DRIVER
        if ((host->id == EMMC_HOST_NUM) && (cmd->opcode == MMC_ALL_SEND_CID)) {
            m_id        = UNSTUFF_BITS(cmd->resp, 120, 8);
            pro_name[0]	= UNSTUFF_BITS(cmd->resp,  96, 8);
            pro_name[1]	= UNSTUFF_BITS(cmd->resp,  88, 8);
            pro_name[2]	= UNSTUFF_BITS(cmd->resp,  80, 8);
            pro_name[3]	= UNSTUFF_BITS(cmd->resp,  72, 8);
            pro_name[4]	= UNSTUFF_BITS(cmd->resp,  64, 8);
            pro_name[5]	= UNSTUFF_BITS(cmd->resp,  56, 8);
            //pro_name[6]	= '\0';     
        }
#endif
        
    } else {
        BUG_ON(data->blksz > HOST_MAX_BLKSZ);
        send_type=SND_DAT;

        data->error = 0;
        read = data->flags & MMC_DATA_READ ? 1 : 0;
        msdc_latest_operation_type[host->id] = read ? OPER_TYPE_READ : OPER_TYPE_WRITE;  
        host->data = data;
        host->xfer_size = data->blocks * data->blksz;
        host->blksz = data->blksz;

        /* deside the transfer mode */
        if (drv_mode[host->id] == MODE_PIO) {
            host->dma_xfer = dma = 0;
            msdc_latest_transfer_mode[host->id] = TRAN_MOD_PIO;
        } else if (drv_mode[host->id] == MODE_DMA) {
            host->dma_xfer = dma = 1;        	
            msdc_latest_transfer_mode[host->id] = TRAN_MOD_DMA;
        } else if (drv_mode[host->id] == MODE_SIZE_DEP) {
            host->dma_xfer = dma = ((host->xfer_size >= dma_size[host->id]) ? 1 : 0);	
            msdc_latest_transfer_mode[host->id] = dma ? TRAN_MOD_DMA: TRAN_MOD_PIO;
        }      

        if (read) {
            if ((host->timeout_ns != data->timeout_ns) ||
                (host->timeout_clks != data->timeout_clks)) {
                msdc_set_timeout(host, data->timeout_ns, data->timeout_clks);
            }
        }
        
        msdc_set_blknum(host, data->blocks);
        //msdc_clr_fifo();  /* no need */
#ifdef MTK_EMMC_SUPPORT
        if(host->id == EMMC_HOST_ID && (cmd->arg == 0) && 
          (cmd->opcode == MMC_READ_SINGLE_BLOCK || cmd->opcode == MMC_READ_MULTIPLE_BLOCK || cmd->opcode == MMC_WRITE_BLOCK || cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK) &&
          (parition_access == 0)) {
            
            offset =  MBR_START_ADDRESS_BYTE - (ext_csd[EXT_CSD_BOOT_SIZE_MULT]* 128 * 1024
                + ext_csd[EXT_CSD_BOOT_SIZE_MULT] * 128 * 1024
                + ext_csd[EXT_CSD_RPMB_SIZE_MULT] * 128 * 1024
                + ext_csd[EXT_CSD_GP1_SIZE_MULT + 2] * 256 * 256 
                + ext_csd[EXT_CSD_GP1_SIZE_MULT + 1] * 256 
                + ext_csd[EXT_CSD_GP1_SIZE_MULT + 0]
                + ext_csd[EXT_CSD_GP2_SIZE_MULT + 2] * 256 * 256 
                + ext_csd[EXT_CSD_GP2_SIZE_MULT + 1] * 256 
                + ext_csd[EXT_CSD_GP2_SIZE_MULT + 0]
                + ext_csd[EXT_CSD_GP3_SIZE_MULT + 2] * 256 * 256 
                + ext_csd[EXT_CSD_GP3_SIZE_MULT + 1] * 256 
                + ext_csd[EXT_CSD_GP3_SIZE_MULT + 0]
                + ext_csd[EXT_CSD_GP4_SIZE_MULT + 2] * 256 * 256 
                + ext_csd[EXT_CSD_GP4_SIZE_MULT + 1] * 256 
                + ext_csd[EXT_CSD_GP4_SIZE_MULT + 0]);
			if(mmc->card->csd.read_blkbits)
					device_legacy_capacity = mmc->card->csd.capacity * (2 << (mmc->card->csd.read_blkbits - 1));
			else{
					device_legacy_capacity = mmc->card->csd.capacity;
					ERR_MSG("XXX read_blkbits = 0 XXX");
				}
			device_capacity = (u64)(mmc->card->ext_csd.sectors)* 512 > device_legacy_capacity ? (u64)(mmc->card->ext_csd.sectors)* 512 : device_legacy_capacity;
			if(device_capacity > CAPACITY_2G)
					offset /= 512;
            ERR_MSG("[SD%d]Address offset in USER REGION(Capacity %lld MB) is 0x%x",host->id,device_capacity/(1024*1024),offset);
            if((int)offset < 0) {
                ERR_MSG("[SD%d] XXX Address offset error(%d),please check MBR start address!!",host->id,(int)offset);
                goto done;
            }
            cmd->arg = offset;
        } else if(host->id == EMMC_HOST_ID && 
            (cmd->opcode == MMC_READ_SINGLE_BLOCK || cmd->opcode == MMC_READ_MULTIPLE_BLOCK || cmd->opcode == MMC_WRITE_BLOCK || cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK) &&
            parition_access == 0) {
            cmd->arg  += offset;
        }
#endif
        
        if (dma) {
            msdc_dma_on();  /* enable DMA mode first!! */
            init_completion(&host->xfer_done);
            
            /* start the command first*/        	
			if(host->id == EMMC_HOST_ID || host->id == SD_HOST_ID){
				sdr_get_field(SDC_CFG ,SDC_CFG_BUSWIDTH ,bus_mode); //MT6577 work arroud for UHS-1 SDR104 
				if(bus_mode == 1 && host->sclk > 100000000)
					host->autocmd = 0;
				else
					host->autocmd = 0;//MSDC_AUTOCMD12;
			}
            if (msdc_command_start(host, cmd, 0, CMD_TIMEOUT) != 0)
                goto done;            

            dir = read ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
            (void)dma_map_sg(mmc_dev(mmc), data->sg, data->sg_len, dir);
            map_sg = 1;
                                  
            /* then wait command done */
            if (msdc_command_resp(host, cmd, 0, CMD_TIMEOUT) != 0) { // not tuning. 
                goto stop;                           
            }
            
            /* for read, the data coming too fast, then CRC error 
               start DMA no business with CRC. */
            //init_completion(&host->xfer_done);           
            msdc_dma_setup(host, &host->dma, data->sg, data->sg_len);  
            sdr_get_field(SDC_CFG ,SDC_CFG_BUSWIDTH ,bus_mode); //MT6577 work arroud for UHS-1 SDR104 
            if(bus_mode == 1 && host->sclk > 100000000)
		          sdr_set_field(MSDC_PATCH_BIT0, 1 << 16, 1);//MT6577 work arroud for UHS-1 SDR104 
            msdc_dma_start(host);
	          if (unlikely(dumpMSDC()))
		          AddStorageTrace(STORAGE_LOGGER_MSG_MSDC_DO,msdc_do_request,"msdc_dma_start",host->xfer_size); 
            spin_unlock(&host->lock);
            if(!wait_for_completion_timeout(&host->xfer_done, DAT_TIMEOUT)){
                ERR_MSG("XXX CMD<%d> wait xfer_done<%d> timeout!!", cmd->opcode, data->blocks * data->blksz);
				host->sw_timeout++;
                if (unlikely(dumpMSDC()))
			            AddStorageTrace(STORAGE_LOGGER_MSG_MSDC_DO,msdc_do_request,"msdc_dma ERR",host->xfer_size); 
                msdc_dump_info(host->id);           
                data->error = (unsigned int)-ETIMEDOUT;
                
                msdc_reset_hw(host->id);
            }
            spin_lock(&host->lock);
            msdc_dma_stop(host);
            if (unlikely(dumpMSDC()))
				      AddStorageTrace(STORAGE_LOGGER_MSG_MSDC_DO,msdc_do_request,"msdc_dma_stop"); 
        } else {
            /* Firstly: send command */
					host->autocmd = 0;
            if (msdc_do_command(host, cmd, 0, CMD_TIMEOUT) != 0) {
                goto stop;
            }
                                             
            /* Secondly: pio data phase */           
            if (read) {
                sdr_get_field(SDC_CFG ,SDC_CFG_BUSWIDTH ,bus_mode); //MT6577  work arroud for UHS-1 SDR104 
                if(bus_mode == 1 && host->sclk > 100000000)
                    sdr_set_field(MSDC_PATCH_BIT0, 1 << 16, 1);//MT6577 work arroud for UHS-1 SDR104 
                if (msdc_pio_read(host, data)){
                    goto stop; 	 // need cmd12. 
                }
            } else {
                sdr_get_field(SDC_CFG ,SDC_CFG_BUSWIDTH ,bus_mode); //MT6577 work arroud for UHS-1 SDR104 
                if(bus_mode == 1 && host->sclk > 100000000)
                    sdr_set_field(MSDC_PATCH_BIT0, 1 << 16, 1);//MT6577 work arroud for UHS-1 SDR104 
                if (msdc_pio_write(host, data)) {
                    goto stop; 		
                }
            }

            /* For write case: make sure contents in fifo flushed to device */           
            if (!read) {           	
                while (1) {
                    left=msdc_txfifocnt();                    
                    if (left == 0) {
                        break;	
                    }  
                    if (msdc_pio_abort(host, data, jiffies + DAT_TIMEOUT)) {
                        break;
                        /* Fix me: what about if data error, when stop ? how to? */
                    }                                    
                }
            } else {
                /* Fix me: read case: need to check CRC error */	
            }

            /* For write case: SDCBUSY and Xfer_Comp will assert when DAT0 not busy. 
               For read case : SDCBUSY and Xfer_Comp will assert when last byte read out from FIFO.
            */                             
            
            /* try not to wait xfer_comp interrupt. 
               the next command will check SDC_BUSY. 
               SDC_BUSY means xfer_comp assert 
            */ 
                      
        } // PIO mode 
        
stop:        
        /* Last: stop transfer */
        sdr_set_field(MSDC_PATCH_BIT0, 1 << 16, 0);//MT6577 work arroud for UHS-1 SDR104 
        if (data->stop){ 
			if(!((cmd->error == 0) && (data->error == 0) && (host->autocmd == MSDC_AUTOCMD12) && (cmd->opcode == MMC_READ_MULTIPLE_BLOCK || cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK))){
            	if (msdc_do_command(host, data->stop, 0, CMD_TIMEOUT) != 0) {
                	goto done; 
            	}
			}
        } 
    }

done:
#ifdef MTK_EMMC_SUPPORT
    if(host->id == EMMC_HOST_ID && 
			(cmd->opcode == MMC_READ_SINGLE_BLOCK || 
			 cmd->opcode == MMC_READ_MULTIPLE_BLOCK || 
			 cmd->opcode == MMC_WRITE_BLOCK || 
			 cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK || 
			 cmd->opcode == MMC_ERASE_GROUP_START || 
			 cmd->opcode == MMC_ERASE_GROUP_END) &&
			 (parition_access == 0))
			{
				cmd->arg  -= offset;
			}
#endif
    if (data != NULL) {
        host->data = NULL;
        host->dma_xfer = 0;    

#if 0   //read MBR
#ifdef MTK_EMMC_SUPPORT
        {
            char *ptr = sg_virt(data->sg);
            int i;
            if (cmd->arg == 0x0 && (cmd->opcode == MMC_READ_SINGLE_BLOCK || cmd->opcode == MMC_READ_MULTIPLE_BLOCK)){
                printk("XXXX CMD<%d> ARG<%X> offset = <%d> data<%s %s> sg <%p> ptr <%p> blksz<%d> block<%d> error<%d>\n",cmd->opcode, cmd->arg, offset, (dma? "dma":"pio"), 
		                  (read ? "read ":"write"), data->sg, ptr, data->blksz, data->blocks, data->error);             
              
                for(i = 0; i < 512; i ++){
                    if (i%32 == 0)
                       printk("\n");
                       printk(" %2x ", ptr[i]);
                } 
                printk("\n[%s]: msdc%d read MBR\n", __func__, host->id);
            }
        }
#endif    
#endif   //end read MBR
        if (dma != 0) {
            msdc_dma_off();     
            host->dma.used_bd  = 0;
            host->dma.used_gpd = 0;
            if (map_sg == 1) {   
				if(data->error == 0){
                int retry = 3;
                int count = 1000;
				msdc_retry(host->dma.gpd->hwo,retry,count,host->id);
				}
                dma_unmap_sg(mmc_dev(mmc), data->sg, data->sg_len, dir);
            }
        }
#ifdef MTK_EMMC_SUPPORT
        if(cmd->opcode == MMC_SEND_EXT_CSD){
            msdc_get_data(ext_csd,data);
        }
#endif
        host->blksz = 0;  
                

        N_MSG(OPS, "CMD<%d> data<%s %s> blksz<%d> block<%d> error<%d>",cmd->opcode, (dma? "dma":"pio"), 
                (read ? "read ":"write") ,data->blksz, data->blocks, data->error);                

        if (!is_card_sdio(host)) {
            if ((cmd->opcode != 17)&&(cmd->opcode != 18)&&(cmd->opcode != 24)&&(cmd->opcode != 25)) {             
                N_MSG(NRW, "CMD<%3d> arg<0x%8x> Resp<0x%8x> data<%s> size<%d>", cmd->opcode, cmd->arg, cmd->resp[0],
                    (read ? "read ":"write") ,data->blksz * data->blocks);  
            } else {
                N_MSG(RW,  "CMD<%3d> arg<0x%8x> Resp<0x%8x> block<%d>", cmd->opcode,
                    cmd->arg, cmd->resp[0], data->blocks);
            }
        }
    } else {
        if (!is_card_sdio(host)) {
            if (cmd->opcode != 13) { // by pass CMD13
                N_MSG(NRW, "CMD<%3d> arg<0x%8x> resp<%8x %8x %8x %8x>", cmd->opcode, cmd->arg, 
                    cmd->resp[0],cmd->resp[1], cmd->resp[2], cmd->resp[3]);                 	
            }
        }
    }

    if (mrq->cmd->error) host->error = REQ_CMD_ERR;
    if (mrq->data && mrq->data->error) host->error |= REQ_DAT_ERR;     
    if (mrq->stop && mrq->stop->error) host->error |= REQ_STOP_ERR; 

    //if (host->error) ERR_MSG("host->error<%d>", host->error);     
	if(host->error == 0){
		if(host->power_cycle)
			host->power_cycle = 0;
		if(host->sw_timeout)
			host->sw_timeout = 0;
	}
    return host->error;
}

#ifdef MTK_EMMC_SUPPORT
//need debug the interface for kernel panic
static unsigned int msdc_command_start_simple(struct msdc_host   *host, 
                                      struct mmc_command *cmd,
                                      int                 tune,   /* not used */
                                      unsigned long       timeout)
{
    u32 base = host->base;
    u32 opcode = cmd->opcode;
    u32 rawcmd;
                   
    u32 resp;  
    unsigned long tmo;
    volatile u32 intsts;

    /* Protocol layer does not provide response type, but our hardware needs 
     * to know exact type, not just size!
     */
    if (opcode == MMC_SEND_OP_COND || opcode == SD_APP_OP_COND)
        resp = RESP_R3;
    else if (opcode == MMC_SET_RELATIVE_ADDR || opcode == SD_SEND_RELATIVE_ADDR)
        resp = (mmc_cmd_type(cmd) == MMC_CMD_BCR) ? RESP_R6 : RESP_R1;
    else if (opcode == MMC_FAST_IO)
        resp = RESP_R4;
    else if (opcode == MMC_GO_IRQ_STATE)
        resp = RESP_R5;
    else if (opcode == MMC_SELECT_CARD)
        resp = (cmd->arg != 0) ? RESP_R1B : RESP_NONE;
    else if (opcode == SD_IO_RW_DIRECT || opcode == SD_IO_RW_EXTENDED)
        resp = RESP_R1; /* SDIO workaround. */
    else if (opcode == SD_SEND_IF_COND && (mmc_cmd_type(cmd) == MMC_CMD_BCR))
        resp = RESP_R1;
    else {
        switch (mmc_resp_type(cmd)) {
        case MMC_RSP_R1:
            resp = RESP_R1;
            break;
        case MMC_RSP_R1B:
            resp = RESP_R1B;
            break;
        case MMC_RSP_R2:
            resp = RESP_R2;
            break;
        case MMC_RSP_R3:
            resp = RESP_R3;
            break;
        case MMC_RSP_NONE:
        default:
            resp = RESP_NONE;              
            break;
        }
    }

    cmd->error = 0;
    /* rawcmd :
     * vol_swt << 30 | auto_cmd << 28 | blklen << 16 | go_irq << 15 | 
     * stop << 14 | rw << 13 | dtype << 11 | rsptyp << 7 | brk << 6 | opcode
     */    
    rawcmd = opcode | msdc_rsp[resp] << 7 | host->blksz << 16;
    
    if (opcode == MMC_READ_MULTIPLE_BLOCK) {
        rawcmd |= (2 << 11);
    } else if (opcode == MMC_READ_SINGLE_BLOCK) {
        rawcmd |= (1 << 11);
    } else if (opcode == MMC_WRITE_MULTIPLE_BLOCK) {
        rawcmd |= ((2 << 11) | (1 << 13));
    } else if (opcode == MMC_WRITE_BLOCK) {
        rawcmd |= ((1 << 11) | (1 << 13));
    } else if (opcode == SD_IO_RW_EXTENDED) {
        if (cmd->data->flags & MMC_DATA_WRITE)
            rawcmd |= (1 << 13);
        if (cmd->data->blocks > 1)
            rawcmd |= (2 << 11);
        else
            rawcmd |= (1 << 11);
    } else if (opcode == SD_IO_RW_DIRECT && cmd->flags == (unsigned int)-1) {
        rawcmd |= (1 << 14);
    } else if (opcode == 11) { /* bug */
        rawcmd |= (1 << 30);
    } else if ((opcode == SD_APP_SEND_SCR) || 
        (opcode == SD_APP_SEND_NUM_WR_BLKS) ||
        (opcode == SD_SWITCH && (mmc_cmd_type(cmd) == MMC_CMD_ADTC)) ||
        (opcode == SD_APP_SD_STATUS && (mmc_cmd_type(cmd) == MMC_CMD_ADTC)) ||
        (opcode == MMC_SEND_EXT_CSD && (mmc_cmd_type(cmd) == MMC_CMD_ADTC))) {
        rawcmd |= (1 << 11);
    } else if (opcode == MMC_STOP_TRANSMISSION) {
        rawcmd |= (1 << 14);
        rawcmd &= ~(0x0FFF << 16);
    }

    //printk("CMD<%d><0x%.8x> Arg<0x%.8x>\n", opcode , rawcmd, cmd->arg);

    tmo = jiffies + timeout;
    if (opcode == MMC_SEND_STATUS) {
        for (;;) {
            if (!sdc_is_cmd_busy())
                break;
                
            if (time_after(jiffies, tmo)) {
                ERR_MSG("XXX cmd_busy timeout: before CMD<%d>", opcode);	
                cmd->error = (unsigned int)-ETIMEDOUT;
                msdc_reset_hw(host->id);
                return cmd->error;
            } 
        }
    }else {
        for (;;) {	 
            if (!sdc_is_busy() && !sdc_is_cmd_busy())
                break;
            if (time_after(jiffies, tmo)) {
                ERR_MSG("XXX sdc_busy timeout: before CMD<%d>", opcode);	
                cmd->error = (unsigned int)-ETIMEDOUT;
                msdc_reset_hw(host->id);
                return cmd->error;    
            }   
        }    
    }   
    
    //BUG_ON(in_interrupt());
    host->cmd     = cmd;
    host->cmd_rsp = resp;		
    
    intsts = 0x1f7fb;
    sdr_write32(MSDC_INT, intsts);  /* clear interrupts */  

    sdc_send_cmd(rawcmd, cmd->arg);        
      
    return 0;  // irq too fast, then cmd->error has value, and don't call msdc_command_resp, don't tune. 
}

u32 msdc_intr_wait(struct msdc_host *host,  struct mmc_command *cmd, u32 intrs,unsigned long timeout)
{
    u32 base = host->base;
    u32 sts;
	unsigned long tmo;

    tmo = jiffies + timeout;
	while (1){
		if (((sts = sdr_read32(MSDC_INT)) & intrs) != 0){
            sdr_write32(MSDC_INT, (sts & intrs));
            break;
        }
        
		 if (time_after(jiffies, tmo)) {
                ERR_MSG("XXX wait cmd response timeout: before CMD<%d>", cmd->opcode);	
                cmd->error = (unsigned int)-ETIMEDOUT;
                msdc_reset_hw(host->id);
                return cmd->error;    
            }   
		}
    
    /* command interrupts */
    if ((sts & MSDC_INT_CMDRDY) || (sts & MSDC_INT_ACMDRDY) || 
            (sts & MSDC_INT_ACMD19_DONE)) {
        u32 *rsp = &cmd->resp[0];

        switch (host->cmd_rsp) {
            case RESP_NONE:
                break;
            case RESP_R2:
                *rsp++ = sdr_read32(SDC_RESP3); *rsp++ = sdr_read32(SDC_RESP2);
                *rsp++ = sdr_read32(SDC_RESP1); *rsp++ = sdr_read32(SDC_RESP0);
                break;
            default: /* Response types 1, 3, 4, 5, 6, 7(1b) */
                if ((sts & MSDC_INT_ACMDRDY) || (sts & MSDC_INT_ACMD19_DONE)) {
                    *rsp = sdr_read32(SDC_ACMD_RESP);
                } else {
                    *rsp = sdr_read32(SDC_RESP0);    
                }

                //msdc_dump_card_status(host, *rsp);
                break;
        }
    } else if ((sts & MSDC_INT_RSPCRCERR) || (sts & MSDC_INT_ACMDCRCERR)) {
        if(sts & MSDC_INT_ACMDCRCERR){
            printk("XXX CMD<%d> MSDC_INT_ACMDCRCERR",cmd->opcode);
        } 
        else {
            printk("XXX CMD<%d> MSDC_INT_RSPCRCERR Arg<0x%x>",cmd->opcode, cmd->arg);
        }
        cmd->error = (unsigned int)-EIO;
        msdc_reset_hw(host->id);    
    } else if ((sts & MSDC_INT_CMDTMO) || (sts & MSDC_INT_ACMDTMO)) {
        if(sts & MSDC_INT_ACMDTMO){
            printk("XXX CMD<%d> MSDC_INT_ACMDTMO",cmd->opcode);
        }
        else {
            printk("XXX CMD<%d> MSDC_INT_CMDTMO Arg<0x%x>",cmd->opcode, cmd->arg);
        }
        cmd->error = (unsigned int)-ETIMEDOUT;
        msdc_reset_hw(host->id);          
    }

    N_MSG(INT, "[SD%d] INT(0x%x)\n", host->id, sts);
    if (~intrs & sts) {
        N_MSG(WRN, "[SD%d]<CHECKME> Unexpected INT(0x%x)\n", 
            host->id, ~intrs & sts);
    }
    return sts;
}

static unsigned int msdc_command_resp_simple(struct msdc_host   *host, 
                                      struct mmc_command *cmd,
                                      int                 tune,
                                      unsigned long       timeout)
{
    //u32 base = host->base;
    u32 resp;
    u32 status;
    u32 wints = MSDC_INT_CMDRDY  | MSDC_INT_RSPCRCERR  | MSDC_INT_CMDTMO  |  
                MSDC_INT_ACMDRDY | MSDC_INT_ACMDCRCERR | MSDC_INT_ACMDTMO | 
                MSDC_INT_ACMD19_DONE;     
    
    resp = host->cmd_rsp;

    /*polling*/
    status = msdc_intr_wait(host,cmd,wints,timeout);

    host->cmd = NULL;

    return cmd->error;
} 

static unsigned int msdc_do_command_simple(struct msdc_host   *host, 
                                      struct mmc_command *cmd,
                                      int                 tune,
                                      unsigned long       timeout)
{
    if (msdc_command_start_simple(host, cmd, tune, timeout)) 
        goto end;      

    if (msdc_command_resp_simple(host, cmd, tune, timeout)) 
        goto end;          
           	    
end:	

    N_MSG(CMD, "        return<%d> resp<0x%.8x>", cmd->error, cmd->resp[0]); 	
    return cmd->error;
}

int simple_sd_request(struct mmc_host*mmc, struct mmc_request* mrq)
{
    struct msdc_host *host = mmc_priv(mmc);
    struct mmc_command *cmd;
    struct mmc_data *data;
    u32 base = host->base;
    unsigned int left=0;
    int dma = 0, read = 1, send_type=0;
    volatile u32 intsts;
    volatile u32 l_sts;


    #define SND_DAT 0
    #define SND_CMD 1

    BUG_ON(mmc == NULL);
    BUG_ON(mrq == NULL);    

    host->error = 0;
    
    cmd  = mrq->cmd;
    data = mrq->cmd->data;
   		
    msdc_ungate_clock(host);  // set sw flag 
    
    if (!data) {
        host->blksz = 0;  
        send_type=SND_CMD;	
        if (msdc_do_command_simple(host, cmd, 1, CMD_TIMEOUT) != 0) {
            if(cmd->opcode == MMC_STOP_TRANSMISSION){
                msdc_dma_stop(host);
                msdc_reset_hw(host->id);
                msdc_clr_fifo(host->id);
                l_sts = sdr_read32(MSDC_CFG);
                if ((l_sts & 0x8)==0){
                    l_sts |= 0x8;
                    sdr_write32(MSDC_CFG, l_sts);
                }
                //msdc_dump_register(host);
            }
            goto done;         
        }

        if(cmd->opcode == MMC_STOP_TRANSMISSION){
            msdc_dma_stop(host);
            msdc_reset_hw(host->id);
            msdc_clr_fifo(host->id);
            l_sts = sdr_read32(MSDC_CFG);
            if ((l_sts & 0x8)==0){
                l_sts |= 0x8;
                sdr_write32(MSDC_CFG, l_sts);
            }

            //msdc_dump_register(host);
        }
    } else {
        BUG_ON(data->blksz > HOST_MAX_BLKSZ);
        send_type=SND_DAT;

        data->error = 0;
        read = data->flags & MMC_DATA_READ ? 1 : 0;
        msdc_latest_operation_type[host->id] = read ? OPER_TYPE_READ : OPER_TYPE_WRITE;  
        host->data = data;
        host->xfer_size = data->blocks * data->blksz;
        host->blksz = data->blksz;

        host->dma_xfer = dma = 0;
        msdc_latest_transfer_mode[host->id] = TRAN_MOD_PIO;
        
        if (read) {
            if ((host->timeout_ns != data->timeout_ns) ||
                (host->timeout_clks != data->timeout_clks)) {
                msdc_set_timeout(host, data->timeout_ns, data->timeout_clks);
            }
        }
        
        msdc_set_blknum(host, data->blocks);
        if(host->id == EMMC_HOST_ID && 
        (cmd->opcode == MMC_READ_SINGLE_BLOCK || cmd->opcode == MMC_READ_MULTIPLE_BLOCK || cmd->opcode == MMC_WRITE_BLOCK || cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK))
        {
            cmd->arg  += offset;
        }
        /* Firstly: send command */
        if (msdc_do_command_simple(host, cmd, 1, CMD_TIMEOUT) != 0) {
            goto done;
        }

        /* Secondly: pio data phase */           
        l_sts = sdr_read32(MSDC_CFG);
        l_sts = 0;
        sdr_clr_bits(MSDC_INTEN, MSDC_INT_XFER_COMPL);  
        if (msdc_pio_write(host, data)) {
            goto done; 		
        }

        /* For write case: make sure contents in fifo flushed to device */           
        if (!read) {           	
            while (1) {
                left=msdc_txfifocnt();                    
                if (left == 0) {
                    break;	
                }  
                if (msdc_pio_abort(host, data, jiffies + DAT_TIMEOUT)) {
                    break;
                    /* Fix me: what about if data error, when stop ? how to? */
                }                                    
            }
        } else {
            /* Fix me: read case: need to check CRC error */	
        }

        /* polling for write */
        intsts = 0;
        //msdc_dump_register(host);
        while ((intsts & MSDC_INT_XFER_COMPL) == 0 ){
            intsts = sdr_read32(MSDC_INT);	                
        }
        sdr_set_bits(MSDC_INT, MSDC_INT_XFER_COMPL);	
    }

done:
#ifdef MTK_EMMC_SUPPORT
    if(host->id == EMMC_HOST_ID && 
       (cmd->opcode == MMC_READ_SINGLE_BLOCK || cmd->opcode == MMC_READ_MULTIPLE_BLOCK || cmd->opcode == MMC_WRITE_BLOCK || cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK))
    {
        cmd->arg  -= offset;
    }
#endif
    if (data != NULL) {
        host->data = NULL;
        host->dma_xfer = 0;    
        host->blksz = 0;  

        N_MSG(OPS, "CMD<%d> data<%s %s> blksz<%d> block<%d> error<%d>",cmd->opcode, (dma? "dma":"pio"), 
                (read ? "read ":"write") ,data->blksz, data->blocks, data->error);                

    } 
        
    if (mrq->cmd->error) host->error = 0x001;
    if (mrq->data && mrq->data->error) host->error |= 0x010;     
    if (mrq->stop && mrq->stop->error) host->error |= 0x100; 

    msdc_gate_clock(host, 1); // clear flag. 

    return host->error;
}
#endif

static int msdc_app_cmd(struct mmc_host *mmc, struct msdc_host *host)
{
    struct mmc_command cmd = {0};    
    struct mmc_request mrq = {0};
    u32 err = -1; 
   
    cmd.opcode = MMC_APP_CMD;    
    cmd.arg = host->app_cmd_arg;  /* meet mmc->card is null when ACMD6 */     
    cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_AC;
    
    mrq.cmd = &cmd; cmd.mrq = &mrq;
    cmd.data = NULL;        

    err = msdc_do_command(host, &cmd, 0, CMD_TIMEOUT);     
    return err;      	
}

static int msdc_lower_freq(struct msdc_host *host)
{
    u32 div, mode; 
    u32 base = host->base;

    ERR_MSG("need to lower freq"); 
    sdr_get_field(MSDC_CFG, MSDC_CFG_CKMOD, mode);                
    sdr_get_field(MSDC_CFG, MSDC_CFG_CKDIV, div);

    if (div >= MSDC_MAX_FREQ_DIV) {
        ERR_MSG("but, div<%d> power tuning", div);	
        return msdc_power_tuning(host);	
    } else if(mode == 1){
        mode = 0;
        msdc_clk_stable(host,mode, div);
        host->sclk = (div == 0) ? hclks[host->hw->clk_src]/2 : hclks[host->hw->clk_src]/(4*div);
		
        ERR_MSG("new div<%d>, mode<%d> new freq.<%dKHz>", div, mode,host->sclk/1000);
        return 0;
    }
    else{
        msdc_clk_stable(host,mode, div + 1);
        host->sclk = (mode == 2) ? hclks[host->hw->clk_src]/(2*4*(div+1)) : hclks[host->hw->clk_src]/(4*(div+1));
        ERR_MSG("new div<%d>, mode<%d> new freq.<%dKHz>", div + 1, mode,host->sclk/1000);
        return 0;
    }          	
}

static int msdc_tune_cmdrsp(struct msdc_host *host)
{
    int result = 0;
    u32 base = host->base;
    u32 sel = 0;
    u32 cur_rsmpl = 0, orig_rsmpl;
    u32 cur_rrdly = 0, orig_rrdly;
    u32 cur_cntr  = 0,orig_cmdrtc;
    u32 cur_dl_cksel = 0, orig_dl_cksel;

    sdr_get_field(MSDC_IOCON, MSDC_IOCON_RSPL, orig_rsmpl);
    sdr_get_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, orig_rrdly);
    sdr_get_field(MSDC_PATCH_BIT1, MSDC_CMD_RSP_TA_CNTR, orig_cmdrtc);
    sdr_get_field(MSDC_PATCH_BIT0, MSDC_INT_DAT_LATCH_CK_SEL, orig_dl_cksel);
    if (unlikely(dumpMSDC()))
    {
	    AddStorageTrace(STORAGE_LOGGER_MSG_MSDC_DO,msdc_do_request,"sd_tune_ori RSPL",orig_rsmpl); 
	    AddStorageTrace(STORAGE_LOGGER_MSG_MSDC_DO,msdc_do_request,"sd_tune_ori RRDLY",orig_rrdly); 
    }
#if 1
    printk("msdc begin tune");
    if (host->id == SD_HOST_ID && host->mclk >= 100000000){
        sel = 1;
        sdr_set_field(MSDC_PATCH_BIT0, MSDC_CKGEN_RX_SDCLKO_SEL,0);
    }
    else if(host->id == SD_HOST_ID){
        sdr_set_field(MSDC_PATCH_BIT1,MSDC_CMD_RSP_TA_CNTR,1);
        sdr_set_field(MSDC_PATCH_BIT0, MSDC_CKGEN_RX_SDCLKO_SEL,1);
        sdr_set_field(MSDC_PATCH_BIT0, MSDC_INT_DAT_LATCH_CK_SEL,0);
    }
		
    cur_rsmpl = (orig_rsmpl + 1);
    sdr_set_field(MSDC_IOCON, MSDC_IOCON_RSPL, cur_rsmpl % 2);
	
    if(cur_rsmpl >= 2){	
        cur_rrdly = (orig_rrdly + 1);
        sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, cur_rrdly % 32);
    }
    if(cur_rrdly >= 32){
        if(sel){
            cur_cntr = (orig_cmdrtc + 1) ;
            sdr_set_field(MSDC_PATCH_BIT1, MSDC_CMD_RSP_TA_CNTR, cur_cntr % 8);
        }
    }
    if(cur_cntr >= 8){
        if(sel){
            cur_dl_cksel = (orig_dl_cksel +1);
            sdr_set_field(MSDC_PATCH_BIT0, MSDC_INT_DAT_LATCH_CK_SEL, cur_dl_cksel % 8);
        }
    }
    ++(host->t_counter.time_cmd);
    if((sel && host->t_counter.time_cmd == CMD_TUNE_UHS_MAX_TIME)||(sel == 0 && host->t_counter.time_cmd == CMD_TUNE_HS_MAX_TIME)){
#ifdef MSDC_LOWER_FREQ
        result = msdc_lower_freq(host);
#else
        result = 1;
#endif
        host->t_counter.time_cmd = 0;
    }
#else
    if (orig_rsmpl == 0) {
        cur_rsmpl = 1; 
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_RSPL, cur_rsmpl);                  	
    } else {
        cur_rsmpl = 0;   
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_RSPL, cur_rsmpl);  // need second layer       
        cur_rrdly = (orig_rrdly + 1); 
        if (cur_rrdly >= 32) {
            ERR_MSG("failed to update rrdly<%d>", cur_rrdly); 
            sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, 0);
        #ifdef MSDC_LOWER_FREQ
            return (msdc_lower_freq(host));                      
        #else
            return 1;
        #endif 	
        } 	
        sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, cur_rrdly);
    }

#endif
    sdr_get_field(MSDC_IOCON, MSDC_IOCON_RSPL, orig_rsmpl);
    sdr_get_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, orig_rrdly);
    sdr_get_field(MSDC_PATCH_BIT1, MSDC_CMD_RSP_TA_CNTR, orig_cmdrtc);
    sdr_get_field(MSDC_PATCH_BIT0, MSDC_INT_DAT_LATCH_CK_SEL, orig_dl_cksel);
    INIT_MSG("TUNE_CMD: rsmpl<%d> rrdly<%d> cmdrtc<%d> dl_cksel<%d> sfreq.<%d>", orig_rsmpl, orig_rrdly,orig_cmdrtc,orig_dl_cksel,host->sclk);      
    if (unlikely(dumpMSDC()))
    {
		  AddStorageTrace(STORAGE_LOGGER_MSG_MSDC_DO,msdc_do_request,"sd_tune_ok RSPL",orig_rsmpl); 
		  AddStorageTrace(STORAGE_LOGGER_MSG_MSDC_DO,msdc_do_request,"sd_tune_ok RRDLY",orig_rrdly); 
    }		
    return result;
}

static int msdc_tune_read(struct msdc_host *host)
{
    u32 base = host->base;
    u32 sel = 0;
    u32 ddr = 0;	
    u32 dcrc;
    u32 clkmode = 0;
    u32 cur_rxdly0, cur_rxdly1;
    u32 cur_dsmpl = 0, orig_dsmpl;
    u32 cur_dsel = 0,orig_dsel;
    u32 cur_dl_cksel = 0,orig_dl_cksel;
    u32 cur_dat0 = 0, cur_dat1 = 0, cur_dat2 = 0, cur_dat3 = 0, 
    cur_dat4 = 0, cur_dat5 = 0, cur_dat6 = 0, cur_dat7 = 0;
    u32 orig_dat0, orig_dat1, orig_dat2, orig_dat3, orig_dat4, orig_dat5, orig_dat6, orig_dat7;
    int result = 0;

	#if 1
    if (host->id == SD_HOST_ID && host->mclk >= 100000000){
        sel = 1;}
    else if(host->id == SD_HOST_ID){
        sdr_set_field(MSDC_PATCH_BIT0, MSDC_CKGEN_MSDC_DLY_SEL, 0);
    }
    sdr_get_field(MSDC_CFG,MSDC_CFG_CKMOD,clkmode);
    ddr = (clkmode == 2) ? 1 : 0;
    if(host->id == SD_HOST_ID && clkmode == 1 && host->mclk >= 100000000)
        sdr_set_field(MSDC_PATCH_BIT0, MSDC_CKGEN_RX_SDCLKO_SEL, 0);
    else if(host->id == SD_HOST_ID){
        sdr_set_field(MSDC_PATCH_BIT0, MSDC_CKGEN_RX_SDCLKO_SEL, 1);
    }
    sdr_get_field(MSDC_PATCH_BIT0, MSDC_CKGEN_MSDC_DLY_SEL, orig_dsel);
    sdr_get_field(MSDC_PATCH_BIT0, MSDC_INT_DAT_LATCH_CK_SEL, orig_dl_cksel);
    sdr_get_field(MSDC_IOCON, MSDC_IOCON_DSPL, orig_dsmpl);
	
    sdr_set_field(MSDC_IOCON, MSDC_IOCON_DDLSEL, 1);
    cur_dsmpl = (orig_dsmpl + 1) ;
    sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, cur_dsmpl % 2);
	
    if(cur_dsmpl >= 2){
        sdr_get_field(SDC_DCRC_STS, SDC_DCRC_STS_POS | SDC_DCRC_STS_NEG, dcrc);        
        if (!ddr) dcrc &= ~SDC_DCRC_STS_NEG;
        cur_rxdly0 = sdr_read32(MSDC_DAT_RDDLY0);
        cur_rxdly1 = sdr_read32(MSDC_DAT_RDDLY1); 
        if (sdr_read32(MSDC_ECO_VER) >= 4) {
            orig_dat0 = (cur_rxdly0 >> 24) & 0x1F;
            orig_dat1 = (cur_rxdly0 >> 16) & 0x1F;
            orig_dat2 = (cur_rxdly0 >>  8) & 0x1F;
            orig_dat3 = (cur_rxdly0 >>  0) & 0x1F;
            orig_dat4 = (cur_rxdly1 >> 24) & 0x1F;
            orig_dat5 = (cur_rxdly1 >> 16) & 0x1F;
            orig_dat6 = (cur_rxdly1 >>  8) & 0x1F;
            orig_dat7 = (cur_rxdly1 >>  0) & 0x1F;
        } else {   
            orig_dat0 = (cur_rxdly0 >>  0) & 0x1F;
            orig_dat1 = (cur_rxdly0 >>  8) & 0x1F;
            orig_dat2 = (cur_rxdly0 >> 16) & 0x1F;
            orig_dat3 = (cur_rxdly0 >> 24) & 0x1F;
            orig_dat4 = (cur_rxdly1 >>  0) & 0x1F;
            orig_dat5 = (cur_rxdly1 >>  8) & 0x1F;
            orig_dat6 = (cur_rxdly1 >> 16) & 0x1F;
            orig_dat7 = (cur_rxdly1 >> 24) & 0x1F;
        }
        
        if (ddr) {
            cur_dat0 = (dcrc & (1 << 0) || dcrc & (1 << 8))  ? (orig_dat0 + 1) : orig_dat0;
            cur_dat1 = (dcrc & (1 << 1) || dcrc & (1 << 9))  ? (orig_dat1 + 1) : orig_dat1;
            cur_dat2 = (dcrc & (1 << 2) || dcrc & (1 << 10)) ? (orig_dat2 + 1) : orig_dat2;
            cur_dat3 = (dcrc & (1 << 3) || dcrc & (1 << 11)) ? (orig_dat3 + 1) : orig_dat3;
        } else {
            cur_dat0 = (dcrc & (1 << 0)) ? (orig_dat0 + 1) : orig_dat0;
            cur_dat1 = (dcrc & (1 << 1)) ? (orig_dat1 + 1) : orig_dat1;
            cur_dat2 = (dcrc & (1 << 2)) ? (orig_dat2 + 1) : orig_dat2;
            cur_dat3 = (dcrc & (1 << 3)) ? (orig_dat3 + 1) : orig_dat3;
        }
        cur_dat4 = (dcrc & (1 << 4)) ? (orig_dat4 + 1) : orig_dat4;
        cur_dat5 = (dcrc & (1 << 5)) ? (orig_dat5 + 1) : orig_dat5;
        cur_dat6 = (dcrc & (1 << 6)) ? (orig_dat6 + 1) : orig_dat6;
        cur_dat7 = (dcrc & (1 << 7)) ? (orig_dat7 + 1) : orig_dat7;
        if (sdr_read32(MSDC_ECO_VER) >= 4) {
            cur_rxdly0 = ((cur_dat0 & 0x1F) << 24) | ((cur_dat1 & 0x1F) << 16) |
            	         ((cur_dat2 & 0x1F) << 8)  | ((cur_dat3 & 0x1F) << 0);
            cur_rxdly1 = ((cur_dat4 & 0x1F) << 24) | ((cur_dat5 & 0x1F) << 16) |
            	         ((cur_dat6 & 0x1F) << 8)  | ((cur_dat7 & 0x1F) << 0);
        }
        else{
            cur_rxdly0 = ((cur_dat3 & 0x1F) << 24) | ((cur_dat2 & 0x1F) << 16) |
            	         ((cur_dat1 & 0x1F) << 8)  | ((cur_dat0 & 0x1F) << 0);
            cur_rxdly1 = ((cur_dat7 & 0x1F) << 24) | ((cur_dat6 & 0x1F) << 16) |
            	         ((cur_dat5 & 0x1F) << 8)  | ((cur_dat4 & 0x1F) << 0);
        }
        sdr_write32(MSDC_DAT_RDDLY0, cur_rxdly0);
        sdr_write32(MSDC_DAT_RDDLY1, cur_rxdly1); 
		
		}
    if((cur_dat0 >= 32) || (cur_dat1 >= 32) || (cur_dat2 >= 32) || (cur_dat3 >= 32)||
        (cur_dat4 >= 32) || (cur_dat5 >= 32) || (cur_dat6 >= 32) || (cur_dat7 >= 32)){
        if(sel){
            cur_dsel = (orig_dsel + 1) ;
            sdr_set_field(MSDC_PATCH_BIT0, MSDC_CKGEN_MSDC_DLY_SEL, cur_dsel % 32);
        }
    }
    if(cur_dsel >= 32 ){
        if(clkmode == 1 && sel){
            cur_dl_cksel = (orig_dl_cksel + 1);
            sdr_set_field(MSDC_PATCH_BIT0, MSDC_INT_DAT_LATCH_CK_SEL, cur_dl_cksel % 8);
        }
    }
    ++(host->t_counter.time_read);
    if((sel == 1 && clkmode == 1 && host->t_counter.time_read == READ_TUNE_UHS_CLKMOD1_MAX_TIME)||
        (sel == 1 && (clkmode == 0 ||clkmode == 2) && host->t_counter.time_read == READ_TUNE_UHS_MAX_TIME)||
        (sel == 0 && (clkmode == 0 ||clkmode == 2) && host->t_counter.time_read == READ_TUNE_HS_MAX_TIME)){
#ifdef MSDC_LOWER_FREQ
        result = msdc_lower_freq(host); 		
#else
        result = 1;
#endif
        host->t_counter.time_read = 0;
    }	
#else
    sdr_set_field(MSDC_IOCON, MSDC_IOCON_DDLSEL, 1);
    
    cur_rxdly0 = sdr_read32(MSDC_DAT_RDDLY0);
    cur_rxdly1 = sdr_read32(MSDC_DAT_RDDLY1);
    sdr_get_field(MSDC_IOCON, MSDC_IOCON_DSPL, orig_dsmpl);   
    if (orig_dsmpl == 0) {
        cur_dsmpl = 1; 
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, cur_dsmpl);                    	
    } else {
        cur_dsmpl = 0; 
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, cur_dsmpl); // need second layer

        sdr_get_field(SDC_DCRC_STS, SDC_DCRC_STS_POS | SDC_DCRC_STS_NEG, dcrc);        
        if (!ddr) dcrc &= ~SDC_DCRC_STS_NEG;

        if (sdr_read32(MSDC_ECO_VER) >= 4) {
            orig_dat0 = (cur_rxdly0 >> 24) & 0x1F;
            orig_dat1 = (cur_rxdly0 >> 16) & 0x1F;
            orig_dat2 = (cur_rxdly0 >>  8) & 0x1F;
            orig_dat3 = (cur_rxdly0 >>  0) & 0x1F;
            orig_dat4 = (cur_rxdly1 >> 24) & 0x1F;
            orig_dat5 = (cur_rxdly1 >> 16) & 0x1F;
            orig_dat6 = (cur_rxdly1 >>  8) & 0x1F;
            orig_dat7 = (cur_rxdly1 >>  0) & 0x1F;
        } else {   
            orig_dat0 = (cur_rxdly0 >>  0) & 0x1F;
            orig_dat1 = (cur_rxdly0 >>  8) & 0x1F;
            orig_dat2 = (cur_rxdly0 >> 16) & 0x1F;
            orig_dat3 = (cur_rxdly0 >> 24) & 0x1F;
            orig_dat4 = (cur_rxdly1 >>  0) & 0x1F;
            orig_dat5 = (cur_rxdly1 >>  8) & 0x1F;
            orig_dat6 = (cur_rxdly1 >> 16) & 0x1F;
            orig_dat7 = (cur_rxdly1 >> 24) & 0x1F;
        }
        
        if (ddr) {
            cur_dat0 = (dcrc & (1 << 0) || dcrc & (1 << 8))  ? (orig_dat0 + 1) : orig_dat0;
            cur_dat1 = (dcrc & (1 << 1) || dcrc & (1 << 9))  ? (orig_dat1 + 1) : orig_dat1;
            cur_dat2 = (dcrc & (1 << 2) || dcrc & (1 << 10)) ? (orig_dat2 + 1) : orig_dat2;
            cur_dat3 = (dcrc & (1 << 3) || dcrc & (1 << 11)) ? (orig_dat3 + 1) : orig_dat3;
        } else {
            cur_dat0 = (dcrc & (1 << 0)) ? (orig_dat0 + 1) : orig_dat0;
            cur_dat1 = (dcrc & (1 << 1)) ? (orig_dat1 + 1) : orig_dat1;
            cur_dat2 = (dcrc & (1 << 2)) ? (orig_dat2 + 1) : orig_dat2;
            cur_dat3 = (dcrc & (1 << 3)) ? (orig_dat3 + 1) : orig_dat3;
        }
        cur_dat4 = (dcrc & (1 << 4)) ? (orig_dat4 + 1) : orig_dat4;
        cur_dat5 = (dcrc & (1 << 5)) ? (orig_dat5 + 1) : orig_dat5;
        cur_dat6 = (dcrc & (1 << 6)) ? (orig_dat6 + 1) : orig_dat6;
        cur_dat7 = (dcrc & (1 << 7)) ? (orig_dat7 + 1) : orig_dat7;

        if (cur_dat0 >= 32 || cur_dat1 >= 32 || cur_dat2 >= 32 || cur_dat3 >= 32) {
            ERR_MSG("failed to update <%xh><%xh><%xh><%xh>", cur_dat0, cur_dat1, cur_dat2, cur_dat3);  
            sdr_write32(MSDC_DAT_RDDLY0, 0); 
            sdr_write32(MSDC_DAT_RDDLY1, 0); 
                      	
#ifdef MSDC_LOWER_FREQ
            return (msdc_lower_freq(host));                       
#else
            return 1;
#endif 	
        }

        if (cur_dat4 >= 32 || cur_dat5 >= 32 || cur_dat6 >= 32 || cur_dat7 >= 32) {
            ERR_MSG("failed to update <%xh><%xh><%xh><%xh>", cur_dat4, cur_dat5, cur_dat6, cur_dat7);
            sdr_write32(MSDC_DAT_RDDLY0, 0); 
            sdr_write32(MSDC_DAT_RDDLY1, 0); 
                      	
#ifdef MSDC_LOWER_FREQ
            return (msdc_lower_freq(host));                      
#else
            return 1;
#endif
        }

        cur_rxdly0 = (cur_dat0 << 24) | (cur_dat1 << 16) | (cur_dat2 << 8) | (cur_dat3 << 0);
        cur_rxdly1 = (cur_dat4 << 24) | (cur_dat5 << 16) | (cur_dat6 << 8) | (cur_dat7 << 0);
        
        sdr_write32(MSDC_DAT_RDDLY0, cur_rxdly0);
        sdr_write32(MSDC_DAT_RDDLY1, cur_rxdly1);        	             	
    }

#endif
    sdr_get_field(MSDC_PATCH_BIT0, MSDC_CKGEN_MSDC_DLY_SEL, orig_dsel);
    sdr_get_field(MSDC_PATCH_BIT0, MSDC_INT_DAT_LATCH_CK_SEL, orig_dl_cksel);
    sdr_get_field(MSDC_IOCON, MSDC_IOCON_DSPL, orig_dsmpl);
    cur_rxdly0 = sdr_read32(MSDC_DAT_RDDLY0);
    cur_rxdly1 = sdr_read32(MSDC_DAT_RDDLY1); 
    INIT_MSG("TUNE_READ: dsmpl<%d> rxdly0<0x%x> rxdly1<0x%x> dsel<%d> dl_cksel<%d> sfreq.<%d>", orig_dsmpl, cur_rxdly0, cur_rxdly1,orig_dsel,orig_dl_cksel,host->sclk);

    return result;
}

static int msdc_tune_write(struct msdc_host *host)
{
    u32 base = host->base;

    u32 cur_wrrdly = 0, orig_wrrdly;
    u32 cur_dsmpl = 0,  orig_dsmpl;
    u32 cur_rxdly0 = 0;
    u32 orig_dat0, orig_dat1, orig_dat2, orig_dat3;
    u32 cur_dat0 = 0,cur_dat1 = 0,cur_dat2 = 0,cur_dat3 = 0;
    u32 cur_d_cntr = 0 , orig_d_cntr;
    int result = 0;

    int sel = 0;
    int clkmode = 0;
    // MSDC_IOCON_DDR50CKD need to check. [Fix me] 
#if 1
    if (host->id == SD_HOST_ID && host->mclk >= 100000000)
        sel = 1;
    else if(host->id == SD_HOST_ID){
        sdr_set_field(MSDC_PATCH_BIT1,MSDC_WRDAT_CRCS_TA_CNTR,2);
    }
    sdr_get_field(MSDC_CFG,MSDC_CFG_CKMOD,clkmode);
    if(host->id == SD_HOST_ID && clkmode == 1 && host->mclk >= 100000000)
        sdr_set_field(MSDC_PATCH_BIT0, MSDC_CKGEN_RX_SDCLKO_SEL, 0);
    else if(host->id == SD_HOST_ID)
        sdr_set_field(MSDC_PATCH_BIT0, MSDC_CKGEN_RX_SDCLKO_SEL, 1);
	
    sdr_get_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, orig_wrrdly);
    sdr_get_field(MSDC_IOCON, MSDC_IOCON_DSPL, orig_dsmpl);  
    sdr_get_field(MSDC_PATCH_BIT1, MSDC_WRDAT_CRCS_TA_CNTR, orig_d_cntr);

    sdr_set_field(MSDC_IOCON, MSDC_IOCON_DDLSEL, 1);
    cur_dsmpl = (orig_dsmpl + 1);
    sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, cur_dsmpl % 2);
	
    if(cur_dsmpl >= 2){
        cur_wrrdly = (orig_wrrdly+1);
        sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, cur_wrrdly % 32);
    }
    if(cur_wrrdly >= 32){
        cur_rxdly0 = sdr_read32(MSDC_DAT_RDDLY0);
        if (sdr_read32(MSDC_ECO_VER) >= 4) {
        	orig_dat0 = (cur_rxdly0 >> 24) & 0x1F;
       	 	orig_dat1 = (cur_rxdly0 >> 16) & 0x1F;
       	 	orig_dat2 = (cur_rxdly0 >>  8) & 0x1F;
        	orig_dat3 = (cur_rxdly0 >>  0) & 0x1F;
        }else {
        	orig_dat0 = (cur_rxdly0 >>  0) & 0x1F;
        	orig_dat1 = (cur_rxdly0 >>  8) & 0x1F;
        	orig_dat2 = (cur_rxdly0 >> 16) & 0x1F;
        	orig_dat3 = (cur_rxdly0 >> 24) & 0x1F;
        }
        cur_dat0 = (orig_dat0 + 1); /* only adjust bit-1 for crc */
        cur_dat1 = orig_dat1;
        cur_dat2 = orig_dat2;
        cur_dat3 = orig_dat3;
        if (sdr_read32(MSDC_ECO_VER) >= 4) {
            cur_rxdly0 = ((cur_dat0 & 0x1F) << 24) | ((cur_dat1 & 0x1F) << 16) |
               	       ((cur_dat2 & 0x1F) << 8) | ((cur_dat3 & 0x1F) << 0);
        }
        else{
            cur_rxdly0 = ((cur_dat3& 0x1F) << 24) | ((cur_dat2 & 0x1F) << 16) |
               	       ((cur_dat1 & 0x1F) << 8) | ((cur_dat0 & 0x1F) << 0);
        }
        sdr_write32(MSDC_DAT_RDDLY0, cur_rxdly0);
    }
    if(cur_dat0 >= 32){
        if(sel){
            cur_d_cntr= (orig_d_cntr + 1 );
            sdr_set_field(MSDC_PATCH_BIT1, MSDC_WRDAT_CRCS_TA_CNTR, cur_d_cntr % 8);
        }
    }
    ++(host->t_counter.time_write);
    if((sel == 0 && host->t_counter.time_write == WRITE_TUNE_HS_MAX_TIME) || (sel && host->t_counter.time_write == WRITE_TUNE_UHS_MAX_TIME)){
#ifdef MSDC_LOWER_FREQ
        result = msdc_lower_freq(host); 		
#else
        result = 1;
#endif
        host->t_counter.time_write = 0;
    }
		
#else
    
    /* Tune Method 2. just DAT0 */  
    sdr_set_field(MSDC_IOCON, MSDC_IOCON_DDLSEL, 1);
    cur_rxdly0 = sdr_read32(MSDC_DAT_RDDLY0);
    if (sdr_read32(MSDC_ECO_VER) >= 4) {
        orig_dat0 = (cur_rxdly0 >> 24) & 0x1F;
        orig_dat1 = (cur_rxdly0 >> 16) & 0x1F;
        orig_dat2 = (cur_rxdly0 >>  8) & 0x1F;
        orig_dat3 = (cur_rxdly0 >>  0) & 0x1F;
    } else {
        orig_dat0 = (cur_rxdly0 >>  0) & 0x1F;
        orig_dat1 = (cur_rxdly0 >>  8) & 0x1F;
        orig_dat2 = (cur_rxdly0 >> 16) & 0x1F;
        orig_dat3 = (cur_rxdly0 >> 24) & 0x1F;
    }

    sdr_get_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, orig_wrrdly);
    cur_wrrdly = orig_wrrdly;         
    sdr_get_field(MSDC_IOCON,    MSDC_IOCON_DSPL,        orig_dsmpl );
    if (orig_dsmpl == 0) {
        cur_dsmpl = 1;
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, cur_dsmpl);    	
    } else {
        cur_dsmpl = 0;
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, cur_dsmpl);  // need the second layer 
        
        cur_wrrdly = (orig_wrrdly + 1);
        if (cur_wrrdly < 32) {
            sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, cur_wrrdly);         	
        } else {
            cur_wrrdly = 0; 
            sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, cur_wrrdly);  // need third 

            cur_dat0 = orig_dat0 + 1; /* only adjust bit-1 for crc */
            cur_dat1 = orig_dat1;
            cur_dat2 = orig_dat2;
            cur_dat3 = orig_dat3;  
            
            if (cur_dat0 >= 32) {
                ERR_MSG("update failed <%xh>", cur_dat0);	
                sdr_write32(MSDC_DAT_RDDLY0, 0); 
                
                #ifdef MSDC_LOWER_FREQ
                    return (msdc_lower_freq(host));                      
                #else
                    return 1;
                #endif 
            }
                       
            cur_rxdly0 = (cur_dat0 << 24) | (cur_dat1 << 16) | (cur_dat2 << 8) | (cur_dat3 << 0); 
            sdr_write32(MSDC_DAT_RDDLY0, cur_rxdly0);      	
        }
               	
    }

#endif
    sdr_get_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, orig_wrrdly);
    sdr_get_field(MSDC_IOCON, MSDC_IOCON_DSPL, orig_dsmpl);  
    sdr_get_field(MSDC_PATCH_BIT1, MSDC_WRDAT_CRCS_TA_CNTR, orig_d_cntr);
    cur_rxdly0 = sdr_read32(MSDC_DAT_RDDLY0);
    INIT_MSG("TUNE_WRITE: dsmpl<%d> wrrdly<%d> rxdly0<0x%x> d_cntr<%d> sfreq.<%d>", orig_dsmpl, orig_wrrdly, cur_rxdly0,orig_d_cntr,host->sclk);
    
    return result;
}

static int msdc_get_card_status(struct mmc_host *mmc, struct msdc_host *host, u32 *status)
{
    struct mmc_command cmd;    
    struct mmc_request mrq;
    u32 err; 

    memset(&cmd, 0, sizeof(struct mmc_command));    
    cmd.opcode = MMC_SEND_STATUS;    // CMD13   	
    cmd.arg = host->app_cmd_arg;    	
    cmd.flags = MMC_RSP_SPI_R2 | MMC_RSP_R1 | MMC_CMD_AC;

    memset(&mrq, 0, sizeof(struct mmc_request));
    mrq.cmd = &cmd; cmd.mrq = &mrq;
    cmd.data = NULL;        

    err = msdc_do_command(host, &cmd, 0, CMD_TIMEOUT);  // tune until CMD13 pass.      
    
    if (status) {
        *status = cmd.resp[0];
    }    
    
    return err;                	
}

//#define TUNE_FLOW_TEST
#ifdef TUNE_FLOW_TEST
static void msdc_reset_para(struct msdc_host *host)
{
    u32 base = host->base; 
    u32 dsmpl, rsmpl;

    // because we have a card, which must work at dsmpl<0> and rsmpl<0>

    sdr_get_field(MSDC_IOCON, MSDC_IOCON_DSPL, dsmpl);
    sdr_get_field(MSDC_IOCON, MSDC_IOCON_RSPL, rsmpl);

    if (dsmpl == 0) {
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, 1);  
        ERR_MSG("set dspl<0>");  	
        sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_CMDRRDLY, 0);        
    }
    
    if (rsmpl == 0) {
        sdr_set_field(MSDC_IOCON, MSDC_IOCON_RSPL, 1);
        ERR_MSG("set rspl<0>");  	
        sdr_write32(MSDC_DAT_RDDLY0, 0);
        sdr_set_field(MSDC_PAD_TUNE, MSDC_PAD_TUNE_DATWRDLY, 0);
    }   	
}
#endif

static void msdc_dump_trans_error(struct msdc_host   *host,
                                  struct mmc_command *cmd,
                                  struct mmc_data    *data,
                                  struct mmc_command *stop)
{
    if ((cmd->opcode == 52) && (cmd->arg == 0xc00)) return; 
    if ((cmd->opcode == 52) && (cmd->arg == 0x80000c08)) return;     

    if (!is_card_sdio(host)) { // by pass the SDIO CMD TO for SD/eMMC 
        if (cmd->opcode == 5) return;             	
    }	else {
        if (cmd->opcode == 8) return;    	
    }
    
    ERR_MSG("XXX CMD<%d><0x%x> Error<%d> Resp<0x%x>", cmd->opcode, cmd->arg, cmd->error, cmd->resp[0]);	
    
    if (data) {
        ERR_MSG("XXX DAT block<%d> Error<%d>", data->blocks, data->error);                	        	
    }   
    
    if (stop) {
        ERR_MSG("XXX STOP<%d><0x%x> Error<%d> Resp<0x%x>", stop->opcode, stop->arg, stop->error, stop->resp[0]);	                    	
    }             	
}

/* ops.request */
static void msdc_ops_request(struct mmc_host *mmc, struct mmc_request *mrq)
{   
    struct msdc_host *host = mmc_priv(mmc);
    struct mmc_command *cmd;    
    struct mmc_data *data;
    struct mmc_command *stop = NULL;
	int data_abort = 0;
    //=== for sdio profile ===
    u32 old_H32, old_L32, new_H32, new_L32;
    u32 ticks = 0, opcode = 0, sizes = 0, bRx = 0; 
    msdc_reset_tune_counter(host,all_counter);      
    if(host->mrq){
        ERR_MSG("XXX host->mrq<0x%.8x>", (int)host->mrq);   
        BUG();	
    }       	 
      
    if (!is_card_present(host) || host->power_mode == MMC_POWER_OFF) {
        ERR_MSG("cmd<%d> card<%d> power<%d>", mrq->cmd->opcode, is_card_present(host), host->power_mode);
        mrq->cmd->error = (unsigned int)-ENOMEDIUM; 
        
#if 1        
        mrq->done(mrq);         // call done directly.
#else
        mrq->cmd->retries = 0;  // please don't retry.
        mmc_request_done(mmc, mrq);
#endif

        return;
    }
      
    /* start to process */
    spin_lock(&host->lock);  

    cmd = mrq->cmd;      
    data = mrq->cmd->data;
    if (data) stop = data->stop;

    msdc_ungate_clock(host);  // set sw flag 
         
    if (sdio_pro_enable) {  //=== for sdio profile ===  
        if (mrq->cmd->opcode == 52 || mrq->cmd->opcode == 53) {    
            GPT_GetCounter64(&old_L32, &old_H32); 
        }
    }
    
    host->mrq = mrq;    

    while (msdc_do_request(mmc,mrq)) { // there is some error     
        // becasue ISR executing time will be monitor, try to dump the info here. 
        msdc_dump_trans_error(host, cmd, data, stop); 
    		data_abort = 0;     
        if (is_card_sdio(host)) {
            goto out;  // sdio not tuning     	
        }       

        if ((cmd->error == (unsigned int)-EIO) || (stop && (stop->error == (unsigned int)-EIO))) {
            if (msdc_tune_cmdrsp(host)){
                ERR_MSG("failed to updata cmd para");
                goto out;	
            } 	
        }        

        if (data && (data->error == (unsigned int)-EIO)) {
            if (data->flags & MMC_DATA_READ) {  // read 
                if (msdc_tune_read(host)) {
                    ERR_MSG("failed to updata read para");   
                    goto out; 
                }                                 	
            } else {
                if (msdc_tune_write(host)) {
                    ERR_MSG("failed to updata write para");
                    goto out;
                }  
            }         	
        }

        // bring the card to "tran" state
        if (data) {
            if (msdc_abort_data(host)) {
                ERR_MSG("abort failed");
				data_abort = 1;
				if(host->id == SD_HOST_ID){
                	host->card_inserted = 0; 
                	goto out;	
				}
            }        	
        }

        // CMD TO -> not tuning 
        if (cmd->error == (unsigned int)-ETIMEDOUT) {
			if(cmd->opcode == MMC_READ_SINGLE_BLOCK || cmd->opcode == MMC_READ_MULTIPLE_BLOCK || cmd->opcode == MMC_WRITE_BLOCK || cmd->opcode == MMC_WRITE_MULTIPLE_BLOCK ){
				if(data_abort){
            		if(msdc_power_tuning(host))
            			goto out;
				}
			}
			else
				goto out;
        } 

        // [ALPS114710] Patch for data timeout issue.
        if (data && (data->error == (unsigned int)-ETIMEDOUT)) {  
            if (data->flags & MMC_DATA_READ) {
				if((host->sw_timeout) || (++(host->read_time_tune) > 3)){
					ERR_MSG("msdc%d exceed max read timeout tune times(%d) or SW timeout(%d),Power cycle\n",host->id,host->read_time_tune,host->sw_timeout);
					 if(msdc_power_tuning(host))
                		goto out;
					}
            }
			else if(data->flags & MMC_DATA_WRITE){
				if((host->sw_timeout) || (++(host->write_time_tune) > 3)){
					ERR_MSG("msdc%d exceed max write timeout tune times(%d) or SW timeout(%d),Power cycle\n",host->id,host->write_time_tune,host->sw_timeout);
					if(msdc_power_tuning(host))
                		goto out;
					}
				}
        }        

        // clear the error condition.
        cmd->error = 0; 
        if (data) data->error = 0;
        if (stop) stop->error = 0; 

        // check if an app commmand.  
        if (host->app_cmd) {
            while (msdc_app_cmd(host->mmc, host)) {
                if (msdc_tune_cmdrsp(host)){
                    ERR_MSG("failed to updata cmd para for app");
                    goto out;	
                }   
            } 
        } 
         
        if (!is_card_present(host)) {
            goto out;
        }        
    }
	host->read_time_tune = 0;
	host->write_time_tune = 0;

out: 
    msdc_reset_tune_counter(host,all_counter);
	
#ifdef TUNE_FLOW_TEST
    if (!is_card_sdio(host)) {
        msdc_reset_para(host);   
    }   
#endif

    /* ==== when request done, check if app_cmd ==== */
    if (mrq->cmd->opcode == MMC_APP_CMD) {
        host->app_cmd = 1; 	  
        host->app_cmd_arg = mrq->cmd->arg;  /* save the RCA */
    } else {
        host->app_cmd = 0; 	 
        //host->app_cmd_arg = 0;    	
    }
        
    host->mrq = NULL; 

    //=== for sdio profile ===
    if (sdio_pro_enable) {  
        if (mrq->cmd->opcode == 52 || mrq->cmd->opcode == 53) {     
            GPT_GetCounter64(&new_L32, &new_H32);
            ticks = msdc_time_calc(old_L32, old_H32, new_L32, new_H32);
            
            opcode = mrq->cmd->opcode;    
            if (mrq->cmd->data) {
                sizes = mrq->cmd->data->blocks * mrq->cmd->data->blksz; 	
                bRx = mrq->cmd->data->flags & MMC_DATA_READ ? 1 : 0 ;
            } else {
                bRx = mrq->cmd->arg	& 0x80000000 ? 1 : 0;  
            }
            
            if (!mrq->cmd->error) {
                msdc_performance(opcode, sizes, bRx, ticks);
            }
        }    
    } 

    msdc_gate_clock(host, 1); // clear flag. 
    spin_unlock(&host->lock);

    mmc_request_done(mmc, mrq);
     
   return;
}

/* called by ops.set_ios */
static void msdc_set_buswidth(struct msdc_host *host, u32 width)
{
    u32 base = host->base;
    u32 val = sdr_read32(SDC_CFG);
    
    val &= ~SDC_CFG_BUSWIDTH;
    
    switch (width) {
    default:
    case MMC_BUS_WIDTH_1:
        width = 1;
        val |= (MSDC_BUS_1BITS << 16);
        break;
    case MMC_BUS_WIDTH_4:
        val |= (MSDC_BUS_4BITS << 16);
        break;
    case MMC_BUS_WIDTH_8:
        val |= (MSDC_BUS_8BITS << 16);
        break;
    }
    
    sdr_write32(SDC_CFG, val);

    N_MSG(CFG, "Bus Width = %d", width);
}

/* ops.set_ios */
static void msdc_ops_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
    struct msdc_host *host = mmc_priv(mmc);
    struct msdc_hw *hw=host->hw;
    u32 base = host->base;
    u32 ddr = 0;
    unsigned long flags;

#ifdef MT6577_SD_DEBUG
    static char *vdd[] = {
        "1.50v", "1.55v", "1.60v", "1.65v", "1.70v", "1.80v", "1.90v",
        "2.00v", "2.10v", "2.20v", "2.30v", "2.40v", "2.50v", "2.60v",
        "2.70v", "2.80v", "2.90v", "3.00v", "3.10v", "3.20v", "3.30v",
        "3.40v", "3.50v", "3.60v"		
    };
    static char *power_mode[] = {
        "OFF", "UP", "ON"
    };
    static char *bus_mode[] = {
        "UNKNOWN", "OPENDRAIN", "PUSHPULL"
    };
    static char *timing[] = {
        "LEGACY", "MMC_HS", "SD_HS"
    };

    N_MSG(CFG, "SET_IOS: CLK(%dkHz), BUS(%s), BW(%u), PWR(%s), VDD(%s), TIMING(%s)",
        ios->clock / 1000, bus_mode[ios->bus_mode],
        (ios->bus_width == MMC_BUS_WIDTH_4) ? 4 : 1,
        power_mode[ios->power_mode], vdd[ios->vdd], timing[ios->timing]);
#endif

    spin_lock_irqsave(&host->lock, flags);
    if(ios->timing == MMC_TIMING_UHS_DDR50)
        ddr = 1;
    msdc_ungate_clock(host);
    
    msdc_set_buswidth(host, ios->bus_width);     
    
    /* Power control ??? */
    switch (ios->power_mode) {
    case MMC_POWER_OFF:
    case MMC_POWER_UP:
		spin_unlock_irqrestore(&host->lock, flags);
        msdc_set_power_mode(host, ios->power_mode);
		spin_lock_irqsave(&host->lock, flags);
        break;
    case MMC_POWER_ON:
        host->power_mode = MMC_POWER_ON;
        break;
    default:
        break;
    }
    if(msdc_host_mode[host->id] != mmc->caps)
		{
    		mmc->caps = msdc_host_mode[host->id];
    		sdr_write32(MSDC_PAD_TUNE,   0x00000000);
    		sdr_write32(MSDC_DAT_RDDLY0, 0x00000000);
    		sdr_write32(MSDC_DAT_RDDLY1, 0x00000000);
    		sdr_write32(MSDC_IOCON,      0x00000000);
	    	sdr_write32(MSDC_PATCH_BIT0, 0x403C000F); /* bit0 modified: Rx Data Clock Source: 1 -> 2.0*/
			if (sdr_read32(MSDC_ECO_VER) >= 4) { 
        		if (host->id == SD_HOST_ID) {	
            		sdr_set_field(MSDC_PATCH_BIT1, MSDC_PATCH_BIT1_WRDAT_CRCS, 2); 
            		sdr_set_field(MSDC_PATCH_BIT1, MSDC_PATCH_BIT1_CMD_RSP,    1);
        		}   
    		}
			if (!is_card_sdio(host)) {  /* internal clock: latch read data, not apply to sdio */           
            	sdr_set_bits(MSDC_PATCH_BIT0, MSDC_PATCH_BIT_CKGEN_CK);
            	host->hw->cmd_edge  = 0; // tuning from 0
            	host->hw->data_edge = 0;            
        	}else if (hw->flags & MSDC_INTERNAL_CLK) {
            	sdr_set_bits(MSDC_PATCH_BIT0, MSDC_PATCH_BIT_CKGEN_CK);
        	}
		}
    if(msdc_clock_src[host->id] != hw->clk_src){
        hw->clk_src = msdc_clock_src[host->id];
        msdc_select_clksrc(host, hw->clk_src);
    }
    
    if(msdc_drv_mode[host->id].clk_drv != hw->clk_drv ||
       msdc_drv_mode[host->id].cmd_drv != hw->cmd_drv ||
       msdc_drv_mode[host->id].dat_drv != hw->dat_drv) {

        hw->clk_drv = msdc_drv_mode[host->id].clk_drv;
        hw->cmd_drv = msdc_drv_mode[host->id].cmd_drv;
        hw->dat_drv = msdc_drv_mode[host->id].dat_drv;
        sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVN, hw->clk_drv);
        sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVP, hw->clk_drv);
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVN, hw->cmd_drv);
        sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVP, hw->cmd_drv);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVN, hw->dat_drv);
        sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVP, hw->dat_drv);
    }
    
    if (host->mclk != ios->clock) { /* not change when clock Freq. not changed */                
        if(ios->clock >= 25000000) {	
            INIT_MSG("SD latch data<%d> cmd<%d>", hw->data_edge, hw->cmd_edge);
            sdr_set_field(MSDC_IOCON, MSDC_IOCON_RSPL, hw->cmd_edge); 
            sdr_set_field(MSDC_IOCON, MSDC_IOCON_DSPL, hw->data_edge);

#ifdef MTK_EMMC_ETT_TO_DRIVER
            if (host->id == EMMC_HOST_NUM) {
                INIT_MSG("m_id     <0x%x>", m_id);
                INIT_MSG("pro_name <%s>",  pro_name);

                msdc_ett_offline_to_driver(host);
            }
#endif	
        }
        
        if (ios->clock == 0) {
            sdr_get_field(MSDC_IOCON, MSDC_IOCON_RSPL, hw->cmd_edge);   // save the para
            sdr_get_field(MSDC_IOCON, MSDC_IOCON_DSPL, hw->data_edge);    
            INIT_MSG("save latch data<%d> cmd<%d>", hw->data_edge, hw->cmd_edge); 
                                            	
            /* reset to default value */
            sdr_write32(MSDC_IOCON,      0x00000000);           
            sdr_write32(MSDC_DAT_RDDLY0, 0x00000000);
            sdr_write32(MSDC_DAT_RDDLY1, 0x00000000);            
            sdr_write32(MSDC_PAD_TUNE,   0x00000000);               	
        }  
        msdc_set_mclk(host, ddr, ios->clock);        
	if (is_card_sdio(host)) { 
//Ivan		
	    printk("Ivan msdc_ops_set_ios SD3 clock on!\n");

//	    mdelay(50);  // wait for WIFI stable.                                     	 
	}	
    }

#if 0  // PM Resume -> set 0 -> 260KHz     
    if (ios->clock == 0) { // only gate clock when set 0Hz   
        msdc_gate_clock(host, 1);
    }
#endif       
    msdc_gate_clock(host, 1);
    spin_unlock_irqrestore(&host->lock, flags);       
    if (is_card_sdio(host)) {
	mdelay(50);  // wait for WIFI stable.                                     	 	
	printk("Ivan msdc_ops_set_ios!\n");
    }
}

/* ops.get_ro */
static int msdc_ops_get_ro(struct mmc_host *mmc)
{
    struct msdc_host *host = mmc_priv(mmc);
    u32 base = host->base;
    unsigned long flags;
    int ro = 0;

    spin_lock_irqsave(&host->lock, flags);
    msdc_ungate_clock(host);
    if (host->hw->flags & MSDC_WP_PIN_EN) { /* set for card */
        ro = (sdr_read32(MSDC_PS) >> 31);
    }
    msdc_gate_clock(host, 1);
    spin_unlock_irqrestore(&host->lock, flags);       
    return ro;
}

/* ops.get_cd */
static int msdc_ops_get_cd(struct mmc_host *mmc)
{
    struct msdc_host *host = mmc_priv(mmc);
    u32 base;
    unsigned long flags;
    int present = 1;

    base = host->base; 
    spin_lock_irqsave(&host->lock, flags);
    
    /* for sdio, depends on USER_RESUME */
//Ivan    
    if (is_card_sdio(host)) { 
        host->card_inserted = (host->pm_state.event == PM_EVENT_USER_RESUME) ? 1 : 0; 
//Ivan added
        host->card_inserted = 1;   
	
        //INIT_MSG("sdio ops_get_cd<%d>", host->card_inserted);
        goto end;    	
    }

    /* for emmc, MSDC_REMOVABLE not set, always return 1 */
    if (!(host->hw->flags & MSDC_REMOVABLE)) {
        host->card_inserted = 1;       
        goto end;
    }

    msdc_ungate_clock(host);
    if (host->hw->flags & MSDC_CD_PIN_EN) { /* for card, MSDC_CD_PIN_EN set*/
        host->card_inserted = (sdr_read32(MSDC_PS) & MSDC_PS_CDSTS) ? 0 : 1; 
    } else {
        host->card_inserted = 1; /* TODO? Check DAT3 pins for card detection */
    }

    if (present == 0) {    
        msdc_gate_clock(host, 0);
    }else {
        msdc_gate_clock(host, 1);
    }
    
    INIT_MSG("Card insert<%d>", host->card_inserted);
    
end:
    spin_unlock_irqrestore(&host->lock, flags);	
    return host->card_inserted;
}

/* ops.enable_sdio_irq */
static void msdc_ops_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
    struct msdc_host *host = mmc_priv(mmc);
    struct msdc_hw *hw = host->hw;
    u32 base = host->base;
    u32 tmp;

    if (hw->flags & MSDC_EXT_SDIO_IRQ) { /* yes for sdio */
        if (enable) {
            hw->enable_sdio_eirq();  /* combo_sdio_enable_eirq */
        } else {
            hw->disable_sdio_eirq(); /* combo_sdio_disable_eirq */
        }
    } else { 
        ERR_MSG("XXX ");  /* so never enter here */
        tmp = sdr_read32(SDC_CFG);
        /* FIXME. Need to interrupt gap detection */
        if (enable) {
            tmp |= (SDC_CFG_SDIOIDE | SDC_CFG_SDIOINTWKUP);           
        } else {
            tmp &= ~(SDC_CFG_SDIOIDE | SDC_CFG_SDIOINTWKUP);
        }
        sdr_write32(SDC_CFG, tmp);      
    }
}
int msdc_ops_switch_volt(struct mmc_host *mmc, struct mmc_ios *ios)
{
    struct msdc_host *host = mmc_priv(mmc);
    u32 base = host->base;
    int err = 0;
    u32 timeout = 100;
    u32 retry = 10;
    u32 status;
    u32 sclk = host->sclk;
    if(ios->signal_voltage != MMC_SIGNAL_VOLTAGE_330){
        /* make sure SDC is not busy (TBC) */
        //WAIT_COND(!SDC_IS_BUSY(), timeout, timeout);
        err = (unsigned int)-EIO;
	msdc_retry(sdc_is_busy(), retry, timeout,host->id);
        if (timeout == 0 && retry == 0) {
            err = (unsigned int)-ETIMEDOUT;
            goto out;
        }

        /* pull up disabled in CMD and DAT[3:0] to allow card drives them to low */

        /* check if CMD/DATA lines both 0 */
        if ((sdr_read32(MSDC_PS) & ((1 << 24) | (0xF << 16))) == 0) {

            /* pull up disabled in CMD and DAT[3:0] */
            msdc_pin_config(host, MSDC_PIN_PULL_NONE);

            /* change signal from 3.3v to 1.8v for FPGA this can not work*/
            if(ios->signal_voltage == MMC_SIGNAL_VOLTAGE_180)
                msdc_sd1_power_switch(1,VOL_1800);
            else if(ios->signal_voltage == MMC_SIGNAL_VOLTAGE_120)
                msdc_sd1_power_switch(1,VOL_1200);

            /* wait at least 5ms for 1.8v signal switching in card */
            mdelay(10);

            /* config clock to 10~12MHz mode for volt switch detection by host. */
	
            msdc_set_mclk(host, 0, 12000000);/*For FPGA 13MHz clock,this not work*/

            /* pull up enabled in CMD and DAT[3:0] */
            msdc_pin_config(host, MSDC_PIN_PULL_UP);
            mdelay(5);

            /* start to detect volt change by providing 1.8v signal to card */
            sdr_set_bits(MSDC_CFG, MSDC_CFG_BV18SDT);

            /* wait at max. 1ms */
            mdelay(1);
            ERR_MSG("before read status\n");

            while ((status = sdr_read32(MSDC_CFG)) & MSDC_CFG_BV18SDT);

            if (status & MSDC_CFG_BV18PSS)
                err = 0;
            ERR_MSG("msdc V1800 status (0x%x),err(%d)\n",status,err);
            /* config clock back to init clk freq. */
            msdc_set_mclk(host, 0, sclk);
        }   
    }
out:
        
    return err;
}

static struct mmc_host_ops mt_msdc_ops = {
    .request         = msdc_ops_request,
    .set_ios         = msdc_ops_set_ios,
    .get_ro          = msdc_ops_get_ro,
    .get_cd          = msdc_ops_get_cd,
    .enable_sdio_irq = msdc_ops_enable_sdio_irq,
    .start_signal_voltage_switch = msdc_ops_switch_volt,
};

/*--------------------------------------------------------------------------*/
/* interrupt handler                                                    */
/*--------------------------------------------------------------------------*/
//static __tcmfunc irqreturn_t msdc_irq(int irq, void *dev_id)
static irqreturn_t msdc_irq(int irq, void *dev_id)
{
    struct msdc_host  *host = (struct msdc_host *)dev_id;
    struct mmc_data   *data = host->data;
    struct mmc_command *cmd = host->cmd;
    struct mmc_command *stop = NULL;
    u32 base = host->base;
        
    u32 cmdsts = MSDC_INT_RSPCRCERR  | MSDC_INT_CMDTMO  | MSDC_INT_CMDRDY  |
                 MSDC_INT_ACMDCRCERR | MSDC_INT_ACMDTMO | MSDC_INT_ACMDRDY |
                 MSDC_INT_ACMD19_DONE;                 
    u32 datsts = MSDC_INT_DATCRCERR  |MSDC_INT_DATTMO;
    u32 intsts, inten;

    if ((host->id == SD_HOST_ID) && (0 == host->core_clkon)) {
#ifndef FPGA_PLATFORM
        enable_clock(PERI_MSDC0_PDN + host->id, "SD"); 
#endif
        host->core_clkon = 1;       
        
        if (sdr_read32(MSDC_ECO_VER) >= 4) {
            sdr_set_field(MSDC_CFG, MSDC_CFG_MODE, MSDC_SDMMC);  /* E2 */	
            intsts = sdr_read32(MSDC_INT);
            sdr_set_field(MSDC_CLKSRC_REG, MSDC1_IRQ_SEL, 0);
            mt65xx_irq_set_sens(host->irq, MT65xx_LEVEL_SENSITIVE);        	
        } else {
            intsts = sdr_read32(MSDC_INT);    	
        }              
    } else {
        intsts = sdr_read32(MSDC_INT);
    }
    
    inten  = sdr_read32(MSDC_INTEN); inten &= intsts; 

    sdr_write32(MSDC_INT, intsts);  /* clear interrupts */
    /* MSG will cause fatal error */
        
    /* card change interrupt */
    if (intsts & MSDC_INT_CDSC){
        IRQ_MSG("MSDC_INT_CDSC irq<0x%.8x>", intsts); 
        tasklet_hi_schedule(&host->card_tasklet);
        /* tuning when plug card ? */
    }
    
    /* sdio interrupt */
    if (intsts & MSDC_INT_SDIOIRQ){
        IRQ_MSG("XXX MSDC_INT_SDIOIRQ");  /* seems not sdio irq */
        //mmc_signal_sdio_irq(host->mmc);
    }

    /* transfer complete interrupt */
    if (data != NULL) {
		stop = data->stop;
        if (inten & MSDC_INT_XFER_COMPL) {       	
            data->bytes_xfered = host->dma.xfersz;
            complete(&host->xfer_done);           
        } 
        
        if (intsts & datsts) {         
            /* do basic reset, or stop command will sdc_busy */
            msdc_reset_hw(host->id);           
            atomic_set(&host->abort, 1);  /* For PIO mode exit */
            
            if (intsts & MSDC_INT_DATTMO){
               	data->error = (unsigned int)-ETIMEDOUT;				
               	IRQ_MSG("XXX CMD<%d> Arg<0x%.8x> MSDC_INT_DATTMO", host->mrq->cmd->opcode, host->mrq->cmd->arg);
            }
            else if (intsts & MSDC_INT_DATCRCERR){
                data->error = (unsigned int)-EIO;			
                IRQ_MSG("XXX CMD<%d> Arg<0x%.8x> MSDC_INT_DATCRCERR, SDC_DCRC_STS<0x%x>", 
                        host->mrq->cmd->opcode, host->mrq->cmd->arg, sdr_read32(SDC_DCRC_STS));
            }
                                    
            //if(sdr_read32(MSDC_INTEN) & MSDC_INT_XFER_COMPL) {  
            if (host->dma_xfer) {
                complete(&host->xfer_done); /* Read CRC come fast, XFER_COMPL not enabled */
            } /* PIO mode can't do complete, because not init */
        }
    }

    /* command interrupts */
    if ((cmd != NULL) && (intsts & cmdsts)) {
        if ((intsts & MSDC_INT_CMDRDY) || (intsts & MSDC_INT_ACMDRDY) || 
            (intsts & MSDC_INT_ACMD19_DONE)) {
            u32 *rsp = NULL;
			if((intsts & MSDC_INT_ACMDRDY) && (host->autocmd == MSDC_AUTOCMD12) && (stop != NULL))
				rsp = &stop->resp[0];
			else
				rsp = &cmd->resp[0];
            
            switch (host->cmd_rsp) {
            case RESP_NONE:
                break;
            case RESP_R2:
                *rsp++ = sdr_read32(SDC_RESP3); *rsp++ = sdr_read32(SDC_RESP2);
                *rsp++ = sdr_read32(SDC_RESP1); *rsp++ = sdr_read32(SDC_RESP0);
                break;
            default: /* Response types 1, 3, 4, 5, 6, 7(1b) */
                if ((intsts & MSDC_INT_ACMDRDY) || (intsts & MSDC_INT_ACMD19_DONE)) {
                    *rsp = sdr_read32(SDC_ACMD_RESP);
                } else {
                    *rsp = sdr_read32(SDC_RESP0);    
                }
                break;
            }
        } else if ((intsts & MSDC_INT_RSPCRCERR) || (intsts & MSDC_INT_ACMDCRCERR)) {
        	if((intsts & MSDC_INT_ACMDCRCERR) && (host->autocmd == MSDC_AUTOCMD12) && (stop != NULL))
					stop->error =(unsigned int)-EIO;
			else{
            		cmd->error = (unsigned int)-EIO;
					IRQ_MSG("XXX CMD<%d> MSDC_INT_RSPCRCERR Arg<0x%.8x>",cmd->opcode, cmd->arg);
				}
            msdc_reset_hw(host->id); 
            if(intsts & MSDC_INT_ACMDCRCERR){
                IRQ_MSG("XXX CMD<%d> MSDC_INT_ACMDCRCERR",cmd->opcode);
            }   
        } else if ((intsts & MSDC_INT_CMDTMO) || (intsts & MSDC_INT_ACMDTMO)) {
        	if((intsts & MSDC_INT_ACMDTMO) && (host->autocmd == MSDC_AUTOCMD12) && (stop != NULL))
					stop->error =(unsigned int)-ETIMEDOUT;
			else{
            		cmd->error = (unsigned int)-ETIMEDOUT;
					IRQ_MSG("XXX CMD<%d> MSDC_INT_CMDTMO Arg<0x%.8x>",cmd->opcode, cmd->arg);
				}
            msdc_reset_hw(host->id); 
            if(intsts & MSDC_INT_ACMDTMO){
                IRQ_MSG("XXX CMD<%d> MSDC_INT_ACMDTMO",cmd->opcode);
            }     
        }
#ifdef RESUME_POLLING_MODE
        if(host->polling)
           g_intsts[host->id] = intsts;
        else{
			if(!(intsts & MSDC_INT_ACMDRDY))
           		complete(&host->cmd_done);
			else if((intsts & MSDC_INT_ACMDCRCERR) || (intsts & MSDC_INT_ACMDTMO))
				complete(&host->xfer_done); //Autocmd12 issued but error occur, the data transfer done INT will not issue,so cmplete is need here 
		}
#else
		if(!(intsts & MSDC_INT_ACMDRDY))
        complete(&host->cmd_done);
		else if((intsts & MSDC_INT_ACMDCRCERR) || (intsts & MSDC_INT_ACMDTMO))
			complete(&host->xfer_done); //Autocmd12 issued but error occur, the data transfer done INT will not issue,so cmplete is need here 
#endif
    }

    /* mmc irq interrupts */
    if (intsts & MSDC_INT_MMCIRQ) {
        //printk(KERN_INFO "msdc[%d] MMCIRQ: SDC_CSTS=0x%.8x\r\n", host->id, sdr_read32(SDC_CSTS));    
    }
    
    return IRQ_HANDLED;
}

/*--------------------------------------------------------------------------*/
/* platform_driver members                                                      */
/*--------------------------------------------------------------------------*/
/* called by msdc_drv_probe/remove */
static void msdc_enable_cd_irq(struct msdc_host *host, int enable)
{
    struct msdc_hw *hw = host->hw;
    u32 base = host->base;

    /* for sdio, not set */
    if ((hw->flags & MSDC_CD_PIN_EN) == 0) {
        /* Pull down card detection pin since it is not avaiable */
        /*
        if (hw->config_gpio_pin) 
            hw->config_gpio_pin(MSDC_CD_PIN, GPIO_PULL_DOWN);
        */
        sdr_clr_bits(MSDC_PS, MSDC_PS_CDEN);
        sdr_clr_bits(MSDC_INTEN, MSDC_INTEN_CDSC);
        sdr_clr_bits(SDC_CFG, SDC_CFG_INSWKUP);
        return;
    }

    N_MSG(CFG, "CD IRQ Eanable(%d)", enable);

    if (enable) {
        if (hw->enable_cd_eirq) { /* not set, never enter */
            hw->enable_cd_eirq();
        } else {
            /* card detection circuit relies on the core power so that the core power 
             * shouldn't be turned off. Here adds a reference count to keep 
             * the core power alive.
             */
            if (hw->config_gpio_pin) /* NULL */
                hw->config_gpio_pin(MSDC_CD_PIN, GPIO_PULL_UP);

            sdr_set_field(MSDC_PS, MSDC_PS_CDDEBOUNCE, DEFAULT_DEBOUNCE);
            sdr_set_bits(MSDC_PS, MSDC_PS_CDEN);
            sdr_set_bits(MSDC_INTEN, MSDC_INTEN_CDSC);
            sdr_set_bits(SDC_CFG, SDC_CFG_INSWKUP);  /* not in document! Fix me */
        }
    } else {
        if (hw->disable_cd_eirq) {
            hw->disable_cd_eirq();
        } else {

            if (hw->config_gpio_pin) /* NULL */
                hw->config_gpio_pin(MSDC_CD_PIN, GPIO_PULL_DOWN);

            sdr_clr_bits(SDC_CFG, SDC_CFG_INSWKUP);
            sdr_clr_bits(MSDC_PS, MSDC_PS_CDEN);
            sdr_clr_bits(MSDC_INTEN, MSDC_INTEN_CDSC);

            /* Here decreases a reference count to core power since card 
             * detection circuit is shutdown.
             */
        }
    }
}

/* called by msdc_drv_probe */
static void msdc_init_hw(struct msdc_host *host)
{
    u32 base = host->base;
    struct msdc_hw *hw = host->hw;

#ifdef MT6577_SD_DEBUG	
    msdc_reg[host->id] = (struct msdc_regs *)host->base;
#endif

    /* Power on */ 
    msdc_pin_reset(host, MSDC_PIN_PULL_UP); 
#ifndef FPGA_PLATFORM  
    enable_clock(PERI_MSDC0_PDN + host->id, "SD"); 
#endif
    host->core_clkon = 1;      
    /* Bug Fix: If clock is disabed, Version Register Can't be read. */    
    msdc_select_clksrc(host, hw->clk_src);     

    /* Configure to MMC/SD mode */
    sdr_set_field(MSDC_CFG, MSDC_CFG_MODE, MSDC_SDMMC); 
       
    /* Reset */
    msdc_reset_hw(host->id);

    /* Disable card detection */
    sdr_clr_bits(MSDC_PS, MSDC_PS_CDEN);

    /* Disable and clear all interrupts */
    sdr_clr_bits(MSDC_INTEN, sdr_read32(MSDC_INTEN));
    sdr_write32(MSDC_INT, sdr_read32(MSDC_INT));
    
#if 1
	/* reset tuning parameter */
    sdr_write32(MSDC_PAD_CTL0,   0x00090000);
    sdr_write32(MSDC_PAD_CTL1,   0x000A0000);
    sdr_write32(MSDC_PAD_CTL2,   0x000A0000);
    sdr_write32(MSDC_PAD_TUNE,   0x00000000);
    sdr_write32(MSDC_DAT_RDDLY0, 0x00000000);
    sdr_write32(MSDC_DAT_RDDLY1, 0x00000000);
    sdr_write32(MSDC_IOCON,      0x00000000);
    sdr_write32(MSDC_PATCH_BIT0, 0x003C000F); /* bit0 modified: Rx Data Clock Source: 1 -> 2.0*/

    if (sdr_read32(MSDC_ECO_VER) >= 4) { 
        if (host->id == SD_HOST_ID) {	
            sdr_set_field(MSDC_PATCH_BIT1, MSDC_PATCH_BIT1_WRDAT_CRCS, 2); 
            sdr_set_field(MSDC_PATCH_BIT1, MSDC_PATCH_BIT1_CMD_RSP,    1);
        }   

        if (!is_card_sdio(host)) {  /* internal clock: latch read data, not apply to sdio */           
            sdr_set_bits(MSDC_PATCH_BIT0, MSDC_PATCH_BIT_CKGEN_CK);
            host->hw->cmd_edge  = 0; // tuning from 0
            host->hw->data_edge = 0;            
        } else {
            if ( MSDC_TABDRV != (hw->flags & MSDC_TABDRV)) {
            sdr_set_bits(MSDC_PATCH_BIT0, MSDC_PATCH_BIT_CKGEN_CK);
                hw->clk_drv = 0;
                hw->cmd_drv = 0;
                hw->dat_drv = 0;
            }
        }
		//sdr_set_field(MSDC_IOCON, MSDC_IOCON_DDLSEL, 1);
    }   
#endif    

    /* for safety, should clear SDC_CFG.SDIO_INT_DET_EN & set SDC_CFG.SDIO in 
       pre-loader,uboot,kernel drivers. and SDC_CFG.SDIO_INT_DET_EN will be only
       set when kernel driver wants to use SDIO bus interrupt */
    /* Configure to enable SDIO mode. it's must otherwise sdio cmd5 failed */
    sdr_set_bits(SDC_CFG, SDC_CFG_SDIO);

    /* disable detect SDIO device interupt function */
    sdr_clr_bits(SDC_CFG, SDC_CFG_SDIOIDE);

    /* eneable SMT for glitch filter */
    sdr_set_bits(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKSMT);
    sdr_set_bits(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDSMT);
    sdr_set_bits(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATSMT);

    /* set clk, cmd, dat pad driving */

    sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVN, hw->clk_drv);
    sdr_set_field(MSDC_PAD_CTL0, MSDC_PAD_CTL0_CLKDRVP, hw->clk_drv);
    sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVN, hw->cmd_drv);
    sdr_set_field(MSDC_PAD_CTL1, MSDC_PAD_CTL1_CMDDRVP, hw->cmd_drv);
    sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVN, hw->dat_drv);
    sdr_set_field(MSDC_PAD_CTL2, MSDC_PAD_CTL2_DATDRVP, hw->dat_drv);
    msdc_drv_mode[host->id].clk_drv = hw->clk_drv;
    msdc_drv_mode[host->id].cmd_drv = hw->cmd_drv;
    msdc_drv_mode[host->id].dat_drv = hw->dat_drv;
    INIT_MSG("msdc drving<%d>", hw->dat_drv);
   
    /* write crc timeout detection */
    sdr_set_field(MSDC_PATCH_BIT0, 1 << 30, 1);

    /* Configure to default data timeout */
    sdr_set_field(SDC_CFG, SDC_CFG_DTOC, DEFAULT_DTOC);

    msdc_set_buswidth(host, MMC_BUS_WIDTH_1);

    N_MSG(FUC, "init hardware done!");
}

/* called by msdc_drv_remove */
static void msdc_deinit_hw(struct msdc_host *host)
{
    u32 base = host->base;

    /* Disable and clear all interrupts */
    sdr_clr_bits(MSDC_INTEN, sdr_read32(MSDC_INTEN));
    sdr_write32(MSDC_INT, sdr_read32(MSDC_INT));

    /* Disable card detection */
    msdc_enable_cd_irq(host, 0);
    msdc_set_power_mode(host, MMC_POWER_OFF);   /* make sure power down */
}

/* init gpd and bd list in msdc_drv_probe */
static void msdc_init_gpd_bd(struct msdc_host *host, struct msdc_dma *dma)
{
    gpd_t *gpd = dma->gpd; 
    bd_t  *bd  = dma->bd; 	
    bd_t  *ptr, *prev;
    
    /* we just support one gpd */     
    int bdlen = MAX_BD_PER_GPD;   	

    /* init the 2 gpd */
    memset(gpd, 0, sizeof(gpd_t) * 2);
    //gpd->next = (void *)virt_to_phys(gpd + 1); /* pointer to a null gpd, bug! kmalloc <-> virt_to_phys */  
    //gpd->next = (dma->gpd_addr + 1);    /* bug */
    gpd->next = (void *)((u32)dma->gpd_addr + sizeof(gpd_t));    

    //gpd->intr = 0;
    gpd->bdp  = 1;   /* hwo, cs, bd pointer */      
    //gpd->ptr  = (void*)virt_to_phys(bd); 
    gpd->ptr = (void *)dma->bd_addr; /* physical address */
    
    memset(bd, 0, sizeof(bd_t) * bdlen);
    ptr = bd + bdlen - 1;
    //ptr->eol  = 1;  /* 0 or 1 [Fix me]*/
    //ptr->next = 0;    
    
    while (ptr != bd) {
        prev = ptr - 1;
        prev->next = (void *)(dma->bd_addr + sizeof(bd_t) *(ptr - bd));
        ptr = prev;
    }
}

static void msdc_init_dma_latest_address(void)
{
    struct dma_addr *ptr, *prev; 
    int bdlen = MAX_BD_PER_GPD;   	

    memset(msdc_latest_dma_address, 0, sizeof(struct dma_addr) * bdlen);
    ptr = msdc_latest_dma_address + bdlen - 1;
    while(ptr != msdc_latest_dma_address){
       prev = ptr - 1; 
       prev->next = (void *)(msdc_latest_dma_address + sizeof(struct dma_addr) * (ptr - msdc_latest_dma_address));
       ptr = prev;
    }

}

struct gendisk	* mmc_get_disk(struct mmc_card *card)
{
	struct mmc_blk_data *md;
	//struct gendisk *disk;

	BUG_ON(!card);
	md = mmc_get_drvdata(card);
	BUG_ON(!md);
	BUG_ON(!md->disk);

	return md->disk;
}

#if defined(MTK_EMMC_SUPPORT) && defined(CONFIG_PROC_FS)
static struct proc_dir_entry *proc_emmc;

static inline int emmc_proc_info(char *buf, struct hd_struct *this)
{
	int i = 0;
    char *no_partition_name = "n/a";

    for (i = 0; i < PART_NUM; i++) {
		if (PartInfo[i].partition_idx != 0 && PartInfo[i].partition_idx == this->partno) {
			break;
	    }
    }			
	
	return sprintf(buf, "emmc_p%d: %8.8x %8.8x \"%s\"\n", this->partno,
		       (unsigned int)this->start_sect,
		       (unsigned int)this->nr_sects, (i >= PART_NUM ? no_partition_name : PartInfo[i].name));
}

static int emmc_read_proc (char *page, char **start, off_t off, int count,
			  int *eof, void *data_unused)
{
	int len, l;
	off_t   begin = 0;
	struct disk_part_iter piter;
	struct hd_struct *part;
	struct msdc_host *host;
	struct gendisk *disk;

        /* emmc always in slot0 */
	host = msdc_6577_host[EMMC_HOST_ID];
	BUG_ON(!host);
	BUG_ON(!host->mmc);
	BUG_ON(!host->mmc->card);
	disk = mmc_get_disk(host->mmc->card);
	
	len = sprintf(page, "partno:    start_sect   nr_sects  partition_name\n");
	disk_part_iter_init(&piter, disk, 0);
	while ((part = disk_part_iter_next(&piter))){
		l = emmc_proc_info(page + len, part);
                len += l;
                if (len+begin > off+count)
                        goto done;
                if (len+begin < off) {
                        begin += len;
                        len = 0;
                }
	}
	disk_part_iter_exit(&piter);
	printk("%s: %s \n", __func__, page);
	*eof = 1;

done:
	if (off >= len+begin)
        return 0;
	*start = page + (off-begin);
	return ((count < begin+len-off) ? count : begin+len-off);
}


#endif /* MTK_EMMC_SUPPORT && CONFIG_PROC_FS */



#if defined(MTK_EMMC_SUPPORT)
#include <linux/syscalls.h>

void emmc_create_sys_symlink (struct mmc_card *card)
{
	int i = 0;
	struct disk_part_iter piter;
	struct hd_struct *part;
	struct msdc_host *host;
	struct gendisk *disk;
	char link_target[256];
	char link_name[256];

    /* emmc always in slot0 */
	host = msdc_6577_host[EMMC_HOST_ID];
	BUG_ON(!host);

	if (host != (struct msdc_host *)card->host->private) {
            printk(KERN_INFO DRV_NAME "%s: NOT emmc card,  \n", __func__);
		return;
	}
	
	disk = mmc_get_disk(card);
	
	disk_part_iter_init(&piter, disk, 0);
	while ((part = disk_part_iter_next(&piter))){
	  for (i = 0; i < PART_NUM; i++) {
		if (PartInfo[i].partition_idx != 0 && PartInfo[i].partition_idx == part->partno) {
			sprintf(link_target, "/dev/block/%sp%d", disk->disk_name, part->partno);
			sprintf(link_name, "/emmc@%s", PartInfo[i].name);
			printk(KERN_INFO DRV_NAME "%s: target=%s, name=%s  \n", __func__, link_target, link_name);
			sys_symlink(link_target, link_name);
			break;
	    }
      }			
	}
	disk_part_iter_exit(&piter);

}

#endif /* MTK_EMMC_SUPPORT */

/* This is called by run_timer_softirq */
static void msdc_timer_pm(unsigned long data)
{
    struct msdc_host *host = (struct msdc_host *)data;   
    unsigned long flags;

    spin_lock_irqsave(&host->clk_gate_lock, flags);
    if (host->clk_gate_count == 0) { 
        msdc_clksrc_onoff(host, 0);
        N_MSG(CLK, "[%s]: msdc%d, time out, dsiable clock, clk_gate_count=%d\n", __func__, host->id, host->clk_gate_count);
    }else{
        printk("[%s]: msdc%d, time out, but clk_gate_count=%d\n", __func__, host->id, host->clk_gate_count);
    }           
    spin_unlock_irqrestore(&host->clk_gate_lock, flags); 
}

static u32 first_probe = 0; 
static int msdc_drv_probe(struct platform_device *pdev)
{
    struct mmc_host *mmc;
    struct resource *mem;
    struct msdc_host *host;
    struct msdc_hw *hw;
    unsigned long base;
    int ret, irq;
    struct irq_data l_irq_data;

    printk("Ivan MTK msdc_drv_probe \n");
#ifdef FPGA_PLATFORM    
    printk("[%s]: init PWR_GPIO_EO\n", __func__);
    //addr = ioremap(PWR_GPIO_EO, 4);
    sdr_write8(PWR_GPIO_EO, (PWR_MASK_VOL_33 | PWR_MASK_EN));
#endif
    if (first_probe == 0) {
        first_probe ++;        	

#ifndef FPGA_PLATFORM    
        // work aqround here. 
        enable_clock(PERI_MSDC0_PDN, "SD");        	
        enable_clock(PERI_MSDC1_PDN, "SD");       
        enable_clock(PERI_MSDC2_PDN, "SD");       
        enable_clock(PERI_MSDC3_PDN, "SD");       

        disable_clock(PERI_MSDC0_PDN, "SD");        	
        disable_clock(PERI_MSDC1_PDN, "SD");       
        disable_clock(PERI_MSDC2_PDN, "SD");       
        disable_clock(PERI_MSDC3_PDN, "SD");    
#endif     
        
    }

    if (pdev->id == 0) {
        msdc_sd0_power(1, VOL_3300);
    } else if (pdev->id == 1) {
        msdc_sd1_power(1, VOL_3300);    	
    }
      
    /* Allocate MMC host for this device */
    mmc = mmc_alloc_host(sizeof(struct msdc_host), &pdev->dev);
    if (!mmc) return -ENOMEM;

    hw   = (struct msdc_hw*)pdev->dev.platform_data;
    mem  = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    irq  = platform_get_irq(pdev, 0);
    base = mem->start;

    l_irq_data.irq = irq;

    BUG_ON((!hw) || (!mem) || (irq < 0));
    
    mem = request_mem_region(mem->start, mem->end - mem->start + 1, DRV_NAME);
    if (mem == NULL) {
        mmc_free_host(mmc);
        return -EBUSY;
    }

    /* Set host parameters to mmc */
    mmc->ops        = &mt_msdc_ops;
    mmc->f_min      = HOST_MIN_MCLK;
    mmc->f_max      = HOST_MAX_MCLK;
    mmc->ocr_avail  = MSDC_OCR_AVAIL;
    
    /* For sd card: MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE | MSDC_HIGHSPEED, 
       For sdio   : MSDC_EXT_SDIO_IRQ | MSDC_HIGHSPEED */
    if (hw->flags & MSDC_HIGHSPEED) {
        mmc->caps   = MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED;
    }
    if (hw->data_pins == 4) { /* current data_pins are all 4*/
        mmc->caps  |= MMC_CAP_4_BIT_DATA;
    } else if (hw->data_pins == 8) {
        mmc->caps  |= MMC_CAP_8_BIT_DATA | MMC_CAP_4_BIT_DATA;
    }
    if ((hw->flags & MSDC_SDIO_IRQ) || (hw->flags & MSDC_EXT_SDIO_IRQ))
        mmc->caps |= MMC_CAP_SDIO_IRQ;  /* yes for sdio */
		if(hw->flags & MSDC_UHS1){
				mmc->caps |= MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 |
				MMC_CAP_UHS_SDR50 | MMC_CAP_UHS_SDR104 | MMC_CAP_SET_XPC_180;
		}
		if(hw->flags & MSDC_DDR)
				mmc->caps |= MMC_CAP_UHS_DDR50;
		mmc->caps |= MMC_CAP_ERASE; 
    /* MMC core transfer sizes tunable parameters */
    //mmc->max_hw_segs   = MAX_HW_SGMTS;
    mmc->max_segs   = MAX_HW_SGMTS;
    //mmc->max_phys_segs = MAX_PHY_SGMTS;
    mmc->max_seg_size  = MAX_SGMT_SZ;
    mmc->max_blk_size  = HOST_MAX_BLKSZ;
    mmc->max_req_size  = MAX_REQ_SZ; 
    mmc->max_blk_count = mmc->max_req_size;

    host = mmc_priv(mmc);
    host->hw        = hw;
    host->mmc       = mmc;
    host->id        = pdev->id;
    host->error     = 0;
    host->irq       = irq;    
    host->base      = base;
    host->mclk      = 0;                   /* mclk: the request clock of mmc sub-system */
    host->hclk      = hclks[hw->clk_src];  /* hclk: clock of clock source to msdc controller */
    host->sclk      = 0;                   /* sclk: the really clock after divition */
    host->pm_state  = PMSG_RESUME;
    host->suspend   = 0;
    host->polling   = 0;
    host->core_clkon = 0;
    host->card_clkon = 0;    
    host->clk_gate_count = 0;  
    host->core_power = 0;
    host->power_mode = MMC_POWER_OFF;
    //host->card_inserted = hw->flags & MSDC_REMOVABLE ? 0 : 1;
    host->timeout_ns = 0;
    host->timeout_clks = DEFAULT_DTOC * 65536;
  	if(host->id == EMMC_HOST_ID || host->id == SD_HOST_ID)
		host->autocmd = 0;//MSDC_AUTOCMD12;
	else
		host->autocmd = 0;
    host->mrq = NULL; 
    //init_MUTEX(&host->sem); /* we don't need to support multiple threads access */
   
    host->dma.used_gpd = 0;
    host->dma.used_bd = 0;

    /* using dma_alloc_coherent*/  /* todo: using 1, for all 4 slots */
    host->dma.gpd = dma_alloc_coherent(NULL, MAX_GPD_NUM * sizeof(gpd_t), &host->dma.gpd_addr, GFP_KERNEL); 
    host->dma.bd =  dma_alloc_coherent(NULL, MAX_BD_NUM  * sizeof(bd_t),  &host->dma.bd_addr,  GFP_KERNEL); 
    BUG_ON((!host->dma.gpd) || (!host->dma.bd));    
    msdc_init_gpd_bd(host, &host->dma);
    msdc_clock_src[host->id] = hw->clk_src;
    msdc_host_mode[host->id] = mmc->caps;
    /*for emmc*/
    msdc_6577_host[pdev->id] = host;
	host->read_time_tune = 0;
    host->write_time_tune = 0;
	host->power_cycle = 0;
	host->sw_timeout = 0;
    tasklet_init(&host->card_tasklet, msdc_tasklet_card, (ulong)host);
    spin_lock_init(&host->lock);
    spin_lock_init(&host->clk_gate_lock);
    /* init dynamtic timer */
    init_timer(&host->timer);
    //host->timer.expires	= jiffies + HZ;
    host->timer.function	= msdc_timer_pm; 
    host->timer.data		= (unsigned long)host;
    
    msdc_init_hw(host);

    //mt65xx_irq_set_sens(irq, MT65xx_EDGE_SENSITIVE);
    //mt65xx_irq_set_polarity(irq, MT65xx_POLARITY_LOW);
    ret = request_irq((unsigned int)irq, msdc_irq, IRQF_TRIGGER_LOW, DRV_NAME, host);
    if (ret) goto release;
    //mt65xx_irq_unmask(&l_irq_data);
    //enable_irq(irq);
    
    if (hw->flags & MSDC_CD_PIN_EN) { /* not set for sdio */
        if (hw->request_cd_eirq) { /* not set for MT6577 */
            hw->request_cd_eirq(msdc_eirq_cd, (void*)host); /* msdc_eirq_cd will not be used! */
        }
    }

    if (hw->request_sdio_eirq) /* set to combo_sdio_request_eirq() for WIFI */
        hw->request_sdio_eirq(msdc_eirq_sdio, (void*)host); /* msdc_eirq_sdio() will be called when EIRQ */

    if (hw->register_pm) {/* yes for sdio */
        hw->register_pm(msdc_pm, (void*)host);  /* combo_sdio_register_pm() */
        if(hw->flags & MSDC_SYS_SUSPEND) { /* will not set for WIFI */
            ERR_MSG("MSDC_SYS_SUSPEND and register_pm both set");
        }
        mmc->pm_flags |= MMC_PM_IGNORE_PM_NOTIFY; /* pm not controlled by system but by client. */
    }
    
    platform_set_drvdata(pdev, mmc);

    /* Config card detection pin and enable interrupts */
    if (hw->flags & MSDC_CD_PIN_EN) {  /* set for card */
        msdc_enable_cd_irq(host, 1);
    } else {
        msdc_enable_cd_irq(host, 0);
    }  

    ret = mmc_add_host(mmc);
    if (ret) goto free_irq;
 	
    return 0;

free_irq:
    free_irq(irq, host);
release:
    platform_set_drvdata(pdev, NULL);
    msdc_deinit_hw(host);

    tasklet_kill(&host->card_tasklet);
    if (mem)
        release_mem_region(mem->start, mem->end - mem->start + 1);

    mmc_free_host(mmc);

    return ret;
}

/* 4 device share one driver, using "drvdata" to show difference */
static int msdc_drv_remove(struct platform_device *pdev)
{
    struct mmc_host *mmc;
    struct msdc_host *host;
    struct resource *mem;

    mmc  = platform_get_drvdata(pdev);
    BUG_ON(!mmc);
    
    host = mmc_priv(mmc);   
    BUG_ON(!host);

    ERR_MSG("removed !!!");

    platform_set_drvdata(pdev, NULL);
    mmc_remove_host(host->mmc);
    msdc_deinit_hw(host);

    tasklet_kill(&host->card_tasklet);
    free_irq(host->irq, host);

    dma_free_coherent(NULL, MAX_GPD_NUM * sizeof(gpd_t), host->dma.gpd, host->dma.gpd_addr);
    dma_free_coherent(NULL, MAX_BD_NUM  * sizeof(bd_t),  host->dma.bd,  host->dma.bd_addr);

    mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    if (mem)
        release_mem_region(mem->start, mem->end - mem->start + 1);

    mmc_free_host(host->mmc);

    return 0;
}

/* Fix me: Power Flow */
#ifdef CONFIG_PM
static int msdc_drv_suspend(struct platform_device *pdev, pm_message_t state)
{
    int ret = 0;
    struct mmc_host *mmc = platform_get_drvdata(pdev);
    struct msdc_host *host = mmc_priv(mmc);
     if (is_card_sdio(host)) return ret;
    if (mmc && state.event == PM_EVENT_SUSPEND && (host->hw->flags & MSDC_SYS_SUSPEND)) { /* will set for card */
        msdc_pm(state, (void*)host);
    }

    // WIFI slot should be off when enter suspend
    if (mmc && state.event == PM_EVENT_SUSPEND && (!(host->hw->flags & MSDC_SYS_SUSPEND))) {
        msdc_suspend_clock(host);
    }

    return ret;
}

static int msdc_drv_resume(struct platform_device *pdev)
{
    int ret = 0;
    struct mmc_host *mmc = platform_get_drvdata(pdev);
    struct msdc_host *host = mmc_priv(mmc);
    struct pm_message state;
    if (is_card_sdio(host)) return ret;
    state.event = PM_EVENT_RESUME;
    if (mmc && (host->hw->flags & MSDC_SYS_SUSPEND)) {/* will set for card */
        msdc_pm(state, (void*)host);
    }

    /* This mean WIFI not controller by PM */
    
    return ret;
}
#endif

static struct platform_driver mt_msdc_driver = {
    .probe   = msdc_drv_probe,
    .remove  = msdc_drv_remove,
#ifdef CONFIG_PM
    .suspend = msdc_drv_suspend,
    .resume  = msdc_drv_resume,
#endif
    .driver  = {
        .name  = DRV_NAME,
        .owner = THIS_MODULE,
    },
};

/*--------------------------------------------------------------------------*/
/* module init/exit                                                      */
/*--------------------------------------------------------------------------*/
static int __init mt_msdc_init(void)
{
    int ret;

    ret = platform_driver_register(&mt_msdc_driver);
    if (ret) {
        printk(KERN_ERR DRV_NAME ": Can't register driver");
        return ret;
    }

	#if defined(MTK_EMMC_SUPPORT) && defined(CONFIG_PROC_FS)
	if ((proc_emmc = create_proc_entry( "emmc", 0, NULL )))
		proc_emmc->read_proc = emmc_read_proc;
	#endif /* MTK_EMMC_SUPPORT && CONFIG_PROC_FS */

    printk(KERN_INFO DRV_NAME ": MediaTek MT6577 MSDC Driver");

    msdc_debug_proc_init();
    msdc_init_dma_latest_address();
    return 0;
}

static void __exit mt_msdc_exit(void)
{
    platform_driver_unregister(&mt_msdc_driver);
}

module_init(mt_msdc_init);
module_exit(mt_msdc_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MediaTek MT6577 SD/MMC Card Driver");
MODULE_AUTHOR("Infinity Chen <infinity.chen@mediatek.com>");
#ifdef MTK_EMMC_SUPPORT
EXPORT_SYMBOL(ext_csd);
#endif
EXPORT_SYMBOL(msdc_6577_host);
