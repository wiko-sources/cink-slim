#ifndef _MT6575_SPC_H_
#define _MT6575_SPC_H_


#define SPC_ID_DEFECT         1  //con0 bit2
#define SPC_ID_CAM            2 
#define SPC_ID_ROTDMA0_OUT1   3 
#define SPC_ID_ROTDMA0_OUT2   4 
#define SPC_ID_ROTDMA1_OUT1   5 
#define SPC_ID_ROTDMA1_OUT2   6 
#define SPC_ID_TVROT_OUT1     7 
#define SPC_ID_TVROT_OUT2     8 
#define SPC_ID_FD1            9 
#define SPC_ID_PCA            10
#define SPC_ID_JPEGDMA_R      11 //con1 bit2
#define SPC_ID_JPEGDMA_W      12
#define SPC_ID_RDMA0_OUT3     13
#define SPC_ID_JPEG_DEC1      14
#define SPC_ID_TVROT_OUT3     15
#define SPC_ID_ROTDMA0_OUT3   16
#define SPC_ID_ROTDMA1_OUT3   17
#define SPC_ID_EMI_18         18    
//18 include:
// jpg_enc, rot_dma0_out0, rot_dma1_out0, tv_rot_out0,
// fd0, fd2, jpg_dec0, r_dma0_out0, r_dma0_out1, r_dma0_out2
#define SPC_ID_ROTDMA2_OUT1   19  //con2 bit23
#define SPC_ID_ROTDMA2_OUT2   20
#define SPC_ID_ROTDMA3_OUT1   21
#define SPC_ID_ROTDMA3_OUT2   22  //con3 bit2
#define SPC_ID_ROTDMA4_OUT1   23
#define SPC_ID_ROTDMA4_OUT2   24
#define SPC_ID_TVC_PFH        25
#define SPC_ID_TVC_RESZ       26
#define SPC_ID_OVL_DCP        27
#define SPC_ID_VRZ            28
#define SPC_ID_GREQ_BLKW      29
#define SPC_ID_GREQ_BLKR      30
#define SPC_ID_RDMA1_OUT3   31
#define SPC_ID_EIS            32  //con4 bit2
#define SPC_ID_ROTDMA2_OUT3   33
#define SPC_ID_ROTDMA3_OUT3   34
#define SPC_ID_ROTDMA4_OUT3   35
#define SPC_ID_EMI_36         36
//36 include:
// ovl_msk, dpi, rot_dma2_out0, rot_dma3_out0, rot_dma4_out0, tvc,
// lcd_r, lcd_w, r_dma1_out0, r_dma1_out1, r_dma1_out2, spi, dpi_hwc
#define SPC_ID_VDEC_POST1     37
#define SPC_ID_EMI_38         38    
//38 include:
// venc_mc, venc_bsdma, venc_mvqp, vdec_dma, vdec_rec, vdec_post0         
#define SPC_ID_AUDIO          39
#define SPC_ID_EMI_40         40
//40 include:
// g2d_r, g2d_w, audio

#define SPC_MASTER_NUMBER     41


//==============================================================================================
//  macros to enable engines that spc will monitor
// =============================================================================================
#define LARB0_DEFECT_EN           (1<<(1 -1))     
#define LARB0_CAM_EN              (1<<(2 -1))     
#define LARB0_ROTDMA0_OUT1_EN     (1<<(3 -1))     
#define LARB0_ROTDMA0_OUT2_EN     (1<<(4 -1))     
#define LARB0_ROTDMA1_OUT1_EN     (1<<(5 -1))     
#define LARB0_ROTDMA1_OUT2_EN     (1<<(6 -1))     
#define LARB0_TVROT_OUT1_EN       (1<<(7 -1))     
#define LARB0_TVROT_OUT2_EN       (1<<(8 -1))     
#define LARB0_FD1_EN              (1<<(9 -1))     
#define LARB0_PCA_EN              (1<<(10-1))     
#define LARB0_JPEGDMA_R_EN        (1<<(11-1))     
#define LARB0_JPEGDMA_W_EN        (1<<(12-1))     
#define LARB0_RDMA0_OUT3_EN       (1<<(13-1))     
#define LARB0_JPEG_DEC1_EN        (1<<(14-1))     
#define LARB0_TVROT_OUT3_EN       (1<<(15-1))     
#define LARB0_ROTDMA0_OUT3_EN     (1<<(16-1))     
#define LARB0_ROTDMA1_OUT3_EN     (1<<(17-1))     
#define LARB0_EMI_18_EN           (1<<(18-1))         
//18 include:
// jpg_enc, rot_dma0_out0, rot_dma1_out0, tv_rot_out0,
// fd0, fd2, jpg_dec0, r_dma0_out0, r_dma0_out1, r_dma0_out2
#define LARB1_ROTDMA2_OUT1_EN  (1<<(19-19)) 
#define LARB1_ROTDMA2_OUT2_EN  (1<<(20-19))
#define LARB1_ROTDMA3_OUT1_EN  (1<<(21-19))
#define LARB1_ROTDMA3_OUT2_EN  (1<<(22-19))
#define LARB1_ROTDMA4_OUT1_EN  (1<<(23-19))
#define LARB1_ROTDMA4_OUT2_EN  (1<<(24-19))
#define LARB1_TVC_PFH_EN       (1<<(25-19))
#define LARB1_TVC_RESZ_EN      (1<<(26-19))
#define LARB1_OVL_DCP_EN       (1<<(27-19))
#define LARB1_VRZ_EN           (1<<(28-19))
#define LARB1_GREQ_BLKW_EN     (1<<(29-19))
#define LARB1_GREQ_BLKR_EN     (1<<(30-19))
#define LARB1_RDMA1_OUT3_EN  (1<<(31-19))
#define LARB1_EIS_EN           (1<<(32-19))
#define LARB1_ROTDMA2_OUT3_EN  (1<<(33-19))
#define LARB1_ROTDMA3_OUT3_EN  (1<<(34-19))
#define LARB1_ROTDMA4_OUT3_EN  (1<<(35-19))
#define LARB1_EMI_36_EN        (1<<(36-19))
//36 include:
// ovl_msk, dpi, rot_dma2_out0, rot_dma3_out0, rot_dma4_out0, tvc,
// lcd_r, lcd_w, r_dma1_out0, r_dma1_out1, r_dma1_out2, spi, dpi_hwc
#define LARB2_VDEC_POST1_EN    (1<<(37-37))
#define LARB2_EMI_38_EN        (1<<(38-37))   
//38 include:
// venc_mc, venc_bsdma, venc_mvqp, vdec_dma, vdec_rec, vdec_post0         
#define LARB3_AUDIO_EN         (1<<(39-39))
#define LARB3_EMI_40_EN        (1<<(40-39))
//40 include:
// g2d_r, g2d_w, audio

#define LARB0_ALL_EN 0x07ffffff
#define LARB1_ALL_EN 0x7fffffff
#define LARB2_ALL_EN 0x000001ff
#define LARB3_ALL_EN 0x0000000f


#define MM_SYSRAM_BASE_PA       (0xc2000000) 
#define MM_SYSRAM_SIZE          (0x18000+0x8000+0x20000)

typedef struct __SPC_cfg
{
    unsigned int RegionID;            // region, 0~4
    unsigned int Enable;              // enable or disable the monitor
    unsigned int EngineMaskLARB0;     // master ID mask, 0~26, one bit represent one master, 1 means monitor this master, 0 means do not care this master
    unsigned int EngineMaskLARB1;     // master ID mask, 0~31
    unsigned int EngineMaskLARB2;     // master ID mask, 0~8
    unsigned int EngineMaskLARB3;     // master ID mask, 0~3
    unsigned int ReadEn; 
    unsigned int WriteEn;       
    unsigned int StartAddr;     
    unsigned int EndAddr;
} SPC_cfg; 

// set region as secure domain, then set engines as non-secure to monitor 
// set the addr range as range1 (SMI_SRAM_RANGE0~SMI_SRAM_RANGE1)
// larb0: CON0[29:0], CON1[29:0], CON2[20:0]
// larb1: CON2[29:21], CON3[29:0], CON4[29:0], CON5[23:0]
// larb2: CON5[29:24], CON6[14:0]
// larb3: CON6[29:21], CON7[2:0]

#define MT6575_SPCMAGICNO 's'
#define MT6575SPC_CONFIG            _IOW(MT6575_SPCMAGICNO, 0, int)
#define MT6575SPC_DUMP_REG          _IOW(MT6575_SPCMAGICNO, 1, int)
#define MT6575SPC_CMD               _IOW(MT6575_SPCMAGICNO, 2, int)
#define MT6575SPC_STATUS_CHECK      _IOW(MT6575_SPCMAGICNO, 3, int)



extern unsigned int spc_config(SPC_cfg *pSpcConf);


#endif
