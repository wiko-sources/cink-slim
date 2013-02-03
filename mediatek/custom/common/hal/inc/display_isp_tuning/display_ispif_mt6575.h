/*
**
** Copyright 2008, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
#ifndef _DISPLAY_ISPIF_MT6575_H_
#define _DISPLAY_ISPIF_MT6575_H_

#include "camera_custom_types.h"

namespace NSDisplayIspTuning
{
/*******************************************************************************
* EE
*******************************************************************************/
// CAM+05A0H: Edge Enhancement Control Register
typedef struct {
    MUINT32 YEDGE_EN         : 1; //[0]
    MUINT32 RGBEDGE_EN       : 1; //[1]
    MUINT32 CLIP_OVER_EN     : 1; //[2]
    MUINT32 CLIP_UNDER_EN    : 1; //[3]
    MUINT32 FILTER_SEL       : 1; //[4]
    MUINT32 ED_GM_EN         : 1; //[5]
    MUINT32 ED_BAND_EN       : 1; //[6]
    MUINT32 RESERVED0        : 1; //[7]
    MUINT32 CLIP_OVER_TH     : 8; //[15:8]
    MUINT32 CLIP_UNDER_TH    : 8; //[23:16]
    MUINT32 RESERVED1        : 8; //[31:24]
} ISP_EE_CTRL_T;

//
typedef union {
    ISP_EE_CTRL_T bits;
    MUINT32 val;
} DISPLAY_ISP_EE_CTRL_T;
//
//
typedef struct {
    DISPLAY_ISP_EE_CTRL_T ee_ctrl;
} DISPLAY_ISP_EE_T;

/*******************************************************************************
* NR2
*******************************************************************************/
// CAM+0500H: NR2 Control Register
typedef struct {
    MUINT32 ENY              : 1; //[0]
    MUINT32 ENC              : 1; //[1]
    MUINT32 RESERVED0        : 14;//[15:2]
    MUINT32 MODE             : 1; //[16]
    MUINT32 RESERVED1        : 3; //[19:17]
    MUINT32 IIR_MODE         : 2; //[21:20]
    MUINT32 RESERVED2        : 10;//[31:22]
} ISP_NR2_CTRL_T;
//
// CAM+0504H: NR2 Common Configuration Register 1
typedef struct {
    MUINT32 Y_DP_MIN_TH      : 5; //[4:0]
    MUINT32 RESERVED0        : 3; //[7:5]
    MUINT32 Y_DP_MAX_TH      : 5; //[12:8]
    MUINT32 RESERVED1        : 2; //[14:13]
    MUINT32 Y_DP_EN          : 1; //[15:]
    MUINT32 YUV444_MODE      : 1; //[16]
    MUINT32 RESERVED2        : 2; //[18:17]
    MUINT32 C_SM             : 1; //[19]
    MUINT32 QEC              : 1; //[20]
    MUINT32 RESERVED3        : 11;//[31:21]
} ISP_NR2_CFG1_T;
//
//CAM+0508H: NR2 Configuration Register 2
typedef struct {
    MUINT32 GNC              : 4; //[3:0]
    MUINT32 GNY              : 4; //[7:4]
    MUINT32 SC1              : 4; //[11:8]
    MUINT32 SY1              : 4; //[15:12]
    MUINT32 S3               : 3; //[18:16]
    MUINT32 S2               : 3; //[21:19]
    MUINT32 UV_SMPL          : 2; //[23:22]
    MUINT32 RESERVED0        : 8; //[31:24]
} ISP_NR2_CFG2_T;
//
// CAM+050CH: NR2 Configuration Register 3
typedef struct {
    MUINT32 PTY4             : 8; //[7:0]
    MUINT32 PTY3             : 8; //[15:8]
    MUINT32 PTY2             : 8; //[23:16]
    MUINT32 PTY1             : 8; //[31:24]
} ISP_NR2_CFG3_T;
//
// CAM+0510H: NR2 Configuration Register 4
typedef struct {
    MUINT32 PTC4             : 8; //[7:0]
    MUINT32 PTC3             : 8; //[15:8]
    MUINT32 PTC2             : 8; //[23:16]
    MUINT32 PTC1             : 8; //[31:24]
} ISP_NR2_CFG4_T;
//
// CAM+0514H: NR2 Common Configuration Register 2
typedef struct {
    MUINT32 LUMA_AVG         : 1; //[0]
    MUINT32 LUMA_LUTY5       : 5; //[5:1]
    MUINT32 LUMA_LUTY4       : 5; //[10:6]
    MUINT32 LUMA_LUTY3       : 5; //[15:11]
    MUINT32 LUMA_LUTY2       : 5; //[20:16]
    MUINT32 LUMA_LUTY1       : 5; //[25:21]
    MUINT32 RESERVED0        : 6; //[31:26]
} ISP_NR2_LUMA_T;
//
// CAM+0518H: NR2 LCE Configuration Register 1
typedef struct {
    MUINT32 LCE_GAIN3        : 6; //[5:0]
    MUINT32 RESERVED0        : 2; //[7:6]
    MUINT32 LCE_GAIN2        : 6; //[13:8]
    MUINT32 RESERVED1        : 2; //[15:14]
    MUINT32 LCE_GAIN1        : 6; //[21:16]
    MUINT32 RESERVED2        : 2; //[23:22]
    MUINT32 LCE_GAIN0        : 6; //[29:24]
    MUINT32 RESERVED3        : 1; //[30]
    MUINT32 LCE_LINK         : 1; //[31]
} ISP_NR2_LCE_GAIN_T;
//
// CAM+051CH: NR2 LCE Configuration Register 2
typedef struct {
    MUINT32 LCE_GAIN_DIV3    : 5; //[4:0]
    MUINT32 RESERVED0        : 3; //[7:5]
    MUINT32 LCE_GAIN_DIV2    : 5; //[12:8]
    MUINT32 RESERVED1        : 3; //[15:13]
    MUINT32 LCE_GAIN_DIV1    : 5; //[20:16]
    MUINT32 RESERVED2        : 3; //[23:21]
    MUINT32 LCE_GAIN_DIV0    : 5; //[28:24]
    MUINT32 RESERVED3        : 3; //[31:29]
} ISP_NR2_LCE_GAIN_DIV_T;
//
// CAM+0520H: NR2 Mode1 Configuration Register 1
typedef struct {
    MUINT32 GNC_H            : 4; //[4:0]
    MUINT32 GNY_H            : 4; //[7:4]
    MUINT32 Y_V_FLT4         : 2; //[9:8]
    MUINT32 Y_V_FLT3         : 2; //[11:10]
    MUINT32 Y_V_FLT2         : 2; //[13:12]
    MUINT32 Y_V_FLT1         : 2; //[15:14]
    MUINT32 C_V_FLT5         : 2; //[17:16]
    MUINT32 C_V_FLT4         : 2; //[19:18]
    MUINT32 C_V_FLT3         : 2; //[21:20]
    MUINT32 C_V_FLT2         : 2; //[23:22]
    MUINT32 C_V_FLT1         : 2; //[25:24]
    MUINT32 RESERVED0        : 6; //[31:26]
} ISP_NR2_MODE1_CFG1_T;
//
// CAM+0524H: NR2 Mode1 Configuration Register 2
typedef struct {
    MUINT32 Y_H_FLT6         : 2; //[1:0]
    MUINT32 Y_H_FLT5         : 2; //[3:2]
    MUINT32 Y_H_FLT4         : 2; //[5:4]
    MUINT32 Y_H_FLT3         : 2; //[7:6]
    MUINT32 Y_H_FLT2         : 2; //[9:8]
    MUINT32 Y_H_FLT1         : 2; //[11:10]
    MUINT32 RESERVED0        : 20; //[31:12]
} ISP_NR2_MODE1_CFG2_T;
//
// CAM+0528H: NR2 Mode1 Configuration Register 3
typedef struct {
    MUINT32 H_PTY4           : 8; //[7:0]
    MUINT32 H_PTY3           : 8; //[15:8]
    MUINT32 H_PTY2           : 8; //[23:16]
    MUINT32 H_PTY1           : 8; //[31:24]
} ISP_NR2_MODE1_CFG3_T;

typedef union {
    ISP_NR2_CTRL_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_CTRL_T;
//
typedef union {
    ISP_NR2_CFG1_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_CFG1_T;
//
typedef union {
    ISP_NR2_CFG2_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_CFG2_T;
//
typedef union {
    ISP_NR2_CFG3_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_CFG3_T;
//
typedef union {
    ISP_NR2_CFG4_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_CFG4_T;
//
typedef union {
    ISP_NR2_LUMA_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_LUMA_T;
//
typedef union {
    ISP_NR2_LCE_GAIN_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_LCE_GAIN_T;
//
typedef union {
    ISP_NR2_LCE_GAIN_DIV_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_LCE_GAIN_DIV_T;
//
typedef union {
    ISP_NR2_MODE1_CFG1_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_MODE1_CFG1_T;
//
typedef union {
    ISP_NR2_MODE1_CFG2_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_MODE1_CFG2_T;
//
typedef union {
    ISP_NR2_MODE1_CFG3_T bits;
    MUINT32 val;
} DISPLAY_ISP_NR2_MODE1_CFG3_T;
//
//
typedef union {
    enum { COUNT = 11 };
    MUINT32 set[COUNT];
    struct {
        DISPLAY_ISP_NR2_CTRL_T            ctrl;
        DISPLAY_ISP_NR2_CFG1_T            cfg1;
        DISPLAY_ISP_NR2_CFG2_T            cfg2;
        DISPLAY_ISP_NR2_CFG3_T            cfg3;
        DISPLAY_ISP_NR2_CFG4_T            cfg4;
        DISPLAY_ISP_NR2_LUMA_T            luma;
        DISPLAY_ISP_NR2_LCE_GAIN_T        lce_gain;
        DISPLAY_ISP_NR2_LCE_GAIN_DIV_T    lce_gain_div;
        DISPLAY_ISP_NR2_MODE1_CFG1_T      mode1_cfg1;
        DISPLAY_ISP_NR2_MODE1_CFG2_T      mode1_cfg2;
        DISPLAY_ISP_NR2_MODE1_CFG3_T      mode1_cfg3;
    };
} DISPLAY_ISP_NR2_T;

/*******************************************************************************
* PCA
*******************************************************************************/
// CAM+0630H: PCA Control Register
typedef struct {
    MUINT32 EN               : 1; //[0]     0
    MUINT32 RESERVED0        : 3; //[3:1]   unused
    MUINT32 BUSY_SKIP        : 1; //[4]     0
    MUINT32 LUT_360          : 1; //[5]     unused (fixed to 0 now)(180bin)
    MUINT32 RESERVED1        : 2; //[7:6] unused
    MUINT32 LOAD_LUT_BUSY    : 1; //[8] 1 means busy
    MUINT32 RESERVED2        : 7; //[15:9] unused
    MUINT32 CHROMA_TH        : 5; //[20:16] H'00
    MUINT32 RESERVED3        : 11;//[31:21] unused
} ISP_PCA_CTRL_T;

// CAM+0638H: PCA GMC Throttle Control
typedef struct {
    MUINT32 THROTTLE_PERIOD  : 16; //[15:0]
    MUINT32 RESERVED0        : 15; //[30:16]    unused
    MUINT32 THEN             : 1; //[31:]
} ISP_PCA_GMC_T;


typedef union {
    ISP_PCA_CTRL_T bits;
    MUINT32 val;
} DISPLAY_ISP_PCA_CTRL_T;
//
typedef union {
    ISP_PCA_GMC_T bits;
    MUINT32 val;
} DISPLAY_ISP_PCA_GMC_T;
//
//
typedef union {
    enum { COUNT = 2 };
    MUINT32 set[COUNT];
    struct {
        DISPLAY_ISP_PCA_CTRL_T  ctrl;
        DISPLAY_ISP_PCA_GMC_T   gmc;
    };
} DISPLAY_ISP_PCA_CFG_T;

/*******************************************************************************
* PCA LUTS
*******************************************************************************/
typedef struct {
    MUINT8  y_gain;
    MUINT8  sat_gain;
    MUINT8  hue_shift;
    MUINT8  reserved;
} DISPLAY_ISP_PCA_BIN_T;

//
enum { PCA_BIN_NUM = 180 };
typedef struct { //180*4=720Bytes
    DISPLAY_ISP_PCA_BIN_T     lut[PCA_BIN_NUM];
} DISPLAY_ISP_PCA_LUT_T;

enum { PCA_SKIN_BIN_NUM = 54 };
enum { PCA_SKIN_BIN_START = 26 };
enum { PCA_GRASS_BIN_NUM = 66 };
enum { PCA_GRASS_BIN_START = 80 };
enum { PCA_SKY_BIN_NUM = 60 };
enum { PCA_SKY_BIN_START = 146 };

typedef struct {
    DISPLAY_ISP_PCA_BIN_T lut[PCA_SKIN_BIN_NUM];
} DISPLAY_ISP_PCA_SKIN_LUT_T;

typedef struct {
    DISPLAY_ISP_PCA_BIN_T lut[PCA_GRASS_BIN_NUM];
} DISPLAY_ISP_PCA_GRASS_LUT_T;

typedef struct {
    DISPLAY_ISP_PCA_BIN_T lut[PCA_SKY_BIN_NUM];
} DISPLAY_ISP_PCA_SKY_LUT_T;

/*******************************************************************************
* YCCGO
*******************************************************************************/
// CAM+0600H: YCCGO Control Register
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
// CAM+0604H: YCCGO Configuration Register 1
typedef struct {
    MUINT32 H12              : 8; //[7:0]   H'01
    MUINT32 H11              : 8; //[15:8]  H'7F
    MUINT32 MV               : 8; //[23:16] unused
    MUINT32 MU               : 8; //[31:24] unused
} ISP_YCCGO_CFG1_T;
//
// CAM+0608H: YCCGO Configuration Register 2
typedef struct {
    MUINT32 Y4               : 8; //[7:0]   H'F0
    MUINT32 Y3               : 8; //[15:8]  H'E0
    MUINT32 Y2               : 8; //[23:16] H'20
    MUINT32 Y1               : 8; //[31:24] H'10
} ISP_YCCGO_CFG2_T;
//
// CAM+060CH: YCCGO Configuration Register 3
typedef struct {
    MUINT32 G4               : 8; //[7:0]   H'40
    MUINT32 G3               : 8; //[15:8]  H'40
    MUINT32 G2               : 8; //[23:16] H'40
    MUINT32 G1               : 8; //[31:24] H'40
} ISP_YCCGO_CFG3_T;
//
// CAM+0610H: YCCGO Configuration Register 4
typedef struct {
    MUINT32 OFSTV            : 8; //[7:0]   0
    MUINT32 OFSTU            : 8; //[15:8]  0
    MUINT32 OFSTY            : 8; //[23:16] unused
    MUINT32 G5               : 8; //[31:24] H'40
} ISP_YCCGO_CFG4_T;
//
// CAM+0614H: YCCGO Configuration Register 5
typedef struct {
    MUINT32 GAINY            : 8; //[7:0]   H'40
    MUINT32 RESERVED0        : 8; //[15:8]  unused
    MUINT32 YBNDL            : 8; //[23:16] H'00
    MUINT32 YBNDH            : 8; //[31:24] H'FF
} ISP_YCCGO_CFG5_T;
//
// CAM+0618H: YCCGO Configuration Register 6
typedef struct {
    MUINT32 VBNDL            : 8; //[7:0]   H'00
    MUINT32 VBNDH            : 8; //[15:8]  H'FF
    MUINT32 UBNDL            : 8; //[23:16] H'00
    MUINT32 UBNDH            : 8; //[31:24] H'FF
} ISP_YCCGO_CFG6_T;
//
// CAM+061CH: YCCGO Configuration Register 7
typedef struct {
    MUINT32 UV_GAIN1         : 7; //[6:0]   H'40
    MUINT32 RESERVED0        : 1; //[7:]    unused
    MUINT32 UV_GAIN2         : 7; //[14:8]  H'40
    MUINT32 RESERVED1        : 1; //[15]    unused
    MUINT32 UV_GAIN3         : 7; //[22:16] H'40
    MUINT32 RESERVED2        : 1; //[23]    unused
    MUINT32 UV_GAIN4         : 7; //[30:24] H'40
    MUINT32 RESERVED3        : 1; //[31]    unused
} ISP_YCCGO_CFG7_T;
//
// CAM+0620H: YCCGO Configuration Register 8
typedef struct {
    MUINT32 UV_GAIN_SLOPE1   : 7; //[6:0]   0
    MUINT32 RESERVED0        : 1; //[7]     unused
    MUINT32 UV_GAIN_SLOPE2   : 7; //[14:8]  0
    MUINT32 RESERVED1        : 1; //[15]    unused
    MUINT32 UV_GAIN_SLOPE3   : 7; //[22:16] 0
    MUINT32 RESERVED2        : 9; //[31:23] unused
} ISP_YCCGO_CFG8_T;
//
// CAM+0624H: YCCGO Configuration Register 9
typedef struct {
    MUINT32 UV_X1            : 7; //[6:0]   H'10
    MUINT32 RESERVED0        : 1; //[7]     unused
    MUINT32 UV_X2            : 7; //[14:8]  H'20
    MUINT32 RESERVED1        : 1; //[15]    unused
    MUINT32 UV_X3            : 7; //[22:16] H'40
    MUINT32 RESERVED2        : 9; //[31:23] unused
} ISP_YCCGO_CFG9_T;


typedef union {
    ISP_YCCGO_CTRL_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CON_T;
//
typedef union {
    ISP_YCCGO_CFG1_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CFG1_T;
typedef union {
    ISP_YCCGO_CFG2_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CFG2_T;
//
typedef union {
    ISP_YCCGO_CFG3_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CFG3_T;
//
typedef union {
    ISP_YCCGO_CFG4_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CFG4_T;
//
typedef union {
    ISP_YCCGO_CFG5_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CFG5_T;
//
typedef union {
    ISP_YCCGO_CFG6_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CFG6_T;
//
typedef union {
    ISP_YCCGO_CFG7_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CFG7_T;
//
typedef union {
    ISP_YCCGO_CFG8_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CFG8_T;
//
typedef union {
    ISP_YCCGO_CFG9_T bits;
    MUINT32 val;
} DISPLAY_ISP_YCCGO_CFG9_T;
//
//
typedef union {
    enum { COUNT = 10 };
    MUINT32 set[COUNT];
    struct {
        DISPLAY_ISP_YCCGO_CON_T  ctrl;
        DISPLAY_ISP_YCCGO_CFG1_T cfg1;
        DISPLAY_ISP_YCCGO_CFG2_T cfg2;
        DISPLAY_ISP_YCCGO_CFG3_T cfg3;
        DISPLAY_ISP_YCCGO_CFG4_T cfg4;
        DISPLAY_ISP_YCCGO_CFG5_T cfg5;
        DISPLAY_ISP_YCCGO_CFG6_T cfg6;
        DISPLAY_ISP_YCCGO_CFG7_T cfg7;
        DISPLAY_ISP_YCCGO_CFG8_T cfg8;
        DISPLAY_ISP_YCCGO_CFG9_T cfg9;
    };
} DISPLAY_ISP_YCCGO_T;

/*******************************************************************************
* PRZ
*******************************************************************************/
typedef union {
    enum { COUNT = 1 };
    MUINT32 set[COUNT];
    struct {
        MUINT8  uUpScaleCoeff;  //  [5 bits; 1~19] Up sample coeff. choose > 12 may get undesirable result, '8' is recommended.
        MUINT8  uDnScaleCoeff;  //  [5 bits; 1~19] Down sample coeff. '15' is recommended.
        MUINT8  uEEHCoeff;      //  [4 bits] The strength for horizontal edge.
        MUINT8  uEEVCoeff;      //  [4 bits] The strength for vertial edge.
    };
} DISPLAY_PRZ_T;

/*******************************************************************************
* Index
*******************************************************************************/
typedef struct
{
    MUINT32 PcaCfg;
    MUINT32 PcaSkinLut;
    MUINT32 PcaGrassLut;
    MUINT32 PcaSkyLut;
    MUINT32 YCCGO;
    MUINT32 PRZ;
    MUINT32 NR2;
}	DISPLAY_ISP_INDEX;


/*******************************************************************************
* Display ISP
*******************************************************************************/
#define DISPLAY_ISP_PCA_CFG_TBL_NUM (1)
#define DISPLAY_ISP_PCA_SKIN_TBL_NUM  (7)
#define DISPLAY_ISP_PCA_GRASS_TBL_NUM (7)
#define DISPLAY_ISP_PCA_SKY_TBL_NUM   (7)
#define DISPLAY_ISP_YCCGO_TBL_NUM     (7)
#define DISPLAY_ISP_PRZ_TBL_NUM     (16)
#define DISPLAY_ISP_NR2_TBL_NUM     (1)

typedef struct
{
    DISPLAY_ISP_INDEX     rIndex;
    DISPLAY_ISP_PCA_CFG_T rPcaCfg[DISPLAY_ISP_PCA_CFG_TBL_NUM];
    DISPLAY_ISP_PCA_SKIN_LUT_T rPcaSkinLut[DISPLAY_ISP_PCA_SKIN_TBL_NUM];
    DISPLAY_ISP_PCA_GRASS_LUT_T rPcaGrassLut[DISPLAY_ISP_PCA_GRASS_TBL_NUM];
    DISPLAY_ISP_PCA_SKY_LUT_T rPcaSkyLut[DISPLAY_ISP_PCA_SKY_TBL_NUM];
    DISPLAY_ISP_YCCGO_T   rYCCGO[DISPLAY_ISP_YCCGO_TBL_NUM];
    DISPLAY_PRZ_T         rPRZ[DISPLAY_ISP_PRZ_TBL_NUM];
    DISPLAY_ISP_NR2_T     rNR2[DISPLAY_ISP_NR2_TBL_NUM];
} DISPLAY_ISP_T;

const DISPLAY_ISP_T getDisplayIspParam();

}

#endif
