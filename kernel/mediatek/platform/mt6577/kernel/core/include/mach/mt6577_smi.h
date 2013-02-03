#define SMI_DEV_MAJOR_NUMBER 250
#define SMIMONITORMAGICNO 'p'
 


typedef struct {
	  unsigned long u4Master;   //SMI master 0~3
    unsigned long u4PortNo;
    unsigned long bBusType : 1;//0 for GMC, 1 for AXI
    unsigned long bDestType : 2;//0 for EMI+internal mem, 1 for EMI, 3 for internal mem
    unsigned long bRWType : 2;//0 for R+W, 1 for read, 2 for write
}SMIBMCfg;

typedef struct {
    unsigned long bIdleSelection : 1; // 0 : idle count increase when no request, and outstanding request is less than , 1 : idle count increase when there is no request and read/write data transfer.
    unsigned long uIdleOutStandingThresh : 3;
    unsigned long bDPSelection : 1; // 0 : data phase incresae 1 when any outstanding transaction waits for data transfer. 1 : data phase increase N when N out standing transaction are waiting.
    unsigned long bMaxPhaseSelection : 1;// 0 : Command pahse , 1 : Data phase.
    unsigned long bStarvationEn : 1; // 0 : disable , 1 : Enable
    unsigned long uStarvationTime : 8;
    unsigned long u2Reserved : 12; //Reserved
}SMIBMCfg_Ext;

 typedef struct {
    unsigned long u4EndTimeSec;
    unsigned long u4EndTimeMicroSec;
    unsigned long u4ActiveCnt;
    unsigned long u4RequestCnt;
    unsigned long u4IdleCnt;
    unsigned long u4ByteCnt;
    unsigned long u4BeatCnt;
    unsigned long u4CommPhaseAccum;
    unsigned long u4DataPhaseAccum;
    unsigned long u4MaxCommOrDataPhase;
    unsigned long u4MaxOutTransaction;
    SMIBMCfg      cfg;                  // For recording Port information
    SMIBMCfg_Ext  cfg_ex;
}SMIBMResult;

#define MT6575_SMI_ALLPORT_NUM 71
#define MT6575_SMI1_PORT_NUM 27
#define MT6575_SMI2_PORT_NUM 31
#define MT6575_SMI3_PORT_NUM 9
#define MT6575_SMI4_PORT_NUM 4


char* MT6575_SMI1_PORT_NAME[256] = 
{
    "DEFECT",
    "JPG_ENC",
    "ROT_DMA0_OUT0",
    "ROT_DMA1_OUT0",
    "TV_ROT_OUT0",
    "CAM",
    "FD0",
    "FD2",
    "JPG_DEC0",
    "R_DMA0_OUT0",
    "R_DMA0_OUT1",
    "R_DMA0_OUT2",
    "FD1",
    "PCA",
    "JPGDMA_R",
    "JPGDMA_W",
    "ROT_DMA0_OUT1",
    "ROT_DMA0_OUT2",
    "ROT_DMA1_OUT1",
    "ROT_DMA1_OUT2",
    "TV_ROT_OUT1",
    "TV_ROT_OUT2",
    "R_DMA0_OUT3",
    "JPG_DEC1",
    "TV_ROT_OUT3",
    "ROT_DMA0_OUT3",
    "ROT_DMA1_OUT3"
};

char* MT6575_SMI2_PORT_NAME[256] = 
{
    "OVL_MSK",
    "OVL_DCP",
    "DPI",
    "ROT_DMA2_OUT0",
    "ROT_DMA3_OUT0",
    "ROT_DMA4_OUT0",
    "TVC",
    "LCD_R",
    "LCD_W",
    "R_DMA1_OUT0",
    "R_DMA1_OUT1",
    "R_DMA1_OUT2",
    "SPI",
    "DUMMY",
    "DPI_HWC",
    "VRZ",
    "ROT_DMA2_OUT1",
    "ROT_DMA2_OUT2",
    "ROT_DMA3_OUT1",
    "ROT_DMA3_OUT2",
    "ROT_DMA4_OUT1",
    "ROT_DMA4_OUT2",
    "GREQ_BLKW",
    "GREQ_BLKR",
    "TVC_PFH",
    "TVC_RESZ",
    "R_DMA1_OUT3",
    "EIS",
    "ROT_DMA2_OUT3",
    "ROT_DMA3_OUT3",
    "ROT_DAM4_OUT3"
};

char* MT6575_SMI3_PORT_NAME[256] = 
{
    "VENC_MC",
    "VENC_BSDMA",
    "VENC_MVQP",
    "VDEC_DMA",
    "VDEC_REC",
    "VDEC_POST0",
    "VDEC_POST1",
    "VDEC_ACP",
    "VENC_MC_ACP"
};

char* MT6575_SMI4_PORT_NAME[256] = 
{
    "G2D_W",
    "G2D_R",
    "AUDIO_0",
    "AUDIO_1"
};




typedef struct{
	
	SMIBMCfg cfg;
	SMIBMCfg_Ext cfg_ext;
	
}stManualTriggerInitCfg;

typedef struct{
	
	SMIBMCfg cfg;
	SMIBMCfg_Ext cfg_ext;
	SMIBMResult result;
	
}stManualTriggerResultCfg;

typedef struct{
	
	SMIBMCfg cfg;
	SMIBMCfg_Ext cfg_ext;
	unsigned long smi_time_count;
	unsigned long smi_time_interval;
	
}stAutoTriggerInitCfg;

typedef struct{
	
	unsigned long total_ports;
	unsigned long smi_time_interval;
	
}stAutoTriggerSelectTime;

//User space interface
//IOCTL commnad
//Measure SMI all ports in different timing
#define SMI_MANUAL_TRIGGER_INIT     _IOWR(SMIMONITORMAGICNO, 0, stManualTriggerInitCfg)
#define SMI_MANUAL_TRIGGER_INIT_EXT _IOWR(SMIMONITORMAGICNO, 1, stManualTriggerInitCfg)
#define SMI_MANUAL_TRIGGER_RESULT   _IOWR(SMIMONITORMAGICNO, 2, stManualTriggerResultCfg)

#define SMI_AUTO_TRIGGER_INIT       _IOWR(SMIMONITORMAGICNO, 3, stAutoTriggerInitCfg)
#define SMI_AUTO_TRIGGER_INIT_EXT   _IOWR(SMIMONITORMAGICNO, 4, stAutoTriggerInitCfg)
#define SMI_AUTO_TRIGGER_RESULT     _IOWR(SMIMONITORMAGICNO, 5, SMIBMResult)

#define SMI_AUTO_TRIGGER_SELECT_TIME      _IOWR(SMIMONITORMAGICNO, 6, stAutoTriggerSelectTime)
#define SMI_AUTO_TRIGGER_SELECT           _IOWR(SMIMONITORMAGICNO, 7, SMIBMCfg)
#define SMI_AUTO_TRIGGER_SELECT_EXT       _IOWR(SMIMONITORMAGICNO, 8, SMIBMCfg_Ext)
#define SMI_AUTO_TRIGGER_SELECT_RESULT    _IOWR(SMIMONITORMAGICNO, 9, SMIBMResult)

#define SMI_MANU_TRIGGER_SELECT_TIME      _IOWR(SMIMONITORMAGICNO, 10, unsigned long)
#define SMI_MANU_TRIGGER_SELECT           _IOWR(SMIMONITORMAGICNO, 11, SMIBMCfg)
#define SMI_MANU_TRIGGER_SELECT_EXT       _IOWR(SMIMONITORMAGICNO, 12, SMIBMCfg_Ext)
#define SMI_MANU_TRIGGER_SELECT_TRIG      _IOWR(SMIMONITORMAGICNO, 13, unsigned long)
#define SMI_MANU_TRIGGER_SELECT_RESULT    _IOWR(SMIMONITORMAGICNO, 14, SMIBMResult)
