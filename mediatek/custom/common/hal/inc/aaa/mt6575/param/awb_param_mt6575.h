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

#ifndef _AWB_PARAM_MT6575_H
#define _AWB_PARAM_MT6575_H

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
    AWB_LIGHT_DAYLIGHT_FLUORESCENT,        // Daylight fluorescent
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
	MINT32 i4IsStrobeFired; // AWB for AF lamp or pre-flash
} AWB_INPUT_T;

// CCT estimation
typedef struct
{
	MINT32 i4CCT; // CCT
	MINT32 i4FluorescentIndex; // Fluorescent index
	MINT32 i4DaylightFluorescentIndex; // Daylight fluorescent index
	MINT32 i4DaylightProb; // Daylight probability
	MINT32 i4DaylightFluorescentProb; // Daylight fluorescent probability
	MINT32 i4SceneLV; // Scene LV
} AWB_CCT_T;

// AWB algorithm output (used for lib3a to mhal interface)
typedef struct
{
	AWB_GAIN_T rPreviewAWBGain; // AWB gain for DIP
	AWB_GAIN_T rPreviewStrobeAWBGain; // AWB gain for DIP
	AWB_GAIN_T rCaptureAWBGain; // AWB gain for DIP
	AWB_CCT_T rCCT; // CCT estimation
	MINT32 i4IsStrobeFired; // AWB for AF lamp or pre-flash
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
	MINT32 i4YR_Mean_F;
    MINT32 i4YR_D65;
    MINT32 i4YR_DF;
	MINT32 i4YR_Mean_DF;
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

// AWB one-shot parameter
typedef struct
{
	MBOOL bSmoothEnable; // Enable smooth one-shot AWB for dark environment: take weighted average of one-shot AWB gain and preview AWB gain based on scene LV
	MINT32 i4LVThrL; // Low LV threshold: take preview AWB gain only when scene LV <= i4LVThrL
	MINT32 i4LVThrH; // High LV threshold: take one-shot AWB gain only when scene LV >= i4LVThrH
	                 // Perform interpolation when i4LVThrH > LV > i4LVThrL
} AWB_ONE_SHOT_T;

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
    AWB_ONE_SHOT_T rOneShotParam;
} AWB_PARAM_T;

// AWB Debug Tag
typedef enum
{
	// AWB
	AWB_TAG_VERSION = 0,// AWB tag version
    AWB_TAG_GAIN_R,     // White balance gain R
    AWB_TAG_GAIN_G,     // White balance gain G
	AWB_TAG_GAIN_B,     // White balance gain B
	AWB_TAG_LIGHT_MODE, // Light mode
	AWB_TAG_SCENE_LV,   // Scene LV
    AWB_TAG_TOTAL_PAXEL_NUM,           // Total paxel number
	AWB_TAG_NEUTRAL_PAXEL_NUM,         // Neutral paxel number
	AWB_TAG_NEUTRAL_PAXEL_NUM_THR,     // Neutral paxel number threshold
	AWB_TAG_NEUTRAL_PAXEL_NUM_THR_CWF, // Neutral paxel number threshold for CWF
	AWB_TAG_NEUTRAL_PAXEL_NUM_THR_DF,  // Neutral paxel number threshold for DF
	AWB_TAG_P0_T,   // P0(Tungsten)
	AWB_TAG_P0_WF,  // P0(Warm Fluorescent)
	AWB_TAG_P0_F,   // P0(Fluorescent)
	AWB_TAG_P0_CWF, // P0(CWF)
	AWB_TAG_P0_D,   // P0(Daylight)
    AWB_TAG_P0_S,   // P0(Shade)
    AWB_TAG_P0_DF,  // P0(Daylight Fluorescent)
	AWB_TAG_P1_T,   // P1(Tungsten)
	AWB_TAG_P1_WF,  // P1(Warm Fluorescent)
	AWB_TAG_P1_F,   // P1(Fluorescent)
	AWB_TAG_P1_CWF, // P1(CWF)
	AWB_TAG_P1_D,   // P1(Daylight)
    AWB_TAG_P1_S,   // P1(Shade)
    AWB_TAG_P1_DF,  // P1(Daylight Fluorescent)
	AWB_TAG_P2_T,   // P2(Tungsten)
	AWB_TAG_P2_WF,  // P2(Warm Fluorescent)
	AWB_TAG_P2_F,   // P2(Fluorescent)
	AWB_TAG_P2_CWF, // P2(CWF)
	AWB_TAG_P2_D,   // P2(Daylight)
    AWB_TAG_P2_S,   // P2(Shade)
    AWB_TAG_P2_DF,  // P2(Daylight Fluorescent)
	AWB_TAG_P_T,    // P(Tungsten)
	AWB_TAG_P_WF,   // P(Warm Fluorescent)
	AWB_TAG_P_F,    // P(Fluorescent)
	AWB_TAG_P_CWF,  // P(CWF)
	AWB_TAG_P_D,    // P(Daylight)
    AWB_TAG_P_S,    // P(Shade)
    AWB_TAG_P_DF,   // P(Daylight Fluorescent)
    AWB_TAG_NEUTRAL_PAXEL_NUM_T,   // NeutralPaxelNum(Tungsten)
	AWB_TAG_NEUTRAL_PAXEL_NUM_WF,  // NeutralPaxelNum(Warm Fluorescent)
	AWB_TAG_NEUTRAL_PAXEL_NUM_F,   // NeutralPaxelNum(Fluorescent)
	AWB_TAG_NEUTRAL_PAXEL_NUM_CWF, // NeutralPaxelNum(CWF)
	AWB_TAG_NEUTRAL_PAXEL_NUM_D,   // NeutralPaxelNum(Daylight)
	AWB_TAG_NEUTRAL_PAXEL_NUM_S,   // NeutralPaxelNum(Shade)
	AWB_TAG_NEUTRAL_PAXEL_NUM_DF,  // NeutralPaxelNum(Daylight Fluorescent)
	AWB_TAG_AVG_XR_T,   // AverageXR(Tungsten)
	AWB_TAG_AVG_YR_T,   // AverageYR(Tungsten)
	AWB_TAG_AVG_XR_WF,  // AverageXR(Warm Fluorescent)
	AWB_TAG_AVG_YR_WF,  // AverageYR(Warm Fluorescent)
	AWB_TAG_AVG_XR_F,   // AverageXR(Fluorescent)
	AWB_TAG_AVG_YR_F,   // AverageYR(Fluorescent)
	AWB_TAG_AVG_XR_CWF, // AverageXR(CWF)
	AWB_TAG_AVG_YR_CWF, // AverageYR(CWF)
	AWB_TAG_AVG_XR_D,   // AverageXR(Daylight)
	AWB_TAG_AVG_YR_D,   // AverageYR(Daylight)
	AWB_TAG_AVG_XR_S,   // AverageXR(Shade)
	AWB_TAG_AVG_YR_S,   // AverageYR(Shade)
	AWB_TAG_AVG_XR_DF,  // AverageXR(Daylight Fluorescent)
	AWB_TAG_AVG_YR_DF,  // AverageYR(Daylight Fluorescent)
	AWB_TAG_GAIN_R_T,   // Rgain(Tungsten)
	AWB_TAG_GAIN_G_T,   // Ggain(Tungsten)
	AWB_TAG_GAIN_B_T,   // Bgain(Tungsten)
	AWB_TAG_GAIN_R_WF,  // Rgain(Warm Fluorescent)
	AWB_TAG_GAIN_G_WF,  // Ggain(Warm Fluorescent)
	AWB_TAG_GAIN_B_WF,  // Bgain(Warm Fluorescent)
	AWB_TAG_GAIN_R_F,   // Rgain(Fluorescent)
	AWB_TAG_GAIN_G_F,   // Ggain(Fluorescent)
	AWB_TAG_GAIN_B_F,   // Bgain(Fluorescent)
	AWB_TAG_GAIN_R_CWF, // Rgain(CWF)
	AWB_TAG_GAIN_G_CWF, // Ggain(CWF)
	AWB_TAG_GAIN_B_CWF, // Bgain(CWF)
	AWB_TAG_GAIN_R_D,   // Rgain(Daylight)
	AWB_TAG_GAIN_G_D,   // Ggain(Daylight)
	AWB_TAG_GAIN_B_D,   // Bgain(Daylight)
	AWB_TAG_GAIN_R_S,   // Rgain(Shade)
	AWB_TAG_GAIN_G_S,   // Ggain(Shade)
	AWB_TAG_GAIN_B_S,   // Bgain(Shade)
	AWB_TAG_GAIN_R_DF,  // Rgain(Daylight Fluorescent)
	AWB_TAG_GAIN_G_DF,  // Ggain(Daylight Fluorescent)
	AWB_TAG_GAIN_B_DF,  // Bgain(Daylight Fluorescent)
	AWB_TAG_RGBSUM_WEIGHT_T,   // Weight(Tungsten)
	AWB_TAG_RGBSUM_WEIGHT_WF,  // Weight(Warm Fluorescent)
	AWB_TAG_RGBSUM_WEIGHT_F,   // Weight(Fluorescent)
	AWB_TAG_RGBSUM_WEIGHT_CWF, // Weight(CWF)
	AWB_TAG_RGBSUM_WEIGHT_D,   // Weight(Daylight)
	AWB_TAG_RGBSUM_WEIGHT_S,   // Weight(Shade)
    AWB_TAG_RGBSUM_WEIGHT_DF,  // Weight(Daylight fluorescent)
	AWB_TAG_RG_T,                            // Tungsten R/G
	AWB_TAG_BG_T,                            // Tungsten B/G
	AWB_TAG_DAY_LOCUS_RG_T,                  // Tungsten daylight locus R/G
	AWB_TAG_DAY_LOCUS_BG_T,                  // Tungsten daylight locus B/G
	AWB_TAG_DAY_LOCUS_TARGET_RG_T,           // Tungsten daylight locus target R/G
	AWB_TAG_DAY_LOCUS_TARGET_BG_T,           // Tungsten daylight locus target B/G
	AWB_TAG_LOG_RG_T,                        // Tungsten log R/G
	AWB_TAG_LOG_BG_T,                        // Tungsten log B/G
	AWB_TAG_DAY_LOCUS_LOG_RG_T,              // Tungsten daylight locus log R/G
	AWB_TAG_DAY_LOCUS_LOG_BG_T,              // Tungsten daylight locus log B/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_RG_T,       // Tungsten daylight locus target log R/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_BG_T,       // Tungsten daylight locus target log B/G
	AWB_TAG_DAY_LOCUS_OFFSET_T,              // Tungsten daylight locus offset
	AWB_TAG_DAY_LOCUS_NEW_OFFSET_T,          // Tungsten daylight locus new offset
	AWB_TAG_DAY_LOCUS_TARGET_OFFSET_T,       // Tungsten daylight locus target offset
    AWB_TAG_DAY_LOCUS_TARGET_OFFSET_RATIO_T, // Tungsten daylight locus target offset ratio
	AWB_TAG_IS_ABOVE_DAY_LOCUS_T,            // Is above Tungsten daylight locus
	AWB_TAG_GM_OFFSET_T,                     // Tungsten daylight locus green/magenta offset
	AWB_TAG_WEIGHT_T,                        // Tungsten light weight
	AWB_TAG_RG_WF,                            // Warm Fluorescent R/G
	AWB_TAG_BG_WF,                            // Warm Fluorescent0 B/G
	AWB_TAG_GREEN_OFFSET_RG_WF,               // Warm Fluorescent greent offset R/G
	AWB_TAG_GREEN_OFFSET_BG_WF,               // Warm Fluorescent green offset B/G
	AWB_TAG_DAY_LOCUS_TARGET_RG_WF,           // Warm Fluorescent daylight locus target R/G
	AWB_TAG_DAY_LOCUS_TARGET_BG_WF,           // Warm Fluorescent daylight locus target B/G
	AWB_TAG_LOG_RG_WF,                        // Warm Fluorescent log R/G
	AWB_TAG_LOG_BG_WF,                        // Warm Fluorescent log B/G
	AWB_TAG_DAY_LOCUS_LOG_RG_WF,              // Warm Fluorescent daylight locus log R/G
	AWB_TAG_DAY_LOCUS_LOG_BG_WF,              // Warm Fluorescent daylight locus log B/G
	AWB_TAG_GREEN_OFFSET_LOG_RG_WF,           // Warm Fluorescent greent offset log R/G
	AWB_TAG_GREEN_OFFSET_LOG_BG_WF,           // Warm Fluorescent green offset log B/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_RG_WF,       // Warm Fluorescent daylight locus target log R/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_BG_WF,       // Warm Fluorescent daylight locus target log B/G
	AWB_TAG_DAY_LOCUS_OFFSET_WF,              // Warm Fluorescent daylight locus offset
	AWB_TAG_DAY_LOCUS_NEW_OFFSET_WF,          // Warm Fluorescent daylight locus new offset
	AWB_TAG_DAY_LOCUS_TARGET_OFFSET_WF,       // Warm Fluorescent daylight locus target offset
    AWB_TAG_DAY_LOCUS_TARGET_OFFSET_RATIO_WF, // Warm Fluorescent daylight locus target offset ratio
	AWB_TAG_GREEN_OFFSET_WF,                  // Warm Fluorescent daylight locus green offset
	AWB_TAG_GREEN_OFFSET_THR_WF,              // Warm Fluorescent daylight locus green offset
	AWB_TAG_WEIGHT_WF,                        // Warm Fluorescent weight
	AWB_TAG_RG_S,                      // Shade R/G
	AWB_TAG_BG_S,                      // Shade B/G
	AWB_TAG_DAY_LOCUS_RG_S,            // Shade daylight locus R/G
	AWB_TAG_DAY_LOCUS_BG_S,            // Shade daylight locus B/G
	AWB_TAG_DAY_LOCUS_TARGET_RG_S,     // Shade daylight locus target R/G
	AWB_TAG_DAY_LOCUS_TARGET_BG_S,     // Shade daylight locus target B/G
	AWB_TAG_LOG_RG_S,                  // Shade log R/G
	AWB_TAG_LOG_BG_S,                  // Shade log B/G
	AWB_TAG_DAY_LOCUS_LOG_RG_S,        // Shade daylight locus log R/G
	AWB_TAG_DAY_LOCUS_LOG_BG_S,        // Shade daylight locus log B/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_RG_S, // Shade daylight locus target log R/G
	AWB_TAG_DAY_LOCUS_TARGET_LOG_BG_S, // Shade daylight locus target log B/G
	AWB_TAG_DAY_LOCUS_OFFSET_S,        // Shade daylight locus offset
	AWB_TAG_DAY_LOCUS_NEW_OFFSET_S,    // Shade daylight locus new offset
	AWB_TAG_WEIGHT_S,                  // Shade light weight

	// PWB
	PWB_TAG_PAXEL_NUM_0,    // PWB paxel number of light area 0
    PWB_TAG_PAXEL_NUM_1,    // PWB paxel number of light area 1
    PWB_TAG_PAXEL_NUM,      // PWB paxel number of light area 0 + light area 1
	PWB_TAG_DEFAULT_GAIN_R, // PWB default gain R
	PWB_TAG_DEFAULT_GAIN_G, // PWB default gain G
	PWB_TAG_DEFAULT_GAIN_B, // PWB default gain B
	PWB_TAG_GAIN0_R, // WB gain R of light area 0
	PWB_TAG_GAIN0_G, // WB gain G of light area 0
	PWB_TAG_GAIN0_B, // WB gain B of light area 0
	PWB_TAG_GAIN1_R, // WB gain R of light area 1
	PWB_TAG_GAIN1_G, // WB gain G of light area 1
	PWB_TAG_GAIN1_B, // WB gain B of light area 1
    PWB_TAG_LIGHT_SOURCE,    // PWB light source
	PWB_TAG_LIGHT_AREA1_XO,  // PWB light area 1 original X coordinate
	PWB_TAG_LIGHT_AREA1_YO,  // PWB light area 1 original Y coordinate
	PWB_TAG_LIGHT_AREA1_XOR, // PWB light area 1 original X coordinate (rotated)
	PWB_TAG_LIGHT_AREA1_YOR, // PWB light area 1 original Y coordinate (rotated)
	PWB_TAG_LIGHT_AREA1_XPR, // PWB light area 1 projected X coordinate (rotated)
	PWB_TAG_LIGHT_AREA1_YPR, // PWB light area 1 projected Y coordinate (rotated)
	PWB_TAG_LIGHT_AREA1_XP,  // PWB light area 1 projected X coordinate
	PWB_TAG_LIGHT_AREA1_YP,  // PWB light area 1 projected Y coordinate
	PWB_TAG_COS_INV,         // PWB Inverse rotation matrix (cos)
	PWB_TAG_SIN_INV,         // PWB Inverse rotation matrix (sin)

	// AWB Statistics Config
    AWB_TAG_STAT_CONFIG_AWBSUM_WIND, // AWB statistics config parameter: AWBSUM_WIND
    AWB_TAG_STAT_CONFIG_AWBSUM_WINU, // AWB statistics config parameter: AWBSUM_WINU
	AWB_TAG_STAT_CONFIG_AWBSUM_WINR, // AWB statistics config parameter: AWBSUM_WINR
    AWB_TAG_STAT_CONFIG_AWBSUM_WINL, // AWB statistics config parameter: AWBSUM_WINL
    AWB_TAG_STAT_CONFIG_PAXEL_YL,    // AWB statistics config parameter: PAXEL_YL
    AWB_TAG_STAT_CONFIG_PAXEL_RGBH,  // AWB statistics config parameter: PAXEL_RGBH
	AWB_TAG_STAT_CONFIG_AWBDM_DEBUG, // AWB statistics config parameter: AWBDM_DEBUG
    AWB_TAG_STAT_CONFIG_SMAREA_NO,   // AWB statistics config parameter: SMAREA_NO
    AWB_TAG_STAT_CONFIG_SMAREA_EN,   // AWB statistics config parameter: SMAREA_EN
    AWB_TAG_STAT_CONFIG_COLOREDGE_EN,// AWB statistics config parameter: COLOREDGE_EN
    AWB_TAG_STAT_CONFIG_NEUTRAL_EN,  // AWB statistics config parameter: NEUTRAL_EN
    AWB_TAG_STAT_CONFIG_AWB_EN,      // AWB statistics config parameter: AWB_EN
    AWB_TAG_STAT_CONFIG_NEUTRAL_TH,  // AWB statistics config parameter: NEUTRAL_TH
    AWB_TAG_STAT_CONFIG_CEDGEY_TH,   // AWB statistics config parameter: CEDGEY_TH
	AWB_TAG_STAT_CONFIG_CEDGEX_TH,   // AWB statistics config parameter: CEDGEX_TH
    AWB_TAG_STAT_CONFIG_AWBH12,      // AWB statistics config parameter: AWBH12
    AWB_TAG_STAT_CONFIG_AWBH11,      // AWB statistics config parameter: AWBH11
    AWB_TAG_STAT_CONFIG_AWBH22,      // AWB statistics config parameter: AWBH22
    AWB_TAG_STAT_CONFIG_AWBH21,      // AWB statistics config parameter: AWBH21
    AWB_TAG_STAT_CONFIG_AWBCE_WINR,  // AWB statistics config parameter: AWBCE_WINR
    AWB_TAG_STAT_CONFIG_AWBCE_WINL,  // AWB statistics config parameter: AWBCE_WINL
    AWB_TAG_STAT_CONFIG_AWBCE_WIND,  // AWB statistics config parameter: AWBCE_WIND
    AWB_TAG_STAT_CONFIG_AWBCE_WINU,  // AWB statistics config parameter: AWBCE_WINU
    AWB_TAG_STAT_CONFIG_AWBXY_WINR0, // AWB statistics config parameter: AWBXY_WINR0
    AWB_TAG_STAT_CONFIG_AWBXY_WINL0, // AWB statistics config parameter: AWBXY_WINL0
    AWB_TAG_STAT_CONFIG_AWBXY_WIND0, // AWB statistics config parameter: AWBXY_WIND0
    AWB_TAG_STAT_CONFIG_AWBXY_WINU0, // AWB statistics config parameter: AWBXY_WINU0
    AWB_TAG_STAT_CONFIG_AWBXY_WINR1, // AWB statistics config parameter: AWBXY_WINR1
    AWB_TAG_STAT_CONFIG_AWBXY_WINL1, // AWB statistics config parameter: AWBXY_WINL1
    AWB_TAG_STAT_CONFIG_AWBXY_WIND1, // AWB statistics config parameter: AWBXY_WIND1
    AWB_TAG_STAT_CONFIG_AWBXY_WINU1, // AWB statistics config parameter: AWBXY_WINU1
    AWB_TAG_STAT_CONFIG_AWBXY_WINR2, // AWB statistics config parameter: AWBXY_WINR2
    AWB_TAG_STAT_CONFIG_AWBXY_WINL2, // AWB statistics config parameter: AWBXY_WINL2
    AWB_TAG_STAT_CONFIG_AWBXY_WIND2, // AWB statistics config parameter: AWBXY_WIND2
    AWB_TAG_STAT_CONFIG_AWBXY_WINU2, // AWB statistics config parameter: AWBXY_WINU2
    AWB_TAG_STAT_CONFIG_AWBXY_WINR3, // AWB statistics config parameter: AWBXY_WINR3
    AWB_TAG_STAT_CONFIG_AWBXY_WINL3, // AWB statistics config parameter: AWBXY_WINL3
    AWB_TAG_STAT_CONFIG_AWBXY_WIND3, // AWB statistics config parameter: AWBXY_WIND3
    AWB_TAG_STAT_CONFIG_AWBXY_WINU3, // AWB statistics config parameter: AWBXY_WINU3
    AWB_TAG_STAT_CONFIG_AWBXY_WINR4, // AWB statistics config parameter: AWBXY_WINR4
    AWB_TAG_STAT_CONFIG_AWBXY_WINL4, // AWB statistics config parameter: AWBXY_WINL4
    AWB_TAG_STAT_CONFIG_AWBXY_WIND4, // AWB statistics config parameter: AWBXY_WIND4
    AWB_TAG_STAT_CONFIG_AWBXY_WINU4, // AWB statistics config parameter: AWBXY_WINU4
    AWB_TAG_STAT_CONFIG_AWBXY_WINR5, // AWB statistics config parameter: AWBXY_WINR5
    AWB_TAG_STAT_CONFIG_AWBXY_WINL5, // AWB statistics config parameter: AWBXY_WINL5
    AWB_TAG_STAT_CONFIG_AWBXY_WIND5, // AWB statistics config parameter: AWBXY_WIND5
    AWB_TAG_STAT_CONFIG_AWBXY_WINU5, // AWB statistics config parameter: AWBXY_WINU5
    AWB_TAG_STAT_CONFIG_AWBXY_WINR6, // AWB statistics config parameter: AWBXY_WINR6
    AWB_TAG_STAT_CONFIG_AWBXY_WINL6, // AWB statistics config parameter: AWBXY_WINL6
    AWB_TAG_STAT_CONFIG_AWBXY_WIND6, // AWB statistics config parameter: AWBXY_WIND6
    AWB_TAG_STAT_CONFIG_AWBXY_WINU6, // AWB statistics config parameter: AWBXY_WINU6
    AWB_TAG_STAT_CONFIG_AWBXY_WINR7, // AWB statistics config parameter: AWBXY_WINR7
    AWB_TAG_STAT_CONFIG_AWBXY_WINL7, // AWB statistics config parameter: AWBXY_WINL7
    AWB_TAG_STAT_CONFIG_AWBXY_WIND7, // AWB statistics config parameter: AWBXY_WIND7
    AWB_TAG_STAT_CONFIG_AWBXY_WINU7, // AWB statistics config parameter: AWBXY_WINU7
    AWB_TAG_STAT_CONFIG_AWBXY_WINR8, // AWB statistics config parameter: AWBXY_WINR8
    AWB_TAG_STAT_CONFIG_AWBXY_WINL8, // AWB statistics config parameter: AWBXY_WINL8
    AWB_TAG_STAT_CONFIG_AWBXY_WIND8, // AWB statistics config parameter: AWBXY_WIND8
    AWB_TAG_STAT_CONFIG_AWBXY_WINU8, // AWB statistics config parameter: AWBXY_WINU8
    AWB_TAG_STAT_CONFIG_AWBXY_WINR9, // AWB statistics config parameter: AWBXY_WINR9
    AWB_TAG_STAT_CONFIG_AWBXY_WINL9, // AWB statistics config parameter: AWBXY_WINL9
    AWB_TAG_STAT_CONFIG_AWBXY_WIND9, // AWB statistics config parameter: AWBXY_WIND9
    AWB_TAG_STAT_CONFIG_AWBXY_WINU9, // AWB statistics config parameter: AWBXY_WINU9
    AWB_TAG_STAT_CONFIG_AWBXY_WINRA, // AWB statistics config parameter: AWBXY_WINRA
    AWB_TAG_STAT_CONFIG_AWBXY_WINLA, // AWB statistics config parameter: AWBXY_WINLA
    AWB_TAG_STAT_CONFIG_AWBXY_WINDA, // AWB statistics config parameter: AWBXY_WINDA
    AWB_TAG_STAT_CONFIG_AWBXY_WINUA, // AWB statistics config parameter: AWBXY_WINUA
    AWB_TAG_STAT_CONFIG_AWBXY_WINRB, // AWB statistics config parameter: AWBXY_WINRB
    AWB_TAG_STAT_CONFIG_AWBXY_WINLB, // AWB statistics config parameter: AWBXY_WINLB
    AWB_TAG_STAT_CONFIG_AWBXY_WINDB, // AWB statistics config parameter: AWBXY_WINDB
    AWB_TAG_STAT_CONFIG_AWBXY_WINUB, // AWB statistics config parameter: AWBXY_WINUB

	// CCT Estimation
	AWB_TAG_CCT,               // CCT estimation
	AWB_TAG_FLUORESCENT_INDEX, // CCT estimation: fluorescent index
	AWB_TAG_DAYLIGHT_FLUORESCENT_INDEX, // CCT estimation: daylight fluorescent index
	AWB_TAG_CCT_LOG_RG,        // CCT estimation: log(R/G) (from AWB gain)
    AWB_TAG_CCT_LOG_BG,        // CCT estimation: log(B/G) (from AWB gain)
	AWB_TAG_CCT_X,             // CCT estimation: X (from AWB gain)
	AWB_TAG_CCT_Y,             // CCT estimation: Y (from AWB gain)
	AWB_TAG_CCT_XR,            // CCT estimation: XR (from AWB gain)
	AWB_TAG_CCT_YR,            // CCT estimation: XR (from AWB gain)
	AWB_TAG_CCT_CAL_LOG_RG,    // CCT estimation: log(R/G) (from AWB calibration gain)
    AWB_TAG_CCT_CAL_LOG_BG,    // CCT estimation: log(B/G) (from AWB calibration gain)
	AWB_TAG_CCT_CAL_X,         // CCT estimation: X (from AWB calibration gain)
	AWB_TAG_CCT_CAL_Y,         // CCT estimation: Y (from AWB calibration gain)
	AWB_TAG_CCT_CAL_XR,        // CCT estimation: XR (from AWB calibration gain)
	AWB_TAG_CCT_CAL_YR,        // CCT estimation: XR (from AWB calibration gain)
	AWB_TAG_CCT_CCT_XR,        // CCT estimation: CCTXR
	AWB_TAG_CCT_CCT_YR,        // CCT estimation: CCTXR
	AWB_TAG_CCT_MIRED_H,       // CCT estimation: MIRED_H
	AWB_TAG_CCT_MIRED_L,       // CCT estimation: MIRED_L
	AWB_TAG_CCT_MIRED,         // CCT estimation: MIRED
	AWB_TAG_CCT_YR_TL84,       // CCT estimation: YR_TL84 (relative rotated Y coordinate of TL84)
    AWB_TAG_CCT_YR_CWF,        // CCT estimation: YR_CWF (relative rotated Y coordinate of CWF)
    AWB_TAG_CCT_YR_MEAN_F,     // CCT estimation: YR_Mean_F = (YR_TL84 + YR_CWF) / 2
    AWB_TAG_CCT_YR_D65,        // CCT estimation: YR_D65 (relative rotated Y coordinate of D65)
    AWB_TAG_CCT_YR_DF,         // CCT estimation: YR_DF (relative rotated Y coordinate of DF)
    AWB_TAG_CCT_YR_MEAN_DF,    // CCT estimation: YR_Mean_DF = (YR_D65 + YR_DF) / 2

	// AWB Calibration Gain
	AWB_TAG_CAL_GAIN_R, // AWB calibration gain R
	AWB_TAG_CAL_GAIN_G, // AWB calibration gain G
	AWB_TAG_CAL_GAIN_B, // AWB calibration gain B

	//____NVRAM Data____

    AWB_NVRAM_CAL_GAIN_R, // AWB calibration gain R
	AWB_NVRAM_CAL_GAIN_G, // AWB calibration gain G
	AWB_NVRAM_CAL_GAIN_B, // AWB calibration gain B
    AWB_NVRAM_DEF_GAIN_R, // AWB default calibration gain R
	AWB_NVRAM_DEF_GAIN_G, // AWB default calibration gain G
	AWB_NVRAM_DEF_GAIN_B, // AWB default calibration gain B
    AWB_NVRAM_D65_GAIN_R, // AWB D65 gain R
	AWB_NVRAM_D65_GAIN_G, // AWB D65 gain G
	AWB_NVRAM_D65_GAIN_B, // AWB D65 gain B

	// Light Source Original XY Coordinate
	AWB_NVRAM_HORIZON_XO, // Horizon Xo
	AWB_NVRAM_HORIZON_YO, // Horizon Yo
	AWB_NVRAM_A_XO,       // A Xo
	AWB_NVRAM_A_YO,       // A Yo
	AWB_NVRAM_TL84_XO,    // TL84 Xo
	AWB_NVRAM_TL84_YO,    // TL84 Yo
	AWB_NVRAM_CWF_XO,     // CWF Xo
	AWB_NVRAM_CWF_YO,     // CWF Yo
	AWB_NVRAM_DNP_XO,     // DNP Xo
	AWB_NVRAM_DNP_YO,     // DNP Yo
	AWB_NVRAM_D65_XO,     // D65 Xo
	AWB_NVRAM_D65_YO,     // D65 Yo
	AWB_NVRAM_D75_XO,     // D75 Xo
	AWB_NVRAM_D75_YO,     // D75 Yo
	AWB_NVRAM_DF_XO,      // DF Xo
	AWB_NVRAM_DF_YO,      // DF Yo

	// Light Source Rotated XY Coordinate
	AWB_NVRAM_HORIZON_XR, // Horizon Xr
	AWB_NVRAM_HORIZON_YR, // Horizon Yr
	AWB_NVRAM_A_XR,       // A Xr
	AWB_NVRAM_A_YR,       // A Yr
	AWB_NVRAM_TL84_XR,    // TL84 Xr
	AWB_NVRAM_TL84_YR,    // TL84 Yr
	AWB_NVRAM_CWF_XR,     // CWF Xr
	AWB_NVRAM_CWF_YR,     // CWF Yr
	AWB_NVRAM_DNP_XR,     // DNP Xr
	AWB_NVRAM_DNP_YR,     // DNP Yr
	AWB_NVRAM_D65_XR,     // D65 Xr
	AWB_NVRAM_D65_YR,     // D65 Yr
	AWB_NVRAM_D75_XR,     // D75 Xr
	AWB_NVRAM_D75_YR,     // D75 Yr
	AWB_NVRAM_DF_XR,      // DF Xr
	AWB_NVRAM_DF_YR,      // DF Yr


	// Rotation Matrix Parameter
	AWB_NVRAM_ROTATION_ANGLE, // Rotation angle
	AWB_NVRAM_ROTATION_H11,   // Rotation matrix: H11
	AWB_NVRAM_ROTATION_H12,   // Rotation matrix: H12
	AWB_NVRAM_ROTATION_H21,   // Rotation matrix: H21
	AWB_NVRAM_ROTATION_H22,   // Rotation matrix: H22

	// Daylight Locus Parameter
	AWB_NVRAM_SLPOE_NUMERATOR,   // Daylight locus slop numerator
	AWB_NVRAM_SLPOE_DENOMINATOR, // Daylight locus slop denominator

	// AWB Light Area
	AWB_NVRAM_TUNGSTEN_RIGHT,         // AWB light area right bound: Tungsten
	AWB_NVRAM_TUNGSTEN_LEFT,          // AWB light area left bound: Tungsten
	AWB_NVRAM_TUNGSTEN_UPPER,         // AWB light area upper bound: Tungsten
	AWB_NVRAM_TUNGSTEN_LOWER,         // AWB light area lower bound: Tungsten
	AWB_NVRAM_WARM_FLUORESCENT_RIGHT, // AWB light area right bound: Warm Fluorescent
	AWB_NVRAM_WARM_FLUORESCENT_LEFT,  // AWB light area left bound: Warm Fluorescent
	AWB_NVRAM_WARM_FLUORESCENT_UPPER, // AWB light area upper bound: Warm Fluorescent
	AWB_NVRAM_WARM_FLUORESCENT_LOWER, // AWB light area lower bound: Warm Fluorescent
	AWB_NVRAM_FLUORESCENT_RIGHT,      // AWB light area right bound: Fluorescent
	AWB_NVRAM_FLUORESCENT_LEFT,       // AWB light area left bound: Fluorescent
	AWB_NVRAM_FLUORESCENT_UPPER,      // AWB light area upper bound: Fluorescent
	AWB_NVRAM_FLUORESCENT_LOWER,      // AWB light area lower bound: Fluorescent
	AWB_NVRAM_CWF_RIGHT,              // AWB light area right bound: CWF
	AWB_NVRAM_CWF_LEFT,               // AWB light area left bound: CWF
	AWB_NVRAM_CWF_UPPER,              // AWB light area upper bound: CWF
	AWB_NVRAM_CWF_LOWER,              // AWB light area lower bound: CWF
	AWB_NVRAM_DAYLIGHT_RIGHT,         // AWB light area right bound: Daylight
	AWB_NVRAM_DAYLIGHT_LEFT,          // AWB light area left bound: Daylight
	AWB_NVRAM_DAYLIGHT_UPPER,         // AWB light area upper bound: Daylight
	AWB_NVRAM_DAYLIGHT_LOWER,         // AWB light area lower bound: Daylight
	AWB_NVRAM_SHADE_RIGHT,            // AWB light area right bound: Shade
	AWB_NVRAM_SHADE_LEFT,             // AWB light area left bound: Shade
	AWB_NVRAM_SHADE_UPPER,            // AWB light area upper bound: Shade
	AWB_NVRAM_SHADE_LOWER,            // AWB light area lower bound: Shade
	AWB_NVRAM_DAYLIGHT_FLUORESCENT_RIGHT, // AWB light area right bound: Daylight Fluorescent
	AWB_NVRAM_DAYLIGHT_FLUORESCENT_LEFT,  // AWB light area left bound: Daylight Fluorescent
	AWB_NVRAM_DAYLIGHT_FLUORESCENT_UPPER, // AWB light area upper bound: Daylight Fluorescent
	AWB_NVRAM_DAYLIGHT_FLUORESCENT_LOWER, // AWB light area lower bound: Daylight Fluorescent

	// PWB Light Area
	PWB_NVRAM_REFERENCE_RIGHT,         // PWB light area right bound: Reference Area
	PWB_NVRAM_REFERENCE_LEFT,          // PWB light area left bound: Reference Area
	PWB_NVRAM_REFERENCE_UPPER,         // PWB light area upper bound: Reference Area
	PWB_NVRAM_REFERENCE_LOWER,         // PWB light area lower bound: Reference Area
	PWB_NVRAM_DAYLIGHT_RIGHT,          // PWB light area right bound: Daylight
	PWB_NVRAM_DAYLIGHT_LEFT,           // PWB light area left bound: Daylight
	PWB_NVRAM_DAYLIGHT_UPPER,          // PWB light area upper bound: Daylight
	PWB_NVRAM_DAYLIGHT_LOWER,          // PWB light area lower bound: Daylight
	PWB_NVRAM_CLOUDY_DAYLIGHT_RIGHT,   // PWB light area right bound: Cloudy Daylight
	PWB_NVRAM_CLOUDY_DAYLIGHT_LEFT,    // PWB light area left bound: Cloudy Daylight
	PWB_NVRAM_CLOUDY_DAYLIGHT_UPPER,   // PWB light area upper bound: Cloudy Daylight
	PWB_NVRAM_CLOUDY_DAYLIGHT_LOWER,   // PWB light area lower bound: Cloudy Daylight
	PWB_NVRAM_SHADE_RIGHT,             // PWB light area right bound: Shade
	PWB_NVRAM_SHADE_LEFT,              // PWB light area left bound: Shade
	PWB_NVRAM_SHADE_UPPER,             // PWB light area upper bound: Shade
	PWB_NVRAM_SHADE_LOWER,             // PWB light area lower bound: Shade
	PWB_NVRAM_TWILIGHT_RIGHT,          // PWB light area right bound: Twilight
	PWB_NVRAM_TWILIGHT_LEFT,           // PWB light area left bound: Twilight
	PWB_NVRAM_TWILIGHT_UPPER,          // PWB light area upper bound: Twilight
	PWB_NVRAM_TWILIGHT_LOWER,          // PWB light area lower bound: Twilight
	PWB_NVRAM_FLUORESCENT_RIGHT,       // PWB light area right bound: Fluorescent
	PWB_NVRAM_FLUORESCENT_LEFT,        // PWB light area left bound: Fluorescent
	PWB_NVRAM_FLUORESCENT_UPPER,       // PWB light area upper bound: Fluorescent
	PWB_NVRAM_FLUORESCENT_LOWER,       // PWB light area lower bound: Fluorescent
	PWB_NVRAM_WARM_FLUORESCENT_RIGHT,  // PWB light area right bound: Warm Fluorescent
	PWB_NVRAM_WARM_FLUORESCENT_LEFT,   // PWB light area left bound: Warm Fluorescent
	PWB_NVRAM_WARM_FLUORESCENT_UPPER,  // PWB light area upper bound: Warm Fluorescent
	PWB_NVRAM_WARM_FLUORESCENT_LOWER,  // PWB light area lower bound: Warm Fluorescent
	PWB_NVRAM_WARM_INCANDESCENT_RIGHT, // PWB light area right bound: Incandescent
	PWB_NVRAM_WARM_INCANDESCENT_LEFT,  // PWB light area left bound: Incandescent
	PWB_NVRAM_WARM_INCANDESCENT_UPPER, // PWB light area upper bound: Incandescent
	PWB_NVRAM_WARM_INCANDESCENT_LOWER, // PWB light area lower bound: Incandescent

	// PWB Default Gain
    PWB_NVRAM_DEF_GAIN_R_DAYLIGHT,         // PWB default gain R: Daylight
	PWB_NVRAM_DEF_GAIN_G_DAYLIGHT,         // PWB default gain G: Daylight
	PWB_NVRAM_DEF_GAIN_B_DAYLIGHT,         // PWB default gain B: Daylight
    PWB_NVRAM_DEF_GAIN_R_CLOUDY_DAYLIGHT,  // PWB default gain R: Cloudy Daylight
	PWB_NVRAM_DEF_GAIN_G_CLOUDY_DAYLIGHT,  // PWB default gain G: Cloudy Daylight
	PWB_NVRAM_DEF_GAIN_B_CLOUDY_DAYLIGHT,  // PWB default gain B: Cloudy Daylight
    PWB_NVRAM_DEF_GAIN_R_SHADE,            // PWB default gain R: Shade
	PWB_NVRAM_DEF_GAIN_G_SHADE,            // PWB default gain G: Shade
	PWB_NVRAM_DEF_GAIN_B_SHADE,            // PWB default gain B: Shade
    PWB_NVRAM_DEF_GAIN_R_TWILIGHT,         // PWB default gain R: Twilight
	PWB_NVRAM_DEF_GAIN_G_TWILIGHT,         // PWB default gain G: Twilight
	PWB_NVRAM_DEF_GAIN_B_TWILIGHT,         // PWB default gain B: Twilight
    PWB_NVRAM_DEF_GAIN_R_FLUORESCENT,      // PWB default gain R: Fluorescent
	PWB_NVRAM_DEF_GAIN_G_FLUORESCENT,      // PWB default gain G: Fluorescent
	PWB_NVRAM_DEF_GAIN_B_FLUORESCENT,      // PWB default gain B: Fluorescent
    PWB_NVRAM_DEF_GAIN_R_WARM_FLUORESCENT, // PWB default gain R: Warm Fluorescent
	PWB_NVRAM_DEF_GAIN_G_WARM_FLUORESCENT, // PWB default gain G: Warm Fluorescent
	PWB_NVRAM_DEF_GAIN_B_WARM_FLUORESCENT, // PWB default gain B: Warm Fluorescent
    PWB_NVRAM_DEF_GAIN_R_INCANDESCENT,     // PWB default gain R: Incandescent
	PWB_NVRAM_DEF_GAIN_G_INCANDESCENT,     // PWB default gain G: Incandescent
	PWB_NVRAM_DEF_GAIN_B_INCANDESCENT,     // PWB default gain B: Incandescent

	// AWB Preference Color Parameter
    AWB_NVRAM_PREFERENCE_COLOR_SLIDER_TUNGSTEN,             // AWB preference color slider value: Tungsten
	AWB_NVRAM_PREFERENCE_COLOR_OFFSET_THR_TUNGSTEN,         // AWB preference color offset threshold: Tungsten
    AWB_NVRAM_PREFERENCE_COLOR_SLIDER_WARM_FLUORESCENT,     // AWB preference color slider value: Warm Fluorescent
	AWB_NVRAM_PREFERENCE_COLOR_OFFSET_THR_WARM_FLUORESCENT, // AWB preference color offset threshold: Warm Fluorescent
    AWB_NVRAM_PREFERENCE_COLOR_SLIDER_SHADE,                // AWB preference color slider value: Shade
	AWB_NVRAM_PREFERENCE_COLOR_OFFSET_THR_SHADE,            // AWB preference color offset threshold: Shade
    AWB_NVRAM_PREFERENCE_COLOR_DAYLIGHT_GAIN_R,             // AWB preference color daylight gain R
	AWB_NVRAM_PREFERENCE_COLOR_DAYLIGHT_GAIN_G,             // AWB preference color daylight gain G
	AWB_NVRAM_PREFERENCE_COLOR_DAYLIGHT_GAIN_B,             // AWB preference color daylight gain B
    AWB_NVRAM_PREFERENCE_GAIN_TUNGSTEN_R,                   // AWB preference color tungsten gain R
	AWB_NVRAM_PREFERENCE_GAIN_TUNGSTEN_G,                   // AWB preference color tungsten gain G
	AWB_NVRAM_PREFERENCE_GAIN_TUNGSTEN_B,                   // AWB preference color tungsten gain B
    AWB_NVRAM_PREFERENCE_GAIN_WARM_FLUORESCENT_R,           // AWB preference color warm fluorescent gain R
	AWB_NVRAM_PREFERENCE_GAIN_WARM_FLUORESCENT_G,           // AWB preference color warm fluorescent gain G
	AWB_NVRAM_PREFERENCE_GAIN_WARM_FLUORESCENT_B,           // AWB preference color warm fluorescent gain B
    AWB_NVRAM_PREFERENCE_GAIN_FLUORESCENT_R,                // AWB preference color fluorescent gain R
	AWB_NVRAM_PREFERENCE_GAIN_FLUORESCENT_G,                // AWB preference color fluorescent gain G
	AWB_NVRAM_PREFERENCE_GAIN_FLUORESCENT_B,                // AWB preference color fluorescent gain B
    AWB_NVRAM_PREFERENCE_GAIN_CWF_R,                        // AWB preference color CWF gain R
	AWB_NVRAM_PREFERENCE_GAIN_CWF_G,                        // AWB preference color CWF gain G
	AWB_NVRAM_PREFERENCE_GAIN_CWF_B,                        // AWB preference color CWF gain B
    AWB_NVRAM_PREFERENCE_GAIN_DAYLIGHT_R,                   // AWB preference color daylight gain R
	AWB_NVRAM_PREFERENCE_GAIN_DAYLIGHT_G,                   // AWB preference color daylight gain G
	AWB_NVRAM_PREFERENCE_GAIN_DAYLIGHT_B,                   // AWB preference color daylight gain B
    AWB_NVRAM_PREFERENCE_GAIN_SHADE_R,                      // AWB preference color shade gain R
	AWB_NVRAM_PREFERENCE_GAIN_SHADE_G,                      // AWB preference color shade gain G
	AWB_NVRAM_PREFERENCE_GAIN_SHADE_B,                      // AWB preference color shade gain B
    AWB_NVRAM_PREFERENCE_GAIN_DAYLIGHT_FLUORESCENT_R,       // AWB preference color daylight fluorescent gain R
	AWB_NVRAM_PREFERENCE_GAIN_DAYLIGHT_FLUORESCENT_G,       // AWB preference color daylight fluorescent gain G
	AWB_NVRAM_PREFERENCE_GAIN_DAYLIGHT_FLUORESCENT_B,       // AWB preference color daylight fluorescent gain B

	// CCT Estimation
	AWB_NVRAM_CCT_HORIZON, // CCT: Horizon
	AWB_NVRAM_CCT_A,       // CCT: A
	AWB_NVRAM_CCT_TL84,    // CCT: TL84
	AWB_NVRAM_CCT_DNP,     // CCT: DNP
	AWB_NVRAM_CCT_D65,     // CCT: D65
	AWB_NVRAM_CCT_D75,     // CCT: D75
	AWB_NVRAM_XR_HORIZON,  // Xr: Horizon
	AWB_NVRAM_XR_A,        // Xr: A
	AWB_NVRAM_XR_TL84,     // Xr: TL84
	AWB_NVRAM_XR_DNP,      // Xr: DNP
	AWB_NVRAM_XR_D65,      // Xr: D65
	AWB_NVRAM_XR_D75,      // Xr: D75

	// AWB Statistics Data
	AWB_TAG_STAT_PCNT0, // paxel count of window 0
	AWB_TAG_STAT_RSUM0, // R sum of window 0
	AWB_TAG_STAT_GSUM0, // R sum of window 0
	AWB_TAG_STAT_BSUM0, // R sum of window 0
	AWB_TAG_STAT_PCNT1, // paxel count of window 1
	AWB_TAG_STAT_RSUM1, // R sum of window 1
	AWB_TAG_STAT_GSUM1, // R sum of window 1
	AWB_TAG_STAT_BSUM1, // R sum of window 1
	AWB_TAG_STAT_PCNT2, // paxel count of window 2
	AWB_TAG_STAT_RSUM2, // R sum of window 2
	AWB_TAG_STAT_GSUM2, // R sum of window 2
	AWB_TAG_STAT_BSUM2, // R sum of window 2
	AWB_TAG_STAT_PCNT3, // paxel count of window 3
	AWB_TAG_STAT_RSUM3, // R sum of window 3
	AWB_TAG_STAT_GSUM3, // R sum of window 3
	AWB_TAG_STAT_BSUM3, // R sum of window 3
	AWB_TAG_STAT_PCNT4, // paxel count of window 4
	AWB_TAG_STAT_RSUM4, // R sum of window 4
	AWB_TAG_STAT_GSUM4, // R sum of window 4
	AWB_TAG_STAT_BSUM4, // R sum of window 4
	AWB_TAG_STAT_PCNT5, // paxel count of window 5
	AWB_TAG_STAT_RSUM5, // R sum of window 5
	AWB_TAG_STAT_GSUM5, // R sum of window 5
	AWB_TAG_STAT_BSUM5, // R sum of window 5
	AWB_TAG_STAT_PCNT6, // paxel count of window 6
	AWB_TAG_STAT_RSUM6, // R sum of window 6
	AWB_TAG_STAT_GSUM6, // R sum of window 6
	AWB_TAG_STAT_BSUM6, // R sum of window 6

	AWB_TAG_NUM
} AWB_DEBUG_TAG_T;

#define AWB_DEBUG_TAG_VERSION (2)
#define AWB_DEBUG_TAG_SIZE (AWB_TAG_NUM + 10)

typedef struct
{
    AAA_DEBUG_TAG_T Tag[AWB_DEBUG_TAG_SIZE];
} AWB_DEBUG_INFO_T;

#endif

