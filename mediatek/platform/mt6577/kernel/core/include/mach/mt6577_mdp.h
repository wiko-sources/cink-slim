#ifndef _MT6577MDP_H
#define _MT6577MDP_H

#include <linux/ioctl.h>

/*-----------------------------------------------------------------------------
    MDP Compile Time Option
    ( MDP_FLAG_# ==> #:means flag level)
  -----------------------------------------------------------------------------*/
#if (defined(CONFIG_MT6577_FPGA)  || defined(CONFIG_MT6575_FPGA))
#define MDP_KERNEL_FLAG_0_FPGA
#endif

#define MDP_KERNEL_FLAG_1_SUPPORT_INT               //ON: MDP use Interrupt. OFF: MDP use polling
#define MDPK_FLAG_SUPPORT_UNLOCKED_IOCTL




/*/////////////////////////////////////////////////////////////////////////////
    Definnition
  /////////////////////////////////////////////////////////////////////////////*/
#define MT6577_MDP_DEV_MAJOR_NUMBER 252
#define MT6577_MDPMAGICNO 'g'



/*/////////////////////////////////////////////////////////////////////////////
    MDP Element ID
  /////////////////////////////////////////////////////////////////////////////*/
#define MID_JPEG_DEC_i        ( 0 )
#define MID_JPEG_ENC_i        ( 1 )
#define MID_CAM_i             ( 2 )
#define MID_BRZ_i             ( 3 )
#define MID_CRZ_i             ( 4 )
#define MID_PRZ0_i            ( 5 )
#define MID_PRZ1_i            ( 6 )
#define MID_VRZ0_i            ( 7 )
#define MID_VRZ1_i            ( 8 )
#define MID_R_DMA0_i          ( 9 )
#define MID_R_DMA1_i          ( 10 )
#define MID_OVL_i             ( 11 )
#define MID_IPP_i             ( 12 )
#define MID_MOUT_i            ( 13 )
#define MID_LCD_i             ( 14 )
#define MID_JPEG_DMA_i        ( 15 )
#define MID_EIS_i             ( 16 )
#define MID_RGB_ROT0_i        ( 17 )
#define MID_RGB_ROT1_i        ( 18 )
#define MID_RGB_ROT2_i        ( 19 )
#define MID_VDO_ROT0_i        ( 20 )
#define MID_VDO_ROT1_i        ( 21 )
#define MID_TV_ROT_i          ( 22 )

#define MID_RDMA_GENERAL_i    ( 23 )
#define MID_ROTDMA_GENERAL_i  ( 24 )





#define MID_JPEG_DEC        ( 1u << MID_JPEG_DEC_i)
#define MID_JPEG_ENC        ( 1u << MID_JPEG_ENC_i )
#define MID_CAM             ( 1u << MID_CAM_i )
#define MID_BRZ             ( 1u << MID_BRZ_i )
#define MID_CRZ             ( 1u << MID_CRZ_i )
#define MID_PRZ0            ( 1u << MID_PRZ0_i )
#define MID_PRZ1            ( 1u << MID_PRZ1_i )
#define MID_VRZ0            ( 1u << MID_VRZ0_i )
#define MID_VRZ1            ( 1u << MID_VRZ1_i )
#define MID_R_DMA0          ( 1u << MID_R_DMA0_i )
#define MID_R_DMA1          ( 1u << MID_R_DMA1_i )
#define MID_OVL             ( 1u << MID_OVL_i )
#define MID_IPP             ( 1u << MID_IPP_i )
#define MID_MOUT            ( 1u << MID_MOUT_i )
#define MID_LCD             ( 1u << MID_LCD_i )
#define MID_JPEG_DMA        ( 1u << MID_JPEG_DMA_i )
#define MID_EIS             ( 1u << MID_EIS_i )
#define MID_RGB_ROT0        ( 1u << MID_RGB_ROT0_i )
#define MID_RGB_ROT1        ( 1u << MID_RGB_ROT1_i )
#define MID_RGB_ROT2        ( 1u << MID_RGB_ROT2_i )
#define MID_VDO_ROT0        ( 1u << MID_VDO_ROT0_i )
#define MID_VDO_ROT1        ( 1u << MID_VDO_ROT1_i )
#define MID_TV_ROT          ( 1u << MID_TV_ROT_i )

#define MID_RDMA_GENERAL    ( 1u << MID_RDMA_GENERAL_i )
#define MID_ROTDMA_GENERAL  ( 1u << MID_ROTDMA_GENERAL_i )
#define MID_RGB_ROT0_EX     (1u << MID_RGB_ROT0_i | 1u << 31)


#define MDP_ELEMENT_MAX_NUM (25)



typedef struct {
    unsigned long u4LockResTable;       //resource want to lock
    unsigned long u4IsTimeShared;       // 0 : No, 1 : Time shared
    unsigned long u4TimeOutInms;
    unsigned long out_Table_status_in_use;      // Lock Table Status return from kernel
    unsigned long out_Table_status_in_occupied; // Lock Table Status return from kernel
} stLockResParam;

typedef struct {
    unsigned long u4IrqNo;
    unsigned long u4TimeOutInms;
} stWaitIrqParam;


#define MT6577_MDP_BUFFCNT 16
typedef struct {
    unsigned long mdp_id;
    unsigned long sec[MT6577_MDP_BUFFCNT];
    unsigned long usec[MT6577_MDP_BUFFCNT];
} stTimeStamp;


typedef struct {
    unsigned long u4StartAddr;//In : 
    unsigned long u4Size;
    unsigned long u4Result;// 0 : out of pmem range, 1 : inside pmem range, 2 : partially overlap with pmem range
} stPMEMRange;


typedef struct {
    unsigned long mdp_id;
    unsigned long linked_mdp_id_set;   /*associated mdps that need to be update in one single cycle*/
    unsigned long reg_CFG;
    unsigned long reg_SRCSZ;
    unsigned long reg_CROPLR;
    unsigned long reg_CROPTB;
    unsigned long reg_HRATIO;
    unsigned long reg_VRATIO;
} stZoomSetting;

//CRZ,PRZ0,VRZ,VDOROT1 settings
typedef struct {
//CRZ
    unsigned long u4CRZLBMAX;//+0x0 CRZ_CFG
    unsigned long u4CRZSrcSZ;//+0x10 CRZ_SRCSZ
    unsigned long u4CRZTarSZ;//+0x14 CRZ_TARSZ
    unsigned long u4CRZHRatio;//+0x18 CRZ_HRATIO
    unsigned long u4CRZVRatio;//+0x1C CRZ_VRATIO
    unsigned long u4CRZCropLR;//+0xF4 CRZ_CropLR
    unsigned long u4CRZCropTB;//+0xF8 CRZ_CropTB
//PRZ0
    unsigned long u4PRZ0LBMAX;//+0x0 PRZ0_CFG
    unsigned long u4PRZ0SrcSZ;//+0x10 PRZ0_SRCSZ
    unsigned long u4PRZ0TarSZ;//+0x14 PRZ0_TARSZ
    unsigned long u4PRZ0HRatio;//+0x18 PRZ0_HRATIO
    unsigned long u4PRZ0VRatio;//+0x1C PRZ0_VRATIO
//VRZ
    unsigned long u4VRZSrcSZ;//+0x10 VRZ_SRCSZ
    unsigned long u4VRZTarSZ;//+0x14 VRZ_TARSZ
    unsigned long u4VRZHRatio;//+0x18 VRZ_HRATIO
    unsigned long u4VRZVRatio;//+0x1C VRZ_VRATIO
    unsigned long u4VRZHRes;//+0x20 VRZ_HRES
    unsigned long u4VRZVRes;//+0x24 VRZ_VRES
//VDOROT1 descriptor
    unsigned long u4VDO1Seg4;//SrcW+SrcH
    unsigned long u4VDO1Seg5;//ClipW+ClipH
    unsigned long u4VDO1Seg6;//ClipX+ClipY
    unsigned long u4VDO1Seg7;//DstW in Bytes
} stZSDZoomReg;

// last preview frame for zsd
//CRZ,RGBROT0 settings
typedef struct {
//
    bool          bStopAfterZSDDone;
//CRZ
    unsigned long u4CRZLBMAX;//+0x0 CRZ_CFG
    unsigned long u4CRZSrcSZ;//+0x10 CRZ_SRCSZ
    unsigned long u4CRZTarSZ;//+0x14 CRZ_TARSZ
    unsigned long u4CRZHRatio;//+0x18 CRZ_HRATIO
    unsigned long u4CRZVRatio;//+0x1C CRZ_VRATIO
    unsigned long u4CRZCropLR;//+0xF4 CRZ_CropLR
    unsigned long u4CRZCropTB;//+0xF8 CRZ_CropTB
//RGBROT0 descriptor
    unsigned long u4RGB0Seg1[16];//dstAddr
    unsigned long u4RGB0Seg4;//SrcW+SrcH
    unsigned long u4RGB0Seg5;//ClipW+ClipH
    unsigned long u4RGB0Seg6;//ClipX+ClipY
    unsigned long u4RGB0Seg7;//DstW in Bytes
    unsigned long u4RGB0Seg8;//ClipW in Bytes
    unsigned long u4RGB0Seg9;//ClipH in Bytes
// return values
    unsigned long u4DescUpdateZone; // 0: unsafe 1:safe
    unsigned long u4DescRemain;
    unsigned long u4DescCnt;
    unsigned long u4DescHwWriteIndex;
    unsigned long u4DescHwReadIndex;
} stZSDPreviewReg;


/*MDPK_BITBLT_FORMAT_ENUM is the same with MHAL_BITBLT_FORMAT_ENUM in MediaHal.h*/
typedef enum
{
    MDPK_FORMAT_RGB_565 = 0,
    MDPK_FORMAT_RGB_888,
    MDPK_FORMAT_ARGB_8888,
    MDPK_FORMAT_ABGR_8888,
    MDPK_FORMAT_YUV_420,
    MDPK_FORMAT_YUV_420_SP,
    MDPK_FORMAT_MTK_YUV,
    MDPK_FORMAT_YUY2,
    MDPK_FORMAT_UYVY,

    MDPK_FORMAT_ERROR,

    MDPK_FORMAT_ALL = 0xFFFFFFFF
} MDPK_BITBLT_FORMAT_ENUM;

typedef enum
{
    MDPK_MEMTYPE_VMALLOC = 0,
    MDPK_MEMTYPE_PHYMEM,

} MDPK_BITBLT_MEMTYPE;



/*MdpkBitbltConfig is the same with mHalBltParam_t in MediaHal.h*/
typedef struct
{
    unsigned int srcX;
    unsigned int srcY;
    unsigned int srcW;
    unsigned int srcWStride;
    unsigned int srcH;
    unsigned int srcHStride;
    unsigned int srcAddr;
    MDPK_BITBLT_FORMAT_ENUM srcFormat;
    unsigned int srcBufferSize;     //Note:Kernel Mdpk Api Only
    MDPK_BITBLT_MEMTYPE srcMemType; //Note:Kernel Mdpk Api Only

    unsigned int dstW;
    unsigned int dstH;
    unsigned int dstAddr;
    MDPK_BITBLT_FORMAT_ENUM dstFormat;
    unsigned int pitch;
    unsigned int dstBufferSize;     //Note:Kernel Mdpk Api Only
    MDPK_BITBLT_MEMTYPE dstMemType; //Note:Kernel Mdpk Api Only

    unsigned int orientation;
    unsigned int doImageProcess;

    unsigned int u4SrcOffsetXFloat;//0x100 stands for 1, 0x40 stands for 0.25 , etc...
    unsigned int u4SrcOffsetYFloat;//0x100 stands for 1, 0x40 stands for 0.25 , etc...

} MdpkBitbltConfig;


typedef struct {
    int                 out_channel;            //[out] Bitblt channel
    int                 out_b_src_addr_dirty;   //[out] if b_addr_dirty , need to remap
    int                 out_b_dst_addr_dirty;   //[out] if b_addr_dirty , need to remap
    MdpkBitbltConfig    out_config;             //[out] Config return

} MDPIOCTL_MdpkBitbltConfig;

typedef struct {
    int                 in_channel;             //[in] Bitblt channel
    int                 in_ret_val;             //[in] return value of user mode bitblt
} MDPIOCTL_MdpkBitbltInformDone;



typedef enum
{
    MLMT_ACTION_FREE,
    MLMT_ACTION_ALLOC

} MLMT_ACTION;


typedef struct {
    int             in_client_id;   /*client id of register loop memory*/
    MLMT_ACTION     in_action;      /*alloc or free?*/
    unsigned long   in_size;
    unsigned long   in_tgid;

    long   out_statistic_alloc_count;
    long   out_statistic_free_count;
    long   out_statistic_count_balance;

    long   out_statistic_alloc_size;
    long   out_statistic_free_size;
    long   out_statistic_size_balance;

    
} MDPIOCTL_MvaLoopMemTrack;


typedef struct {
    unsigned long long  sec; 
    unsigned long       usec;
} MDPIOCTL_CpuClockGet;




/*/////////////////////////////////////////////////////////////////////////////
    IOCTL commnad
  /////////////////////////////////////////////////////////////////////////////*/
//Lock NODES
#define MT6577MDP_T_LOCKRESOURCE _IOWR(MT6577_MDPMAGICNO,0,stLockResParam)

//Unlock NODES
#define MT6577MDP_T_UNLOCKRESOURCE _IOW(MT6577_MDPMAGICNO,1,int)

//Wait IRQ
#define MT6577MDP_X_WAITIRQ _IOW(MT6577_MDPMAGICNO,2,stWaitIrqParam)

//Clear IRQ flag
#define MT6577MDP_T_CLRIRQ _IOW(MT6577_MDPMAGICNO,3,int)

//Dump registers
#define MT6577MDP_T_DUMPREG _IO(MT6577_MDPMAGICNO,4)

//Get ROTDMA0 time stamp
#define MT6577MDP_G_TIMESTAMP _IOR(MT6577_MDPMAGICNO , 5 , stTimeStamp)

//Get pmem range
#define MT6577MDP_G_PMEMRANGE _IOR(MT6577_MDPMAGICNO , 6 , stPMEMRange)

//Set zoom ratio
#define MT6577MDP_T_SETZOOM _IOR(MT6577_MDPMAGICNO , 7 , stZoomSetting)

/*-----------------------------------------------------------------------------
    MDP Kernel Mode API
  -----------------------------------------------------------------------------*/
//MDPK Bitblit
#define MDPK_BITBLT_G_WAIT_REQUEST  _IOR(MT6577_MDPMAGICNO , 8 , MDPIOCTL_MdpkBitbltConfig)
#define MDPK_BITBLT_T_INFORM_DONE   _IOW(MT6577_MDPMAGICNO , 9 , MDPIOCTL_MdpkBitbltInformDone)
/*-----------------------------------------------------------------------------*/

//Set ZSD zoom ratio
#define MT6577MDP_T_SETZSDZOOM _IOW(MT6577_MDPMAGICNO , 10 , stZSDZoomReg)

/*-----------------------------------------------------------------------------
    MDP MVA Register loop memory tracker
  -----------------------------------------------------------------------------*/
#define MT6577MDP_T_MVALOOPMEMTRACK _IOWR(MT6577_MDPMAGICNO,11,MDPIOCTL_MvaLoopMemTrack)

// last preview frame for zsd
#define MT6577MDP_T_SETZSDPREVIEWFRAME _IOW(MT6577_MDPMAGICNO , 12 , stZSDPreviewReg)

// Get CPU Clock
#define MDPIO_T_CPUCLOCKGET _IOR(MT6577_MDPMAGICNO , 13 , MDPIOCTL_CpuClockGet )





/*/////////////////////////////////////////////////////////////////////////////
    Register Map
  /////////////////////////////////////////////////////////////////////////////*/

#define MMSYS1_CG_CON1_ADDR         (MMSYS1_CONFIG_BASE + 0x04)
#define MMSYS1_RESZ_IRQ_FLAG        (MMSYS1_CONFIG_BASE + 0xF0)
#define MMSYS1_ROT_DMA_IRQ_FLAG     (MMSYS1_CONFIG_BASE + 0xF4)
#define MMSYS1_OVL_JPG_DMA_IRQ_FLAG (MMSYS1_CONFIG_BASE + 0xF8)
#define MMSYS1_DISP_MDP_IRQ_FLAG    (MMSYS1_CONFIG_BASE + 0xFC)


#define MT6577_RDMA_IRQ_FLAG_ADDR(i)                (i)
#define MT6577_RDMA_IRQ_FLAG_CLR_ADDR(i)            (i + 0x08)
#define MT6577_RDMA_QUEUE_RSTA_ADDR(i)	            (i + 0x40)
#define MT6577_RDMA_EXEC_CNT_ADDR(i)                (i + 0x80)


#define MT6577_ROT_DMA_IRQ_FLAG_ADDR(i)             (i)
#define MT6577_ROT_DMA_IRQ_FLAG_CLR_ADDR(i)         (i + 0x08)
#define MT6577_ROT_DMA_CFG_ADDR(i)                  (i + 0x18)
#define MT6577_ROT_DMA_LOCK_ADDR(i)                 (i + 0x3C)
#define MT6577_ROT_DMA_QUEUE_RSTA_ADDR(i)           (i + 0x40)
#define MT6577_ROT_DMA_RD_BASE_ADDR(i)              (i + 0x48)
#define MT6577_ROT_DMA_RPT_ADVANCE_ADDR(i)          (i + 0x50)
#define MT6577_ROT_DMA_QUEUE_WSTA_ADDR(i)           (i + 0x58)
#define MT6577_ROT_DMA_QUEUE_BASE_ADDR(i)           (i + 0x78)
#define MT6577_ROT_DMA_QUEUE_DATA_ADDR(i)           (i + 0x70)
#define MT6577_ROT_DMA_EXEC_CNT_ADDR(i)             (i + 0x80)
#define MT6577_ROT_DMA_DROPPED_FRAME_CNT_ADDR(i)    (i + 0x88)

#define MT6577_RZ_INFO0(i)                          (i + 0x00B0) // only CRZ
#define MT6577_RZ_INFO1(i)                          (i + 0x00B4) // only CRZ
#define MT6577_RZ_CFG_ADDR(i)                       (i + 0x0000)
#define MT6577_RZ_STA_ADDR(i)                       (i + 0x0008)
#define MT6577_RZ_INT_ADDR(i)                       (i + 0x000C)
#define MT6577_RZ_SRCSZ_ADDR(i)                     (i + 0x0010)
#define MT6577_RZ_TARSZ_ADDR(i)                     (i + 0x0014)
#define MT6577_RZ_HRATIO_ADDR(i)                    (i + 0x0018)
#define MT6577_RZ_VRATIO_ADDR(i)                    (i + 0x001C)
#define MT6577_RZ_LOCK_ADDR(i)                      (i + 0x0020)
#define MT6577_RZ_CROPLR_ADDR(i)                    (i + 0x00F4)
#define MT6577_RZ_CROPTB_ADDR(i)                    (i + 0x00F8)


#define MT6577_VRZ_CFG_ADDR(i)                      (i + 0x00)
#define MT6577_VRZ_INT_ADDR(i)                      (i + 0x0C)
#define MT6577_VRZ_HRES_ADDR(i)                     (i + 0x20)
#define MT6577_VRZ_VRES_ADDR(i)                     (i + 0x24)
//#define MT6577_VRZ_FRCFG_ADDR(i)                  (i + 0x40)


#define MT6577_JPEG_DMA_CON_ADDR(i)                 (i + 0x0C)
#define MT6577_JPEG_DMA_INTERRUPT_ADDR(i)           (i + 0x20)

#define MT6577_BRZ_INT_ADDR(i)                      (i + 0x08)

#define MT6577_OVL_DMA_IRQ_FLAG_ADDR(i)             (i)
#define MT6577_OVL_DMA_IRQ_FLAG_CLR_ADDR(i)         (i + 0x08)


/*Copy From MT6577MDPReg.h*/
#define MT6577_MMSYS1_REG_RANGE    0x230

#define MT6577_RDMA0_REG_RANGE 0x3D0
#define MT6577_RDMA1_REG_RANGE 0x3D0

#define MT6577_BRZ_REG_RANGE    0x34

#define MT6577_CRZ_REG_RANGE    0xFC
#define MT6577_PRZ0_REG_RANGE   0xFC
#define MT6577_PRZ1_REG_RANGE   0xFC

#define MT6577_VRZ0_REG_RANGE      0xB8
#define MT6577_VRZ1_REG_RANGE      0xB8

#define MT6577_IPP_REG_RANGE        0x32C
#define MT6577_OVL_DMA_REG_RANGE    0x14C

#define MT6577_RGB_ROT0_REG_RANGE           0x434
#define MT6577_RGB_ROT1_REG_RANGE           0x434
#define MT6577_RGB_ROT2_REG_RANGE           0x434
#define MT6577_VDO_ROT0_REG_RANGE           0x434
#define MT6577_VDO_ROT1_REG_RANGE           0x434

#define MT6577_JPEG_DMA_REG_RANGE      0xB0


/*/////////////////////////////////////////////////////////////////////////////
    Debug Message
  /////////////////////////////////////////////////////////////////////////////*/
extern int b_mdp_show_info;
extern int b_mdp_show_warning;
extern int b_mdp_show_error;   //Always kepp this on
extern int b_mdp_show_isrinfo;
extern int b_mdp_show_powerinfo;
extern int b_mdp_show_zoominfo;
extern int b_mdp_show_fpsinfo;
extern int b_mdp_show_bbinfo;  //MDPK Bitblt info
extern int b_mdp_show_zsdinfo;

extern int b_mdp_timeout_dumpreg;





#if 0 /*Xlog version*/

#define MDPK_PRINTF                 xlog_printk

//#define MDPK_SHOWFUNCTION( b_valid_function )     {  MDPK_PRINTF("[MDPK Func Call]: %s()%s\n\r", __FUNCTION__, b_valid_function?"":":Under Implement!" );  } 
#define MDPK_SHOWFUNCTION( b_valid_function )

#define MDPK_MSG(fmt, arg...)                                   { MDPK_PRINTF(ANDROID_LOG_INFO,"MDPK","[MDPK MSG]: %s(): "fmt,__FUNCTION__, ##arg); }
#define MDPK_INFO(fmt, arg...)      if( b_mdp_show_info )       { MDPK_PRINTF(ANDROID_LOG_INFO,"MDPK","[MDPK INFO]: %s(): "fmt,__FUNCTION__, ##arg); }
#define MDPK_WARNING(fmt, arg...)   if( b_mdp_show_warning )    { MDPK_PRINTF(ANDROID_LOG_WARN,"MDPK","[MDPK W]: %s(): "fmt,__FUNCTION__, ##arg);   }
#define MDPK_ERROR(fmt, arg...)     if( b_mdp_show_error )      { MDPK_PRINTF(ANDROID_LOG_ERROR,"MDPK","[MDPK E]: %s(): %s@%d: "fmt,__FUNCTION__, __FILE__,__LINE__, ##arg); }
//#define MDPK_ISRINFO(fmt, arg...)   if( b_mdp_show_isrinfo )    { MDPK_PRINTF(ANDROID_LOG_INFO,"MDPK","[MDPK ISR]: %s(): "fmt,__FUNCTION__, ##arg);    }
#define MDPK_ISRINFO(fmt, arg...) {;}
#define MDPK_POWERINFO(fmt, arg...) if( b_mdp_show_powerinfo )  { MDPK_PRINTF(ANDROID_LOG_INFO,"MDPK","[MDPK CG]: "fmt, ##arg);  }
#define MDPK_ZOOMINFO(fmt, arg...)  if( b_mdp_show_zoominfo )   { MDPK_PRINTF(ANDROID_LOG_INFO,"MDPK","[MDPK ZOOM]: "fmt, ##arg); }
#define MDPK_FPSINFO(fmt, arg...)   if( b_mdp_show_fpsinfo )    { MDPK_PRINTF(ANDROID_LOG_INFO,"MDPK","[MDPK FPS]: "fmt, ##arg); }
#define MDPK_BBINFO(fmt, arg...)    if( b_mdp_show_bbinfo )     { MDPK_PRINTF(ANDROID_LOG_INFO,"MDPK","[MDPK BB]: "fmt, ##arg); }

#define MDPK_DEBUG(fmt, arg...)                                 { MDPK_PRINTF(ANDROID_LOG_DEBUG,"MDPK","[MDPK DEBUG]: "fmt, ##arg); }

#else /*printk version*/

#define MDPK_PRINTF               printk

//#define MDPK_SHOWFUNCTION( b_valid_function )     {  MDPK_PRINTF("[MDPK Func Call]: %s()%s\n\r", __FUNCTION__, b_valid_function?"":":Under Implement!" );  } 
#define MDPK_SHOWFUNCTION( b_valid_function )

#define MDPK_MSG(fmt, arg...)                                   { MDPK_PRINTF("[MDPK MSG]: %s(): "fmt,__FUNCTION__, ##arg); }
#define MDPK_INFO(fmt, arg...)      if( b_mdp_show_info )       { MDPK_PRINTF("[MDPK INFO]: %s(): "fmt,__FUNCTION__, ##arg); }
#define MDPK_WARNING(fmt, arg...)   if( b_mdp_show_warning )    { MDPK_PRINTF("[MDPK W]: %s(): "fmt,__FUNCTION__, ##arg);   }
#define MDPK_ERROR(fmt, arg...)     if( b_mdp_show_error )      { MDPK_PRINTF("[MDPK E]: %s(): %s@%d: "fmt,__FUNCTION__, __FILE__,__LINE__, ##arg); }
#define MDPK_ISRINFO(fmt, arg...)   if( b_mdp_show_isrinfo )    { MDPK_PRINTF("[MDPK ISR]: %s(): "fmt,__FUNCTION__, ##arg);    }
//#define MDPK_ISRINFO(fmt, arg...) {;}
#define MDPK_POWERINFO(fmt, arg...) if( b_mdp_show_powerinfo )  { MDPK_PRINTF("[MDPK CG]: "fmt, ##arg);  }
#define MDPK_ZOOMINFO(fmt, arg...)  if( b_mdp_show_zoominfo )   { MDPK_PRINTF("[MDPK ZOOM]: "fmt, ##arg); }
#define MDPK_FPSINFO(fmt, arg...)   if( b_mdp_show_fpsinfo )    { MDPK_PRINTF("[MDPK FPS]: "fmt, ##arg); }
#define MDPK_BBINFO(fmt, arg...)    if( b_mdp_show_bbinfo )     { MDPK_PRINTF("[MDPK BB]: "fmt, ##arg); }
#define MDPK_ZSDINFO(fmt, arg...)    if( b_mdp_show_zsdinfo )     { MDPK_PRINTF("[MDPK ZSD]: "fmt, ##arg); }

#define MDPK_DEBUG(fmt, arg...)                                 { MDPK_PRINTF("[MDPK DEBUG]: "fmt, ##arg); }


#endif



/*/////////////////////////////////////////////////////////////////////////////
    MDPK Function Prototype
  /////////////////////////////////////////////////////////////////////////////*/
/*Channel definition for MDPK Bitblit*/
#define MDPKBITBLIT_CHNL_COUNT  (32)

#define MDPK_BB_CH_HDMI_0   (0)
#define MDPK_BB_CH_HDMI_1   (1)
#define MDPK_BB_CH_HDMI_2   (2)
#define MDPK_BB_CH_HDMI_3   (3)




int MDPK_Bitblt_Init( void );   //Call once only when system boot
int MDPK_Bitblt_Config( int channel, MdpkBitbltConfig* pParam );
int MDPK_Bitblt( int channel );
int MDPK_Bitblt_IOCTL_Wait_Request( unsigned long ioctl_user_param  );
int MDPK_Bitblt_IOCTL_Inform_Done( unsigned long ioctl_user_param  );
int MDPK_Util_Map_Vmalloc_to_User( struct vm_area_struct* p_vma );

unsigned long MDPK_Util_MsToJiffies(unsigned long u4ms);

int MDPK_Util_Spc_Monitor( int b_on, unsigned long start_addr, unsigned long end_addr );



/*-----------------------------------------------------------------------------
    Test Harness
  -----------------------------------------------------------------------------*/
void MDPK_TestFunc_1( unsigned long channel );
void MDPK_TestFunc_2( unsigned long channel );




#endif
