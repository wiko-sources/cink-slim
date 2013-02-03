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

#ifndef _AWB_PARAM_MT6573_H
#define _AWB_PARAM_MT6573_H

//____General____
#define AWB_LOG_TABLE1_SIZE (256)

#define AWB_LOG_TABLE2_SIZE (2048)
#define AWB_LOG_TABLE2_INDEX_MAX ((MUINT32)(AWB_LOG_TABLE2_SIZE - 1))
#define AWB_LOG_TABLE2_INDEX_MIN ((MUINT32)(52)) // i.e. around 1/10

#define AWB_LV_INDEX_UNIT (10) // 1.0 LV
#define AWB_LV_INDEX_MIN  (0)  // LV 0
#define AWB_LV_INDEX_MAX  (18) // LV 18
#define AWB_LV_INDEX_NUM ((AWB_LV_INDEX_MAX - AWB_LV_INDEX_MIN) + 1)

#define AWB_DAYLIGHT_LOCUS_OFFSET_INDEX_UNIT (500)
#define AWB_DAYLIGHT_LOCUS_OFFSET_INDEX_NUM (21)
#define AWB_DAYLIGHT_LOCUS_OFFSET_INDEX_MAX (AWB_DAYLIGHT_LOCUS_OFFSET_INDEX_NUM - 1)

#define AWB_DAYLIGHT_LOCUS_NEW_OFFSET_INDEX_UNIT (500)
#define AWB_DAYLIGHT_LOCUS_NEW_OFFSET_INDEX_NUM (21)
#define AWB_DAYLIGHT_LOCUS_NEW_OFFSET_INDEX_MAX (AWB_DAYLIGHT_LOCUS_NEW_OFFSET_INDEX_NUM - 1)

#define AWB_WARM_FLUORESCENT_GREEN_OFFSET_INDEX_UNIT (200)
#define AWB_WARM_FLUORESCENT_GREEN_OFFSET_INDEX_NUM (11)
#define AWB_WARM_FLUORESCENT_GREEN_OFFSET_INDEX_MAX (AWB_WARM_FLUORESCENT_GREEN_OFFSET_INDEX_NUM - 1)

#define AWB_TUNGSTEN_MAGENTA_OFFSET_INDEX_UNIT (100)
#define AWB_TUNGSTEN_MAGENTA_OFFSET_INDEX_NUM (11)
#define AWB_TUNGSTEN_MAGENTA_OFFSET_INDEX_MAX (AWB_TUNGSTEN_MAGENTA_OFFSET_INDEX_NUM - 1)

#define AWB_SHADE_GREEN_OFFSET_INDEX_UNIT (100)
#define AWB_SHADE_GREEN_OFFSET_INDEX_NUM (11)
#define AWB_SHADE_GREEN_OFFSET_INDEX_MAX (AWB_SHADE_GREEN_OFFSET_INDEX_NUM - 1)

#define AWB_SHADE_MAGENTA_OFFSET_INDEX_UNIT (100)
#define AWB_SHADE_MAGENTA_OFFSET_INDEX_NUM (11)
#define AWB_SHADE_MAGENTA_OFFSET_INDEX_MAX (AWB_TUNGSTEN_MAGENTA_OFFSET_INDEX_NUM - 1)

#define AWB_SPEED_MIN (0)
#define AWB_SPEED_MAX (100)

#define AWB_LIGHT_WEIGHT_UNIT ((MUINT32)(256)) // 1.0 = 256

//____Auto White Balance Config____
#define AWB_SCALE_UNIT ((MUINT32)(512)) // 1.0 = 512

//____Preset White Balance Config____
#define PWB_LIGHT_AREA_NUM (2)

#define PWB_NEUTRAL_AREA_INDEX (0)
#define PWB_REFERENCE_AREA_INDEX (1)

// Light source definition
typedef enum
{
	AWB_LIGHT_TUNGSTEN = 0,                // Tungsten
    AWB_LIGHT_WARM_FLUORESCENT,            // Warm fluorescent
    AWB_LIGHT_FLUORESCENT,                 // Fluorescent (TL84)
	AWB_LIGHT_CWF,                         // CWF
	AWB_LIGHT_DAYLIGHT,                    // Daylight
    AWB_LIGHT_SHADE,                       // Shade
	AWB_LIGHT_NUM,                         // Light source number
    AWB_LIGHT_NONE = AWB_LIGHT_NUM,        // None: not neutral block
	AWB_LIGHT_DONT_CARE = 0xFF             // Don't care: don't care the light source of block
} AWB_LIGHT_T;

// Preset white balance light source definition
typedef enum
{
	PWB_DAYLIGHT = 0,         // Preset white balance: Daylight
	PWB_CLOUDY_DAYLIGHT,      // Preset white balance: Cloudy daylight
	PWB_SHADE,                // Preset white balance: Shade
    PWB_TWILIGHT,             // Preset white balance: Twilight
	PWB_FLUORESCENT,          // Preset white balance: Fluorescent
    PWB_WARM_FLUORESCENT,     // Preset white balance: Warm fluorescent
	PWB_INCANDESCENT,         // Preset white balance: Incandescent
	PWB_LIGHT_NUM             // Preset white balance light source number
} PWB_LIGHT_T;

// Light source probability
typedef struct
{
	MUINT32 u4P0[AWB_LIGHT_NUM]; // Probability 0
	MUINT32 u4P1[AWB_LIGHT_NUM]; // Probability 1
	MUINT32 u4P2[AWB_LIGHT_NUM]; // Probability 2
	MUINT32 u4P[AWB_LIGHT_NUM];  // Probability
} AWB_LIGHT_PROBABILITY_T;

// AWB speed mode definition
typedef enum
{
	AWB_SPEED_MODE_ONESHOT = 0, // full AWB compensation for half-push/capture
    AWB_SPEED_MODE_SMOOTH_TRANSITION = 1  // partial AWB compensation for preview/movie
} AWB_SPEED_MODE_T;

#define AWB_WIN_NUM (12)

// AWB statistics config
typedef struct
{
    // AWB Sum Window Config Register
    MUINT32 u4AWBSUM_WIND;
    MUINT32 u4AWBSUM_WINU;
	MUINT32 u4AWBSUM_WINR;
    MUINT32 u4AWBSUM_WINL;

	// AWB Control Register
    MUINT32 u4PAXEL_YL;
    MUINT32 u4PAXEL_RGBH;
	MUINT32 u4AWBDM_DEBUG;
    MUINT32 u4SMAREA_NO;
    MUINT32 u4SMAREA_EN;
    MUINT32 u4COLOREDGE_EN;
    MUINT32 u4NEUTRAL_EN;
    MUINT32 u4AWB_EN;

    // AWB Threshold Config
    MUINT32 u4NEUTRAL_TH;
    MUINT32 u4CEDGEY_TH;
	MUINT32 u4CEDGEX_TH;

	// AWB Color Space H1 Config Register
    MINT32 i4AWBH12;
    MINT32 i4AWBH11;

	// AWB Color Space H2 Config Register
    MINT32 i4AWBH22;
    MINT32 i4AWBH21;

	// AWB Color Edge Window Horizontal Config Register
    MINT32 i4AWBCE_WINR;
    MINT32 i4AWBCE_WINL;

    // AWB Color Edge Window Vertical Config Register
    MINT32 i4AWBCE_WIND;
    MINT32 i4AWBCE_WINU;

	// AWB XY Window Horizontal Config Register
    MINT32 i4AWBXY_WINR[AWB_WIN_NUM];
    MINT32 i4AWBXY_WINL[AWB_WIN_NUM];

    // AWB XY Window Vertical Config Register
    MINT32 i4AWBXY_WIND[AWB_WIN_NUM];
    MINT32 i4AWBXY_WINU[AWB_WIN_NUM];

} AWB_STAT_CONFIG_T;

// AWB statistics output
typedef struct
{
    // CAM + 1148h~1204h: AWB XY Window Result (1-12) (Paxel Count, Rsum, Gsum, Bsum)
    struct CAM_1148H
    {
        MUINT32 u4PCNT;
		MUINT32 u4RSUM;
		MUINT32 u4GSUM;
		MUINT32 u4BSUM;
    } AWBXY_RESULT[12];

	// CAM + 1208h~1214h: AWB Sum Window Result (Paxel Count, Rsum, Gsum, Bsum)
    struct CAM_1208H
    {
        MUINT32 u4PCNT;
		MUINT32 u4RSUM;
		MUINT32 u4GSUM;
		MUINT32 u4BSUM;
    } AWBSUM_RESULT;

    // CAM + 1218h~1224h: AWB Color Edge Window Result (Paxel Count, Rsum, Gsum, Bsum)
    struct CAM_1218H
    {
        MUINT32 u4PCNT;
		MUINT32 u4RSUM;
		MUINT32 u4GSUM;
		MUINT32 u4BSUM;
    } AWBCE_RESULT;

} AWB_STAT_T;

// AWB frame-based input parameters
typedef struct
{
	AWB_STAT_T rAWBStat; // AWB statistics
	AWB_SPEED_MODE_T eAWBSpeedMode; // AWB speed mode
	MINT32 i4SceneLV; // Scene LV
	MUINT32 u4TotalPaxelNum; // Total paxel number
} AWB_INPUT_T;

// CCT estimation
typedef struct
{
	MINT32 i4CCT; // CCT
	MINT32 i4FluorescentIndex; // Fluorescent index
} AWB_CCT_T;

// AWB algorithm output (used for lib3a to mhal interface)
typedef struct
{
	AWB_GAIN_T rPreviewAWBGain; // AWB gain for DIP
	AWB_GAIN_T rCaptureAWBGain; // AWB gain for DIP
	AWB_CCT_T rCCT; // CCT estimation
} AWB_OUTPUT_T;

// Tungsten specific algorithm data
typedef struct
{
    MUINT32 u4RG;                            // Tungsten R/G
	MUINT32 u4BG;                            // Tungsten B/G
	MINT32 i4LogRG;                          // Tungsten LOG(R/G)
	MINT32 i4LogBG;                          // Tungsten LOG(B/G)
	MUINT32 u4DaylightLocusRG;               // Daylight locus R/G, by projection in log domain
	MUINT32 u4DaylightLocusBG;               // Daylight locus B/G, by projection in log domain
	MINT32 i4DaylightLocusLogRG;             // Daylight locus LOG(R/G), by projection in log domain
	MINT32 i4DaylightLocusLogBG;             // Daylight locus LOG(B/G), by projection in log domain
	MUINT32 u4DaylightLocusTargetRG;         // Daylight locus target R/G, target for AWB compensation
	MUINT32 u4DaylightLocusTargetBG;         // Daylight locus target B/G, target for AWB compensation
	MINT32 i4DaylightLocusTargetLogRG;       // Daylight locus target LOG(R/G), target for AWB compensation
	MINT32 i4DaylightLocusTargetLogBG;       // Daylight locus target LOG(B/G), target for AWB compensation
    MINT32 i4IsAboveDaylightLocus;           // 1: above or on daylight locus, 0: below daylight locus
	MINT32 i4GMOffset;                       // Tungsten green/magenta offset
	MINT32 i4DaylightLocusOffset;            // Daylight locus offset
	MINT32 i4DaylightLocusNewOffset;         // Daylight locus new offset
	MINT32 i4DaylightLocusTargetOffset;      // Daylight locus target offset
	MINT32 i4DaylightLocusTargetOffsetRatio; // Daylight locus target offset ratio
	MINT32 i4Weight;                         // Tungsten light weight
} AWB_TUNGSTEN_ALGORITHM_DATA_T;

// Warm fluorescent specific algorithm data
typedef struct
{
    MUINT32 u4RG;                            // Warm fluorescent R/G
	MUINT32 u4BG;                            // Warm fluorescent B/G
	MINT32 i4LogRG;                          // Warm fluorescent LOG(R/G)
	MINT32 i4LogBG;                          // Warm fluorescent LOG(B/G)
	MINT32 i4DaylightLocusLogRG;             // Daylight locus LOG(R/G), by projection in log domain
	MINT32 i4DaylightLocusLogBG;             // Daylight locus LOG(B/G), by projection in log domain
	MUINT32 u4DaylightLocusTargetRG;         // Daylight locus target R/G, target for AWB compensation
	MUINT32 u4DaylightLocusTargetBG;         // Daylight locus target B/G, target for AWB compensation
	MINT32 i4DaylightLocusTargetLogRG;       // Daylight locus target LOG(R/G), target for AWB compensation
	MINT32 i4DaylightLocusTargetLogBG;       // Daylight locus target LOG(B/G), target for AWB compensation
	MUINT32 u4GOffsetRG;                     // Green offset R/G
	MUINT32 u4GOffsetBG;                     // Green offset B/G
	MINT32 i4GOffsetThr;                     // Green offset threshold
	MINT32 i4GOffsetLogRG;                   // Green offset LOG(R/G)
	MINT32 i4GOffsetLogBG;                   // Green offset LOG(B/G)
	MINT32 i4GOffset;                        // Green offset
	MINT32 i4DaylightLocusOffset;            // Daylight locus offset
	MINT32 i4DaylightLocusNewOffset;         // Daylight locus new offset
	MINT32 i4DaylightLocusTargetOffset;      // Daylight locus target offset
	MINT32 i4DaylightLocusTargetOffsetRatio; // Daylight locus target offset ratio
	MINT32 i4Weight;                         // Warm fluorescent weight
} AWB_WARM_FLUORESCENT_ALGORITHM_DATA_T;

// Shade specific algorithm data
typedef struct
{
    MUINT32 u4RG;                       // Shade R/G
	MUINT32 u4BG;                       // Shade B/G
	MINT32 i4LogRG;                     // Shade LOG(R/G)
	MINT32 i4LogBG;                     // Shade LOG(B/G)
    MUINT32 u4DaylightLocusRG;          // Daylight locus R/G, by projection in log domain
	MUINT32 u4DaylightLocusBG;          // Daylight locus B/G, by projection in log domain
 	MINT32 i4DaylightLocusLogRG;        // Daylight locus LOG(R/G), by projection in log domain
	MINT32 i4DaylightLocusLogBG;        // Daylight locus LOG(B/G), by projection in log domain
	MUINT32 u4DaylightLocusTargetRG;    // Daylight locus target R/G, target for AWB compensation
	MUINT32 u4DaylightLocusTargetBG;    // Daylight locus target B/G, target for AWB compensation
	MINT32 i4DaylightLocusTargetLogRG;  // Daylight locus target LOG(R/G), target for AWB compensation
	MINT32 i4DaylightLocusTargetLogBG;  // Daylight locus target LOG(R/G), target for AWB compensation
	MINT32 i4IsAboveDaylightLocus;      // 1: above or on daylight locus, 0: below daylight locus
	MINT32 i4GMOffset;                  // Shade green/magenta offset
	MINT32 i4DaylightLocusOffset;       // Daylight locus offset
	MINT32 i4DaylightLocusNewOffset;    // Daylight locus new offset
	MINT32 i4DaylightLocusTargetOffset; // Daylight locus target offset
	MINT32 i4Weight;                    // Shade light weight
} AWB_SHADE_ALGORITHM_DATA_T;

// AWB algorithm related data structure
typedef struct
{
	MUINT32 u4WBGainR[AWB_LIGHT_NUM]; // Gain R
    MUINT32 u4WBGainG[AWB_LIGHT_NUM]; // Gain G
	MUINT32 u4WBGainB[AWB_LIGHT_NUM]; // Gain B
	MUINT32 u4Weight[AWB_LIGHT_NUM]; // Light source weight

	// Tungsten specific algorithm data
    AWB_TUNGSTEN_ALGORITHM_DATA_T rTungsten;

	// Warm fluorescent specific algorithm data
    AWB_WARM_FLUORESCENT_ALGORITHM_DATA_T rWF;

	// Shade specific algorithm data
	AWB_SHADE_ALGORITHM_DATA_T rShade;

} AWB_ALGORITHM_DATA_T;

// Strobe AWB algorithm related data structure
typedef struct
{
    AWB_GAIN_T rPresetGain; // Preset strobe AWB gain after AWB calibration
    AWB_GAIN_T rNonStrobeGain; // Non-strobe AWB gain
    AWB_GAIN_T rAWBGain; // Strobe AWB gain
} STROBE_AWB_ALGORITHM_DATA_T;

// PWB algorithm statistics
typedef struct
{
	MUINT32 u4PaxelNum0; // Paxel number of light area 0
	MUINT32 u4PaxelNum1; // Paxel number of light area 1
	MUINT32 u4PaxelNum;  // Paxel number of light area 0 + light area 1
	AWB_GAIN_T rDefaultGain; // Default gain
	AWB_GAIN_T rWBGain0; // WB gain of light area 0
	AWB_GAIN_T rWBGain1; // WB gain of light area 1
	PWB_LIGHT_T ePWBLight; // PWB light source
	MINT32 i4Xo; // light area 1 original X coordinate
	MINT32 i4Yo; // light area 1 original Y coordinate
	MINT32 i4Xor; // light area 1 original X coordinate (rotated)
	MINT32 i4Yor; // light area 1 original Y coordinate (rotated)
	MINT32 i4Xpr; // light area 1 projected X coordinate (rotated)
	MINT32 i4Ypr; // light area 1 projected Y coordinate (rotated)
	MINT32 i4Xp; // light area 1 projected X coordinate
	MINT32 i4Yp; // light area 1 projected Y coordinate
	MINT32 i4CosInv; // Inverse rotation matrix
	MINT32 i4SinInv; // Inverse rotation matrix
} PWB_ALGORITHM_DATA_T;

// CCT estimation
typedef struct
{
    MINT32 i4LogRG;
	MINT32 i4LogBG;
	MINT32 i4X;
    MINT32 i4Y;
    MINT32 i4XR;
	MINT32 i4YR;
	MINT32 i4CalLogRG;
	MINT32 i4CalLogBG;
	MINT32 i4CalX;
    MINT32 i4CalY;
    MINT32 i4CalXR;
	MINT32 i4CalYR;
	MINT32 i4CCTXR;
	MINT32 i4CCTYR;
    MINT32 i4MIRED_H;
    MINT32 i4MIRED_L;
    MINT32 i4MIRED;
    MINT32 i4YR_TL84;
    MINT32 i4YR_CWF;
	MINT32 i4YR_Mean;
} CCT_INFO_T;

//____AWB Algorithm Parameters____

// Chip dependent parameter
typedef struct
{
    MUINT32 u4AWBGainOutputScaleUnit; // AWB gain output scale unit
	MUINT32 u4AWBGainOutputUpperLimit; // AWB gain output upper limit
	MINT32 i4RotationMatrixUnit; // Rotation matrix unit
} AWB_CHIP_PARAM_T;

// AWB light source probability look-up table
typedef struct
{
	MINT32 i4SizeX; // AWB light source probability look-up table horizontal dimension
	MINT32 i4SizeY; // AWB light source probability look-up table vertical dimension
	MINT32 i4LUT[AWB_LIGHT_NUM][AWB_LV_INDEX_NUM];  // AWB light source probability look-up table
} AWB_LIGHT_PROBABILITY_LUT_T;

// AWB convergence parameter
typedef struct
{
	MINT32 i4Speed; // Convergence speed
	MINT32 i4StableThr; // Stable threshold
} AWB_CONVERGENCE_PARAM_T;

// AWB daylight locus target offset ratio LUT for tungsten and fluorescent0
typedef struct
{
	MINT32 i4LUTSize; // LUT dimension
	MINT32 i4LUT[AWB_DAYLIGHT_LOCUS_NEW_OFFSET_INDEX_NUM]; // Look-up table
} AWB_DAYLIGHT_LOCUS_TARGET_OFFSET_RATIO_T;

// AWB green offset threshold LUT for fluorescent0
typedef struct
{
	MINT32 i4LUTSize; // LUT dimension
	MINT32 i4LUT[AWB_DAYLIGHT_LOCUS_OFFSET_INDEX_NUM]; // Look-up table
} AWB_GREEN_OFFSET_THRESHOLD_LUT_T;

// AWB tungsten light weight LUT
typedef struct
{
	MINT32 i4LUTSize; // LUT dimension
	MINT32 i4LUT[AWB_TUNGSTEN_MAGENTA_OFFSET_INDEX_NUM]; // Look-up table
} AWB_LIGHT_WEIGHT_LUT_TUNGSTEN_T;

// AWB warm fluorescent weight LUT
typedef struct
{
	MINT32 i4LUTSize; // LUT dimension
	MINT32 i4LUT[AWB_WARM_FLUORESCENT_GREEN_OFFSET_INDEX_NUM]; // Look-up table
} AWB_LIGHT_WEIGHT_LUT_WARM_FLUORESCENT_T;

// AWB shade light weight LUT
typedef struct
{
	MINT32 i4MagentaLUTSize; // LUT dimension
	MINT32 i4MagentaLUT[AWB_SHADE_MAGENTA_OFFSET_INDEX_NUM]; // Look-up table
	MINT32 i4GreenLUTSize; // LUT dimension
	MINT32 i4GreenLUT[AWB_SHADE_GREEN_OFFSET_INDEX_NUM]; // Look-up table
} AWB_LIGHT_WEIGHT_LUT_SHADE_T;

// AWB algorithm parameter
typedef struct
{
	AWB_CHIP_PARAM_T rChipParam;
    AWB_LIGHT_PROBABILITY_LUT_T rLightProb;
    AWB_CONVERGENCE_PARAM_T rConvergence;
    AWB_DAYLIGHT_LOCUS_TARGET_OFFSET_RATIO_T rDaylightLocusTargetOffsetRatio;
    AWB_GREEN_OFFSET_THRESHOLD_LUT_T rGreenOffsetThr;
    AWB_LIGHT_WEIGHT_LUT_TUNGSTEN_T rLightWeight_Tungsten;
    AWB_LIGHT_WEIGHT_LUT_WARM_FLUORESCENT_T rLightWeight_WF;
    AWB_LIGHT_WEIGHT_LUT_SHADE_T rLightWeight_Shade;
} AWB_PARAM_T;

// AWB Debug Tag
typedef enum
{
	// AWB
    AWB_TAG_GAIN_R = 0, // [0] White balance gain R
    AWB_TAG_GAIN_G,     // [1] White balance gain G
	AWB_TAG_GAIN_B,     // [2] White balance gain B
	AWB_TAG_LIGHT_MODE, // [3] Light mode
	AWB_TAG_SCENE_LV,   // [4] Scene LV
    AWB_TAG_TOTAL_PAXEL_NUM, // [5] Total paxel number
	AWB_TAG_NEUTRAL_PAXEL_NUM, // [6] Neutral paxel number
	AWB_TAG_NEUTRAL_PAXEL_NUM_THR, // [7] Neutral paxel number threshold
	AWB_TAG_NEUTRAL_PAXEL_NUM_THR_CWF, // [8] Neutral paxel number threshold for CWF
	AWB_TAG_P0_T,   // [9] P0(Tungsten)
	AWB_TAG_P0_WF,  // [10] P0(Warm Fluorescent)
	AWB_TAG_P0_F,   // [11] P0(Fluorescent)
	AWB_TAG_P0_CWF, // [12] P0(CWF)
	AWB_TAG_P0_D,   // [13] P0(Daylight)
    AWB_TAG_P0_S,   // [14] P0(Shade)
	AWB_TAG_P1_T,   // [15] P1(Tungsten)
	AWB_TAG_P1_WF,  // [16] P1(Warm Fluorescent)
	AWB_TAG_P1_F,   // [17] P1(Fluorescent)
	AWB_TAG_P1_CWF, // [18] P1(CWF)
	AWB_TAG_P1_D,   // [19] P1(Daylight)
    AWB_TAG_P1_S,   // [20] P1(Shade)
	AWB_TAG_P2_T,   // [21] P2(Tungsten)
	AWB_TAG_P2_WF,  // [22] P2(Warm Fluorescent)
	AWB_TAG_P2_F,   // [23] P2(Fluorescent)
	AWB_TAG_P2_CWF, // [24] P2(CWF)
	AWB_TAG_P2_D,   // [25] P2(Daylight)
    AWB_TAG_P2_S,   // [26] P2(Shade)
	AWB_TAG_P_T,    // [27] P(Tungsten)
	AWB_TAG_P_WF,   // [28] P(Warm Fluorescent)
	AWB_TAG_P_F,    // [29] P(Fluorescent)
	AWB_TAG_P_CWF,  // [30] P(CWF)
	AWB_TAG_P_D,    // [31] P(Daylight)
    AWB_TAG_P_S,    // [32] P(Shade)
    AWB_TAG_NEUTRAL_PAXEL_NUM_T,   // [33] NeutralPaxelNum(Tungsten)
	AWB_TAG_NEUTRAL_PAXEL_NUM_WF,  // [34] NeutralPaxelNum(Warm Fluorescent)
	AWB_TAG_NEUTRAL_PAXEL_NUM_F,   // [35] NeutralPaxelNum(Fluorescent)
	AWB_TAG_NEUTRAL_PAXEL_NUM_CWF, // [36] NeutralPaxelNum(CWF)
	AWB_TAG_NEUTRAL_PAXEL_NUM_D,   // [37] NeutralPaxelNum(Daylight)
	AWB_TAG_NEUTRAL_PAXEL_NUM_S,   // [38] NeutralPaxelNum(Shade)
	AWB_TAG_GAIN_R_T,   // [39] Rgain(Tungsten)
	AWB_TAG_GAIN_B_T,   // [40] Bgain(Tungsten)
	AWB_TAG_GAIN_R_WF,  // [41] Rgain(Warm Fluorescent)
	AWB_TAG_GAIN_B_WF,  // [42] Bgain(Warm Fluorescent)
	AWB_TAG_GAIN_R_F,   // [43] Rgain(Fluorescent)
	AWB_TAG_GAIN_B_F,   // [44] Bgain(Fluorescent)
	AWB_TAG_GAIN_R_CWF, // [45] Rgain(CWF)
	AWB_TAG_GAIN_B_CWF, // [46] Bgain(CWF)
	AWB_TAG_GAIN_R_D,   // [47] Rgain(Daylight)
	AWB_TAG_GAIN_B_D,   // [48] Bgain(Daylight)
	AWB_TAG_GAIN_R_S,   // [49] Rgain(Shade)
	AWB_TAG_GAIN_B_S,   // [50] Bgain(Shade)
	AWB_TAG_RGBSUM_WEIGHT_T,   // [51] Weight(Tungsten)
	AWB_TAG_RGBSUM_WEIGHT_WF,  // [52] Weight(Warm Fluorescent)
	AWB_TAG_RGBSUM_WEIGHT_F,   // [53] Weight(Fluorescent)
	AWB_TAG_RGBSUM_WEIGHT_CWF, // [54] Weight(CWF)
	AWB_TAG_RGBSUM_WEIGHT_D,   // [55] Weight(Daylight)
	AWB_TAG_RGBSUM_WEIGHT_S,   // [56] Weight(Shade)
	AWB_TAG_RG_T, // [57] Tungsten R/G
	AWB_TAG_BG_T, // [58] Tungsten B/G
	AWB_TAG_DAY_LOCUS_RG_T, // [59] Tungsten daylight locus R/G
	AWB_TAG_DAY_LOCUS_BG_T, // [60] Tungsten daylight locus B/G
	AWB_TAG_DAY_LOCUS_TARGET_RG_T, // [61] Tungsten daylight locus target R/G
	AWB_TAG_DAY_LOCUS_TARGET_BG_T, // [62] Tungsten daylight locus target B/G
	AWB_TAG_LOG_RG_T, // [63] Tungsten log R/G
	AWB_TAG_LOG_BG_T, // [64] Tungsten log B/G
	AWB_TAG_DAY_LOCUS_LOG_RG_T, // [65] Tungsten daylight locus log R/G
	AWB_TAG_DAY_LOCUS_LOG_BG_T, // [66] Tungsten daylight locus log B/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_RG_T, // [67] Tungsten daylight locus target log R/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_BG_T, // [68] Tungsten daylight locus target log B/G
	AWB_TAG_DAY_LOCUS_OFFSET_T,     // [69] Tungsten daylight locus offset
	AWB_TAG_DAY_LOCUS_NEW_OFFSET_T, // [70] Tungsten daylight locus new offset
	AWB_TAG_DAY_LOCUS_TARGET_OFFSET_T,       // [71] Tungsten daylight locus target offset
    AWB_TAG_DAY_LOCUS_TARGET_OFFSET_RATIO_T, // [72] Tungsten daylight locus target offset ratio
	AWB_TAG_IS_ABOVE_DAY_LOCUS_T, // [73] Is above Tungsten daylight locus
	AWB_TAG_GM_OFFSET_T, // [74] Tungsten daylight locus green/magenta offset
	AWB_TAG_WEIGHT_T, // [75] Tungsten light weight
	AWB_TAG_RG_WF, // [76] Warm Fluorescent R/G
	AWB_TAG_BG_WF, // [77] Warm Fluorescent0 B/G
	AWB_TAG_GREEN_OFFSET_RG_WF, // [78] Warm Fluorescent greent offset R/G
	AWB_TAG_GREEN_OFFSET_BG_WF, // [79] Warm Fluorescent green offset B/G
	AWB_TAG_DAY_LOCUS_TARGET_RG_WF, // [80] Warm Fluorescent daylight locus target R/G
	AWB_TAG_DAY_LOCUS_TARGET_BG_WF, // [81] Warm Fluorescent daylight locus target B/G
	AWB_TAG_LOG_RG_WF, // [82] Warm Fluorescent log R/G
	AWB_TAG_LOG_BG_WF, // [83] Warm Fluorescent log B/G
	AWB_TAG_DAY_LOCUS_LOG_RG_WF, // [84] Warm Fluorescent daylight locus log R/G
	AWB_TAG_DAY_LOCUS_LOG_BG_WF, // [85] Warm Fluorescent daylight locus log B/G
	AWB_TAG_GREEN_OFFSET_LOG_RG_WF, // [86] Warm Fluorescent greent offset log R/G
	AWB_TAG_GREEN_OFFSET_LOG_BG_WF, // [87] Warm Fluorescent green offset log B/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_RG_WF, // [88] Warm Fluorescent daylight locus target log R/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_BG_WF, // [89] Warm Fluorescent daylight locus target log B/G
	AWB_TAG_DAY_LOCUS_OFFSET_WF,     // [90] Warm Fluorescent daylight locus offset
	AWB_TAG_DAY_LOCUS_NEW_OFFSET_WF, // [91] Warm Fluorescent daylight locus new offset
	AWB_TAG_DAY_LOCUS_TARGET_OFFSET_WF,       // [92] Warm Fluorescent daylight locus target offset
    AWB_TAG_DAY_LOCUS_TARGET_OFFSET_RATIO_WF, // [93] Warm Fluorescent daylight locus target offset ratio
	AWB_TAG_GREEN_OFFSET_WF,     // [94] Warm Fluorescent daylight locus green offset
	AWB_TAG_GREEN_OFFSET_THR_WF, // [95] Warm Fluorescent daylight locus green offset
	AWB_TAG_WEIGHT_WF, // [96] Warm Fluorescent weight
	AWB_TAG_RG_S, // [97] Shade R/G
	AWB_TAG_BG_S, // [98] Shade B/G
	AWB_TAG_DAY_LOCUS_RG_S, // [99] Shade daylight locus R/G
	AWB_TAG_DAY_LOCUS_BG_S, // [100] Shade daylight locus B/G
	AWB_TAG_DAY_LOCUS_TARGET_RG_S, // [101] Shade daylight locus target R/G
	AWB_TAG_DAY_LOCUS_TARGET_BG_S, // [102] Shade daylight locus target B/G
	AWB_TAG_LOG_RG_S, // [103] Shade log R/G
	AWB_TAG_LOG_BG_S, // [104] Shade log B/G
	AWB_TAG_DAY_LOCUS_LOG_RG_S, // [105] Shade daylight locus log R/G
	AWB_TAG_DAY_LOCUS_LOG_BG_S, // [106] Shade daylight locus log B/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_RG_S, // [107] Shade daylight locus target log R/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_BG_S, // [108] Shade daylight locus target log B/G
	AWB_TAG_DAY_LOCUS_OFFSET_S,     // [109] Shade daylight locus offset
	AWB_TAG_DAY_LOCUS_NEW_OFFSET_S, // [110] Shade daylight locus new offset
	AWB_TAG_WEIGHT_S, // [111] Shade light weight

	// Strobe AWB
	STBAWB_TAG_FLASH_ENV_RATIO, // [112] Flash to environment contribution ratio
	STBAWB_TAG_FLASH_RGAIN, // [113] Flash R gain
	STBAWB_TAG_FLASH_BGAIN, // [114] Flash B gain
	STBAWB_TAG_FLASH_BASE, // [115] Base for flash gain and ratio (1.0X = 512)
    STBAWB_TAG_PRESET_GAIN_R, // [116] Strobe preset gain R
    STBAWB_TAG_PRESET_GAIN_G, // [117] Strobe preset gain G
    STBAWB_TAG_PRESET_GAIN_B, // [118] Strobe preset gain B
    STBAWB_TAG_NONSTROBE_GAIN_R, // [119] Non-strobe AWB gain R
    STBAWB_TAG_NONSTROBE_GAIN_G, // [120] Non-strobe AWB gain G
    STBAWB_TAG_NONSTROBE_GAIN_B, // [121] Non-strobe AWB gain B
    STBAWB_TAG_GAIN_R, // [122] Strobe AWB gain R
    STBAWB_TAG_GAIN_G, // [123] Strobe AWB gain G
    STBAWB_TAG_GAIN_B, // [124] Strobe AWB gain B

	// PWB
	PWB_TAG_PAXEL_NUM_0, // [125] PWB paxel number of light area 0
    PWB_TAG_PAXEL_NUM_1, // [126] PWB paxel number of light area 1
    PWB_TAG_PAXEL_NUM,   // [127] PWB paxel number of light area 0 + light area 1
	PWB_TAG_DEFAULT_GAIN_R, // [128] PWB default gain R
	PWB_TAG_DEFAULT_GAIN_G, // [129] PWB default gain G
	PWB_TAG_DEFAULT_GAIN_B, // [130] PWB default gain B
	PWB_TAG_GAIN0_R, // [131] WB gain R of light area 0
	PWB_TAG_GAIN0_G, // [132] WB gain G of light area 0
	PWB_TAG_GAIN0_B, // [133] WB gain B of light area 0
	PWB_TAG_GAIN1_R, // [134] WB gain R of light area 1
	PWB_TAG_GAIN1_G, // [135] WB gain G of light area 1
	PWB_TAG_GAIN1_B, // [136] WB gain B of light area 1
    PWB_TAG_LIGHT_SOURCE, // [137] PWB light source
	PWB_TAG_LIGHT_AREA1_XO, // [138] PWB light area 1 original X coordinate
	PWB_TAG_LIGHT_AREA1_YO, // [139] PWB light area 1 original Y coordinate
	PWB_TAG_LIGHT_AREA1_XOR, // [140] PWB light area 1 original X coordinate (rotated)
	PWB_TAG_LIGHT_AREA1_YOR, // [141] PWB light area 1 original Y coordinate (rotated)
	PWB_TAG_LIGHT_AREA1_XPR, // [142] PWB light area 1 projected X coordinate (rotated)
	PWB_TAG_LIGHT_AREA1_YPR, // [143] PWB light area 1 projected Y coordinate (rotated)
	PWB_TAG_LIGHT_AREA1_XP, // [144] PWB light area 1 projected X coordinate
	PWB_TAG_LIGHT_AREA1_YP, // [145] PWB light area 1 projected Y coordinate
	PWB_TAG_COS_INV, // [146] PWB Inverse rotation matrix (cos)
	PWB_TAG_SIN_INV, // [147] PWB Inverse rotation matrix (sin)

	// AWB Statistics Config
    AWB_TAG_STAT_CONFIG_AWBSUM_WIND, // [148] AWB statistics config parameter: AWBSUM_WIND
    AWB_TAG_STAT_CONFIG_AWBSUM_WINU, // [149] AWB statistics config parameter: AWBSUM_WINU
	AWB_TAG_STAT_CONFIG_AWBSUM_WINR, // [150] AWB statistics config parameter: AWBSUM_WINR
    AWB_TAG_STAT_CONFIG_AWBSUM_WINL, // [151] AWB statistics config parameter: AWBSUM_WINL
    AWB_TAG_STAT_CONFIG_PAXEL_YL,    // [152] AWB statistics config parameter: PAXEL_YL
    AWB_TAG_STAT_CONFIG_PAXEL_RGBH,  // [153] AWB statistics config parameter: PAXEL_RGBH
	AWB_TAG_STAT_CONFIG_AWBDM_DEBUG, // [154] AWB statistics config parameter: AWBDM_DEBUG
    AWB_TAG_STAT_CONFIG_SMAREA_NO,   // [155] AWB statistics config parameter: SMAREA_NO
    AWB_TAG_STAT_CONFIG_SMAREA_EN,   // [156] AWB statistics config parameter: SMAREA_EN
    AWB_TAG_STAT_CONFIG_COLOREDGE_EN,// [157] AWB statistics config parameter: COLOREDGE_EN
    AWB_TAG_STAT_CONFIG_NEUTRAL_EN,  // [158] AWB statistics config parameter: NEUTRAL_EN
    AWB_TAG_STAT_CONFIG_AWB_EN,      // [159] AWB statistics config parameter: AWB_EN
    AWB_TAG_STAT_CONFIG_NEUTRAL_TH,  // [160] AWB statistics config parameter: NEUTRAL_TH
    AWB_TAG_STAT_CONFIG_CEDGEY_TH,   // [161] AWB statistics config parameter: CEDGEY_TH
	AWB_TAG_STAT_CONFIG_CEDGEX_TH,   // [162] AWB statistics config parameter: CEDGEX_TH
    AWB_TAG_STAT_CONFIG_AWBH12,      // [163] AWB statistics config parameter: AWBH12
    AWB_TAG_STAT_CONFIG_AWBH11,      // [164] AWB statistics config parameter: AWBH11
    AWB_TAG_STAT_CONFIG_AWBH22,      // [165] AWB statistics config parameter: AWBH22
    AWB_TAG_STAT_CONFIG_AWBH21,      // [166] AWB statistics config parameter: AWBH21
    AWB_TAG_STAT_CONFIG_AWBCE_WINR,  // [167] AWB statistics config parameter: AWBCE_WINR
    AWB_TAG_STAT_CONFIG_AWBCE_WINL,  // [168] AWB statistics config parameter: AWBCE_WINL
    AWB_TAG_STAT_CONFIG_AWBCE_WIND,  // [169] AWB statistics config parameter: AWBCE_WIND
    AWB_TAG_STAT_CONFIG_AWBCE_WINU,  // [170] AWB statistics config parameter: AWBCE_WINU
    AWB_TAG_STAT_CONFIG_AWBXY_WINR0, // [171] AWB statistics config parameter: AWBXY_WINR0
    AWB_TAG_STAT_CONFIG_AWBXY_WINL0, // [172] AWB statistics config parameter: AWBXY_WINL0
    AWB_TAG_STAT_CONFIG_AWBXY_WIND0, // [173] AWB statistics config parameter: AWBXY_WIND0
    AWB_TAG_STAT_CONFIG_AWBXY_WINU0, // [174] AWB statistics config parameter: AWBXY_WINU0
    AWB_TAG_STAT_CONFIG_AWBXY_WINR1, // [175] AWB statistics config parameter: AWBXY_WINR1
    AWB_TAG_STAT_CONFIG_AWBXY_WINL1, // [176] AWB statistics config parameter: AWBXY_WINL1
    AWB_TAG_STAT_CONFIG_AWBXY_WIND1, // [177] AWB statistics config parameter: AWBXY_WIND1
    AWB_TAG_STAT_CONFIG_AWBXY_WINU1, // [178] AWB statistics config parameter: AWBXY_WINU1
    AWB_TAG_STAT_CONFIG_AWBXY_WINR2, // [179] AWB statistics config parameter: AWBXY_WINR2
    AWB_TAG_STAT_CONFIG_AWBXY_WINL2, // [180] AWB statistics config parameter: AWBXY_WINL2
    AWB_TAG_STAT_CONFIG_AWBXY_WIND2, // [181] AWB statistics config parameter: AWBXY_WIND2
    AWB_TAG_STAT_CONFIG_AWBXY_WINU2, // [182] AWB statistics config parameter: AWBXY_WINU2
    AWB_TAG_STAT_CONFIG_AWBXY_WINR3, // [183] AWB statistics config parameter: AWBXY_WINR3
    AWB_TAG_STAT_CONFIG_AWBXY_WINL3, // [184] AWB statistics config parameter: AWBXY_WINL3
    AWB_TAG_STAT_CONFIG_AWBXY_WIND3, // [185] AWB statistics config parameter: AWBXY_WIND3
    AWB_TAG_STAT_CONFIG_AWBXY_WINU3, // [186] AWB statistics config parameter: AWBXY_WINU3
    AWB_TAG_STAT_CONFIG_AWBXY_WINR4, // [187] AWB statistics config parameter: AWBXY_WINR4
    AWB_TAG_STAT_CONFIG_AWBXY_WINL4, // [188] AWB statistics config parameter: AWBXY_WINL4
    AWB_TAG_STAT_CONFIG_AWBXY_WIND4, // [189] AWB statistics config parameter: AWBXY_WIND4
    AWB_TAG_STAT_CONFIG_AWBXY_WINU4, // [190] AWB statistics config parameter: AWBXY_WINU4
    AWB_TAG_STAT_CONFIG_AWBXY_WINR5, // [191] AWB statistics config parameter: AWBXY_WINR5
    AWB_TAG_STAT_CONFIG_AWBXY_WINL5, // [192] AWB statistics config parameter: AWBXY_WINL5
    AWB_TAG_STAT_CONFIG_AWBXY_WIND5, // [193] AWB statistics config parameter: AWBXY_WIND5
    AWB_TAG_STAT_CONFIG_AWBXY_WINU5, // [194] AWB statistics config parameter: AWBXY_WINU5
    AWB_TAG_STAT_CONFIG_AWBXY_WINR6, // [195] AWB statistics config parameter: AWBXY_WINR6
    AWB_TAG_STAT_CONFIG_AWBXY_WINL6, // [196] AWB statistics config parameter: AWBXY_WINL6
    AWB_TAG_STAT_CONFIG_AWBXY_WIND6, // [197] AWB statistics config parameter: AWBXY_WIND6
    AWB_TAG_STAT_CONFIG_AWBXY_WINU6, // [198] AWB statistics config parameter: AWBXY_WINU6
    AWB_TAG_STAT_CONFIG_AWBXY_WINR7, // [199] AWB statistics config parameter: AWBXY_WINR7
    AWB_TAG_STAT_CONFIG_AWBXY_WINL7, // [200] AWB statistics config parameter: AWBXY_WINL7
    AWB_TAG_STAT_CONFIG_AWBXY_WIND7, // [201] AWB statistics config parameter: AWBXY_WIND7
    AWB_TAG_STAT_CONFIG_AWBXY_WINU7, // [202] AWB statistics config parameter: AWBXY_WINU7
    AWB_TAG_STAT_CONFIG_AWBXY_WINR8, // [203] AWB statistics config parameter: AWBXY_WINR8
    AWB_TAG_STAT_CONFIG_AWBXY_WINL8, // [204] AWB statistics config parameter: AWBXY_WINL8
    AWB_TAG_STAT_CONFIG_AWBXY_WIND8, // [205] AWB statistics config parameter: AWBXY_WIND8
    AWB_TAG_STAT_CONFIG_AWBXY_WINU8, // [206] AWB statistics config parameter: AWBXY_WINU8
    AWB_TAG_STAT_CONFIG_AWBXY_WINR9, // [207] AWB statistics config parameter: AWBXY_WINR9
    AWB_TAG_STAT_CONFIG_AWBXY_WINL9, // [208] AWB statistics config parameter: AWBXY_WINL9
    AWB_TAG_STAT_CONFIG_AWBXY_WIND9, // [209] AWB statistics config parameter: AWBXY_WIND9
    AWB_TAG_STAT_CONFIG_AWBXY_WINU9, // [210] AWB statistics config parameter: AWBXY_WINU9
    AWB_TAG_STAT_CONFIG_AWBXY_WINRA, // [211] AWB statistics config parameter: AWBXY_WINRA
    AWB_TAG_STAT_CONFIG_AWBXY_WINLA, // [212] AWB statistics config parameter: AWBXY_WINLA
    AWB_TAG_STAT_CONFIG_AWBXY_WINDA, // [213] AWB statistics config parameter: AWBXY_WINDA
    AWB_TAG_STAT_CONFIG_AWBXY_WINUA, // [214] AWB statistics config parameter: AWBXY_WINUA
    AWB_TAG_STAT_CONFIG_AWBXY_WINRB, // [215] AWB statistics config parameter: AWBXY_WINRB
    AWB_TAG_STAT_CONFIG_AWBXY_WINLB, // [216] AWB statistics config parameter: AWBXY_WINLB
    AWB_TAG_STAT_CONFIG_AWBXY_WINDB, // [217] AWB statistics config parameter: AWBXY_WINDB
    AWB_TAG_STAT_CONFIG_AWBXY_WINUB, // [218] AWB statistics config parameter: AWBXY_WINUB

	// CCT Estimation
	AWB_TAG_CCT,            // [219] CCT estimation
	AWB_TAG_FLUORESCENT_INDEX, // [220] CCT estimation: fluorescent index
	AWB_TAG_CCT_LOG_RG,        // [221] CCT estimation: log(R/G) (from AWB gain)
    AWB_TAG_CCT_LOG_BG,        // [222] CCT estimation: log(B/G) (from AWB gain)
	AWB_TAG_CCT_X,             // [223] CCT estimation: X (from AWB gain)
	AWB_TAG_CCT_Y,             // [224] CCT estimation: Y (from AWB gain)
	AWB_TAG_CCT_XR,            // [225] CCT estimation: XR (from AWB gain)
	AWB_TAG_CCT_YR,            // [226] CCT estimation: XR (from AWB gain)
	AWB_TAG_CCT_CAL_LOG_RG,    // [227] CCT estimation: log(R/G) (from AWB calibration gain)
    AWB_TAG_CCT_CAL_LOG_BG,    // [228] CCT estimation: log(B/G) (from AWB calibration gain)
	AWB_TAG_CCT_CAL_X,         // [229] CCT estimation: X (from AWB calibration gain)
	AWB_TAG_CCT_CAL_Y,         // [230] CCT estimation: Y (from AWB calibration gain)
	AWB_TAG_CCT_CAL_XR,        // [231] CCT estimation: XR (from AWB calibration gain)
	AWB_TAG_CCT_CAL_YR,        // [232] CCT estimation: XR (from AWB calibration gain)
	AWB_TAG_CCT_CCT_XR,        // [233] CCT estimation: CCTXR
	AWB_TAG_CCT_CCT_YR,        // [234] CCT estimation: CCTXR
	AWB_TAG_CCT_MIRED_H,       // [235] CCT estimation: MIRED_H
	AWB_TAG_CCT_MIRED_L,       // [236] CCT estimation: MIRED_L
	AWB_TAG_CCT_MIRED,         // [237] CCT estimation: MIRED
	AWB_TAG_CCT_YR_TL84,       // [238] CCT estimation: YR_TL84 (relative rotated Y coordinate of TL84)
    AWB_TAG_CCT_YR_CWF,        // [239] CCT estimation: YR_CWF (relative rotated Y coordinate of CWF)
    AWB_TAG_CCT_YR_MEAN,       // [240] CCT estimation: YR_Mean = (YR_TL84 + YR_CWF) / 2

	// AWB Calibration Gain
	AWB_TAG_CAL_GAIN_R, // [241] AWB calibration gain R
	AWB_TAG_CAL_GAIN_G, // [242] AWB calibration gain G
	AWB_TAG_CAL_GAIN_B, // [243] AWB calibration gain B

	//____NVRAM Data____

	// AWB Calibration Data
    AWB_NVRAM_CAL_GAIN_R, // [244] AWB calibration gain R
	AWB_NVRAM_CAL_GAIN_G, // [245] AWB calibration gain G
	AWB_NVRAM_CAL_GAIN_B, // [246] AWB calibration gain B
    AWB_NVRAM_DEF_GAIN_R, // [247] AWB default calibration gain R
	AWB_NVRAM_DEF_GAIN_G, // [248] AWB default calibration gain G
	AWB_NVRAM_DEF_GAIN_B, // [249] AWB default calibration gain B
    AWB_NVRAM_D65_GAIN_R, // [250] AWB D65 gain R
	AWB_NVRAM_D65_GAIN_G, // [251] AWB D65 gain G
	AWB_NVRAM_D65_GAIN_B, // [252] AWB D65 gain B

	// Light Source Original XY Coordinate
	AWB_NVRAM_HORIZON_XO, // [253] Horizon Xo
	AWB_NVRAM_HORIZON_YO, // [254] Horizon Yo
	AWB_NVRAM_A_XO,       // [255] A Xo
	AWB_NVRAM_A_YO,       // [256] A Yo
	AWB_NVRAM_TL84_XO,    // [257] TL84 Xo
	AWB_NVRAM_TL84_YO,    // [258] TL84 Yo
	AWB_NVRAM_CWF_XO,     // [259] CWF Xo
	AWB_NVRAM_CWF_YO,     // [260] CWF Yo
	AWB_NVRAM_DNP_XO,     // [261] DNP Xo
	AWB_NVRAM_DNP_YO,     // [262] DNP Yo
	AWB_NVRAM_D65_XO,     // [263] D65 Xo
	AWB_NVRAM_D65_YO,     // [264] D65 Yo
	AWB_NVRAM_D75_XO,     // [265] D75 Xo
	AWB_NVRAM_D75_YO,     // [266] D75 Yo

	// Light Source Rotated XY Coordinate
	AWB_NVRAM_HORIZON_XR, // [267] Horizon Xr
	AWB_NVRAM_HORIZON_YR, // [268] Horizon Yr
	AWB_NVRAM_A_XR,       // [269] A Xr
	AWB_NVRAM_A_YR,       // [270] A Yr
	AWB_NVRAM_TL84_XR,    // [271] TL84 Xr
	AWB_NVRAM_TL84_YR,    // [272] TL84 Yr
	AWB_NVRAM_CWF_XR,     // [273] CWF Xr
	AWB_NVRAM_CWF_YR,     // [274] CWF Yr
	AWB_NVRAM_DNP_XR,     // [275] DNP Xr
	AWB_NVRAM_DNP_YR,     // [276] DNP Yr
	AWB_NVRAM_D65_XR,     // [277] D65 Xr
	AWB_NVRAM_D65_YR,     // [278] D65 Yr
	AWB_NVRAM_D75_XR,     // [279] D75 Xr
	AWB_NVRAM_D75_YR,     // [280] D75 Yr

	// Rotation Matrix Parameter
	AWB_NVRAM_ROTATION_ANGLE, // [281] Rotation angle
	AWB_NVRAM_ROTATION_H11,   // [282] Rotation matrix: H11
	AWB_NVRAM_ROTATION_H12,   // [283] Rotation matrix: H12
	AWB_NVRAM_ROTATION_H21,   // [284] Rotation matrix: H21
	AWB_NVRAM_ROTATION_H22,   // [285] Rotation matrix: H22

	// Daylight Locus Parameter
	AWB_NVRAM_SLPOE_NUMERATOR,   // [286] Daylight locus slop numerator
	AWB_NVRAM_SLPOE_DENOMINATOR, // [287] Daylight locus slop denominator

	// AWB Light Area
	AWB_NVRAM_TUNGSTEN_RIGHT, // [288] AWB light area right bound: Tungsten
	AWB_NVRAM_TUNGSTEN_LEFT,  // [289] AWB light area left bound: Tungsten
	AWB_NVRAM_TUNGSTEN_UPPER, // [290] AWB light area upper bound: Tungsten
	AWB_NVRAM_TUNGSTEN_LOWER, // [291] AWB light area lower bound: Tungsten
	AWB_NVRAM_WARM_FLUORESCENT_RIGHT, // [292] AWB light area right bound: Warm Fluorescent
	AWB_NVRAM_WARM_FLUORESCENT_LEFT,  // [293] AWB light area left bound: Warm Fluorescent
	AWB_NVRAM_WARM_FLUORESCENT_UPPER, // [294] AWB light area upper bound: Warm Fluorescent
	AWB_NVRAM_WARM_FLUORESCENT_LOWER, // [295] AWB light area lower bound: Warm Fluorescent
	AWB_NVRAM_FLUORESCENT_RIGHT, // [296] AWB light area right bound: Fluorescent
	AWB_NVRAM_FLUORESCENT_LEFT,  // [297] AWB light area left bound: Fluorescent
	AWB_NVRAM_FLUORESCENT_UPPER, // [298] AWB light area upper bound: Fluorescent
	AWB_NVRAM_FLUORESCENT_LOWER, // [299] AWB light area lower bound: Fluorescent
	AWB_NVRAM_CWF_RIGHT, // [300] AWB light area right bound: CWF
	AWB_NVRAM_CWF_LEFT,  // [301] AWB light area left bound: CWF
	AWB_NVRAM_CWF_UPPER, // [302] AWB light area upper bound: CWF
	AWB_NVRAM_CWF_LOWER, // [303] AWB light area lower bound: CWF
	AWB_NVRAM_DAYLIGHT_RIGHT, // [304] AWB light area right bound: Daylight
	AWB_NVRAM_DAYLIGHT_LEFT,  // [305] AWB light area left bound: Daylight
	AWB_NVRAM_DAYLIGHT_UPPER, // [306] AWB light area upper bound: Daylight
	AWB_NVRAM_DAYLIGHT_LOWER, // [307] AWB light area lower bound: Daylight
	AWB_NVRAM_SHADE_RIGHT, // [308] AWB light area right bound: Shade
	AWB_NVRAM_SHADE_LEFT,  // [309] AWB light area left bound: Shade
	AWB_NVRAM_SHADE_UPPER, // [310] AWB light area upper bound: Shade
	AWB_NVRAM_SHADE_LOWER, // [311] AWB light area lower bound: Shade

	// PWB Light Area
	PWB_NVRAM_REFERENCE_RIGHT, // [312] PWB light area right bound: Reference Area
	PWB_NVRAM_REFERENCE_LEFT,  // [313] PWB light area left bound: Reference Area
	PWB_NVRAM_REFERENCE_UPPER, // [314] PWB light area upper bound: Reference Area
	PWB_NVRAM_REFERENCE_LOWER, // [315] PWB light area lower bound: Reference Area
	PWB_NVRAM_DAYLIGHT_RIGHT, // [316] PWB light area right bound: Daylight
	PWB_NVRAM_DAYLIGHT_LEFT,  // [317] PWB light area left bound: Daylight
	PWB_NVRAM_DAYLIGHT_UPPER, // [318] PWB light area upper bound: Daylight
	PWB_NVRAM_DAYLIGHT_LOWER, // [319] PWB light area lower bound: Daylight
	PWB_NVRAM_CLOUDY_DAYLIGHT_RIGHT, // [320] PWB light area right bound: Cloudy Daylight
	PWB_NVRAM_CLOUDY_DAYLIGHT_LEFT,  // [321] PWB light area left bound: Cloudy Daylight
	PWB_NVRAM_CLOUDY_DAYLIGHT_UPPER, // [322] PWB light area upper bound: Cloudy Daylight
	PWB_NVRAM_CLOUDY_DAYLIGHT_LOWER, // [323] PWB light area lower bound: Cloudy Daylight
	PWB_NVRAM_SHADE_RIGHT, // [324] PWB light area right bound: Shade
	PWB_NVRAM_SHADE_LEFT,  // [325] PWB light area left bound: Shade
	PWB_NVRAM_SHADE_UPPER, // [326] PWB light area upper bound: Shade
	PWB_NVRAM_SHADE_LOWER, // [327] PWB light area lower bound: Shade
	PWB_NVRAM_TWILIGHT_RIGHT, // [328] PWB light area right bound: Twilight
	PWB_NVRAM_TWILIGHT_LEFT,  // [329] PWB light area left bound: Twilight
	PWB_NVRAM_TWILIGHT_UPPER, // [330] PWB light area upper bound: Twilight
	PWB_NVRAM_TWILIGHT_LOWER, // [331] PWB light area lower bound: Twilight
	PWB_NVRAM_FLUORESCENT_RIGHT, // [332] PWB light area right bound: Fluorescent
	PWB_NVRAM_FLUORESCENT_LEFT,  // [333] PWB light area left bound: Fluorescent
	PWB_NVRAM_FLUORESCENT_UPPER, // [334] PWB light area upper bound: Fluorescent
	PWB_NVRAM_FLUORESCENT_LOWER, // [335] PWB light area lower bound: Fluorescent
	PWB_NVRAM_WARM_FLUORESCENT_RIGHT, // [336] PWB light area right bound: Warm Fluorescent
	PWB_NVRAM_WARM_FLUORESCENT_LEFT,  // [337] PWB light area left bound: Warm Fluorescent
	PWB_NVRAM_WARM_FLUORESCENT_UPPER, // [338] PWB light area upper bound: Warm Fluorescent
	PWB_NVRAM_WARM_FLUORESCENT_LOWER, // [339] PWB light area lower bound: Warm Fluorescent
	PWB_NVRAM_WARM_INCANDESCENT_RIGHT, // [340] PWB light area right bound: Incandescent
	PWB_NVRAM_WARM_INCANDESCENT_LEFT,  // [341] PWB light area left bound: Incandescent
	PWB_NVRAM_WARM_INCANDESCENT_UPPER, // [342] PWB light area upper bound: Incandescent
	PWB_NVRAM_WARM_INCANDESCENT_LOWER, // [343] PWB light area lower bound: Incandescent

	// PWB Default Gain
    PWB_NVRAM_DEF_GAIN_R_DAYLIGHT, // [344] PWB default gain R: Daylight
	PWB_NVRAM_DEF_GAIN_G_DAYLIGHT, // [345] PWB default gain G: Daylight
	PWB_NVRAM_DEF_GAIN_B_DAYLIGHT, // [346] PWB default gain B: Daylight
    PWB_NVRAM_DEF_GAIN_R_CLOUDY_DAYLIGHT, // [347] PWB default gain R: Cloudy Daylight
	PWB_NVRAM_DEF_GAIN_G_CLOUDY_DAYLIGHT, // [348] PWB default gain G: Cloudy Daylight
	PWB_NVRAM_DEF_GAIN_B_CLOUDY_DAYLIGHT, // [349] PWB default gain B: Cloudy Daylight
    PWB_NVRAM_DEF_GAIN_R_SHADE, // [350] PWB default gain R: Shade
	PWB_NVRAM_DEF_GAIN_G_SHADE, // [351] PWB default gain G: Shade
	PWB_NVRAM_DEF_GAIN_B_SHADE, // [352] PWB default gain B: Shade
    PWB_NVRAM_DEF_GAIN_R_TWILIGHT, // [353] PWB default gain R: Twilight
	PWB_NVRAM_DEF_GAIN_G_TWILIGHT, // [354] PWB default gain G: Twilight
	PWB_NVRAM_DEF_GAIN_B_TWILIGHT, // [355] PWB default gain B: Twilight
    PWB_NVRAM_DEF_GAIN_R_FLUORESCENT, // [356] PWB default gain R: Fluorescent
	PWB_NVRAM_DEF_GAIN_G_FLUORESCENT, // [357] PWB default gain G: Fluorescent
	PWB_NVRAM_DEF_GAIN_B_FLUORESCENT, // [358] PWB default gain B: Fluorescent
    PWB_NVRAM_DEF_GAIN_R_WARM_FLUORESCENT, // [359] PWB default gain R: Warm Fluorescent
	PWB_NVRAM_DEF_GAIN_G_WARM_FLUORESCENT, // [360] PWB default gain G: Warm Fluorescent
	PWB_NVRAM_DEF_GAIN_B_WARM_FLUORESCENT, // [361] PWB default gain B: Warm Fluorescent
    PWB_NVRAM_DEF_GAIN_R_INCANDESCENT, // [362] PWB default gain R: Incandescent
	PWB_NVRAM_DEF_GAIN_G_INCANDESCENT, // [363] PWB default gain G: Incandescent
	PWB_NVRAM_DEF_GAIN_B_INCANDESCENT, // [364] PWB default gain B: Incandescent

	// AWB Preference Color Parameter
    AWB_NVRAM_PREFERENCE_COLOR_SLIDER_TUNGSTEN,     // [365] AWB preference color slider value: Tungsten
	AWB_NVRAM_PREFERENCE_COLOR_OFFSET_THR_TUNGSTEN, // [366] AWB preference color offset threshold: Tungsten
    AWB_NVRAM_PREFERENCE_COLOR_SLIDER_WARM_FLUORESCENT,     // [367] AWB preference color slider value: Warm Fluorescent
	AWB_NVRAM_PREFERENCE_COLOR_OFFSET_THR_WARM_FLUORESCENT, // [368] AWB preference color offset threshold: Warm Fluorescent
    AWB_NVRAM_PREFERENCE_COLOR_SLIDER_SHADE,     // [369] AWB preference color slider value: Shade
	AWB_NVRAM_PREFERENCE_COLOR_OFFSET_THR_SHADE, // [370] AWB preference color offset threshold: Shade
    AWB_NVRAM_PREFERENCE_COLOR_DAYLIGHT_GAIN_R, // [371] AWB preference color daylight gain R
	AWB_NVRAM_PREFERENCE_COLOR_DAYLIGHT_GAIN_G, // [372] AWB preference color daylight gain G
	AWB_NVRAM_PREFERENCE_COLOR_DAYLIGHT_GAIN_B, // [373] AWB preference color daylight gain B

	// CCT Estimation
	AWB_NVRAM_CCT_HORIZON, // [374] CCT: Horizon
	AWB_NVRAM_CCT_A,       // [375] CCT: A
	AWB_NVRAM_CCT_TL84,    // [376] CCT: TL84
	AWB_NVRAM_CCT_DNP,     // [377] CCT: DNP
	AWB_NVRAM_CCT_D65,     // [378] CCT: D65
	AWB_NVRAM_CCT_D75,     // [379] CCT: D75
	AWB_NVRAM_XR_HORIZON,  // [380] Xr: Horizon
	AWB_NVRAM_XR_A,        // [381] Xr: A
	AWB_NVRAM_XR_TL84,     // [382] Xr: TL84
	AWB_NVRAM_XR_DNP,      // [383] Xr: DNP
	AWB_NVRAM_XR_D65,      // [384] Xr: D65
	AWB_NVRAM_XR_D75,      // [385] Xr: D75

	// AWB Statistics Data
	AWB_TAG_STAT_PCNT0, // [386] paxel count of window 0
	AWB_TAG_STAT_RSUM0, // [387] R sum of window 0
	AWB_TAG_STAT_GSUM0, // [388] R sum of window 0
	AWB_TAG_STAT_BSUM0, // [389] R sum of window 0
	AWB_TAG_STAT_PCNT1, // [390] paxel count of window 1
	AWB_TAG_STAT_RSUM1, // [391] R sum of window 1
	AWB_TAG_STAT_GSUM1, // [392] R sum of window 1
	AWB_TAG_STAT_BSUM1, // [393] R sum of window 1
	AWB_TAG_STAT_PCNT2, // [394] paxel count of window 2
	AWB_TAG_STAT_RSUM2, // [395] R sum of window 2
	AWB_TAG_STAT_GSUM2, // [396] R sum of window 2
	AWB_TAG_STAT_BSUM2, // [397] R sum of window 2
	AWB_TAG_STAT_PCNT3, // [398] paxel count of window 3
	AWB_TAG_STAT_RSUM3, // [399] R sum of window 3
	AWB_TAG_STAT_GSUM3, // [400] R sum of window 3
	AWB_TAG_STAT_BSUM3, // [401] R sum of window 3
	AWB_TAG_STAT_PCNT4, // [402] paxel count of window 4
	AWB_TAG_STAT_RSUM4, // [403] R sum of window 4
	AWB_TAG_STAT_GSUM4, // [404] R sum of window 4
	AWB_TAG_STAT_BSUM4, // [405] R sum of window 4
	AWB_TAG_STAT_PCNT5, // [406] paxel count of window 5
	AWB_TAG_STAT_RSUM5, // [407] R sum of window 5
	AWB_TAG_STAT_GSUM5, // [408] R sum of window 5
	AWB_TAG_STAT_BSUM5  // [409] R sum of window 5

} AWB_DEBUG_TAG_T;

#define AWB_DEBUG_TAG_SIZE (420)

typedef struct
{
    AAA_DEBUG_TAG_T Tag[AWB_DEBUG_TAG_SIZE];
} AWB_DEBUG_INFO_T;

#endif

