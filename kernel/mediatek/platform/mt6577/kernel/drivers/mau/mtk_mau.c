#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/mm.h>
#include <linux/slab.h>

#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()¡¢kthread_run()

#include <linux/aee.h>


#include <mach/irqs.h>
//#include <asm/tcm.h>
#include <asm/io.h>


#include <mach/mtk_mau.h>
#include "mtk_mau_reg.h"
#include "mtk_mau_debug.h"



#include <mach/mt6577_typedefs.h>
#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_clock_manager.h>
#include <mach/mt6577_irq.h>
#include <mach/m4u.h>
#include <linux/xlog.h>

#if 0
#define MAU_PRINTF             printk
#define MAU_INFO(fmt, arg...)       MAU_PRINTF("[MAU INFO]: %s(): "fmt,__FUNCTION__, ##arg)
#define MAU_WARNING(fmt, arg...)    MAU_PRINTF("[MAU WARNING]: %s(): "fmt,__FUNCTION__, ##arg)
#define MAU_ERROR(fmt, arg...)      MAU_PRINTF("[MAU ERROR]: %s(): %s@%d: "fmt,__FUNCTION__, __FILE__,__LINE__, ##arg)


#define MAU_ASSERT(x) if(!(x)){printk("[MAU] assert fail, file:%s, line:%d", __FILE__, __LINE__);}
#endif

#define MAU_DBG(fmt, arg...)      xlog_printk( ANDROID_LOG_DEBUG, "MAU", "[DBG]: %s() "fmt"\n", __FUNCTION__, ##arg)
#define MAU_INFO(fmt, arg...)     xlog_printk( ANDROID_LOG_INFO, "MAU", "[INFO]: %s() "fmt"\n", __FUNCTION__, ##arg)
#define MAU_WARN(fmt, arg...)     xlog_printk( ANDROID_LOG_WARN, "MAU", "[WARN]: %s() "fmt"\n", __FUNCTION__, ##arg)
#define MAU_ERROR(fmt, arg...)    xlog_printk( ANDROID_LOG_ERROR, "MAU", "[ERR]: %s(): %s@%d "fmt"\n", __FUNCTION__, __FILE__,__LINE__,##arg)
#define MAU_FATAL(fmt, arg...)    xlog_printk( ANDROID_LOG_FATAL, "MAU", "[FATAL]: %s(): %s@%d "fmt"\n", __FUNCTION__, __FILE__,__LINE__,##arg)



#ifdef CONFIG_MT6575_FPGA
#define ENABLE_MAU_INTERRUPT 0
#else
#define ENABLE_MAU_INTERRUPT 1
#endif

#ifdef MTK_M4U_SUPPORT
  extern M4U_DEBUG_FUNCTION_STRUCT _m4u_debug_func;
#endif

#ifndef SLEEP_MILLI_SEC
#define SLEEP_MILLI_SEC(nMilliSec)\
    do { \
        long timeout = (nMilliSec) * HZ / 1000; \
        while(timeout > 0) \
        { \
            timeout = schedule_timeout(timeout); \
        } \
    } while(0)
#endif

#if ENABLE_MAU_INTERRUPT
#define ENABLE_MAU0_INTERRUPT(enable) \
    do { \
        unsigned int smi_ctl = ioread32(MAU0_SMI_LARB_CON);  \
        smi_ctl = enable ? (smi_ctl | (0x1<<10)) : (smi_ctl & ~(0x1<<10)); \
        iowrite32(smi_ctl, MAU0_SMI_LARB_CON_SET); \
    } while(0)

#define ENABLE_MAU1_INTERRUPT(enable) \
    do { \
        unsigned int smi_ctl = ioread32(MAU1_SMI_LARB_CON);  \
        smi_ctl = enable ? (smi_ctl | (0x1<<10)) : (smi_ctl & ~(0x1<<10)); \
        iowrite32(smi_ctl, MAU1_SMI_LARB_CON_SET); \
    } while(0)

#define ENABLE_MAU2_INTERRUPT(enable) \
    do { \
        unsigned int smi_ctl = ioread32(MAU2_SMI_LARB_CON);  \
        smi_ctl = enable ? (smi_ctl | (0x1<<10)) : (smi_ctl & ~(0x1<<10)); \
        iowrite32(smi_ctl, MAU2_SMI_LARB_CON_SET); \
    } while(0)

#define ENABLE_MAU3_INTERRUPT(enable) \
    do { \
        unsigned int smi_ctl = ioread32(MAU3_SMI_LARB_CON);  \
        smi_ctl = enable ? (smi_ctl | (0x1<<10)) : (smi_ctl & ~(0x1<<10)); \
        iowrite32(smi_ctl, MAU3_SMI_LARB_CON_SET); \
    } while(0)
#else
#define ENABLE_MAU0_INTERRUPT(enable)
#define ENABLE_MAU1_INTERRUPT(enable)
#define ENABLE_MAU2_INTERRUPT(enable)
#define ENABLE_MAU3_INTERRUPT(enable)
#endif

void RESET_MAU_INTERRUPT(MTK_MAU_ID mauID)
{
    UINT32 LARB_CON, LARB_RESET;
	unsigned int smi_ctl;
    
	switch(mauID) {
        case MAU_ID_0:
            LARB_CON = MAU0_SMI_LARB_CON;
            LARB_RESET= MAU0_SMI_LARB_CON_RESET;
            break;
        case MAU_ID_1:
            LARB_CON = MAU1_SMI_LARB_CON;
            LARB_RESET= MAU1_SMI_LARB_CON_RESET;
            break;
        case MAU_ID_2:
            LARB_CON = MAU2_SMI_LARB_CON;
            LARB_RESET= MAU2_SMI_LARB_CON_RESET;
            break;
        case MAU_ID_3:
            LARB_CON = MAU3_SMI_LARB_CON;
            LARB_RESET= MAU3_SMI_LARB_CON_RESET;
            break;
        default:
            return;
    }
    smi_ctl = ioread32(LARB_CON);
    smi_ctl = (smi_ctl & ~(0x1<<10));
    iowrite32(smi_ctl, LARB_RESET);

}


PMAU_REGS const MAU0_REG = (PMAU_REGS)(MAU0_REG_BASE);
PMAU_REGS const MAU1_REG = (PMAU_REGS)(MAU1_REG_BASE);
PMAU_REGS const MAU2_REG = (PMAU_REGS)(MAU2_REG_BASE);
PMAU_REGS const MAU3_REG = (PMAU_REGS)(MAU3_REG_BASE);


//static bool is_mau_isr_log_on = true;
static bool is_mau_isr_log_on = false;
//static bool is_mau_enabled =false;


typedef struct
{
    bool         	isEnabled;
    MTK_MAU_CONFIG  entryConf[3];
    unsigned long   violationLARB0Cnt[3][MAU0_MASK_ALL];
    unsigned long   violationLARB1Cnt[3][MAU1_MASK_ALL];
    unsigned long   violationLARB2Cnt[3][MAU2_MASK_ALL];
    unsigned long   violationLARB3Cnt[3][MAU3_MASK_ALL];
} _MAU_CONTEXT;



static _MAU_CONTEXT mau_ctxt = {0};


/*****************************************************************************/
static char const* const Mau0PortName[] = {
	  "rsv",
    "defect",
    "jpg_enc",
    "rot_dma0_out0",
    "rot_dma1_out0",
    "tv_rot_out0",
    "cam",
    "fd0",
    "fd2",
    "jpg_dec0",
    "r_dma0_out0",
    "r_dma0_out1",
    "r_dma0_out2",
    "fd1",
    "pca",
    "jpgdma_r",
    "jpgdma_w",
    "rot_dma0_out1",
    "rot_dma0_out2",
    "rot_dma1_out1",
    "rot_dma1_out2",
    "tv_rot_out1",
    "tv_rot_out2",
    "r_dma0_out3",
    "jpg_dec1",
    "tv_rot_out3",
    "rot_dma0_out3",
    "rot_dma1_out3",  //27
};

static char const* const Mau1PortName[] = {
	  "rsv",
    "ovl_msk",
    "ovl_dcp",
    "dpi",
    "rot_dma2_out0",
    "rot_dma3_out0",
    "rot_dma4_out0",
    "tvc",
    "lcd_r",
    "lcd_w",
    "r_dma1_out0",
    "r_dma1_out1",
    "r_dma1_out2",
    "spi",
    "spi",
    "dpi_hwc",
    "vrz",
    "rot_dma2_out1",
    "rot_dma2_out2",
    "rot_dma3_out1",
    "rot_dma3_out2",
    "rot_dma4_out1",
    "rot_dma4_out2",
    "greq_blkw ",
    "greq_blkr",
    "tvc_pfh",
    "tvc_resz",
    "r_dma1_out3",
    "eis",
    "rot_dma2_out3",
    "rot_dma3_out3",
    "rot_dma4_out3", //30
};

static char const* const Mau2PortName[] = {
	  "rsv",
    "venc_mc",
    "venc_bsdma",
    "venc_mvqp",
    "vdec_dma",
    "vdec_rec",
    "vdec_post0",
    "vdec_post1",
    "vdec_acp",
    "venc_mc_acp",  //9
};

static char const* const Mau3PortName[] = {
	  "rsv",
    "g2d_r",
    "g2d_w",
    "audio",
    "audio",  //4
};



static MAU_REGS mau0RegBackup;
static MAU_REGS mau1RegBackup;
static MAU_REGS mau2RegBackup;
static MAU_REGS mau3RegBackup;


static bool mau0IntrEnableBackup = false;
static bool mau1IntrEnableBackup = false;
static bool mau2IntrEnableBackup = false;
static bool mau3IntrEnableBackup = false;


#define MAU_REG_OFFSET(r)       offsetof(MAU_REGS, r)
#define REG_ADDR(base, offset)  (((BYTE *)(base)) + (offset))

const UINT32 BACKUP_MAU_REG_OFFSETS[] =
{
    MAU_REG_OFFSET(ENT0_STR),
    MAU_REG_OFFSET(ENT0_END),
    MAU_REG_OFFSET(ENT0_GID),
    MAU_REG_OFFSET(ENT1_STR),
    MAU_REG_OFFSET(ENT1_END),
    MAU_REG_OFFSET(ENT1_GID),
    MAU_REG_OFFSET(ENT2_STR),
    MAU_REG_OFFSET(ENT2_END),
    MAU_REG_OFFSET(ENT2_GID),
    MAU_REG_OFFSET(INTERRUPT),
};

/*****************************************************************************/




static PMAU_REGS _get_mau_reg_backup(MTK_MAU_ID mauID)
{
    switch (mauID)
    {
        case MAU_ID_0:
            return &mau0RegBackup;
        case MAU_ID_1:
            return &mau1RegBackup;
        case MAU_ID_2:
            return &mau2RegBackup;
        case MAU_ID_3:
            return &mau3RegBackup;
        default:
            MAU_ERROR("Invalid MAU ID %d", mauID);
            return NULL;
    }
}

static UINT32 _get_mau_reg_base(MTK_MAU_ID mauID)
{
    switch (mauID)
    {
        case MAU_ID_0:
            return MAU0_REG_BASE;
        case MAU_ID_1:
            return MAU1_REG_BASE;
        case MAU_ID_2:
            return MAU2_REG_BASE;
        case MAU_ID_3:
            return MAU3_REG_BASE;
        default:
            MAU_ERROR("wrong MAU ID %d\n", mauID);
            return 0;
    }
}


static PMAU_REGS _get_mau_reg(MTK_MAU_ID mauID)
{
    switch (mauID)
    {
        case MAU_ID_0:
            return MAU0_REG;
        case MAU_ID_1:
            return MAU1_REG;
        case MAU_ID_2:
            return MAU2_REG;
        case MAU_ID_3:
            return MAU3_REG;
        default:
            MAU_ERROR("Invalid MAU ID %d", mauID);
            return NULL;
    }
}


static void _enable_mau_interrupt(MTK_MAU_ID mauID, BOOL en)
{
    switch (mauID)
    {
        case MAU_ID_0:
            ENABLE_MAU0_INTERRUPT(en);
            break;
        case MAU_ID_1:
            ENABLE_MAU1_INTERRUPT(en);
            break;
        case MAU_ID_2:
            ENABLE_MAU2_INTERRUPT(en);
            break;
        case MAU_ID_3:
            ENABLE_MAU3_INTERRUPT(en);
            break;
        default:
            MAU_ERROR("Invalid MAU ID %d", mauID);
            return;
    }
    return;
}

static void _mau_interrupt_backup(MTK_MAU_ID mauID)
{
    unsigned int smi_ctl;  
    switch (mauID)
    {
        case MAU_ID_0:
            smi_ctl = ioread32(MAU0_SMI_LARB_CON);  
            if ( (smi_ctl & (0x1<<10)) == (0x1 << 10))
                mau0IntrEnableBackup = true;
            else
                mau0IntrEnableBackup = false;
            break;
        case MAU_ID_1:
            smi_ctl = ioread32(MAU1_SMI_LARB_CON);  
            if ( (smi_ctl & (0x1<<10)) == (0x1 << 10))
                mau1IntrEnableBackup = true;
            else
                mau1IntrEnableBackup = false;
            break;
        case MAU_ID_2:
            smi_ctl = ioread32(MAU2_SMI_LARB_CON);  
            if ( (smi_ctl & (0x1<<10)) == (0x1 << 10))
                mau2IntrEnableBackup = true;
            else
                mau2IntrEnableBackup = false;
            break;
        case MAU_ID_3:
            smi_ctl = ioread32(MAU3_SMI_LARB_CON);  
            if ( (smi_ctl & (0x1<<10)) == (0x1 << 10))
                mau3IntrEnableBackup = true;
            else
                mau3IntrEnableBackup = false;            
            break;
        default:
            MAU_ERROR("Invalid MAU ID %d", mauID);
            return;
    }
    return;
}
static void _mau_interrupt_restore(MTK_MAU_ID mauID)
{
    switch (mauID)
    {
        case MAU_ID_0:
            ENABLE_MAU0_INTERRUPT(mau0IntrEnableBackup);
            break;
        case MAU_ID_1:
            ENABLE_MAU1_INTERRUPT(mau1IntrEnableBackup);
            break;
        case MAU_ID_2:
            ENABLE_MAU2_INTERRUPT(mau2IntrEnableBackup);
            break;
        case MAU_ID_3:
            ENABLE_MAU3_INTERRUPT(mau3IntrEnableBackup);
            break;
        default:
            MAU_ERROR("Invalid MAU ID %d", mauID);
            return;
    }
    return;
}

static UINT32 _select_mau_mask(MTK_MAU_ID mauID, MTK_MAU_CONFIG* pMauConf)
{
    switch (mauID)
    {
        case MAU_ID_0:
            return pMauConf->InvalidMasterLARB0;
        case MAU_ID_1:
            return pMauConf->InvalidMasterLARB1;
        case MAU_ID_2:
            return pMauConf->InvalidMasterLARB2;
        case MAU_ID_3:
            return pMauConf->InvalidMasterLARB3;
        default:
            MAU_ERROR("Invalid MAU ID %d", mauID);
            return -1;
    }
}

static const char* _get_port_name(MTK_MAU_ID mauID, UINT32 astID)
{
    switch (mauID)
    {
        case MAU_ID_0:
            return Mau0PortName[astID];
        case MAU_ID_1:
            return Mau1PortName[astID];
        case MAU_ID_2:
            return Mau2PortName[astID];
        case MAU_ID_3:
            return Mau3PortName[astID];
        default:
            MAU_ERROR("Invalid MAU ID %d", mauID);
            return NULL;
    }
}

static void _print_port_working_addr(MTK_MAU_ID mauID, UINT32 astID)
{
    MAU_DBG("print (%s) working address", _get_port_name(mauID, astID));

    switch (mauID)
    {
        case MAU_ID_0:
            switch (astID)
            {
                case MAU0_MASK_RSV_0 :
                    MAU_ERROR();
                    break;
                case MAU0_MASK_DEFECT:
                    MAU_INFO("TG RDMA R, CAM_INADDR=0x%x ", ioread32(CAMINF_BASE+0x0028));
                    MAU_INFO("Cam WDMA W, CAM_OUTADDR=0x%x ", ioread32(CAMINF_BASE+0x002C));
                    MAU_INFO("RawCodec DMA R/W, COM_RESULT_ADDR=0x%x, COM_RESULT_LEN_ADDR=0x%x, COM_BASE_ADDR=0x%x, COM_BASE_LEN_ADDR=0x%x",
                      ioread32(CAMINF_BASE+0x0644), ioread32(CAMINF_BASE+0x0648), ioread32(CAMINF_BASE+0x064C), ioread32(CAMINF_BASE+0x0650));
                    break;
                case MAU0_MASK_JPE_ENC:
                    MAU_INFO("JPG_ENC_DST_ADDR0=0x%x ", ioread32(0xF209B120));
                    break;
                case MAU0_MASK_ROT_DMA0_OUT0:
                    MAU_INFO("VDO_ROT0, VDO_ROT0_Y=0x%x, VDO_ROT0_U=0x%x, VDO_ROT0_V=0x%x ",
                        ioread32(VDO_ROT0_BASE+0x318), ioread32(VDO_ROT0_BASE+0x320), ioread32(VDO_ROT0_BASE+0x328));
                    break;
                case MAU0_MASK_ROT_DMA1_OUT0:
                   MAU_INFO("RGB_ROT0, RGB_ROT0_Y=0x%x, RGB_ROT0_U=0x%x, RGB_ROT0_V=0x%x ",
                        ioread32(RGB_ROT0_BASE+0x318), ioread32(RGB_ROT0_BASE+0x320), ioread32(RGB_ROT0_BASE+0x328));

                    break;
                case MAU0_MASK_TV_ROT_OUT0:
                    MAU_INFO("TV_ROT_Y_DST_STR_ADDR=0x%x ", ioread32(0xF209F318));
                    break;
                case MAU0_MASK_CAM:
                    MAU_INFO("CAM_INADDR(R)=0x%x, CAM_OUTADDR(W)=0x%x ", ioread32(CAMINF_BASE+0x28), ioread32(CAMINF_BASE+0x2C));
                    break;
                case MAU0_MASK_FD0:
                    MAU_INFO("FDVT_SRC_IMG_BASE_ADDR (R)=0x%x ", ioread32(FDVT_BASE+0x164));
                    break;
                case MAU0_MASK_FD1:
                case MAU0_MASK_FD2:
                    MAU_INFO("FDVT_RSCON_ADR (R)=0x%x, FDVT_FD_CON_BASE_ADR (R)=0x%x, FDVT_FD_RLT_BASE_ADR (R,W)=0x%x, "
                        "FDVT_TC_RLT_BASE_ADR(W)=0x%x, LD_BADR_0(R)=0x%x, LD_BADR_1 (R)=0x%x, LD_BADR_2(R)=0x%x, "
                        "LD_BADR_3 (R)=0x%x, LD_BADR_4(R)=0x%x, LD_BADR_5(R)=0x%x, LD_BADR_6(R)=0x%x, LD_BADR_7(R)=0x%x ",
                        ioread32(FDVT_BASE+0xC ), ioread32(FDVT_BASE+0x80), ioread32(FDVT_BASE+0x9C),
                        ioread32(FDVT_BASE+0xA0), ioread32(FDVT_BASE+0x54), ioread32(FDVT_BASE+0x58),
                        ioread32(FDVT_BASE+0x5C), ioread32(FDVT_BASE+0x60), ioread32(FDVT_BASE+0x64),
                        ioread32(FDVT_BASE+0x68), ioread32(FDVT_BASE+0x6C), ioread32(FDVT_BASE+0x70));
                    break;
                case MAU0_MASK_JPG_DEC0:
                    MAU_INFO("JPG_DEC_FILE_ADDR=0x%x ", ioread32(0xF209B000));
                    break;
                case MAU0_MASK_R_DMA0_OUT0:
                    MAU_INFO("RDMA0_Y=0x%x ", ioread32(R_DMA0_BASE+0x340));
                    break;
                case MAU0_MASK_R_DMA0_OUT1:
                    MAU_INFO("RDMA0_U=0x%x ", ioread32(R_DMA0_BASE+0x344));
                    break;
                case MAU0_MASK_R_DMA0_OUT2:
                    MAU_INFO("RDMA0_V=0x%x ", ioread32(R_DMA0_BASE+0x348));
                    break;

                case MAU0_MASK_PCA:
                    MAU_INFO("PCA: STNR_INPUT0 =0x%x, STNR_INPUT1 =0x%x, STNR_INPUT2 =0x%x, "
                        "STNR_INPUT4 =0x%x, STNR_BASE1  =0x%x, STNR_BASE2  =0x%x, "
                        "STNR_RESULT1=0x%x, STNR_RESULT2=0x%x ",
                        ioread32(CAMINF_BASE+0x06C0), ioread32(CAMINF_BASE+0x06C4), ioread32(CAMINF_BASE+0x06C8),
                        ioread32(CAMINF_BASE+0x06D0), ioread32(CAMINF_BASE+0x06B4), ioread32(CAMINF_BASE+0x06B8),
                        ioread32(CAMINF_BASE+0x06D4), ioread32(CAMINF_BASE+0x06D8));
                    break;
                case MAU0_MASK_JPGDMA_R:  // 15
                case MAU0_MASK_JPGDMA_W:  // 15
                case MAU0_MASK_ROT_DMA0_OUT1:// 15
                case MAU0_MASK_ROT_DMA0_OUT2:// 15
                case MAU0_MASK_ROT_DMA1_OUT1:// 15
                case MAU0_MASK_ROT_DMA1_OUT2:// 15
                case MAU0_MASK_TV_ROT_OUT1:  // no use
                case MAU0_MASK_TV_ROT_OUT2:  // no use
                case MAU0_MASK_R_DMA0_OUT3: // no use
                case MAU0_MASK_JPG_DEC1:    // 15
                    MAU_ERROR();
                    break;
                default:
                    MAU_ERROR();
                    break;

            }
            break;
       case MAU_ID_1:
            switch (astID)
            {
                case MAU1_MASK_RSV_0:
                case MAU1_MASK_OVL_MSK:    // 1
                case MAU1_MASK_OVL_DCP:
                    MAU_ERROR();
                    break;
                case MAU1_MASK_DPI:
                    MAU_INFO("DPI buf0=0x%x, buf1=0x%x, buf2=0x%x ",
                        ioread32(DPI_BASE+0x30), ioread32(DPI_BASE+0x38), ioread32(DPI_BASE+0x40));
                    break;
                case MAU1_MASK_ROT_DMA2_OUT0:
                    MAU_INFO("RGB_ROT1, RGB_ROT1_Y=0x%x, RGB_ROT1_U=0x%x, RGB_ROT1_V=0x%x ",
                        ioread32(RGB_ROT1_BASE+0x318), ioread32(RGB_ROT1_BASE+0x320), ioread32(RGB_ROT1_BASE+0x328));
                    break;
                case MAU1_MASK_ROT_DMA3_OUT0:
                    MAU_INFO("VDO_ROT1, VDO_ROT1_Y=0x%x, VDO_ROT1_U=0x%x, VDO_ROT1_V=0x%x ",
                        ioread32(VDO_ROT1_BASE+0x318), ioread32(VDO_ROT1_BASE+0x320), ioread32(VDO_ROT1_BASE+0x328));
                    break;
                case MAU1_MASK_ROT_DMA4_OUT0:
                    MAU_INFO("RGB_ROT2, RGB_ROT2_Y=0x%x, RGB_ROT2_U=0x%x, RGB_ROT2_V=0x%x ",
                        ioread32(RGB_ROT2_BASE+0x318), ioread32(RGB_ROT2_BASE+0x320), ioread32(RGB_ROT2_BASE+0x328));
                    break;
                case MAU1_MASK_TVC:
                    MAU_INFO("TVC: TVC_YADR_TSRC=0x%x, TVC_UADR_TSRC=0x%x, TVC_VADR_TSRC=0x%x ",
                        ioread32(0xF209D00C), ioread32(0xF209D010), ioread32(0xF209D014));
                    break;
                case MAU1_MASK_LCD_R:
                    MAU_INFO("LCD_LAYER_0=0x%x, LCD_LAYER_1=0x%x, LCD_LAYER_2=0x%x, LCD_LAYER_3=0x%x, "
                        "LCD_LAYER_4=0x%x, LCD_LAYER_5=0x%x,  ",
                        ioread32(0xF20A10BC), ioread32(0xF20A10EC), ioread32(0xF20A111C),
                        ioread32(0xF20A114C), ioread32(0xF20A117C), ioread32(0xF20A11AC));
                    break;
                case MAU1_MASK_LCD_W:
                    MAU_INFO("LCD_W: lcd_w2m_fb0=0x%x, lcd_w2m_fb1=0x%x, lcd_w2m_fb2=0x%x ",
                        ioread32(0xF20A1060), ioread32(0xF20A1064), ioread32(0xF20A1068));
                    break;
                case MAU1_MASK_R_DMA1_OUT0:
                    MAU_INFO("RDMA1_Y=0x%x ", ioread32(R_DMA1_BASE+0x340));
                    break;
                case MAU1_MASK_R_DMA1_OUT1:
                    MAU_INFO("RDMA1_U=0x%x ", ioread32(R_DMA1_BASE+0x344));
                    break;
                case MAU1_MASK_R_DMA1_OUT2:
                    MAU_INFO("RDMA1_V=0x%x ", ioread32(R_DMA1_BASE+0x348));
                    break;
                case MAU1_MASK_SPI:
                case MAU1_MASK_RSV_14:
                case MAU1_MASK_DPI_HWC:
                case MAU1_MASK_VRZ:            //16
                case MAU1_MASK_ROT_DMA2_OUT1:  //20
                case MAU1_MASK_ROT_DMA2_OUT2:  //20
                case MAU1_MASK_ROT_DMA3_OUT1:  //20
                case MAU1_MASK_ROT_DMA3_OUT2:  //20
                case MAU1_MASK_ROT_DMA4_OUT1:  //20
                case MAU1_MASK_ROT_DMA4_OUT2:  //20
                case MAU1_MASK_GREQ_BLKW:      //17
                case MAU1_MASK_GREQ_BLKR:      //18
                case MAU1_MASK_TVC_PFH:      //no use
                case MAU1_MASK_TVC_RESZ:      //no use
                case MAU1_MASK_R_DMA1_OUT3:      //no use
                case MAU1_MASK_EIS:          //19
                    MAU_ERROR();
                    break;
                default:
                    MAU_ERROR();
                    break;
            }
       case MAU_ID_2:
            switch (astID)
            {
                case MAU2_MASK_RSV_0:
                    MAU_ERROR();
                    break;
                case MAU2_MASK_VENC_MC:
                    MAU_INFO("VENC_MC, REFADDR_Y=0x%x, REFADDR_U=0x%x, REFADDR_V=0x%x ",
                        ioread32(VENC_BASE+0x54), ioread32(VENC_BASE+0x58), ioread32(VENC_BASE+0x5c));
                    break;
                case MAU2_MASK_VENC_BSDMA:
                    MAU_INFO("VENC_BSDMA, VENC_BITADR=0x%x ", ioread32(VENC_BASE+0x60));
                    break;
                case MAU2_MASK_VENC_MVQP:
                    MAU_INFO("VENC_MVQP, SIDE_ADDR=0x%x ", ioread32(VENC_BASE+0x38));
                    break;
                case MAU2_MASK_VENC_DMA:
                    MAU_INFO("VDEC_DMA, SRCADDR_Y=0x%x, SRCADDR_U=0x%x, SRCADDR_V=0x%x ",
                        ioread32(VENC_BASE+0x3c), ioread32(VENC_BASE+0x40), ioread32(VENC_BASE+0x44));
                    break;
                case MAU2_MASK_VENC_REC:
                    MAU_INFO("VENC_REC, RECADDR_Y=0x%x, RECADDR_U=0x%x, RECADDR_V=0x%x ",
                        ioread32(VENC_BASE+0x48), ioread32(VENC_BASE+0x4c), ioread32(VENC_BASE+0x50));
                    break;
                case MAU2_MASK_VENC_POST0:
                    MAU_INFO("VDEC_REC, RECADDR_Y=0x%x, RECADDR_U=0x%x, RECADDR_V=0x%x ",
                        ioread32(VDEC_BASE+0x30), ioread32(VDEC_BASE+0x34), ioread32(VDEC_BASE+0x38));
                    MAU_INFO("VDEC_DB_YADDR=0x%x, VDEC_DB_UADDR=0x%x, VDEC_DB_VADDR=0x%x ",
                        ioread32(VDEC_BASE +0x3C ), ioread32(VDEC_BASE +0x40 ), ioread32(VDEC_BASE +0x44 ));
                    MAU_INFO("VDEC_MBIF_ADDR=0x%x, VDEC_IDMA_PAC_ADDR=0x%x, VDEC_POST_DM_MB_BASE=0x%x ",
                        ioread32(VDEC_BASE +0x48 ), ioread32(VDEC_BASE +0x84 ), ioread32(VDEC_BASE +0xFF8 ));
                    break;
                case MAU2_MASK_VENC_POST1:
                    MAU_ERROR();
                    break;
                default:
                    MAU_ERROR();
                    break;
            }
            break;
       case MAU_ID_3:
            switch (astID)
            {
                case MAU3_MASK_RSV_0:
                    MAU_ERROR();
                    break;
                case MAU3_MASK_G2D_R:
                    MAU_INFO("G2D_R: L0_ADDR=0x%x ", ioread32(0xF20C5084));
                    MAU_INFO("G2D_R: L1_ADDR=0x%x ", ioread32(0xF20C50C4));
                    break;
                case MAU3_MASK_G2D_W:
                    MAU_INFO("G2D_W: W2M_ADDR=0x%x ", ioread32(0xF20C5044));
                    break;
                case MAU3_MASK_AUDIO:
                    MAU_ERROR();
                    break;
                default:
                    MAU_ERROR();
                    break;
            }
            break;
       default:
            MAU_ERROR();
            break;
    }
}

static void mau_reset_ctxt(void)
{
	MAU_DBG();
    memset(&mau_ctxt, 0, sizeof(mau_ctxt));
}

static void mtk_mau_stop(MTK_MAU_ID mauID, MTK_MAU_ENTRY eID)
{
    PMAU_REGS MAU_REG = _get_mau_reg(mauID);
    MAU_REG_RANGE_START config = {0};
    config.RD = 0;
    config.WR = 0;

    if (MAU_REG == NULL)
    {
        MAU_ERROR("invalid MAU REG addr");
        return;
    }

    switch (eID) {
        case MAU_ENTRY_0:
            OUTREG32(&MAU_REG->ENT0_STR, AS_UINT32(&config));
            break;
        case MAU_ENTRY_1:
            OUTREG32(&MAU_REG->ENT1_STR, AS_UINT32(&config));
            break;
        case MAU_ENTRY_2:
            OUTREG32(&MAU_REG->ENT2_STR, AS_UINT32(&config));
            break;
        case MAU_ENTRY_ALL:
            OUTREG32(&MAU_REG->ENT0_STR, AS_UINT32(&config));
            OUTREG32(&MAU_REG->ENT1_STR, AS_UINT32(&config));
            OUTREG32(&MAU_REG->ENT2_STR, AS_UINT32(&config));
            OUTREG32(&MAU_REG->INTERRUPT, 0x0);

            _enable_mau_interrupt(mauID, false);

            mau_reset_ctxt();
            break;
        default:
            MAU_ERROR("Invalid entry ID");
            break;


    }

}


static int mtk_mau_config(MTK_MAU_ID mauID, MTK_MAU_CONFIG* pMauConf)
{
	PMAU_REGS MAU_REG;
    
    MAU_INFO("mau:%d ID:%d en:%d m:%d "
        "{0x%x 0x%x 0x%x 0x%x} "
        "R:%x W:%x [0x%x 0x%x]",
        mauID,
        pMauConf->EntryID,
        pMauConf->Enable,
        pMauConf->Mode,
        pMauConf->InvalidMasterLARB0,
        pMauConf->InvalidMasterLARB1,
        pMauConf->InvalidMasterLARB2,
        pMauConf->InvalidMasterLARB3,
        pMauConf->ReadEn,
        pMauConf->WriteEn,
        pMauConf->StartAddr,
        pMauConf->EndAddr);

    MAU_REG = _get_mau_reg(mauID);

    if (MAU_REG == NULL)
    {
        MAU_ERROR("invalid MAU REG addr");
        return -1;
    }

    memcpy(&mau_ctxt.entryConf[pMauConf->EntryID], pMauConf, sizeof(MTK_MAU_CONFIG));

    if (pMauConf->EndAddr < pMauConf->StartAddr) {
		MAU_ERROR("Invalid Param");
		return -1;
	}

    if (pMauConf->Enable == false) {
        mtk_mau_stop(mauID, pMauConf->EntryID);
    }
    else {

        MAU_REG_RANGE_START rangeStart = {0};
        MAU_REG_RANGE_END   rangeEnd;
        UINT32 portMask;


        rangeStart.RD = pMauConf->ReadEn;
        rangeStart.WR = pMauConf->WriteEn;
        rangeStart.STR = (pMauConf->StartAddr)>>2;

        rangeEnd.END = (pMauConf->EndAddr)>>2;
        rangeEnd.VP_CTRL = pMauConf->Mode;

        portMask = _select_mau_mask(mauID, pMauConf);

        switch (pMauConf->EntryID) {
            case MAU_ENTRY_0:
                OUTREG32(&MAU_REG->ENT0_STR, AS_UINT32(&rangeStart));
                OUTREG32(&MAU_REG->ENT0_END, AS_UINT32(&rangeEnd));
                OUTREG32(&MAU_REG->ENT0_GID, AS_UINT32(&portMask));
                break;
            case MAU_ENTRY_1:
                OUTREG32(&MAU_REG->ENT1_STR, AS_UINT32(&rangeStart));
                OUTREG32(&MAU_REG->ENT1_END, AS_UINT32(&rangeEnd));
                OUTREG32(&MAU_REG->ENT1_GID, AS_UINT32(&portMask));
                break;
            case MAU_ENTRY_2:
                OUTREG32(&MAU_REG->ENT2_STR, AS_UINT32(&rangeStart));
                OUTREG32(&MAU_REG->ENT2_END, AS_UINT32(&rangeEnd));
                OUTREG32(&MAU_REG->ENT2_GID, AS_UINT32(&portMask));
                break;
            default:
                MAU_ERROR("invalid entry ID");
                break;
        }


        _enable_mau_interrupt(mauID, true);
        //MAU_Mau2DumpReg();

    }
    return 0;
}



static void _mau_isr(MTK_MAU_ID mauID)
{

    PMAU_REGS MAU_REG = _get_mau_reg(mauID);

    if (MAU_REG == NULL)
    {
        MAU_ERROR("invalid MAU REG addr");
        return;
    }

    if (MAU_REG->ENT0_STATUS.ASSERT) {
    	//mau_ctxt.violationLARB0Cnt[0][MAU_REG->ENT0_STATUS.ASSERT_ID]++;
    	if (is_mau_isr_log_on) {
        	MAU_INFO("Violation! Entry:0 Larb:%d Port:%d Name:%s->[0x%x,0x%x]",
        	    mauID,
        	    MAU_REG->ENT0_STATUS.ASSERT_ID,
                _get_port_name(mauID, MAU_REG->ENT0_STATUS.ASSERT_ID),
            	mau_ctxt.entryConf[0].StartAddr,
            	mau_ctxt.entryConf[0].EndAddr);
            _print_port_working_addr(mauID, MAU_REG->ENT0_STATUS.ASSERT_ID);
		}

        OUTREG32(&MAU_REG->ENT0_STATUS, 0x1<<7);
    }

    if (MAU_REG->ENT1_STATUS.ASSERT) {
    	//mau_ctxt.violationLARB0Cnt[1][MAU_REG->ENT1_STATUS.ASSERT_ID]++;
    	if (is_mau_isr_log_on) {
        	MAU_INFO("Violation! Entry:1 Larb:%d Port:%d Name:%s->[0x%x,0x%x]",
        	    mauID,
        	    MAU_REG->ENT1_STATUS.ASSERT_ID,
                _get_port_name(mauID, MAU_REG->ENT1_STATUS.ASSERT_ID),
            	mau_ctxt.entryConf[1].StartAddr,
            	mau_ctxt.entryConf[1].EndAddr);
            _print_port_working_addr(mauID, MAU_REG->ENT1_STATUS.ASSERT_ID);
		}

        OUTREG32(&MAU_REG->ENT1_STATUS, 0x1<<7);
    }

    if (MAU_REG->ENT2_STATUS.ASSERT) {
    	//mau_ctxt.violationLARB0Cnt[2][MAU_REG->ENT2_STATUS.ASSERT_ID]++;
    	if (is_mau_isr_log_on) {
        	MAU_INFO("Violation! Entry:2 Larb:%d Port:%d Name:%s->[0x%x,0x%x]",
        	    mauID,
        	    MAU_REG->ENT2_STATUS.ASSERT_ID,
                _get_port_name(mauID, MAU_REG->ENT2_STATUS.ASSERT_ID),
            	mau_ctxt.entryConf[2].StartAddr,
            	mau_ctxt.entryConf[2].EndAddr);
            _print_port_working_addr(mauID, MAU_REG->ENT2_STATUS.ASSERT_ID);
		}
        OUTREG32(&MAU_REG->ENT2_STATUS, 0x1<<7);
    }

}

#if ENABLE_MAU_INTERRUPT
//static __tcmfunc irqreturn_t mau0_isr(int irq, void *dev_id)
static irqreturn_t mau0_isr(int irq, void *dev_id)
{

    unsigned int  status0 = (MAU0_REG->INTERRUPT);
    unsigned int  status1 = (MAU1_REG->INTERRUPT);


    //mt6575_irq_mask(MT6575_SMI_LARB0_VAMAU_IRQ_ID);
    //disable_irq(MT6575_SMI_LARB0_VAMAU_IRQ_ID);

    if (1 == (status0 & 0x1)) {
        status0 &= ~0x1;
        _mau_isr(MAU_ID_0);
    }

    if (1 == (status1 & 0x1)) {
        status1 &= ~0x1;
        _mau_isr(MAU_ID_1);
    }

    //mt6575_irq_unmask(MT6575_SMI_LARB0_VAMAU_IRQ_ID);
    //enable_irq(MT6575_SMI_LARB0_VAMAU_IRQ_ID);

    return IRQ_HANDLED;
}


//static __tcmfunc irqreturn_t mau2_isr(int irq, void *dev_id)
static irqreturn_t mau2_isr(int irq, void *dev_id)
{

    unsigned int  status = (MAU2_REG->INTERRUPT);

    //mt6575_irq_mask(MT6575_SMI_LARB2_VAMAU_IRQ_ID);
    //disable_irq(MT6575_SMI_LARB2_VAMAU_IRQ_ID);

    if (1 == (status & 0x1)) {
        status &= ~0x1;
        _mau_isr(MAU_ID_2);
    }

    //mt6575_irq_unmask(MT6575_SMI_LARB2_VAMAU_IRQ_ID);
    //enable_irq(MT6575_SMI_LARB2_VAMAU_IRQ_ID);

    return IRQ_HANDLED;
}


//static __tcmfunc irqreturn_t mau3_isr(int irq, void *dev_id)
static irqreturn_t mau3_isr(int irq, void *dev_id)
{

    unsigned int  status = (MAU3_REG->INTERRUPT);

    //mt6575_irq_mask(MT6575_LARB3_VAMAU_IRQ_ID);
    //disable_irq(MT6575_LARB3_VAMAU_IRQ_ID);

    if (1 == (status & 0x1)) {
        status &= ~0x1;
        _mau_isr(MAU_ID_3);
    }

    //mt6575_irq_unmask(MT6575_LARB3_VAMAU_IRQ_ID);
    //enable_irq(MT6575_LARB3_VAMAU_IRQ_ID);

    return IRQ_HANDLED;
}
#endif

int MAU_PowerOn(MTK_MAU_ID mauID)
{
    //MAU_INFO("ID %d\n", mauID);
    switch(mauID)
    {
        case MAU_ID_0:
            enable_clock(MT65XX_PDN_MM_SMI_LARB0, "MAU");
            enable_clock(MT65XX_PDN_MM_SMI_LARB0_EMI, "MAU");
            break;
        case MAU_ID_1:
            enable_clock(MT65XX_PDN_MM_SMI_LARB1, "MAU");
            enable_clock(MT65XX_PDN_MM_SMI_LARB1_EMI, "MAU");
            break;
        case MAU_ID_2:
            enable_clock(MT65XX_PDN_MM_SMI_LARB2_ACP_BUS, "MAU");
            enable_clock(MT65XX_PDN_MM_SMI_LARB2_260MHZ, "MAU");
            enable_clock(MT65XX_PDN_MM_SMI_LARB2_EMI, "MAU");
            enable_clock(MT65XX_PDN_MM_SMI_LARB2_ACP_BUS_EMI, "MAU");
            enable_clock(MT65XX_PDN_MM_SMI_LARB2, "MAU");
            break;
        case MAU_ID_3:
            enable_clock(MT65XX_PDN_MM_SMI_LARB3_FULL, "MAU");
            enable_clock(MT65XX_PDN_MM_SMI_LARB3_HALF, "MAU");
            break;
        default:
        	MAU_ERROR("invalid ID=%d ", mauID);
    }

    return 0;
}

int MAU_PowerOff(MTK_MAU_ID mauID)
{
    //MAU_INFO("ID %d\n", mauID);
    switch(mauID)
    {
        case MAU_ID_0:
            disable_clock(MT65XX_PDN_MM_SMI_LARB0, "MAU");
            disable_clock(MT65XX_PDN_MM_SMI_LARB0_EMI, "MAU");
            break;
        case MAU_ID_1:
            disable_clock(MT65XX_PDN_MM_SMI_LARB1, "MAU");
            disable_clock(MT65XX_PDN_MM_SMI_LARB1_EMI, "MAU");
            break;
        case MAU_ID_2:
            disable_clock(MT65XX_PDN_MM_SMI_LARB2_ACP_BUS, "MAU");
            disable_clock(MT65XX_PDN_MM_SMI_LARB2_260MHZ, "MAU");
            disable_clock(MT65XX_PDN_MM_SMI_LARB2_EMI, "MAU");
            disable_clock(MT65XX_PDN_MM_SMI_LARB2_ACP_BUS_EMI, "MAU");
            disable_clock(MT65XX_PDN_MM_SMI_LARB2, "MAU");
            break;
        case MAU_ID_3:
            disable_clock(MT65XX_PDN_MM_SMI_LARB3_FULL, "MAU");
            disable_clock(MT65XX_PDN_MM_SMI_LARB3_HALF, "MAU");
            break;
        default:
        	MAU_ERROR("invalid ID=%d ", mauID);
    }

    return 0;
}




#define MAU_CLOCK_MONITOR_CALLBACK
void MAU_BackupReg(MTK_MAU_ID mauID)
{
    PMAU_REGS MAU_REG = _get_mau_reg(mauID);
    MAU_REGS *reg = _get_mau_reg_backup(mauID);
    UINT32 i;

    if (MAU_REG == NULL || reg == NULL)
    {
        MAU_ERROR("invalid param");
        return;
    }
#if defined MAU_CLOCK_MONITOR_CALLBACK
    //MAU_PowerOn(mauID);
#endif

    for (i = 0; i < ARY_SIZE(BACKUP_MAU_REG_OFFSETS); ++ i)
    {
        OUTREG32(REG_ADDR(reg, BACKUP_MAU_REG_OFFSETS[i]),
                 AS_UINT32(REG_ADDR(MAU_REG, BACKUP_MAU_REG_OFFSETS[i])));
    }
    _mau_interrupt_backup(mauID);

#if defined MAU_CLOCK_MONITOR_CALLBACK
    //MAU_PowerOff(mauID);
#endif
}

void MAU_RestoreReg(MTK_MAU_ID mauID)
{
    PMAU_REGS MAU_REG = _get_mau_reg(mauID);
    MAU_REGS *reg = _get_mau_reg_backup(mauID);

    UINT32 i;

    if (MAU_REG == NULL || reg == NULL)
    {
        MAU_ERROR("invalid param");
        return;
    }
#if defined MAU_CLOCK_MONITOR_CALLBACK
    //MAU_PowerOn(mauID);
#endif
    for (i = 0; i < ARY_SIZE(BACKUP_MAU_REG_OFFSETS); ++ i)
    {
        OUTREG32(REG_ADDR(MAU_REG, BACKUP_MAU_REG_OFFSETS[i]),
                 AS_UINT32(REG_ADDR(reg, BACKUP_MAU_REG_OFFSETS[i])));
    }

    _mau_interrupt_restore(mauID);

#if defined MAU_CLOCK_MONITOR_CALLBACK
    //MAU_PowerOff(mauID);
#endif
}


/*****************************************************************************/
int MAU_Config(MTK_MAU_CONFIG* pMauConf)
{
#if defined MTK_M4U_SUPPORT
    if (pMauConf->Mode == MAU_PA)
    {
        unsigned int portMask[4];
        if(_m4u_debug_func.isInit==false)
        {
            MAU_WARN("M4U Init Fail");
            return -1;
        }
        _m4u_debug_func.m4u_mau_get_physical_port(portMask);
        pMauConf->InvalidMasterLARB0 &= portMask[0];
        pMauConf->InvalidMasterLARB1 &= portMask[1];
        pMauConf->InvalidMasterLARB2 &= portMask[2];
        pMauConf->InvalidMasterLARB3 &= portMask[3];
    }
#endif

#if defined MAU_CLOCK_MONITOR_CALLBACK
    MAU_PowerOn(MAU_ID_0);
    MAU_PowerOn(MAU_ID_1);
    MAU_PowerOn(MAU_ID_2);
    MAU_PowerOn(MAU_ID_3);
#endif

    mtk_mau_config(MAU_ID_0, pMauConf);
    mtk_mau_config(MAU_ID_1, pMauConf);
    mtk_mau_config(MAU_ID_2, pMauConf);
    mtk_mau_config(MAU_ID_3, pMauConf);

#if defined MAU_CLOCK_MONITOR_CALLBACK
    MAU_BackupReg(MAU_ID_0);
    MAU_BackupReg(MAU_ID_1);
    MAU_BackupReg(MAU_ID_2);
    MAU_BackupReg(MAU_ID_3);

    MAU_PowerOff(MAU_ID_0);
    MAU_PowerOff(MAU_ID_1);
    MAU_PowerOff(MAU_ID_2);
    MAU_PowerOff(MAU_ID_3);
#endif

    return 0;
}

int MAU_get_port_with_m4u(unsigned  int start_addr, unsigned int end_addr)
{
#if defined MTK_M4U_SUPPORT
    if(_m4u_debug_func.isInit==false)
    {
        MAU_WARN("M4U Init Fail ");
        return -1;
    }
    _m4u_debug_func.m4u_mau_check_pagetable(start_addr, end_addr);	
#endif
    
    return 0;
}

void MAU_LogSwitch(bool enable)
{
    MAU_INFO(" %s", (enable?"ON":"OFF"));
    if (is_mau_isr_log_on == enable) return;
    else is_mau_isr_log_on = enable;

    return;
}


void MAU_DumpReg(MTK_MAU_ID mauID)
{

    UINT32 i;
    UINT32 MAU_BASE = _get_mau_reg_base(mauID);

    if (MAU_BASE == 0)
    {
        MAU_ERROR("invalid param");
        return;
    }


    printk("---------- Start dump MAU%d registers ----------\n"
           "MAU%d_BASE: 0x%08x\n", mauID, mauID, MAU_BASE);

    for (i = 0; i <= offsetof(MAU_REGS, ENT2_GID); i += 4)
    {
        printk("MAU%d+%04x : 0x%08x\n", mauID, i, INREG32(MAU_BASE + i));
    }

    for (i = offsetof(MAU_REGS, ENT0_STATUS); i < sizeof(MAU_REGS); i += 4)
    {
        printk("MAU%d+%04x : 0x%08x\n", mauID, i, INREG32(MAU_BASE + i));
    }


    return;

}


#if 0

void MAU_PrintStatus(char* buf, unsigned int buf_len, unsigned int* num)
{
    unsigned int n = *num;
    int id = 0;
    int i = 0;
    is_mau_enabled = mau_ctxt.entryConf[0].Enable ||
                      mau_ctxt.entryConf[1].Enable ||
                      mau_ctxt.entryConf[2].Enable;

    n += scnprintf(buf + n, buf_len - n, "[MM MAU] ----------debug info status start ----------\n");
    n += scnprintf(buf + n, buf_len - n, "--mm mau is %s\n", (is_mau_enabled?"ON":"OFF"));

    if (is_mau_enabled) {

        n += scnprintf(buf + n, buf_len - n, "---------------------------------------------\n");

        for (id = 0; id < MAU_ENTRY_ALL; id++) {
            if (mau_ctxt.entryConf[id].Enable == false) continue;
            n += scnprintf(buf + n, buf_len - n, "--MAU entry [%d] \n", id);
            n += scnprintf(buf + n, buf_len - n, "--protected mem: [0x%x, 0x%x]\n",
                           mau_ctxt.entryConf[id].StartAddr,
                           mau_ctxt.entryConf[id].EndAddr);
            n += scnprintf(buf + n, buf_len - n, "--read(%s) write(%s)\n",
                           (mau_ctxt.entryConf[id].ReadEn?"Y":"N"),
                           (mau_ctxt.entryConf[id].WriteEn?"Y":"N"));
            n += scnprintf(buf + n, buf_len - n, "--guarded modules: ");
            if (mau_ctxt.entryConf[id].InvalidMasterGMC1 == 0xffffffff &&
                mau_ctxt.entryConf[id].InvalidMasterGMC2 == 0xffffffff)   {
                n += scnprintf(buf + n, buf_len - n, "all ");
            } else {
                for (i=1; i < MAU_MASK_ALL; i++ ) {
                    //MAU_ASSERT(i<32);
                    if (i < MAU1_MASK_ALL) {
                        if ((0x1<<(i-1)) & mau_ctxt.entryConf[id].InvalidMasterGMC1) {
                            n += scnprintf(buf + n, buf_len - n, "%s ", mauPortName[i]);
                        }
                    }
                    else if (i > MAU1_MASK_ALL)
                    {
                        if ((0x1<<(i-MAU1_MASK_ALL-1)) & mau_ctxt.entryConf[id].InvalidMasterGMC2) {
                            n += scnprintf(buf + n, buf_len - n, "%s ", mauPortName[i]);
                        }

                    }
                }
            }
            n += scnprintf(buf + n, buf_len - n, "\n");
            n += scnprintf(buf + n, buf_len - n, "--violation report: \n");
	        for ( i = 0; i < MAU_MASK_ALL; i++ ) {
                //if (i%8 == 0) n += scnprintf(buf + n, buf_len - n, "\n");
                if (mau_ctxt.violationCnt[id][i] > 0) {
		            n += scnprintf(buf + n, buf_len - n, "%s:%d\n", mauPortName[i], (int)mau_ctxt.violationCnt[id][i]);
                }
		    }
            n += scnprintf(buf + n, buf_len - n, "\n");
            n += scnprintf(buf + n, buf_len - n, "---------------------------------------------");
            n += scnprintf(buf + n, buf_len - n, "\n");
	    }
    }
    else
    {
        n += scnprintf(buf + n, buf_len - n, "--MAU Module ID List:");
        for (i = 0; i<MAU_MASK_ALL; i++) {
            //if (i%8 == 0) n += scnprintf(buf + n, buf_len - n, "\n");
	        n += scnprintf(buf + n, buf_len - n, "%02d:%s\n", i, mauPortName[i]);
        }
        n += scnprintf(buf + n, buf_len - n, "\n");

    }


    n += scnprintf(buf + n, buf_len - n, "[MM MAU] ----------debug info status end   ----------\n");
    buf[n++] = 0;

    *num = n;
}

#endif


#ifdef CONFIG_HAS_EARLYSUSPEND
static void mau_early_suspend(struct early_suspend *h)
{
#if defined MAU_CLOCK_MONITOR_CALLBACK
    MAU_INFO("clock monitor enabled skip early suspend");
#else
    MAU_INFO();

    MAU_BackupReg(MAU_ID_0);
    MAU_BackupReg(MAU_ID_1);
    MAU_BackupReg(MAU_ID_2);
    MAU_BackupReg(MAU_ID_3);

    MAU_PowerOff(MAU_ID_0);
    MAU_PowerOff(MAU_ID_1);
    MAU_PowerOff(MAU_ID_2);
    MAU_PowerOff(MAU_ID_3);
#endif

}

static void mau_late_resume(struct early_suspend *h)
{
#if defined MAU_CLOCK_MONITOR_CALLBACK
    MAU_INFO("clock monitor enabled skip late resume");
#else
    MAU_INFO();

    MAU_PowerOn(MAU_ID_0);
    MAU_PowerOn(MAU_ID_1);
    MAU_PowerOn(MAU_ID_2);
    MAU_PowerOn(MAU_ID_3);

    MAU_RestoreReg(MAU_ID_0);
    MAU_RestoreReg(MAU_ID_1);
    MAU_RestoreReg(MAU_ID_2);
    MAU_RestoreReg(MAU_ID_3);
#endif
}

static struct early_suspend mau_early_suspend_handler =
{
	.level = EARLY_SUSPEND_LEVEL_DISABLE_FB,
	.suspend = mau_early_suspend,
	.resume = mau_late_resume,
};
#endif

#if 0
static void mau_high_gate(struct clock_listener *cl)
{
   printk("[%s]:clock_listener:gate high\r\n", __FUNCTION__);
   if (!is_mau_isr_log_on) MAU_Mau1DumpReg();
   MAU_Mau1BackupReg();
   MM_MpuBackupReg();
}

static void mau_high_ungate(struct clock_listener *cl)
{
   printk("[%s]:clock_listener:ungate high\r\n", __FUNCTION__);
   MAU_Mau1RestoreReg();
   MM_MpuRestoreReg();
   if (!is_mau_isr_log_on) MAU_Mau1DumpReg();
}

struct clock_listener mau_clock_listener = {
   .name = "mau_high",
   .level = CLOCK_LISTEN_HIGH,
   .gate = mau_high_gate,
   .ungate = mau_high_ungate,
};

#endif


static long mau_ioctl( struct file * pFile,
						 unsigned int cmd,
						 unsigned long param)
{
    int ret = 0;
    switch (cmd)
    {
        case MTK_CONFIG_MM_MAU:
        {
        	MTK_MAU_CONFIG b;
        	MAU_DBG("ioctl: MTK_CONFIG_MM_MAU");

       		if (copy_from_user(&b, (void __user *)param, sizeof(b)))
        	{
            	MAU_ERROR("copy_from_user failed!");
            	ret = -EFAULT;
        	} else {
                MAU_Config(&b);
			}
        	return ret;
    	}
        default:
            return -1;
    }

	return ret;
}


static const struct file_operations mauFops =
{
	.owner = THIS_MODULE,
	.unlocked_ioctl = mau_ioctl,
};

static struct cdev * pMauDev = NULL;
static dev_t mauDevNo = MKDEV(MTK_MAU_MAJOR_NUMBER,0);
static inline int mau_register(void)
{
    MAU_DBG();
    if (alloc_chrdev_region(&mauDevNo, 0, 1,"MTK_MAU")){
        MAU_ERROR("Allocate device No. failed");
        return -EAGAIN;
    }

    //Allocate driver
    pMauDev = cdev_alloc();

    if (NULL == pMauDev) {
        unregister_chrdev_region(mauDevNo, 1);
        MAU_ERROR("Allocate mem for kobject failed");
        return -ENOMEM;
    }

    //Attatch file operation.
    cdev_init(pMauDev, &mauFops);
    pMauDev->owner = THIS_MODULE;


    //Add to system
    if (cdev_add(pMauDev, mauDevNo, 1)) {
        MAU_ERROR("Attatch file operation failed");
        unregister_chrdev_region(mauDevNo, 1);
        return -EAGAIN;
    }

    return 0;
}

#ifdef MAU_CLOCK_MONITOR_CALLBACK
// callback after larb clock is enabled
void mau_clock_enable(struct larb_monitor *h, int larb_idx)
{
	  MAU_INFO("larb_idx=%d", larb_idx);
      //MAU_PowerOn(larb_idx);
      MAU_RestoreReg(larb_idx);
      //MAU_PowerOff(larb_idx);
	  return;
}
// callback before larb clock is disabled
void mau_clock_disable(struct larb_monitor *h, int larb_idx)
{
	  MAU_INFO("larb_idx=%d", larb_idx);
      //MAU_PowerOn(larb_idx);
      MAU_BackupReg(larb_idx);
      //MAU_PowerOff(larb_idx);
	  
	  return;	
}

struct larb_monitor mau_larb_monitor_handler =
{
    .level = LARB_MONITOR_LEVEL_HIGH,
    .backup = mau_clock_disable,
    .restore = mau_clock_enable	
};
#endif

static struct class *pMauClass = NULL;
// Called to probe if the device really exists. and create the semaphores
static int mau_probe(struct platform_device *pdev)
{
    struct device* mauDevice = NULL;
    MAU_DBG();

    //Check platform_device parameters
    if (NULL == pdev) {
        MAU_ERROR("platform data missed");
        return -ENXIO;
    }

    //register char driver
    //Allocate major no
    if (mau_register()) {
        dev_err(&pdev->dev,"register char failed\n");
        return -EAGAIN;
    }

    pMauClass = class_create(THIS_MODULE, "MTK_MAU");
    if (IS_ERR(pMauClass)) {
        int ret = PTR_ERR(pMauClass);
        MAU_ERROR("Unable to create class, err = %d", ret);
        return ret;
    }
    mauDevice = device_create(pMauClass, NULL, mauDevNo, NULL, "MTK_MAU");


    MAU_DBG_Init();
	mau_reset_ctxt();

#if ENABLE_MAU_INTERRUPT
    // LARB0 & LARB1 will use same interrupt 106
    if(request_irq(MT6577_SMI_LARB0_VAMAU_IRQ_ID , (irq_handler_t)mau0_isr, IRQF_TRIGGER_LOW, "MAU0-1" , NULL))
    {
        MAU_ERROR("request MAU0 IRQ line failed");
        return -ENODEV;
    }
    //mt6575_irq_set_sens(MT6575_SMI_LARB0_VAMAU_IRQ_ID, MT65xx_LEVEL_SENSITIVE);
    //mt6575_irq_set_polarity(MT6575_SMI_LARB0_VAMAU_IRQ_ID, MT65xx_POLARITY_LOW);
    //mt6575_irq_unmask(MT6575_SMI_LARB0_VAMAU_IRQ_ID);



    if(request_irq(MT6577_SMI_LARB2_VAMAU_IRQ_ID , (irq_handler_t)mau2_isr, IRQF_TRIGGER_LOW, "MAU2" , NULL))
    {
        MAU_ERROR("request MAU2 IRQ line failed");
        return -ENODEV;
    }
    //mt6575_irq_set_sens(MT6575_SMI_LARB2_VAMAU_IRQ_ID, MT65xx_LEVEL_SENSITIVE);
    //mt6575_irq_set_polarity(MT6575_SMI_LARB2_VAMAU_IRQ_ID, MT65xx_POLARITY_LOW);
    //mt6575_irq_unmask(MT6575_SMI_LARB2_VAMAU_IRQ_ID);


    if(request_irq(MT6577_LARB3_VAMAU_IRQ_ID , (irq_handler_t)mau3_isr, IRQF_TRIGGER_LOW, "MAU3" , NULL))
    {
        MAU_ERROR("request MAU3 IRQ line failed");
        return -ENODEV;
    }
    //mt6575_irq_set_sens(MT6575_LARB3_VAMAU_IRQ_ID, MT65xx_LEVEL_SENSITIVE);
    //mt6575_irq_set_polarity(MT6575_LARB3_VAMAU_IRQ_ID, MT65xx_POLARITY_LOW);
    //mt6575_irq_unmask(MT6575_LARB3_VAMAU_IRQ_ID);

#endif
/*
    if (request_irq(MT6575_SMI_LARB0_VAMAU_IRQ_ID,
                    (irq_handler_t)mau0_isr,
                    IRQF_TRIGGER_HIGH, "MAU0", NULL) < 0)
    {
        MAU_ERROR("fail to request MAU0 irq\n");
        return -EINTR;
    }

    if (request_irq(MT6575_SMI_LARB1_VAMAU_IRQ_ID,
                    (irq_handler_t)mau1_isr,
                    IRQF_TRIGGER_HIGH, "MAU1", NULL) < 0)
    {
        MAU_ERROR("fail to request MAU1 irq\n");
        return -EINTR;
    }

    if (request_irq(MT6575_SMI_LARB2_VAMAU_IRQ_ID,
                    (irq_handler_t)mau2_isr,
                    IRQF_TRIGGER_HIGH, "MAU2", NULL) < 0)
    {
        MAU_ERROR("[MAU][ERROR] fail to request MAU2 irq\n");
        return -EINTR;
    }
    if (request_irq(MT6575_LARB3_VAMAU_IRQ_ID,
                    (irq_handler_t)mau3_isr,
                    IRQF_TRIGGER_HIGH, "MAU3", NULL) < 0)
    {
        printk("[MAU][ERROR] fail to request MAU3 irq\n");
        return -EINTR;
    }

*/


#ifdef CONFIG_HAS_EARLYSUSPEND
    register_early_suspend(&mau_early_suspend_handler);
#endif
    //register_clock_listener(&mau_clock_listener);


#ifdef MAU_CLOCK_MONITOR_CALLBACK    
    MAU_INFO("register_larb_monitor done!");
    register_larb_monitor(&mau_larb_monitor_handler);
#else

    MAU_PowerOn(MAU_ID_0);
    MAU_PowerOn(MAU_ID_1);
    MAU_PowerOn(MAU_ID_2);
    MAU_PowerOn(MAU_ID_3);
#endif
    return 0;
}



// Called when the device is being detached from the driver
static int mau_remove(struct platform_device *pdev)
{
    //Release char driver
    cdev_del(pMauDev);

    unregister_chrdev_region(mauDevNo, 1);

    device_destroy(pMauClass, mauDevNo);

    class_destroy(pMauClass);

    MAU_DBG("MTK_MAU driver is removed");

    return 0;
}


static int mau_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    MAU_DBG("suspend");
    return 0;
}

static int mau_resume(struct platform_device *pdev)
{
    MAU_DBG("resume");
    return 0;
}

static struct platform_driver mauDrv = {
    .probe	= mau_probe,
    .remove	= mau_remove,
    .suspend= mau_suspend,
    .resume	= mau_resume,
    .driver	= {
    .name	= "MTK_MAU",
    .owner	= THIS_MODULE,
    }
};


static int __init mau_init(void)
{
    if(platform_driver_register(&mauDrv)){
        MAU_ERROR("failed to register MAU driver");
        return -ENODEV;
    }
    MAU_DBG();
    
	return 0;
}

static void __exit mau_exit(void)
{
    MAU_DBG();
    platform_driver_unregister(&mauDrv);

#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&mau_early_suspend_handler);
#endif
}

module_init(mau_init);
module_exit(mau_exit);

MODULE_DESCRIPTION("MTK MAU driver");
MODULE_AUTHOR("Mingchen <Mingchen.gao@Mediatek.com>");
MODULE_LICENSE("GPL");



