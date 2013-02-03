#ifndef _ISPIF_MT6575_
#define _ISPIF_MT6575_

/*******************************************************************************
* Preprocessing Control Register 1
*******************************************************************************/
//CAM_CTRL1 CAM+0040H
typedef struct {
    MUINT32 NONE0       :1; //[0]
    MUINT32 PGAIN_FRAC  :7; //[7:1]
    MUINT32 PGAIN_INT   :2; //[9:8]
    MUINT32 NONE1       :2; //[11:10]
    MUINT32 GPID        :1; //[12]
    MUINT32 GLID        :1; //[13]
    MUINT32 REV1        :1; //[14]
    MUINT32 PGAIN_DB    :1; //[15]
    MUINT32 P_LIMIT     :8; //[23:16]
    MUINT32 GAIN_COMP   :8; //[31:24]
} ISP_CAM_CTRL1_T;

/*******************************************************************************
* Shading
*******************************************************************************/
//RAWACC_REG CAM+01BCH
typedef struct {
    MUINT32 RAWACC_EN        : 1; //[0]
    MUINT32 RESERVED0        : 31; //[31:1] unused
} ISP_RAW_ACC_CFG_T;
//
//RAWWIN_REG CAM+01C0H
typedef struct {
    MUINT32 RAWWIN_D         : 8; //[7:0]
    MUINT32 RAWWIN_U         : 8; //[15:8]
    MUINT32 RAWWIN_R         : 8; //[23:16]
    MUINT32 RAWWIN_L         : 8; //[31:24]
} ISP_RAW_ACC_WIN_T;
//
//RAWSUM CAM+01C4H/01C8H/01CCH/01D0H
typedef struct {
    MUINT32 RAWSUM           : 32;//[31:0]
} ISP_RAW_ACC_RESULT_T;
//
//CAM_SHADING1 CAM+0214H
typedef struct {
    MUINT32 SDBLK_YOFST      : 6;  //[5:0] unused (new added: must be 0)
    MUINT32 NONE0            : 10; //[15:6] unused
    MUINT32 SDBLK_XOFST      : 6;  //[21:16] unused (new added: must be 0)
    MUINT32 NONE1            : 2;  //[23:22] unused
    MUINT32 SD_COEFRD_MODE   : 1;  //[24] unused (new added: must be 0)
    MUINT32 NONE2            : 3;  //[27:25] unused
    MUINT32 SHADING_EN       : 1;  //[28] 0
    MUINT32 SDBLK_TRIG       : 1;  //[29] 0
    MUINT32 NONE3            : 2;  //[31:30] unused
} ISP_SHADING_CTRL1_T;
//
//CAM_SHADING2 CAM+0218H
typedef struct {
    MUINT32 SDBLK_HEIGHT     : 12; //[11:0]
    MUINT32 SDBLK_YNUM       : 4; //[15:12]
    MUINT32 SDBLK_WIDTH      : 12; //[27:16]
    MUINT32 SDBLK_XNUM       : 4; //[31:28]
} ISP_SHADING_CTRL2_T;
//
//SD_RADDR CAM+021CH
typedef struct {
    MUINT32 SHADING_RADDR    : 32; //[31:0]
} ISP_SHADING_READ_ADDR_T;
//
//SD_LBLOCK CAM+0220H
typedef struct {
    MUINT32 SDBLK_LHEIGHT    : 12; //[11:0]
    MUINT32 RESERVED2        : 4; //[15:12] unused
    MUINT32 SDBLK_LWIDTH     : 12; //[27:16]
    MUINT32 RESERVED3        : 4; //[31:28] unused
} ISP_SHADING_LAST_BLK_T;
//
//SD_RATIO CAM+0224H
typedef struct {
    MUINT32 SDBLK_RATIO11    : 6; //[5:0]
    MUINT32 RESERVED4        : 2; //[7:6]   unused
    MUINT32 SDBLK_RATIO10    : 6; //[13:8]
    MUINT32 RESERVED5        : 2; //[15:14] unused
    MUINT32 SDBLK_RATIO01    : 6; //[21:16]
    MUINT32 RESERVED6        : 2; //[23:22] unused
    MUINT32 SDBLK_RATIO00    : 6; //[29:24]
    MUINT32 RESERVED7        : 2; //[31:30] unused
} ISP_SHADING_RATIO_CFG_T;

/*******************************************************************************
* OB
*******************************************************************************/
//CAM_RGBOFF CAM+0090H
typedef struct {
    MUINT32 OFF11            : 7; //[6:0]
    MUINT32 S11              : 1; //[7]
    MUINT32 OFF10            : 7; //[14:8]
    MUINT32 S10              : 1; //[15]
    MUINT32 OFF01            : 7; //[22:16]
    MUINT32 S01              : 1; //[23:]
    MUINT32 OFF00            : 7; //[30:24]
    MUINT32 S00              : 1; //[31]
} ISP_RGBOFF_T;

/*******************************************************************************
* NR1
*******************************************************************************/
//NR1_CON CAM+0550H
typedef struct {
    MUINT32 DP_EN            : 1; //[0]
    MUINT32 CT_EN            : 1; //[1]
    MUINT32 NR_EN            : 1; //[2]
    MUINT32 BEN              : 1; //[3] unused (binning)
    MUINT32 BMD              : 1; //[4] unused (binning)
    MUINT32 BOP              : 1; //[5] unused (binning)
    MUINT32 CT_MD            : 2; //[7:6]
    MUINT32 RESERVED0        : 1; //[8]     unused
    MUINT32 BSFT             : 1; //[9] unused (binning)
    MUINT32 HALF_THRPT       : 1; //[10]
    MUINT32 RESERVED1        : 21; //[31:11]unused
} ISP_NR1_CTRL_T;
//
//NR1_DP1 CAM+0554H
typedef struct {
    MUINT32 DP_THRD1         : 10; //[9:0]      B'0000000000
    MUINT32 RESERVED2        : 2; //[11:10]     unused
    MUINT32 DP_THRD3         : 4; //[15:12]     B'0000
    MUINT32 DP_THRD0         : 10; //[25:16]    B'0000000000
    MUINT32 RESERVED101      : 2; //[27:26]     unused
    MUINT32 DP_THRD2         : 4; //[31:28]     B'0000
} ISP_NR1_DP1_T;
//
//NR1_DP2 CAM+0558H
typedef struct {
    MUINT32 DP_THRD5         : 10; //9:0]       B'0000000000
    MUINT32 RESERVED3        : 6; //[15:10]     unused
    MUINT32 DP_THRD4         : 10; //[25:16]    B'0000000000
    MUINT32 RESERVED4        : 6; //[31:26]     unused
} ISP_NR1_DP2_T;
//
//NR1_DP3 CAM+055CH
typedef struct {
    MUINT32 DP_THRD7         : 10; //9:0]       B'0000000000
    MUINT32 RESERVED5        : 6; //[15:10]     unused
    MUINT32 DP_THRD6         : 10; //[25:16]    B'0000000000
    MUINT32 RESERVED6        : 6; //[31:26]     unused
} ISP_NR1_DP3_T;
//
//NR1_DP4 CAM+0560H
typedef struct {
    MUINT32 DP_CD1           : 1; //[0]         0
    MUINT32 DP_CD2           : 1; //[1]         0
    MUINT32 DP_CD3           : 1; //[2]         0
    MUINT32 DP_CD4           : 1; //[3]         0
    MUINT32 DP_CD5           : 1; //[4]         0
    MUINT32 DP_CD6           : 1; //[5]         0
    MUINT32 DP_CD7           : 1; //[6]         0
    MUINT32 DP_SEL           : 3; //[9:7]       B'00
    MUINT32 DP_NUM           : 3; //[12:10]     B'000
    MUINT32 RESERVED7        : 19; //[31:13]    unused
} ISP_NR1_DP4_T;
//
//NR1_CT CAM+0564H
typedef struct {
    MUINT32 CT_THRD          : 8; //[7:0]       B'10100000
    MUINT32 CT_DIV           : 1; //[8]         0
    MUINT32 RESERVED8        : 3; //[11:9]      unused
    MUINT32 CT_MD2           : 1; //[12]        0
    MUINT32 RESERVED9        : 19; //[31:13]    unused
} ISP_NR1_CT_T;
//
//NR1_NR1 CAM+0568H
typedef struct {
    MUINT32 MBND             : 10; //[9:0]      B'0010000000
    MUINT32 S2               : 3; //[12:10]     B'100
    MUINT32 S1               : 3; //[15:13]     B'011
    MUINT32 GNF              : 5; //[20:16]     B'10000
    MUINT32 GNF_EDGE         : 5; //[25:21]     B'10000
    MUINT32 S3               : 3; //[28:26]     B'001
    MUINT32 RESERVED10       : 3; //[31:29]     unused
} ISP_NR1_NR_CFG1_T;
//
//NR1_NR2 CAM+056CH
typedef struct {
    MUINT32 GN3              : 4; //[3:0]       B'1000
    MUINT32 GN2              : 4; //[7:4]       B'0110
    MUINT32 GN1              : 4; //[11:8]      B'0101
    MUINT32 RESERVED11       : 20; //[31:12]    unused
} ISP_NR1_NR_CFG2_T;
//
//NR1_NR3 CAM+0570H
typedef struct {
    MUINT32 VLR4             : 6; //[5:0]       B'001000
    MUINT32 RESERVED12       : 2; //[7:6]       unused
    MUINT32 VLR3             : 6; //[13:8]      B'001000
    MUINT32 RESERVED13       : 2; //[15:14]     unused
    MUINT32 VLR2             : 6; //[21:16]     B'000110
    MUINT32 RESERVED14       : 2; //[23:22]     unused
    MUINT32 VLR1             : 6; //[29:24]     B'000100
    MUINT32 RESERVED15       : 2; //[31:30]     unused
} ISP_NR1_NR_VLR14_T;
//
//NR1_NR4 CAM+0574H
typedef struct {
    MUINT32 VLR8             : 6; //[5:0]       B'001100
    MUINT32 RESERVED16       : 2; //[7:6]       unused
    MUINT32 VLR7             : 6; //[13:8]      B'001010
    MUINT32 RESERVED17       : 2; //[15:14]     unused
    MUINT32 VLR6             : 6; //[21:16]     B'001000
    MUINT32 RESERVED18       : 2; //[23:22]     unused
    MUINT32 VLR5             : 6; //[29:24]     B'001000
    MUINT32 RESERVED19       : 2; //[31:30]     unused
} ISP_NR1_NR_VLR58_T;
//
//NR1_NR5 CAM+0578H
typedef struct {
    MUINT32 VLGR4            : 6; //[5:0]       B'001000
    MUINT32 RESERVED20       : 2; //[7:6]       unused
    MUINT32 VLGR3            : 6; //[13:8]      B'001000
    MUINT32 RESERVED21       : 2; //[15:14]     unused
    MUINT32 VLGR2            : 6; //[21:16]     B'000110
    MUINT32 RESERVED22       : 2; //[23:22]     unused
    MUINT32 VLGR1            : 6; //[29:24]     B'000100
    MUINT32 RESERVED23       : 2; //[31:30]     unused
} ISP_NR1_NR_VLGR14_T;
//
//NR1_NR6 CAM+057CH
typedef struct {
    MUINT32 VLGR8            : 6; //[5:0]       B'001100
    MUINT32 RESERVED24       : 2; //[7:6]       unused
    MUINT32 VLGR7            : 6; //[13:8]      B'001010
    MUINT32 RESERVED25       : 2; //[15:14]     unused
    MUINT32 VLGR6            : 6; //[21:16]     B'001000
    MUINT32 RESERVED26       : 2; //[23:22]     unused
    MUINT32 VLGR5            : 6; //[29:24]     B'001000
    MUINT32 RESERVED27       : 2; //[31:30]     unused
} ISP_NR1_NR_VLGR58_T;
//
//NR1_NR7 CAM+0580H
typedef struct {
    MUINT32 VLGB4            : 6; //[5:0]       B'001000
    MUINT32 RESERVED28       : 2; //[7:6]       unused
    MUINT32 VLGB3            : 6; //[13:8]      B'001000
    MUINT32 RESERVED29       : 2; //[15:14]     unused
    MUINT32 VLGB2            : 6; //[21:16]     B'000110
    MUINT32 RESERVED30       : 2; //[23:22]     unused
    MUINT32 VLGB1            : 6; //[29:24]     B'000100
    MUINT32 RESERVED31       : 2; //[31:30]     unused
} ISP_NR1_NR_VLGB14_T;
//
//NR1_NR8 CAM+0584H
typedef struct {
    MUINT32 VLGB8            : 6; //[5:0]       B'001100
    MUINT32 RESERVED32       : 2; //[7:6]       unused
    MUINT32 VLGB7            : 6; //[13:8]      B'001010
    MUINT32 RESERVED33       : 2; //[15:14]     unused
    MUINT32 VLGB6            : 6; //[21:16]     B'001000
    MUINT32 RESERVED34       : 2; //[23:22]     unused
    MUINT32 VLGB5            : 6; //[29:24]     B'001000
    MUINT32 RESERVED35       : 2; //[31:30]     unused
} ISP_NR1_NR_VLGB58_T;
//
//NR1_NR9 CAM+0588H
typedef struct {
    MUINT32 VLB4             : 6; //[5:0]       B'001000
    MUINT32 RESERVED36       : 2; //[7:6]       unused
    MUINT32 VLB3             : 6; //[13:8]      B'001000
    MUINT32 RESERVED37       : 2; //[15:14]     unused
    MUINT32 VLB2             : 6; //[21:16]     B'000110
    MUINT32 RESERVED38       : 2; //[23:22]     unused
    MUINT32 VLB1             : 6; //[29:24]     B'000100
    MUINT32 RESERVED39       : 2; //[31:30]     unused
} ISP_NR1_NR_VLB14_T;
//
//NR1_NR10 CAM+058CH
typedef struct {
    MUINT32 VLB8             : 6; //[5:0]       B'001100
    MUINT32 RESERVED40       : 2; //[7:6]       unused
    MUINT32 VLB7             : 6; //[13:8]      B'001010
    MUINT32 RESERVED41       : 2; //[15:14]     unused
    MUINT32 VLB6             : 6; //[21:16]     B'001000
    MUINT32 RESERVED42       : 2; //[23:22]     unused
    MUINT32 VLB5             : 6; //[29:24]     B'001000
    MUINT32 RESERVED43       : 2; //[31:30]     unused
} ISP_NR1_NR_VLB58_T;
//
//NR1_NR11 CAM+0590H
typedef struct {
    MUINT32 EDGE128          : 5; //[4:0]       B'01101
    MUINT32 RESERVED44       : 3; //[7:5]       unused
    MUINT32 EDGE96           : 5; //[12:8]      B'01001
    MUINT32 RESERVED45       : 3; //[15:13]     unused
    MUINT32 EDGE64           : 5; //[20:16]     B'00101
    MUINT32 RESERVED46       : 3; //[23:21]     unused
    MUINT32 EDGE32           : 5; //[28:24]     B'00001
    MUINT32 RESERVED47       : 3; //[31:29]     unused
} ISP_NR1_NR_EDGE0_T;
//
//NR1_NR12 CAM+0594H
typedef struct {
    MUINT32 EDGE256          : 5; //[4:0]       B'10000
    MUINT32 RESERVED48       : 3; //[7:5]       unused
    MUINT32 EDGE224          : 5; //[12:8]      B'10000
    MUINT32 RESERVED49       : 3; //[15:13]     unused
    MUINT32 EDGE192          : 5; //[20:16]     B'10000
    MUINT32 RESERVED50       : 3; //[23:21]     unused
    MUINT32 EDGE160          : 5; //[28:24]     B'10000
    MUINT32 RESERVED51       : 3; //[31:29]     unused
} ISP_NR1_NR_EDGE1_T;

/*******************************************************************************
* NR2
*******************************************************************************/
//NR2_CON CAM+0500H
typedef struct {
    MUINT32 ENY             : 1; //[0]         1
    MUINT32 ENC             : 1; //[1]         1
    MUINT32 RESERVED0       : 14; //[15:2]     unused
    MUINT32 MODE            : 1; //[16]        0
    MUINT32 RESERVED1       : 3; //[19:17]     unused
    MUINT32 IIR_MODE        : 2; //[21:20]     0
    MUINT32 RESERVED2       : 10; //[31:22]    unused
} ISP_NR2_CTRL_T;
//
//NR2_CFG_C1 CAM+0504H
typedef struct {
    MUINT32 Y_DP_MIN_TH     : 5; //[4:0] H'05
    MUINT32 RESERVED3       : 3; //[7:5]        unused
    MUINT32 Y_DP_MAX_TH     : 5; //[12:8] H'05
    MUINT32 RESERVED4       : 2; //[14:13]      unused
    MUINT32 Y_DP_EN         : 1; //[15] B'1
    MUINT32 YUV444_MODE     : 1; //[16] B'1
    MUINT32 RESERVED5       : 2; //[18:17] unused
    MUINT32 S_SM_EDGE       : 1; //[19] 0 (new added)
    MUINT32 QEC             : 1; //[20] B'1
    MUINT32 RESERVED6       : 11;//[31:21]      unused
} ISP_NR2_CFG1_T;
//
//NR2_CFG2 CAM+0508H
typedef struct {
    MUINT32 GNC              : 4; //[3:0]   B'0100
    MUINT32 GNY              : 4; //[7:4]   B'0100
    MUINT32 SC1              : 4; //[11:8]  B'0010
    MUINT32 SY1              : 4; //[15:12] B'0010
    MUINT32 S3               : 3; //[18:16] B'100
    MUINT32 S2               : 3; //[21:19] B'010
    MUINT32 UV_SMPL          : 2; //[23:22] B'11
    MUINT32 RESERVED6        : 8; //[31:24] unused
} ISP_NR2_CFG2_T;
//
//NR2_CFG3 CAM+050CH
typedef struct {
    MUINT32 PTY4             : 8; //[7:0]   H'02
    MUINT32 PTY3             : 8; //[15:8]  H'04
    MUINT32 PTY2             : 8; //[23:16] H'06
    MUINT32 PTY1             : 8; //[31:24] H'08
} ISP_NR2_PTY_T;
//
//NR2_CFG4 CAM+0510H
typedef struct {
    MUINT32 PTC4             : 8; //[7:0]   H'04
    MUINT32 PTC3             : 8; //[15:8]  H'06
    MUINT32 PTC2             : 8; //[23:16] H'08
    MUINT32 PTC1             : 8; //[31:24] H'0A
} ISP_NR2_PTC_T;
//
//NR2_CFG_C2 CAM+0514H
typedef struct {
    MUINT32 LUMA_AVG         : 1; //[0]     B'1
    MUINT32 LUMA_LUTY5       : 5; //[5:1]   H'01
    MUINT32 LUMA_LUTY4       : 5; //[10:6]  H'05
    MUINT32 LUMA_LUTY3       : 5; //[15:11] H'0A
    MUINT32 LUMA_LUTY2       : 5; //[20:16] H'10
    MUINT32 LUMA_LUTY1       : 5; //[25:21] H'10
    MUINT32 RESERVED7        : 6; //[31:26] unused
} ISP_NR2_LUMA_T;
//
//NR2_CFG_L1 CAM+0518H
typedef struct {
    MUINT32 LCE_GAIN3        : 6; //[5:0]   B'100000
    MUINT32 RESERVED8        : 2; //[7:6]   unused
    MUINT32 LCE_GAIN2        : 6; //[13:8]  B'011000
    MUINT32 RESERVED9        : 2; //[15:14] unused
    MUINT32 LCE_GAIN1        : 6; //[21:16] B'010000
    MUINT32 RESERVED10       : 2; //[23:22] unused
    MUINT32 LCE_GAIN0        : 6; //[29:24] B'001000
    MUINT32 RESERVED11       : 1; //[30]    unused
    MUINT32 LCE_LINK         : 1; //[31]    B'0
} ISP_NR2_LCE_GAIN_T;
//
//NR2_CFG_L2 CAM+051CH
typedef struct {
    MUINT32 LCE_GAIN_DIV3    : 5; //[4:0]   B'00100
    MUINT32 RESERVED12       : 3; //[7:5]   unused
    MUINT32 LCE_GAIN_DIV2    : 5; //[12:8]  B'00110
    MUINT32 RESERVED13       : 3; //[15:13] unused
    MUINT32 LCE_GAIN_DIV1    : 5; //[20:16] B'01000
    MUINT32 RESERVED14       : 3; //[23:21] unused
    MUINT32 LCE_GAIN_DIV0    : 5; //[28:24] B'10000
    MUINT32 RESERVED15       : 3; //[31:29] unused
} ISP_NR2_LCE_GAIN_DIV_T;
//
//NR2_CFG_N1 CAM+0520H
typedef struct {
    MUINT32 GNC_H            : 4; //[4:0]   B'0100
    MUINT32 GNY_H            : 4; //[7:4]   B'0100
    MUINT32 Y_V_FLT4         : 2; //[9:8]   B'11
    MUINT32 Y_V_FLT3         : 2; //[11:10] B'10
    MUINT32 Y_V_FLT2         : 2; //[13:12] B'01
    MUINT32 Y_V_FLT1         : 2; //[15:14] B'01
    MUINT32 C_V_FLT5         : 2; //[17:16] B'11
    MUINT32 C_V_FLT4         : 2; //[19:18] B'10
    MUINT32 C_V_FLT3         : 2; //[21:20] B'01
    MUINT32 C_V_FLT2         : 2; //[23:22] B'01
    MUINT32 C_V_FLT1         : 2; //[25:24] B'01
    MUINT32 RESERVED16       : 6; //[31:26] unused
} ISP_NR2_MODE1_CFG1_T;
//
//NR2_CFG_N2 CAM+0524H
typedef struct {
    MUINT32 Y_H_FLT6         : 2; //[1:0]       B'11
    MUINT32 Y_H_FLT5         : 2; //[3:2]       B'10
    MUINT32 Y_H_FLT4         : 2; //[5:4]       B'10
    MUINT32 Y_H_FLT3         : 2; //[7:6]       B'01
    MUINT32 Y_H_FLT2         : 2; //[9:8]       B'01
    MUINT32 Y_H_FLT1         : 2; //[11:10]     B'01
    MUINT32 RESERVED17       : 20; //[31:12]    unused
} ISP_NR2_MODE1_CFG2_T;
//
//NR2_CFG_N3 CAM+0528H
typedef struct {
    MUINT32 H_PTY4           : 8; //[7:0]   H'02
    MUINT32 H_PTY3           : 8; //[15:8]  H'04
    MUINT32 H_PTY2           : 8; //[23:16] H'06
    MUINT32 H_PTY1           : 8; //[31:24] H'08
} ISP_NR2_MODE1_CFG3_T;

/*******************************************************************************
* DEMOSAIC
*******************************************************************************/
//CAM_CPSCON1 CAM+0070H
typedef struct {
    MUINT32 DISLJ            : 1; //[0]     0
    MUINT32 VLEDGEN          : 1;  //[1] unused (special effect)
    MUINT32 RESERVED0        : 3; //[4:2]   unused
    MUINT32 HLEDGEN          : 1;  //[5] unused (special effect)
    MUINT32 RESERVED1        : 2; //[7:6]   unused
    MUINT32 DM_BPCMP_EN      : 1; //[8]     0
    MUINT32 RESERVED2        : 23; //[31:9] unused
} ISP_CPSCON1_T;
//
//CAM_INTER1 CAM+0074H
typedef struct {
    MUINT32 EDGEB_RT         : 5; //[4:0] unused (edgeb threshold)
    MUINT32 RESERVED3        : 3; //[7:5]   unused
    MUINT32 THRE_DHV         : 6; //[13:8]  B'01000
    MUINT32 RESERVED4        : 2; //[15:14] unused
    MUINT32 THRE_SM          : 5; //[20:16] B'00
    MUINT32 RESERVED5        : 2; //[22:21] unused
    MUINT32 RB_SMOOTH_EN     : 1; //[23]    0
    MUINT32 THRE_V           : 6; //[29:24] B'001000
    MUINT32 RESERVED6        : 2; //[31:30] unused
} ISP_INTER1_T;
//
//CAM_INTER2 CAM+0078H
typedef struct {
    MUINT32 THRE_LEDGE       : 7; //[6:0] unused (special effect)
    MUINT32 RESERVED7        : 1; //[7]     unused
    MUINT32 LINE_RBTHD       : 5; //[12:8]  B'00100
    MUINT32 RESERVED8        : 3; //[15:13] unused
    MUINT32 NYQ_TH           : 8; //[23:16] H'1F (Bit 23 must be 0)
    MUINT32 RESERVED9        : 8; //[31:24] unused
} ISP_INTER2_T;

/*******************************************************************************
* EDGE
*******************************************************************************/
//
//CAM_EDGCORE CAM+05B8H
typedef struct {
    MUINT32 CORE_CON         : 7; //[6:0]   H'14
    MUINT32 TOP_SLOPE        : 1; //[7]     0
    MUINT32 SUP_H            : 2; //[9:8]   0
    MUINT32 SDN_H            : 2; //[11:10] 2
    MUINT32 SPECIAL_EN       : 1; //[12]    0
    MUINT32 RESERVED0        : 3; //[15:13] unused
    MUINT32 COREH2           : 6; //[21:16] H'1F
    MUINT32 EMBOSS2_EN       : 1; //[22]    0
    MUINT32 EMBOSS1_EN       : 1; //[23]    0
    MUINT32 COREH            : 7; //[30:24] H'08
    MUINT32 RESERVED1        : 1; //[31]    unused
} ISP_EDGCORE_T;
//
//CAM_EDGGAIN1 CAM+05BCH
typedef struct {
    MUINT32 EGAINLINE        : 4; //[3:0]
    MUINT32 KNEESEL          : 2; //[5:4]
    MUINT32 RESERVED2        : 1; //[6]     unused
    MUINT32 OILEN            : 1; //[7]
    MUINT32 EGAIN_VB         : 5; //[12:8]
    MUINT32 RESERVED3        : 3; //[15:13] unused
    MUINT32 EGAIN_H2         : 5; //[20:16]
    MUINT32 RESERVED4        : 3; //[23:24] unused
    MUINT32 EGAIN_H          : 4; //[27:24]
    MUINT32 SPECIPONLY       : 2; //[29:28]
    MUINT32 SPECIGAIN        : 2; //[31:30]
} ISP_EDGGAIN1_T;
//
//CAM_EDGGAIN2 CAM+05C0H
typedef struct {
    MUINT32 EGAIN_HC         : 5; //[4:0]
    MUINT32 RESERVED5        : 1; //[5]     unused
    MUINT32 SPECIINV         : 1; //[6]
    MUINT32 SPECIABS         : 1; //[7]
    MUINT32 RESERVED6        : 8; //[15:8]  unused
    MUINT32 EGAIN_VC         : 5; //[20:16]
    MUINT32 RESERVED7        : 3; //[23:21] unused
    MUINT32 EGAIN_VA         : 4; //[27:24]
    MUINT32 RESERVED8        : 4; //[31:28] unused
} ISP_EDGGAIN2_T;
//
//CAM_EDGTHRE CAM+05C4H
typedef struct {
    MUINT32 THRL_EDGE_SUP    : 7; //[6:0]
    MUINT32 RESERVED9        : 1; //[7]     unused
    MUINT32 THRE_EDGE_SUP    : 7; //[14:8]
    MUINT32 ONLYC            : 1; //[15]
    MUINT32 ETH_CON          : 8; //[23:16]
    MUINT32 ETH3             : 8; //[31:24]
} ISP_EDGTHRE_T;
//
//CAM_EDGVCON CAM+05C8H
typedef struct {
    MUINT32 E_TH3_V          : 8; //[7:0]
    MUINT32 SDN_V            : 2; //[9:8]
    MUINT32 SUP_V            : 2; //[11:10]
    MUINT32 RESERVED10       : 3; //[14:12] unused
    MUINT32 VPEN             : 1; //[15]
    MUINT32 HALF_V           : 6; //[21:16]
    MUINT32 RESERVED11       : 2; //[23:22] unused
    MUINT32 E_TH1_V          : 7; //[30:24]
    MUINT32 HPEN             : 1; //[31]
} ISP_EDGVCON_T;
//
//CAM_CPSCON2 CAM+00ACH
typedef struct {
    MUINT32 Y_EGAIN          : 4; //[3:0]   H'8
    MUINT32 OPRGM_IVT        : 1; //[4]     unused (gamma)
    MUINT32 RESERVED12       : 1; //[5]   unused
    MUINT32 GMA_ECO_EN       : 1; //[6] 0
    MUINT32 BYPGM            : 1; //[7]     unused (gamma)
    MUINT32 RESERVED13       : 24; //[31:8] unused
} ISP_CPSCON2_T;
//
//EE_CTRL CAM+05A0H
typedef struct {
    MUINT32 YEDGE_EN         : 1; //[0]     0
    MUINT32 RGBEDGE_EN       : 1; //[1] unused (EE on G pixels)
    MUINT32 CLIP_OVER_EN     : 1; //[2]     0
    MUINT32 CLIP_UNDER_EN    : 1; //[3]     0
    MUINT32 FILTER_SEL       : 1; //[4]     0
    MUINT32 ED_GM_EN         : 1; //[5] 0
    MUINT32 ED_BOUND_EN      : 1; //[6] unused
    MUINT32 RESERVED14       : 1; //[7]     unused
    MUINT32 CLIP_OVER_TH     : 8; //[15:8]  H'24
    MUINT32 CLIP_UNDER_TH    : 8; //[23:16] H'64
    MUINT32 NONE1            : 8; //[31:24] unused (new removed)
} ISP_EE_CTRL_T;
//
//ISP_ED_CTRL1_T CAM+05A4H (new)
typedef struct {
    MUINT32 USM_ED_X1        : 8; //[7:0] H'23
    MUINT32 USM_ED_S1        : 8; //[15:8] H'01
    MUINT32 USM_ED_Y1        : 10;//[25:16] H'014
    MUINT32 NONE0            : 6; //[31:26] unused
} ISP_ED_CTRL1_T;
//
//ISP_ED_CTRL2_T CAM+05A8H (new)
typedef struct {
    MUINT32 USM_ED_X2        : 8; //[7:0] H'3C
    MUINT32 USM_ED_S2        : 8; //[15:8] H'18
    MUINT32 USM_ED_Y2        : 10;//[25:16] H'266
    MUINT32 NONE0            : 6; //[31:26] unused
} ISP_ED_CTRL2_T;
//
//ISP_ED_CTRL3_T CAM+05ACH (new)
typedef struct {
    MUINT32 USM_ED_X3        : 8; //[7:0] H'64
    MUINT32 USM_ED_S3        : 8; //[15:8] H'F8
    MUINT32 USM_ED_Y3        : 10;//[25:16] H'133
    MUINT32 NONE0            : 6; //[31:26] unused
} ISP_ED_CTRL3_T;
//
//ISP_ED_CTRL4_T CAM+05B0H (new)
typedef struct {
    MUINT32 USM_ED_X4        : 8; //[7:0] H'DC
    MUINT32 USM_ED_S4        : 8; //[15:8] H'FE
    MUINT32 USM_ED_Y4        : 10;//[25:16] H'029
    MUINT32 NONE0            : 6; //[31:26] unused
} ISP_ED_CTRL4_T;
//
//ISP_ED_CTRL5_T CAM+05B4H (new)
typedef struct {
    MUINT32 USM_ED_DIAG_AMP     : 3; //[2:0] B'011
    MUINT32 USM_ED_2DFILTER_EN  : 1; //[3] B'1
    MUINT32 NONE0               : 4; //[7:4] unused
    MUINT32 USM_ED_S5           : 8; //[15:8] H'FF
    MUINT32 USM_ED_TH_OVER      : 8; //[23:16] H'FF
    MUINT32 USM_ED_TH_UNDER     : 8; //[31:24] H'FF
} ISP_ED_CTRL5_T;
//
//EGAMMA1 CAM+05CCH
typedef struct {
    MUINT32 EGAMMA_B4        : 8; //[7:0]
    MUINT32 EGAMMA_B3        : 8; //[15:8]
    MUINT32 EGAMMA_B2        : 8; //[23:16]
    MUINT32 EGAMMA_B1        : 8; //[31:24]
} ISP_EDGE_GAMMA_CFG1_T;
//
//EGAMMA2 CAM+05D0H
typedef struct {
    MUINT32 EGAMMA_B8        : 8; //[7:0]
    MUINT32 EGAMMA_B7        : 8; //[15:8]
    MUINT32 EGAMMA_B6        : 8; //[23:16]
    MUINT32 EGAMMA_B5        : 8; //[31:24]
} ISP_EDGE_GAMMA_CFG2_T;
//
//EGAMMA3 CAM+05D4H
typedef struct {
    MUINT32 RESERVED         : 8; //[7:0]
    MUINT32 EGAMMA_B11       : 8; //[15:8]
    MUINT32 EGAMMA_B10       : 8; //[23:16]
    MUINT32 EGAMMA_B9        : 8; //[31:24]
} ISP_EDGE_GAMMA_CFG3_T;

/*******************************************************************************
* YCCGO
*******************************************************************************/
//YCCGO_CON CAM+0600H
typedef struct {
    MUINT32 ENC3             : 1; //[0]     0
    MUINT32 ENC2             : 1; //[1]     0
    MUINT32 ENC1             : 1; //[2]     0
    MUINT32 ENY3             : 1; //[3]     0
    MUINT32 ENY2             : 1; //[4]     0
    MUINT32 ENY1             : 1; //[5]     0
    MUINT32 RESERVED0        : 2; //[7:6]   unused
    MUINT32 ENBDV            : 1; //[8]     0
    MUINT32 ENBDU            : 1; //[9]     0
    MUINT32 ENBDY            : 1; //[10]    0
    MUINT32 RESERVED1        : 5; //[15:11] unused
    MUINT32 UV_GAIN_ON       : 1; //[16]    0
    MUINT32 RESERVED2        : 15;//[31:17] unused
} ISP_YCCGO_CTRL_T;
//
//YCCGO_CFG1 CAM+0604H
typedef struct {
    MUINT32 H12              : 8; //[7:0]   H'01
    MUINT32 H11              : 8; //[15:8]  H'7F
    MUINT32 MV               : 8; //[23:16] unused
    MUINT32 MU               : 8; //[31:24] unused
} ISP_YCCGO_CFG1_T;
//
//YCCGO_CFG2 CAM+0608H
typedef struct {
    MUINT32 Y4               : 8; //[7:0]   H'F0
    MUINT32 Y3               : 8; //[15:8]  H'E0
    MUINT32 Y2               : 8; //[23:16] H'20
    MUINT32 Y1               : 8; //[31:24] H'10
} ISP_YCCGO_CFG2_T;
//
//YCCGO_CFG3 CAM+060CH
typedef struct {
    MUINT32 G4               : 8; //[7:0]   H'40
    MUINT32 G3               : 8; //[15:8]  H'40
    MUINT32 G2               : 8; //[23:16] H'40
    MUINT32 G1               : 8; //[31:24] H'40
} ISP_YCCGO_CFG3_T;
//
//YCCGO_CFG4 CAM+0610H
typedef struct {
    MUINT32 OFSTV            : 8; //[7:0]   0
    MUINT32 OFSTU            : 8; //[15:8]  0
    MUINT32 OFSTY            : 8; //[23:16] unused
    MUINT32 G5               : 8; //[31:24] H'40
} ISP_YCCGO_CFG4_T;
//
//YCCGO_CFG5 CAM+0614H
typedef struct {
    MUINT32 GAINY            : 8; //[7:0]   H'40
    MUINT32 RESERVED3        : 8; //[15:8]  unused
    MUINT32 YBNDL            : 8; //[23:16] H'00
    MUINT32 YBNDH            : 8; //[31:24] H'FF
} ISP_YCCGO_CFG5_T;
//
//YCCGO_CFG6 CAM+0618H
typedef struct {
    MUINT32 VBNDL            : 8; //[7:0]   H'00
    MUINT32 VBNDH            : 8; //[15:8]  H'FF
    MUINT32 UBNDL            : 8; //[23:16] H'00
    MUINT32 UBNDH            : 8; //[31:24] H'FF
} ISP_YCCGO_CFG6_T;
//
//YCCGO_CFG7 CAM+061CH
typedef struct {
    MUINT32 UV_GAIN1         : 7; //[6:0]   H'40
    MUINT32 RESERVED4        : 1; //[7:]    unused
    MUINT32 UV_GAIN2         : 7; //[14:8]  H'40
    MUINT32 RESERVED5        : 1; //[15]    unused
    MUINT32 UV_GAIN3         : 7; //[22:16] H'40
    MUINT32 RESERVED6        : 1; //[23]    unused
    MUINT32 UV_GAIN4         : 7; //[30:24] H'40
    MUINT32 RESERVED7        : 1; //[31]    unused
} ISP_YCCGO_CFG7_T;
//
//YCCGO_CFG8 CAM+0620H
typedef struct {
    MUINT32 UV_GAIN_SLOPE1   : 7; //[6:0]   0
    MUINT32 RESERVED8        : 1; //[7]     unused
    MUINT32 UV_GAIN_SLOPE2   : 7; //[14:8]  0
    MUINT32 RESERVED9        : 1; //[15]    unused
    MUINT32 UV_GAIN_SLOPE3   : 7; //[22:16] 0
    MUINT32 RESERVED10       : 9; //[31:23] unused
} ISP_YCCGO_CFG8_T;
//
//YCCGO_CFG9 CAM+0624H
typedef struct {
    MUINT32 UV_X1            : 7; //[6:0]   H'10
    MUINT32 RESERVED11       : 1; //[7]     unused
    MUINT32 UV_X2            : 7; //[14:8]  H'20
    MUINT32 RESERVED12       : 1; //[15]    unused
    MUINT32 UV_X3            : 7; //[22:16] H'40
    MUINT32 RESERVED13       : 9; //[31:23] unused
} ISP_YCCGO_CFG9_T;

/*******************************************************************************
* CCM
*******************************************************************************/
//
//CAM_MATRIX1 CAM+0094H (new)
typedef struct {
    MUINT32 M12              : 11; //[10:0] M12+M12_EXT: H'400
    MUINT32 NONE0            : 5;  //[15:11] unused
    MUINT32 M11              : 11; //[26:16] M11+M11_EXT: H'100
    MUINT32 NONE1            : 5;  //[31:27] unused
} ISP_CCM1_T;
//
//CAM_MATRIX2 CAM+0098H (new)
typedef struct {
    MUINT32 M21              : 11; //[10:0] M21+M21_EXT: H'400
    MUINT32 NONE0            : 5;  //[15:11] unused
    MUINT32 M13              : 11; //[26:16] M13+M13_EXT: H'400
    MUINT32 NONE1            : 5;  //[31:27] unused
} ISP_CCM2_T;
//
//CAM_MATRIX3 CAM+009CH (new)
typedef struct {
    MUINT32 M23              : 11; //[10:0] M23+M23_EXT: H'400
    MUINT32 NONE0            : 5;  //[15:11] unused
    MUINT32 M22              : 11; //[26:16] M22+M22_EXT: H'100
    MUINT32 NONE1            : 5;  //[31:27] unused
} ISP_CCM3_T;
//
//CAM_MATRIX4 CAM+00A0H (new)
typedef struct {
    MUINT32 M32              : 11; //[10:0] M32+M32_EXT: H'400
    MUINT32 NONE0            : 5;  //[15:11] unused
    MUINT32 M31              : 11; //[26:16] M31+M31_EXT: H'400
    MUINT32 NONE1            : 5;  //[31:27] unused
} ISP_CCM4_T;
//
//CAM_MATRIX5 CAM+00A4H (new)
typedef struct {
    MUINT32 NONE0            : 16; //[15:0] unused
    MUINT32 M33              : 11; //[26:16] M33+M33_EXT: H'100
    MUINT32 NONE1            : 5;  //[31:27] unused
} ISP_CCM5_T;
/*******************************************************************************
* GAMMA
*******************************************************************************/
//
//GMA_REG1 CAM+01A8H
typedef struct {
    MUINT32 Y01              : 8; //[7:0]   H'09
    MUINT32 Y02              : 8; //[15:8]  H'1E
    MUINT32 Y03              : 8; //[23:16] H'32
    MUINT32 Y04              : 8; //[31:24] H'46
} ISP_GAMMA1_T;
//
//GMA_REG2 CAM+01ACH
typedef struct {
    MUINT32 Y05              : 8; //[7:0]   H'59
    MUINT32 Y06              : 8; //[15:8]  H'6B
    MUINT32 Y07              : 8; //[23:16] H'7C
    MUINT32 Y08              : 8; //[31:24] H'8C
} ISP_GAMMA2_T;
//
//GMA_REG3 CAM+01B0H
typedef struct {
    MUINT32 Y09              : 8; //[7:0]   H'9A
    MUINT32 Y10              : 8; //[15:8]  H'A7
    MUINT32 Y11              : 8; //[23:16] H'B4
    MUINT32 Y12              : 8; //[31:24] H'BF
} ISP_GAMMA3_T;
//
//GMA_REG4 CAM+01B4H
typedef struct {
    MUINT32 Y13              : 8; //[7:0]   H'CA
    MUINT32 Y14              : 8; //[15:8]  H'D3
    MUINT32 Y15              : 8; //[23:16] H'DD
    MUINT32 Y16              : 8; //[31:24] H'E6
} ISP_GAMMA4_T;
//
//GMA_REG5 CAM+01B8H
typedef struct {
    MUINT32 Y17              : 8; //[7:0]   H'ED
    MUINT32 Y18              : 8; //[15:8]  H'F2
    MUINT32 Y19              : 8; //[23:16] H'F7
    MUINT32 Y20              : 8; //[31:24] H'F8
} ISP_GAMMA5_T;

/*******************************************************************************
* PCA
*******************************************************************************/
//PCA_CON CAM+0630H
typedef struct {
    MUINT32 EN               : 1; //[0]     0
    MUINT32 RESERVED0        : 3; //[3:1]   unused
    MUINT32 BUSY_SKIP        : 1; //[4]     0
    MUINT32 LUT_360          : 1; //[5]     unused (fixed to 0 now)(180bin)
    MUINT32 RESERVED1        : 10; //[15:6] unused
    MUINT32 CHROMA_TH        : 5; //[20:16] H'00
    MUINT32 RESERVED2        : 11;//[31:21] unused
} ISP_PCA_CTRL_T;
//
//PCA_TBA CAM+0634H
//
//PCA_GMC CAM+0638H
typedef struct {
    MUINT32 THROTTLE_PERIOD  : 16; //[15:0]
    MUINT32 RESERVED4        : 15; //[30:16]    unused
    MUINT32 THEN             : 1; //[31:]
} ISP_PCA_GMC_T;

/*******************************************************************************
* RGB2YCC
*******************************************************************************/
// RGB2YCC_CON CAM+00BCH
typedef struct {
    MUINT32 VGAIN  : 8; //[7:0]
    MUINT32 UGAIN  : 8; //[15:8]
    MUINT32 YOFST  : 8; //[23:16]
    MUINT32 YGAIN  : 8; //[31:24]
} ISP_RGB2YCC_CON_T;

/*******************************************************************************
* Color Clip
*******************************************************************************/
//
// CCLIP_CON CAM+00CCH
typedef struct {
    MUINT32 EN         :1;
    MUINT32 RESERVED0  :31;
} ISP_CCLIP_CON_T;
//
// CCLIP_GTC CAM+00D0H
typedef struct {
    MUINT32 CCLIP_TC   :10;
    MUINT32 RESERVED0  :22;
} ISP_CCLIP_GTC_T;
//
// CCLIP_ADC CAM+00D4H
typedef struct {
    MUINT32 CCLIP_D_TH1   :10;
    MUINT32 CCLIP_D_SLOPE :6;
    MUINT32 RESERVED0     :16;
} ISP_CCLIP_ADC_T;
//
// CCLIP_BAC CAM+00D4H
typedef struct {
    MUINT32 CCLIP_B_OFFSET :10;
    MUINT32 CCLIP_B_SLOPE  :4;
    MUINT32 RESERVED0      :2;
    MUINT32 CCLIP_B_DIFF   :10;
    MUINT32 RESERVED1      :6;
} ISP_CCLIP_BAC_T;

namespace NS_MT6575ISP_NVRAM
{
/*******************************************************************************
* Color Clip
*******************************************************************************/
typedef union {
    //CCLIP_CON CAM+00CCH
    enum { MASK     = 0x00000001 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_CCLIP_CON_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CCLIP_CTRL_T;
//
typedef union {
    //CCLIP_GTC CAM+00D0H
    enum { MASK     = 0x000003FF };
    enum { DEFAULT  = 0x00000332 };
    typedef ISP_CCLIP_GTC_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CCLIP_GTC_T;
//
typedef union {
    //CCLIP_ADC CAM+00D4H
    enum { MASK     = 0x0000FFFF };
    enum { DEFAULT  = 0x00004E00 };
    typedef ISP_CCLIP_ADC_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CCLIP_ADC_T;
//
typedef union {
    //CCLIP_BAC CAM+00D8H
    enum { MASK     = 0x03FF3FFF };
    enum { DEFAULT  = 0x01000814 };
    typedef ISP_CCLIP_BAC_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CCLIP_BAC_T;
//
//
typedef union {
    enum { COUNT = 4 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_CCLIP_CTRL_T cclip_ctrl;
        ISP_NVRAM_CCLIP_GTC_T  cclip_gtc;
        ISP_NVRAM_CCLIP_ADC_T  cclip_adc;
        ISP_NVRAM_CCLIP_BAC_T  cclip_bac;
    };
} ISP_NVRAM_CCLIP_T;

/*******************************************************************************
* Gain Control
*******************************************************************************/
typedef union {
    //CAM_CTRL1 CAM+0040H: for GAIN_COMP and PGAIN control
    enum { MASK     = 0xFF0003FE };
    enum { DEFAULT  = 0x80000100 };
    typedef ISP_CAM_CTRL1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CAM_CTRL1_T;
//
//
typedef union {
    enum { COUNT = 1 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_CAM_CTRL1_T cam_ctrl1;
    };
} ISP_NVRAM_GAIN_CTRL_T;

/*******************************************************************************
* Gamma ECO
*******************************************************************************/
typedef union {
    //CAM_CPSCON2 CAM+00ACH: for GMA_ECO_EN
    enum { MASK     = 0x00000040 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_CPSCON2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CPSCON2_T;
//
//
typedef union {
    enum { COUNT = 1 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_CPSCON2_T gamma_eco_en;
    };
} ISP_NVRAM_GAMMA_ECO_T;


/*******************************************************************************
* RGB2YCC Y Offset
*******************************************************************************/
//
typedef union {
    //RGB2YCC_CON CAM+00BCH: for YOFST adjustment
    enum { MASK     = 0x00FF0000 };
    enum { DEFAULT  = 0x00010000 };
    typedef ISP_RGB2YCC_CON_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_RGB2YCC_CON_T;
//
//
typedef union {
    enum { COUNT = 1 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_RGB2YCC_CON_T rgb2ycc_yofst;
    };
} ISP_NVRAM_RGB2YCC_YOFST_T;


/*******************************************************************************
* Shading
*******************************************************************************/
typedef union {
    //RAWACC_REG CAM+01BCH
    enum { MASK     = 0x00000001 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_RAW_ACC_CFG_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_RAW_ACC_CFG_T;
//
typedef union {
    //RAWWIN_REG CAM+01C0H
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_RAW_ACC_WIN_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_RAW_ACC_WIN_T;
//
typedef union {
    //RAWSUM CAM+01C4H/01C8H/01CCH/01D0H
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_RAW_ACC_RESULT_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_RAW_ACC_RESULT_T;
//
//
typedef union {
    //CAM_SHADING1 CAM+0214H
    enum { MASK     = 0x30000000 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_SHADING_CTRL1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_SHADING_CTRL1_T;
//
typedef union {
    //CAM_SHADING2 CAM+0218H
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x60004000 };
    typedef ISP_SHADING_CTRL2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_SHADING_CTRL2_T;
//
typedef union {
    //SD_RADDR CAM+021CH
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x00200060 };
    typedef ISP_SHADING_READ_ADDR_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_SHADING_READ_ADDR_T;
//
typedef union {
    //SD_LBLOCK CAM+0220H
    enum { MASK     = 0x0FFF0FFF };
    enum { DEFAULT  = 0x00FF00FF };
    typedef ISP_SHADING_LAST_BLK_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_SHADING_LAST_BLK_T;
//
typedef union {
    //SD_RATIO CAM+0224H
    enum { MASK     = 0x3F3F3F3F };
    enum { DEFAULT  = 0x20202020 };
    typedef ISP_SHADING_RATIO_CFG_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_SHADING_RATIO_CFG_T;
//
//
typedef union {
    enum { COUNT = 11 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_RAW_ACC_CFG_T         raw_acc_cfg;
        ISP_NVRAM_RAW_ACC_WIN_T         raw_acc_win;
        ISP_NVRAM_RAW_ACC_RESULT_T      raw_acc_result_b;
        ISP_NVRAM_RAW_ACC_RESULT_T      raw_acc_result_gb;
        ISP_NVRAM_RAW_ACC_RESULT_T      raw_acc_result_gr;
        ISP_NVRAM_RAW_ACC_RESULT_T      raw_acc_result_r;
        ISP_NVRAM_SHADING_CTRL1_T       shading_ctrl1;
        ISP_NVRAM_SHADING_CTRL2_T       shading_ctrl2;
        ISP_NVRAM_SHADING_READ_ADDR_T   shading_read_addr;
        ISP_NVRAM_SHADING_LAST_BLK_T    shading_last_blk;
        ISP_NVRAM_SHADING_RATIO_CFG_T   shading_ratio_cfg;
    };
} ISP_NVRAM_SHADING_T;

/*******************************************************************************
* OB
*******************************************************************************/
typedef union {
    //CAM_RGBOFF CAM+0090H
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_RGBOFF_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_RGBOFF_T;
//
//
typedef union {
    enum { COUNT = 1 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_RGBOFF_T rgboff;
    };
} ISP_NVRAM_OB_T;

/*******************************************************************************
* DEMOSAIC
*******************************************************************************/
typedef union {
    //CAM_CPSCON1 CAM+0070H
    enum { MASK     = 0x00000101 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_CPSCON1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_DM_CTRL_T;
//
typedef union {
    //CAM_INTER1 CAM+0074H
    enum { MASK     = 0x3F9F3F00 };
    enum { DEFAULT  = 0x08000800 };
    typedef ISP_INTER1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_DM_INTER1_T;
//
typedef union {
    //CAM_INTER2 CAM+0078H
    enum { MASK     = 0x007F1F00 }; // Bit 23 must be 0 (NYQ_TH)
    enum { DEFAULT  = 0x001F0400 };
    typedef ISP_INTER2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_DM_INTER2_T;
//
//
typedef union {
    enum { COUNT = 3 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_DM_CTRL_T   ctrl;
        ISP_NVRAM_DM_INTER1_T inter1;
        ISP_NVRAM_DM_INTER2_T inter2;
    };
} ISP_NVRAM_DEMOSAIC_T;

/*******************************************************************************
* NR1 - Defect Pixel
*******************************************************************************/
typedef union {
    //NR1_CON CAM+0550H
    enum { MASK     = 0x00000001 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_NR1_CTRL_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_DP_CTRL_T;
//
typedef union {
    //NR1_DP1 CAM+0554H
    enum { MASK     = 0xF3FFF3FF };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_NR1_DP1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_DP1_T;
//
typedef union {
    //NR1_DP2 CAM+0558H
    enum { MASK     = 0x03FF03FF };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_NR1_DP2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_DP2_T;
//
typedef union {
    //NR1_DP3 CAM+055CH
    enum { MASK     = 0x03FF03FF };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_NR1_DP3_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_DP3_T;
//
typedef union {
    //NR1_DP4 CAM+0560H
    enum { MASK     = 0x00001FFF };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_NR1_DP4_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_DP4_T;
//
//
typedef union{
    enum { COUNT = 5 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_NR1_DP_CTRL_T ctrl;
        ISP_NVRAM_NR1_DP1_T     dp1;
        ISP_NVRAM_NR1_DP2_T     dp2;
        ISP_NVRAM_NR1_DP3_T     dp3;
        ISP_NVRAM_NR1_DP4_T     dp4;
    };
} ISP_NVRAM_DP_T;

/*******************************************************************************
* NR1 - Crosstalk / NR
*******************************************************************************/
typedef union {
    //NR1_CON CAM+0550H
    enum { MASK     = 0x000000C6 };
    enum { DEFAULT  = 0x000000C6 };
    typedef ISP_NR1_CTRL_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_CTRL_T;
//
typedef union {
    //NR1_CT CAM+0564H
    enum { MASK     = 0x000011FF };
    enum { DEFAULT  = 0x000000A0 };
    typedef ISP_NR1_CT_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_CT_T;
//
typedef union {
    //NR1_NR1 CAM+0568H
    enum { MASK     = 0x1FFFFFFF };
    enum { DEFAULT  = 0x06107080 };
    typedef ISP_NR1_NR_CFG1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_CFG1_T;
//
typedef union {
    //NR1_NR2 CAM+056CH
    enum { MASK     = 0x00000FFF };
    enum { DEFAULT  = 0x00000568 };
    typedef ISP_NR1_NR_CFG2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_CFG2_T;
//
typedef union {
    //NR1_NR3 CAM+0570H
    enum { MASK     = 0x3F3F3F3F };
    enum { DEFAULT  = 0x04060808 };
    typedef ISP_NR1_NR_VLR14_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_VLR14_T;
//
typedef union {
    //NR1_NR4 CAM+0574H
    enum { MASK     = 0x3F3F3F3F };
    enum { DEFAULT  = 0x08080A0C };
    typedef ISP_NR1_NR_VLR58_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_VLR58_T;
//
typedef union {
    //NR1_NR5 CAM+0578H
    enum { MASK     = 0x3F3F3F3F };
    enum { DEFAULT  = 0x04060808 };
    typedef ISP_NR1_NR_VLGR14_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_VLGR14_T;
//
typedef union {
    //NR1_NR6 CAM+057CH
    enum { MASK     = 0x3F3F3F3F };
    enum { DEFAULT  = 0x08080A0C };
    typedef ISP_NR1_NR_VLGR58_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_VLGR58_T;
//
typedef union {
    //NR1_NR7 CAM+0580H
    enum { MASK     = 0x3F3F3F3F };
    enum { DEFAULT  = 0x04060808 };
    typedef ISP_NR1_NR_VLGB14_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_VLGB14_T;
//
typedef union {
    //NR1_NR8 CAM+0584H
    enum { MASK     = 0x3F3F3F3F };
    enum { DEFAULT  = 0x08080A0C };
    typedef ISP_NR1_NR_VLGB58_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_VLGB58_T;
//
typedef union {
    //NR1_NR9 CAM+0588H
    enum { MASK     = 0x3F3F3F3F };
    enum { DEFAULT  = 0x04060808 };
    typedef ISP_NR1_NR_VLB14_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_VLB14_T;
//
typedef union {
    //NR1_NR10 CAM+058CH
    enum { MASK     = 0x3F3F3F3F };
    enum { DEFAULT  = 0x08080A0C };
    typedef ISP_NR1_NR_VLB58_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_VLB58_T;
//
typedef union {
    //NR1_NR11 CAM+0590H (new)
    enum { MASK     = 0x1F1F1F1F };
    enum { DEFAULT  = 0x0105090D };
    typedef ISP_NR1_NR_EDGE0_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_EDGE0_T;
//
typedef union {
    //NR1_NR12 CAM+0594H
    enum { MASK     = 0x1F1F1F1F };
    enum { DEFAULT  = 0x10101010 };
    typedef ISP_NR1_NR_EDGE1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR1_NR_EDGE1_T;
//
//
typedef union{
    enum { COUNT = 14 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_NR1_CTRL_T        ctrl;
        ISP_NVRAM_NR1_CT_T          ct;
        ISP_NVRAM_NR1_NR_CFG1_T     nr_cfg1;
        ISP_NVRAM_NR1_NR_CFG2_T     nr_cfg2;
        ISP_NVRAM_NR1_NR_VLR14_T    nr_vlr14;
        ISP_NVRAM_NR1_NR_VLR58_T    nr_vlr58;
        ISP_NVRAM_NR1_NR_VLGR14_T   nr_vlgr14;
        ISP_NVRAM_NR1_NR_VLGR58_T   nr_vlgr58;
        ISP_NVRAM_NR1_NR_VLGB14_T   nr_vlgb14;
        ISP_NVRAM_NR1_NR_VLGB58_T   nr_vlgb58;
        ISP_NVRAM_NR1_NR_VLB14_T    nr_vlb14;
        ISP_NVRAM_NR1_NR_VLB58_T    nr_vlb58;
        ISP_NVRAM_NR1_NR_EDGE0_T    nr_edge0;
        ISP_NVRAM_NR1_NR_EDGE1_T    nr_edge1;
    };
} ISP_NVRAM_NR1_T;

/*******************************************************************************
* NR2
*******************************************************************************/
typedef union {
    //NR2_CON CAM+0500H
    enum { MASK     = 0x00310003 };
    enum { DEFAULT  = 0x00000003 };
    typedef ISP_NR2_CTRL_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_CTRL_T;
//
typedef union {
    //NR2_CFG_C1 CAM+0504H
    enum { MASK     = 0x00199F1F };
    enum { DEFAULT  = 0x00118505 };
    typedef ISP_NR2_CFG1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_CFG1_T;
//
typedef union {
    //NR2_CFG2 CAM+0508H
    enum { MASK     = 0x00FFFFFF };
    enum { DEFAULT  = 0x00D42244 };
    typedef ISP_NR2_CFG2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_CFG2_T;
//
typedef union {
    //NR2_CFG3 CAM+050CH
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x02040608 };
    typedef ISP_NR2_PTY_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_PTY_T;
//
typedef union {
    //NR2_CFG4 CAM+0510H
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x0406080A };
    typedef ISP_NR2_PTC_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_PTC_T;
//
typedef union {
    //NR2_CFG_C2 CAM+0514H (new)
    enum { MASK     = 0x03FFFFFF };
    enum { DEFAULT  = 0x02105183 };
    typedef ISP_NR2_LUMA_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_LUMA_T;
//
typedef union {
    //NR2_CFG_L1 CAM+0518H (new)
    enum { MASK     = 0xBF3F3F3F };
    enum { DEFAULT  = 0x08101820 };
    typedef ISP_NR2_LCE_GAIN_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_LCE_GAIN_T;
//
typedef union {
    //NR2_CFG_L2 CAM+051CH (new)
    enum { MASK     = 0x1F1F1F1F };
    enum { DEFAULT  = 0x10080604 };
    typedef ISP_NR2_LCE_GAIN_DIV_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_LCE_GAIN_DIV_T;
//
typedef union {
    //NR2_CFG_N1 CAM+0520H (new)
    enum { MASK     = 0x03FFFFFF };
    enum { DEFAULT  = 0x015B5B44 };
    typedef ISP_NR2_MODE1_CFG1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_MODE1_CFG1_T;
//
typedef union {
    //NR2_CFG_N2 CAM+0524H (new)
    enum { MASK     = 0x00000FFF };
    enum { DEFAULT  = 0x0000056B };
    typedef ISP_NR2_MODE1_CFG2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_MODE1_CFG2_T;
//
typedef union {
    //NR2_CFG_N3 CAM+0528H (new)
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x02040608 };
    typedef ISP_NR2_MODE1_CFG3_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_NR2_MODE1_CFG3_T;
//
//
typedef union{
    enum { COUNT = 11 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_NR2_CTRL_T            ctrl;
        ISP_NVRAM_NR2_CFG1_T            cfg1;           // (new)
        ISP_NVRAM_NR2_CFG2_T            cfg2;
        ISP_NVRAM_NR2_PTY_T             pty;
        ISP_NVRAM_NR2_PTC_T             ptc;
        ISP_NVRAM_NR2_LUMA_T            luma;           // (new)
        ISP_NVRAM_NR2_LCE_GAIN_T        lce_gain;       // (new)
        ISP_NVRAM_NR2_LCE_GAIN_DIV_T    lce_gain_div;   // (new)
        ISP_NVRAM_NR2_MODE1_CFG1_T      mode1_cfg1;     // (new)
        ISP_NVRAM_NR2_MODE1_CFG2_T      mode1_cfg2;     // (new)
        ISP_NVRAM_NR2_MODE1_CFG3_T      mode1_cfg3;     // (new)
    };
} ISP_NVRAM_NR2_T;

/*******************************************************************************
* EDGE - EE
*******************************************************************************/
typedef union {
    //CAM_CPSCON2 CAM+00ACH
    enum { MASK     = 0x0000000F };
    enum { DEFAULT  = 0x00000008 };
    typedef ISP_CPSCON2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_Y_EGAIN_T;
//
typedef union {
    //EE_CTRL CAM+05A0H
    enum { MASK     = 0x00FFFF1D };
    enum { DEFAULT  = 0x00642400 };
    typedef ISP_EE_CTRL_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_EE_CTRL_T;
//
typedef union {
    //ED_CTRL1 CAM+05A4H
    enum { MASK     = 0x03FFFFFF };
    enum { DEFAULT  = 0x00140123 };
    typedef ISP_ED_CTRL1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_ED_CTRL1_T;
//
typedef union {
    //ED_CTRL2 CAM+05A8H
    enum { MASK     = 0x03FFFFFF };
    enum { DEFAULT  = 0x0266183C };
    typedef ISP_ED_CTRL2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_ED_CTRL2_T;
//
typedef union {
    //ED_CTRL3 CAM+05ACH
    enum { MASK     = 0x03FFFFFF };
    enum { DEFAULT  = 0x0133F864 };
    typedef ISP_ED_CTRL3_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_ED_CTRL3_T;
//
typedef union {
    //ED_CTRL4 CAM+05B0H
    enum { MASK     = 0x03FFFFFF };
    enum { DEFAULT  = 0x0029FEDC };
    typedef ISP_ED_CTRL4_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_ED_CTRL4_T;
//
typedef union {
    //ED_CTRL5 CAM+05B4H
    enum { MASK     = 0xFFFFFF0F };
    enum { DEFAULT  = 0xFFFFFF0B };
    typedef ISP_ED_CTRL5_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_ED_CTRL5_T;
//
//
typedef union{
    enum { COUNT = 7 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_Y_EGAIN_T       y_egain;
        ISP_NVRAM_EE_CTRL_T       ee_ctrl;
        ISP_NVRAM_ED_CTRL1_T      ed_ctrl1;
        ISP_NVRAM_ED_CTRL2_T      ed_ctrl2;
        ISP_NVRAM_ED_CTRL3_T      ed_ctrl3;
        ISP_NVRAM_ED_CTRL4_T      ed_ctrl4;
        ISP_NVRAM_ED_CTRL5_T      ed_ctrl5;
    };
} ISP_NVRAM_EE_T;

/*******************************************************************************
* EDGE - Edge Gamma
*******************************************************************************/
typedef union {
    //EE_CTRL CAM+05A0H
    enum { MASK     = 0x00000020 };
    enum { DEFAULT  = 0x00000020 };
    typedef ISP_EE_CTRL_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_EGAMMA_CTRL_T;
//
typedef union {
    //EGAMMA1 CAM+05CCH
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x32506576 };
    typedef ISP_EDGE_GAMMA_CFG1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_EGAMMA_CFG1_T;
//
typedef union {
    //EGAMMA1 CAM+05D0H
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x94AEC5DA };
    typedef ISP_EDGE_GAMMA_CFG2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_EGAMMA_CFG2_T;
//
typedef union {
    //EGAMMA1 CAM+05D4H
    enum { MASK     = 0xFFFFFF00 };
    enum { DEFAULT  = 0xE4EDF700 };
    typedef ISP_EDGE_GAMMA_CFG3_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_EGAMMA_CFG3_T;
//
//
typedef union{
    enum { COUNT = 4 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_EGAMMA_CTRL_T ctrl;
        ISP_NVRAM_EGAMMA_CFG1_T cfg1;
        ISP_NVRAM_EGAMMA_CFG2_T cfg2;
        ISP_NVRAM_EGAMMA_CFG3_T cfg3;
    };
} ISP_NVRAM_EDGE_GAMMA_T;

/*******************************************************************************
* YCCGO - Saturation
*******************************************************************************/
typedef union {
    //YCCGO_CON CAM+0600H
    enum { MASK     = 0x00010301 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_YCCGO_CTRL_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_SATURATION_CTRL_T;
//
//YCCGO_CON CAM+0604H
//
typedef union {
    //YCCGO_CFG2 CAM+0608H
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x1020E0F0 };
    typedef ISP_YCCGO_CFG2_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_SATURATION_Y_T;
//
typedef union {
    //YCCGO_CFG3 CAM+060CH
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x40404040 };
    typedef ISP_YCCGO_CFG3_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_SATURATION_GAIN_T;
//
typedef union {
    //YCCGO_CFG4 CAM+0610H
    enum { MASK     = 0xFF00FFFF };
    enum { DEFAULT  = 0x40000000 };
    typedef ISP_YCCGO_CFG4_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_SATURATION_GAIN_OFS_T;
//
//YCCGO_CFG5 CAM+0614H
//
typedef union {
    //YCCGO_CFG6 CAM+0618H
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0xFF00FF00 };
    typedef ISP_YCCGO_CFG6_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_SATURATION_BND_T;
//
typedef union {
    //YCCGO_CFG7 CAM+061CH
    enum { MASK     = 0x7F7F7F7F };
    enum { DEFAULT  = 0x40404040 };
    typedef ISP_YCCGO_CFG7_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_SATURATION_UV_GAIN_T;
//
typedef union {
    //YCCGO_CFG8 CAM+0620H
    enum { MASK     = 0x007F7F7F };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_YCCGO_CFG8_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_SATURATION_UV_GAIN_SLOPE_T;
//
typedef union {
    //YCCGO_CFG9 CAM+0624H
    enum { MASK     = 0x007F7F7F };
    enum { DEFAULT  = 0x00402010 };
    typedef ISP_YCCGO_CFG9_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_SATURATION_UV_X_T;
//
//
typedef union{
    enum { COUNT = 8 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_SATURATION_CTRL_T             ctrl;
        ISP_NVRAM_SATURATION_Y_T                y;
        ISP_NVRAM_SATURATION_GAIN_T             gain;
        ISP_NVRAM_SATURATION_GAIN_OFS_T         gain_ofs;
        ISP_NVRAM_SATURATION_BND_T              bnd;
        ISP_NVRAM_SATURATION_UV_GAIN_T          uv_gain;
        ISP_NVRAM_SATURATION_UV_GAIN_SLOPE_T    uv_gain_slope;
        ISP_NVRAM_SATURATION_UV_X_T             uv_x;
    };
} ISP_NVRAM_SATURATION_T;

/*******************************************************************************
* YCCGO - Contrast
*******************************************************************************/
typedef union {
    //YCCGO_CON CAM+0600H
    enum { MASK     = 0x00000408 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_YCCGO_CTRL_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_CONTRAST_CTRL_T;
//
//YCCGO_CON CAM+0604H
//
//YCCGO_CFG2 CAM+0608H
//
//YCCGO_CFG3 CAM+060CH
//
typedef union {
    //YCCGO_CFG4 CAM+0610H
    enum { MASK     = 0x00FF0000 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_YCCGO_CFG4_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_CONTRAST_OFS_T;
//
typedef union {
    //YCCGO_CFG5 CAM+0614H
    enum { MASK     = 0xFFFF00FF };
    enum { DEFAULT  = 0xFF000040 };
    typedef ISP_YCCGO_CFG5_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_CONTRAST_GAIN_BND_T;
//
//YCCGO_CFG6 CAM+0618H
//
//YCCGO_CFG7 CAM+061CH
//
//YCCGO_CFG8 CAM+0620H
//
//YCCGO_CFG9 CAM+0624H
//
typedef union{
    enum { COUNT = 3 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_CONTRAST_CTRL_T       ctrl;
        ISP_NVRAM_CONTRAST_OFS_T        ofs;
        ISP_NVRAM_CONTRAST_GAIN_BND_T   gain_bnd;
    };
} ISP_NVRAM_CONTRAST_T;

/*******************************************************************************
* YCCGO - Hue
*******************************************************************************/
typedef union {
    //YCCGO_CON CAM+0600H
    enum { MASK     = 0x00000002 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_YCCGO_CTRL_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_HUE_CTRL_T;
//
typedef union {
    //YCCGO_CFG1 CAM+0604H
    enum { MASK     = 0x0000FFFF };
    enum { DEFAULT  = 0x00007F01 };
    typedef ISP_YCCGO_CFG1_T ret_t;
    ret_t   bits;
    MUINT32 val;
} ISP_NVRAM_HUE_CFG_T;
//
//YCCGO_CFG2 CAM+0608H
//
//YCCGO_CFG3 CAM+060CH
//
//YCCGO_CFG4 CAM+0610H
//
//YCCGO_CFG5 CAM+0614H
//
//YCCGO_CFG6 CAM+0618H
//
//YCCGO_CFG7 CAM+061CH
//
//YCCGO_CFG8 CAM+0620H
//
//YCCGO_CFG9 CAM+0624H
//
typedef union{
    enum { COUNT = 2 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_HUE_CTRL_T    ctrl;
        ISP_NVRAM_HUE_CFG_T     cfg;
    };
} ISP_NVRAM_HUE_T;

/*******************************************************************************
* CCM
*******************************************************************************/
typedef union {
    //CAM_MATRIX1 CAM+0094H
    enum { MASK     = 0x07FF07FF };
    enum { DEFAULT  = 0x01000400 };
    typedef ISP_CCM1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CCM1_T;
//
typedef union {
    //CAM_MATRIX2 CAM+0098H
    enum { MASK     = 0x07FF07FF };
    enum { DEFAULT  = 0x04000400 };
    typedef ISP_CCM2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CCM2_T;
//
typedef union {
    //CAM_MATRIX3 CAM+009CH
    enum { MASK     = 0x07FF07FF };
    enum { DEFAULT  = 0x01000400 };
    typedef ISP_CCM3_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CCM3_T;
//
typedef union {
    //CAM_MATRIX4 CAM+00A0H
    enum { MASK     = 0x07FF07FF };
    enum { DEFAULT  = 0x04000400 };
    typedef ISP_CCM4_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CCM4_T;
//
typedef union {
    //CAM_MATRIX5 CAM+00A4H
    enum { MASK     = 0x07FF0000 };
    enum { DEFAULT  = 0x01000000 };
    typedef ISP_CCM5_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_CCM5_T;
//
//
typedef union{
    enum { COUNT = 5 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_CCM1_T    ccm1;
        ISP_NVRAM_CCM2_T    ccm2;
        ISP_NVRAM_CCM3_T    ccm3;
        ISP_NVRAM_CCM4_T    ccm4;
        ISP_NVRAM_CCM5_T    ccm5;
    };
} ISP_NVRAM_CCM_T;

/*******************************************************************************
* GAMMA
*******************************************************************************/
typedef union {
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x46321E09 };
    typedef ISP_GAMMA1_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_GAMMA1_T;
//
typedef union {
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0x8C7C6B59 };
    typedef ISP_GAMMA2_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_GAMMA2_T;
//
typedef union {
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0xBFB4A79A };
    typedef ISP_GAMMA3_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_GAMMA3_T;
//
typedef union {
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0xE6DDD3CA };
    typedef ISP_GAMMA4_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_GAMMA4_T;
//
typedef union {
    enum { MASK     = 0xFFFFFFFF };
    enum { DEFAULT  = 0xF8F7F2ED };
    typedef ISP_GAMMA5_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_GAMMA5_T;
//
//
typedef union{
    enum { COUNT = 5 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_GAMMA1_T  gamma1;
        ISP_NVRAM_GAMMA2_T  gamma2;
        ISP_NVRAM_GAMMA3_T  gamma3;
        ISP_NVRAM_GAMMA4_T  gamma4;
        ISP_NVRAM_GAMMA5_T  gamma5;
    };
} ISP_NVRAM_GAMMA_T;

/*******************************************************************************
* PCA
*******************************************************************************/
typedef union {
    //PCA_CON CAM+0630H
    enum { MASK     = 0x001F0031 };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_PCA_CTRL_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_PCA_CTRL_T;
//
typedef union {
    enum { MASK     = 0x8000FFFF };
    enum { DEFAULT  = 0x00000000 };
    typedef ISP_PCA_GMC_T reg_t;
    reg_t   bits;
    MUINT32 val;
} ISP_NVRAM_PCA_GMC_T;
//
//
typedef union{
    enum { COUNT = 2 };
    MUINT32 set[COUNT];
    struct {
        ISP_NVRAM_PCA_CTRL_T  ctrl;
        ISP_NVRAM_PCA_GMC_T   gmc;
    };
} ISP_NVRAM_PCA_T;

/*******************************************************************************
* PCA LUTS
*******************************************************************************/
enum { PCA_BIN_NUM = 180 };
//#define PCA_BIN_FMT(hue, sat, y)    (0x00FFFFFF&((((MUINT32)(hue))<<16)|(((MUINT32)(sat))<<8)|((MUINT32)(y))))
typedef struct {
    MUINT8  y_gain;
    MUINT8  sat_gain;
    MUINT8  hue_shift;
    MUINT8  reserved;
} ISP_NVRAM_PCA_BIN_T;
//
//
typedef struct { //180*4*3=2160Bytes
    ISP_NVRAM_PCA_BIN_T     lut_lo[PCA_BIN_NUM]; //color temperature: low
    ISP_NVRAM_PCA_BIN_T     lut_md[PCA_BIN_NUM]; //color temperature: middle
    ISP_NVRAM_PCA_BIN_T     lut_hi[PCA_BIN_NUM]; //color temperature: high
} ISP_NVRAM_PCA_LUTS_T;


/*******************************************************************************
* 
*******************************************************************************/
};  //NS_MT6575ISP_NVRAM


/*******************************************************************************
*
*******************************************************************************/


namespace NS_MT6575ISP_EFFECT
{
template <MUINT32 _mask, MUINT32 _default, class _reg_t>
struct ISP_XXX_T
{
    typedef union
    {
        enum { MASK     = _mask     };
        enum { DEFAULT  = _default  };
        typedef _reg_t  reg_t;
        reg_t   bits;
        MUINT32 val;
    } type;
};

template <class ISP_xxx_T>
inline
MVOID
setDefault_ISP_XXX(ISP_xxx_T& r)
{
    r.val = ISP_xxx_T::DEFAULT;
}

/*******************************************************************************
* YCCGO
*******************************************************************************/
//YCCGO_CON CAM+0600H
typedef ISP_XXX_T<
    0x0001073F, //  MASK
    0x00000000, //  DEFAULT
    ISP_YCCGO_CTRL_T
>::type ISP_EFFECT_YCCGO_CTRL_T;
//
//YCCGO_CFG1 CAM+0604H
typedef ISP_XXX_T<
    0xFFFFFFFF, //  MASK
    0x00007F01, //  DEFAULT
    ISP_YCCGO_CFG1_T
>::type ISP_EFFECT_YCCGO_CFG1_T;
//
//YCCGO_CFG2 CAM+0608H
typedef ISP_XXX_T<
    0xFFFFFFFF, //  MASK
    0x1020E0F0, //  DEFAULT
    ISP_YCCGO_CFG2_T
>::type ISP_EFFECT_YCCGO_CFG2_T;
//
//YCCGO_CFG3 CAM+060CH
typedef ISP_XXX_T<
    0xFFFFFFFF, //  MASK
    0x40404040, //  DEFAULT
    ISP_YCCGO_CFG3_T
>::type ISP_EFFECT_YCCGO_CFG3_T;
//
//YCCGO_CFG4 CAM+0610H
typedef ISP_XXX_T<
    0xFFFFFFFF, //  MASK
    0x40000000, //  DEFAULT
    ISP_YCCGO_CFG4_T
>::type ISP_EFFECT_YCCGO_CFG4_T;
//
//YCCGO_CFG5 CAM+0614H
typedef ISP_XXX_T<
    0xFFFF00FF, //  MASK
    0xFF000040, //  DEFAULT
    ISP_YCCGO_CFG5_T
>::type ISP_EFFECT_YCCGO_CFG5_T;
//
//YCCGO_CFG6 CAM+0618H
typedef ISP_XXX_T<
    0xFFFFFFFF, //  MASK
    0xFF00FF00, //  DEFAULT
    ISP_YCCGO_CFG6_T
>::type ISP_EFFECT_YCCGO_CFG6_T;
//
//YCCGO_CFG7 CAM+061CH
typedef ISP_XXX_T<
    0x7F7F7F7F, //  MASK
    0x40404040, //  DEFAULT
    ISP_YCCGO_CFG7_T
>::type ISP_EFFECT_YCCGO_CFG7_T;
//
//YCCGO_CFG8 CAM+0620H
typedef ISP_XXX_T<
    0x007F7F7F, //  MASK
    0x00000000, //  DEFAULT
    ISP_YCCGO_CFG8_T
>::type ISP_EFFECT_YCCGO_CFG8_T;
//
//YCCGO_CFG9 CAM+0624H
typedef ISP_XXX_T<
    0x007F7F7F, //  MASK
    0x00402010, //  DEFAULT
    ISP_YCCGO_CFG9_T
>::type ISP_EFFECT_YCCGO_CFG9_T;
//
//
typedef union{
    enum { COUNT = 10 };
    MUINT32 set[COUNT];
    struct {
        ISP_EFFECT_YCCGO_CTRL_T                 ctrl;
        ISP_EFFECT_YCCGO_CFG1_T                 cfg1;
        ISP_EFFECT_YCCGO_CFG2_T                 cfg2;
        ISP_EFFECT_YCCGO_CFG3_T                 cfg3;
        ISP_EFFECT_YCCGO_CFG4_T                 cfg4;
        ISP_EFFECT_YCCGO_CFG5_T                 cfg5;
        ISP_EFFECT_YCCGO_CFG6_T                 cfg6;
        ISP_EFFECT_YCCGO_CFG7_T                 cfg7;
        ISP_EFFECT_YCCGO_CFG8_T                 cfg8;
        ISP_EFFECT_YCCGO_CFG9_T                 cfg9;
    };
} ISP_EFFECT_YCCGO_T;


/*******************************************************************************
* EDGE
*******************************************************************************/
//CAM_CPSCON1 CAM+0070H
typedef ISP_XXX_T<
    0x00000022, //  MASK
    0x00000022, //  DEFAULT
    ISP_CPSCON1_T
>::type ISP_EFFECT_ED_CTRL_T;
//
//CAM_INTER1 CAM+0074H
typedef ISP_XXX_T<
    0x0000001F, //  MASK
    0x00000010, //  DEFAULT
    ISP_INTER1_T
>::type ISP_EFFECT_ED_INTER1_T;
//
//CAM_INTER2 CAM+0078H
typedef ISP_XXX_T<
    0x0000007F, //  MASK
    0x00000014, //  DEFAULT
    ISP_INTER2_T
>::type ISP_EFFECT_ED_INTER2_T;
//
//CAM_EDGCORE CAM+05B8H
typedef ISP_XXX_T<
    0x7FFF1FFF, //  MASK
    0x081F0814, //  DEFAULT
    ISP_EDGCORE_T
>::type ISP_EFFECT_EDGCORE_T;
//
//CAM_EDGGAIN1 CAM+05BCH
typedef ISP_XXX_T<
    0xFF5F1FBF, //  MASK
    0x01030332, //  DEFAULT
    ISP_EDGGAIN1_T
>::type ISP_EFFECT_EDGGAIN1_T;
//
//CAM_EDGGAIN2 CAM+05C0H
typedef ISP_XXX_T<
    0xFF5F1FBF, //  MASK
    0x010F000F, //  DEFAULT
    ISP_EDGGAIN2_T
>::type ISP_EFFECT_EDGGAIN2_T;
//
//CAM_EDGTHRE CAM+05C4H
typedef ISP_XXX_T<
    0xFFFFFF7F, //  MASK
    0x32800707, //  DEFAULT
    ISP_EDGTHRE_T
>::type ISP_EFFECT_EDGTHRE_T;
//
//CAM_EDGVCON CAM+05C8H
typedef ISP_XXX_T<
    0xFF3F8FFF, //  MASK
    0x181F0232, //  DEFAULT
    ISP_EDGVCON_T
>::type ISP_EFFECT_EDGVCON_T;
//
//CAM_CPSCON2 CAM+00ACH
typedef ISP_XXX_T<
    0x0000009F, //  MASK
    0x00000008, //  DEFAULT
    ISP_CPSCON2_T
>::type ISP_EFFECT_CPSCON2_T;
//
//EE_CTRL CAM+05A0H
typedef ISP_XXX_T<
    0x00FFFF1F, //  MASK
    0x00642400, //  DEFAULT
    ISP_EE_CTRL_T
>::type ISP_EFFECT_EE_CTRL_T;
//
//ED_CTRL1 CAM+05A4H
typedef ISP_XXX_T<
    0x03FFFFFF, //  MASK
    0x00140123, //  DEFAULT
    ISP_ED_CTRL1_T
>::type ISP_EFFECT_ED_CTRL1_T;
//
//ED_CTRL2 CAM+05A8H
typedef ISP_XXX_T<
    0x03FFFFFF, //  MASK
    0x0266183C, //  DEFAULT
    ISP_ED_CTRL2_T
>::type ISP_EFFECT_ED_CTRL2_T;
//
//ED_CTRL3 CAM+05ACH
typedef ISP_XXX_T<
    0x03FFFFFF, //  MASK
    0x0133F864, //  DEFAULT
    ISP_ED_CTRL3_T
>::type ISP_EFFECT_ED_CTRL3_T;
//
//ED_CTRL4 CAM+05B0H
typedef ISP_XXX_T<
    0x03FFFFFF, //  MASK
    0x0029FEDC, //  DEFAULT
    ISP_ED_CTRL4_T
>::type ISP_EFFECT_ED_CTRL4_T;
//
//ED_CTRL5 CAM+05B4H
typedef ISP_XXX_T<
    0xFFFFFF0F, //  MASK
    0xFFFFFF0B, //  DEFAULT
    ISP_ED_CTRL5_T
>::type ISP_EFFECT_ED_CTRL5_T;
//
//
typedef union{
    enum { COUNT = 15 };
    MUINT32 set[COUNT];
    struct {
        ISP_EFFECT_ED_CTRL_T    ed_ctrl;
        ISP_EFFECT_ED_INTER1_T  ed_inter1;
        ISP_EFFECT_ED_INTER2_T  ed_inter2;
        ISP_EFFECT_EDGCORE_T    edgcore;
        ISP_EFFECT_EDGGAIN1_T   edggain1;
        ISP_EFFECT_EDGGAIN2_T   edggain2;
        ISP_EFFECT_EDGTHRE_T    edgthre;
        ISP_EFFECT_EDGVCON_T    edgvcon;
        ISP_EFFECT_CPSCON2_T    cpscon2;
        ISP_EFFECT_EE_CTRL_T    ee_ctrl;
        ISP_EFFECT_ED_CTRL1_T   ed_ctrl1;
        ISP_EFFECT_ED_CTRL2_T   ed_ctrl2;
        ISP_EFFECT_ED_CTRL3_T   ed_ctrl3;
        ISP_EFFECT_ED_CTRL4_T   ed_ctrl4;
        ISP_EFFECT_ED_CTRL5_T   ed_ctrl5;
    };
} ISP_EFFECT_EDGE_T;

/*******************************************************************************
* CCM
*******************************************************************************/
//CAM_MATRIX1 CAM+0094H
typedef ISP_XXX_T<
    0x07FF07FF, //  MASK
    0x01000400, //  DEFAULT
    ISP_CCM1_T
>::type ISP_EFFECT_CCM1_T;
//
//CAM_MATRIX2 CAM+0098H
typedef ISP_XXX_T<
    0x07FF07FF, //  MASK
    0x04000400, //  DEFAULT
    ISP_CCM2_T
>::type ISP_EFFECT_CCM2_T;
//
//CAM_MATRIX3 CAM+009CH
typedef ISP_XXX_T<
    0x07FF07FF, //  MASK
    0x01000400, //  DEFAULT
    ISP_CCM3_T
>::type ISP_EFFECT_CCM3_T;
//
//CAM_MATRIX4 CAM+00A0H
typedef ISP_XXX_T<
    0x07FF07FF, //  MASK
    0x04000400, //  DEFAULT
    ISP_CCM4_T
>::type ISP_EFFECT_CCM4_T;
//
//CAM_MATRIX5 CAM+00A4H
typedef ISP_XXX_T<
    0x07FF0000, //  MASK
    0x01000000, //  DEFAULT
    ISP_CCM5_T
>::type ISP_EFFECT_CCM5_T;
//
//
typedef union{
    enum { COUNT = 5 };
    MUINT32 set[COUNT];
    struct {
        ISP_EFFECT_CCM1_T   ccm1;
        ISP_EFFECT_CCM2_T   ccm2;
        ISP_EFFECT_CCM3_T   ccm3;
        ISP_EFFECT_CCM4_T   ccm4;
        ISP_EFFECT_CCM5_T   ccm5;
    };
} ISP_EFFECT_CCM_T;


/*******************************************************************************
* Effect
*******************************************************************************/
typedef struct
{
    ISP_EFFECT_YCCGO_T  yccgo;
    ISP_EFFECT_EDGE_T   edge;
    ISP_EFFECT_CCM_T    ccm;
} ISP_EFFECT_T;


/*******************************************************************************
* 
*******************************************************************************/
};  //NS_MT6575ISP_EFFECT
#endif  //  _ISPIF_MT6575_

