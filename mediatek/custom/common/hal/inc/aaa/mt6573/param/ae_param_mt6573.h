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

#ifndef _AE_PARAM_MT6573_H
#define _AE_PARAM_MT6573_H

#define AE_BLOCK_NO  5
#define HYS_ZONE   0X01    //hysteresis zone

#define AE_STABLE_THRES 3   //0.3 ev

// AE debug info
#define AE_DEBUG_TAG_SIZE 400  //follow MTK3ACommon define

typedef struct
{
    AAA_DEBUG_TAG_T Tag[AE_DEBUG_TAG_SIZE];
} AE_DEBUG_INFO_T;


//AE Parameter Structure
typedef enum
{
    AETABLE_RPEVIEW_AUTO = 0,     // default 60Hz
//    AETABLE_RPEVIEW_AUTO_50HZ = 1,
//    AETABLE_RPEVIEW_AUTO_HYSTERESIS,
//    AETABLE_RPEVIEW_AUTO_HYSTERESIS_50HZ,

    AETABLE_CAPTURE_AUTO,
//    AETABLE_CAPTURE_AUTO_50HZ,

    AETABLE_CAPTURE_ISO50,
    AETABLE_CAPTURE_ISO60,
    AETABLE_CAPTURE_ISO64,
    AETABLE_CAPTURE_ISO80,

    AETABLE_CAPTURE_ISO100,
//    AETABLE_CAPTURE_ISO100_50HZ,
    AETABLE_CAPTURE_ISO200,
//    AETABLE_CAPTURE_ISO200_50HZ,
    AETABLE_CAPTURE_ISO400,
//    AETABLE_CAPTURE_ISO400_50HZ,
    AETABLE_CAPTURE_ISO800,
//    AETABLE_CAPTURE_ISO800_50HZ,
    AETABLE_CAPTURE_ISO1600,
//    AETABLE_CAPTURE_ISO1600_50HZ,
    AETABLE_CAPTURE_ISO3200,
//    AETABLE_CAPTURE_ISO3200_50HZ,

    AETABLE_VIDEO,
//    AETABLE_VIDEO_50HZ,
//    AETABLE_VIDEO_30FPS,
//    AETABLE_VIDEO_50HZ_30FPS,
    AETABLE_VIDEO_NIGHT,
//    AETABLE_VIDEO_50HZ_15FPS,
//    AETABLE_CAPTURE_NIGHTMODE,     // default 60Hz
//    AETABLE_CAPTURE_NIGHTMODE_50HZ,
    AETABLE_MODE_INDEX1,                           // for mode used of capture
//    AETABLE_MODE_INDEX1_50HZ,						   // for mode used of capture
    AETABLE_MODE_INDEX2,
//    AETABLE_MODE_INDEX2_50HZ,
    AETABLE_MODE_INDEX3,
//    AETABLE_MODE_INDEX3_50HZ,
    AETABLE_MODE_INDEX4,
//    AETABLE_MODE_INDEX4_50HZ,
    AETABLE_MODE_INDEX5,
//    AETABLE_MODE_INDEX5_50HZ,
    AETABLE_MODE_INDEX6,
//    AETABLE_MODE_INDEX6_50HZ,
    AETABLE_MODE_INDEX7,
//    AETABLE_MODE_INDEX7_50HZ,
    AETABLE_INDEX1,
    AETABLE_INDEX2,
    AETABLE_INDEX3
}eAETableID;

typedef enum
{
    AE_WEIGHTING_CENTRALWEIGHT=0,
    AE_WEIGHTING_SPOT,
    AE_WEIGHTING_AVERAGE
}eWeightingID;

typedef struct
{
   eWeightingID eID;  //weighting table ID
   MUINT32 W[5][5];    //AE weighting table
}strWeightTable;

typedef struct
{
    MBOOL   bEnableSaturationCheck;		//if toward high saturation scene , then reduce AE target
    MBOOL   bEnableFaceAE;
    MBOOL   bFaceAEAvoidOverExp;
    MUINT32 u4BackLightStrength;         // strength of backlight condtion
    MUINT32 u4OverExpStrength;           // strength of anti over exposure
    MUINT32 u4HistStretchStrength;       //strength of  histogram stretch
    MUINT32 u4SmoothLevel;               // time LPF smooth level , internal use
    MUINT32 u4TimeLPFLevel;              //time LOW pass filter level
    MUINT32 u4BlockNumX;                 //AE X block number;
    MUINT32 u4BlockNumY;                 //AE Yblock number;
    MUINT32 u4SatBlockCheckLow;          //saturation block check , low thres
    MUINT32 u4SatBlockCheckHigh;         //sturation  block check , hight thres
    MUINT32 u4SatBlockAdjustFactor;      // adjust factore , to adjust central weighting target value
                                        // 20 --> max reduce 20%
    MUINT32 u4FaceYLowBound;
    MUINT32 u4FaceYHighBound;
}strAEParamCFG;

typedef struct 
{
    MINT32 Diff_EV;     //  delta EVx10 ,different between Yavg and Ytarget     Diff_EV=    log(  Yarg/Ytarget,2)
    MINT32  Ration;        //  Yarg/Ytarget  *100
    MINT32  move_index;   // move index
}strAEMOVE;

#if 0
//strAETable AE table Setting
typedef struct
{
    MBOOL   bEnableBlackLight;           //enable back light detector
    MBOOL   bEnableHistStretch;          //enable histogram stretch
    MBOOL   bEnableAntiOverExposure;     //enable anti over exposure
    MBOOL   bEnableTimeLPF;              //enable time domain LPF

    MBOOL   bEnableSaturationCheck;      //if toward high saturation scene , then reduce AE target
    MBOOL   bEnableFaceAE;

    MUINT32 u4CWTarget;                  //central weighting target


    MUINT32 u4CentralHighBound;          //central block high boundary
    MUINT32 u4CentralLowBound;           // central block low bounary
    MUINT32 u4BackLightStrength;         // strength of backlight condtion
    MUINT32 u4OverExpStrength;           // strength of anti over exposure
    MUINT32 u4HistStretchStrength;       //strength of  histogram stretch
    MUINT32 u4ConvergeSpeedBright;       // AE converge speed bright part
    MUINT32 u4ConvergeSpeedDark;         // AE converge speed dark part
    MUINT32 u4SmoothLevel;               // time LPF smooth level , internal use
    MUINT32 u4TimeLPFLevel;              //time LOW pass filter level


    MUINT32 u4BlockNumX;                 //AE X block number;
    MUINT32 u4BlockNumY;                 //AE Yblock number;

                                        //definition of low light index
                                        // use in backlight check if too dark then disable back light check
    MUINT32 u4LowLightIndex;

    ///////////////////////////
    //  reduce CW target if toward high saturation scene
    //  reduce if high saturation block more then low threshold, then start reduce
    //  it will clip in high threshold
    //  maximum reduce in define in "u4SatBlockAdjustFactor"
    /////////////////////////////

    MUINT32 u4SatBlockCheckLow;          //saturation block check , low thres
    MUINT32 u4SatBlockCheckHigh;         //sturation  block check , hight thres
    MUINT32 u4SatBlockAdjustFactor;      // adjust factore , to adjust central weighting target value
                                        // 20 --> max reduce 20%
    MUINT32 u4InitIndex;                 //AE initiail index

    MUINT32 u4BackLightWeight;
    MUINT32 u4HistStretchWeight;
    MUINT32 u4AntiOverExpWeight;
    MUINT32 u4FaceYLowBound;
    MUINT32 u4FaceYHighBound;
    MBOOL   bFaceAEAvoidOverExp;
}strAEConfig;//histogram control information

//histogram control information
typedef struct
{
   //histogram info
    MUINT32 u4HistHighThres;           // definition of histogram high threshold
    MUINT32 u4HistLowThres;            // definition of histogram low threshold
    MUINT32 u4OverExpThres;            //!<: over exposure threshold
    MUINT32 u4HistStretchThres;      //!<: histogram stretch trheshold
    MUINT32 u4BlackLightThres;       //!<: backlight threshold

    MUINT32 u4CentralHistMinX;      //hardware model configuration
    MUINT32 u4CentralHistMinY;      //hardware model configuration

    MUINT32 u4CentralHistMaxX;      //hardware model configuration
    MUINT32 u4CentralHistMaxY;      //hardware model configuration
}strHistConfig;//histogram control information

typedef struct
{
    MUINT32 PreviewISO;
    MUINT32 PreviewGain;     //UINT 128
    MUINT32 CaptureISO;
    MUINT32 CaptureGain;     //UINT 128
    MUINT32 ApertureDelay;
    MINT32  MShutterOffset;  // ahead time of M shutter
    MUINT32 OBgain;          // UINT 512;
    MUINT8  CaptureMaxFrame;
    MUINT8  PreCapRatio;     //8 base
    MUINT32  PreviewMiniGain;
    MUINT32  CaptureMiniGain;
}strDeviceInfo;
#endif
typedef struct	strEvSetting
{
    MUINT32 u4Eposuretime;   //!<: Exposure time in ms
    MUINT32 u4AfeGain;       //!<: raw gain
    MUINT32 u4IspGain;       //!<: sensor gain
    MUINT8  uIris;           //!<: Iris
    MUINT8  uSensorMode;     //!<: sensor mode
    MUINT8  uFlag;           //!<: flag to indicate hysteresis ...
//    MUINT8  uLV;                        //!<: LV avlue , in ISO 100 condition  LV=TV+AV
}strEvSetting;

typedef struct
{
   eAETableID   eID;
   MUINT32       u4TotalIndex;      //preview table Tatal index
//   MUINT32       u4InitialIndex;    // initial state AE index , only use in preview
   MINT32        i4StrobeTrigerBV;  // Strobe triger point in strobe auto mode
   MINT32        i4MaxBV;
   MINT32        i4MinBV;
   LIB3A_AE_ISO_SPEED_T   ISOSpeed;
   strEvSetting *pTable60Hz;   //point to 50Hz table
   strEvSetting *pTable50Hz;   //point to 60Hz table
   strEvSetting *pCurrentTable;   //point to current table
}strAETable;

typedef struct
{
   MBOOL bAFPlineEnable;
   MINT16 i2FrameRate[5][2];
}strAFPlineInfo;

typedef struct
{
	LIB3A_AE_MODE_T eAEMode;
	eAETableID ePLineID;
} strAEPLineMapping;

#define MAX_PLINE_TABLE 40
typedef struct
{
    strAETable     *PlineTable[MAX_PLINE_TABLE];    //AE Pline Table
}strAEPLineTable;

typedef struct
{
    MINT16 i2GainThr;
    MINT16 i2LowLightMode;
    MINT16 i2Saturated_percentage;                // 10 base
    MINT16 i2Saturated_percentage_Level2;   // 10 base
    MINT16 i2Saturated_percentage_Level3;   // 10 base
    MINT16 i2Percentage_thr1;              // 10 base
    MINT16 i2Percentage_thr2;              // 10 base
    MINT16 i2Percentage_thr3;              // 10 base
    MINT16 i2Must_enhance_highlight;         
    MINT16 i2GAIN_LEVEL1;
    MINT16 i2GAIN_LEVEL2;
    MINT16 i2GAIN_LEVEL3;
    MINT16 i2MAX_EV_GAIN_LEVEL1;
    MINT16 i2MAX_EV_GAIN_LEVEL2;
    MINT16 i2MAX_EV_GAIN_LEVEL3;
    MINT16 i2MAX_EV_GAIN_LEVEL4;    
    MINT16 i2Lowlight_histogram_shift;
}strAEHDRTable;                 

typedef struct
{
    MINT8 iLEVEL1_GAIN;
    MINT8 iLEVEL2_GAIN;
    MINT8 iLEVEL3_GAIN;   
    MINT8 iLEVEL4_GAIN;   
    MINT8 iLEVEL5_GAIN;   
    MINT8 iLEVEL6_GAIN;    
    MINT8 iLEVEL1_TARGET_DIFFERENCE;         
    MINT8 iLEVEL2_TARGET_DIFFERENCE;
    MINT8 iLEVEL3_TARGET_DIFFERENCE;
    MINT8 iLEVEL4_TARGET_DIFFERENCE;
    MINT8 iLEVEL5_TARGET_DIFFERENCE;
    MINT8 iLEVEL6_TARGET_DIFFERENCE;
    MINT8 iLEVEL1_GAINH;
    MINT8 iLEVEL1_GAINL;    
    MINT8 iLEVEL2_GAINH;
    MINT8 iLEVEL2_GAINL;    
    MINT8 iLEVEL3_GAINH;
    MINT8 iLEVEL3_GAINL;    
    MINT8 iLEVEL4_GAINH;
    MINT8 iLEVEL4_GAINL;    
    MINT8 iLEVEL5_GAINH;
    MINT8 iLEVEL5_GAINL;    
    MINT8 iLEVEL6_GAINH;
    MINT8 iLEVEL6_GAINL;    
    MINT8 iGAIN_DIFFERENCE_LIMITER;
}strAELimiterTable;                 

//////////////////////////////////////////
//
//  AE Parameter structure
//  Define AE algorithm initialize parameter here
//
////////////////////////////////////////

#define MAX_WEIGHT_TABLE 4
#define MAX_MAPPING_PLINE_TABLE 30

struct AE_PARAMETER
{
//    strAEConfig    AeConfig;         //all ae donfiguration
//    strHistConfig  HistConfig;     //Ae Histogram control
//    strDeviceInfo  DeviceInfo;
    strAEParamCFG strAEParasetting;
    strAEPLineMapping   *pAEModePLineMapping;   // Get PLine ID for different AE mode
    strAEPLineTable AEPlineTable;
    strWeightTable *pWeighting[MAX_WEIGHT_TABLE];   //AE WEIGHTING TABLE
    strAFPlineInfo strAFPLine;
    MUINT32 *pEVValueArray;
    strAEMOVE *pAEMovingTable;
    strAEHDRTable strAEHDRInfo;
    strAELimiterTable strAELimiterData;
};

typedef struct AE_PARAMETER struct_AE_Para;

/***********************
    Exposure time value , use in AE TV mode
***********************/
typedef enum
{
    TV_1_2      =0x00000002,    //!<: TV= 1/2 sec
    TV_1_3      =0x00000003,    //!<: TV= 1/3 sec
    TV_1_4      =0x00000004,    //!<: TV= 1/4 sec
    TV_1_5      =0x00000005,    //!<: TV= 1/5 sec
    TV_1_6      =0x00000006,    //!<: TV= 1/6 sec
    TV_1_7      =0x00000007,    //!<: TV= 1/7 sec
    TV_1_8      =0x00000008,    //!<: TV= 1/8 sec
    TV_1_10     =0x0000000A,    //!<: TV= 1/10 sec
    TV_1_13     =0x0000000D,    //!<: TV= 1/13 sec
    TV_1_15     =0x0000000F,    //!<: TV= 1/15 sec
    TV_1_20     =0x00000014,    //!<: TV= 1/20 sec
    TV_1_25     =0x00000019,    //!<: TV= 1/25 sec
    TV_1_30     =0x0000001E,    //!<: TV= 1/30 sec
    TV_1_40     =0x00000028,    //!<: TV= 1/40 sec
    TV_1_50     =0x00000032,    //!<: TV= 1/50 sec
    TV_1_60     =0x0000003C,    //!<: TV= 1/60 sec
    TV_1_80     =0x00000050,    //!<: TV= 1/80 sec
    TV_1_100    =0x00000064,    //!<: TV= 1/100 sec
    TV_1_125    =0x0000007D,    //!<: TV= 1/125 sec
    TV_1_160    =0x000000A0,    //!<: TV= 1/160  sec
    TV_1_200    =0x000000C8,    //!<: TV= 1/200 sec
    TV_1_250    =0x000000FA,    //!<: TV= 1/250 sec
    TV_1_320    =0x00000140,    //!<: TV= 1/320 sec
    TV_1_400    =0x00000190,    //!<: TV= 1/400 sec
    TV_1_500    =0x000001F4,    //!<: TV= 1/500 sec
    TV_1_640    =0x00000280,    //!<: TV= 1/640 sec
    TV_1_800    =0x00000320,    //!<: TV= 1/800 sec
    TV_1_1000   =0x000003E8,    //!<: TV= 1/1000 sec
    TV_1_1250   =0x000004E2,    //!<: TV= 1/1250 sec
    TV_1_1600   =0x00000640,    //!<: TV= 1/1600 sec

    TV_1_1      =0xFFFF0001,    //!<: TV= 1sec
    TV_2_1      =0xFFFF0002,    //!<: TV= 2sec
    TV_3_1      =0xFFFF0003,    //!<: TV= 3sec
    TV_4_1      =0xFFFF0004,    //!<: TV= 4sec
    TV_8_1      =0xFFFF0008,    //!<: TV= 8sec
    TV_16_1     =0xFFFF0016    //!<: TV= 16 sec
}eTimeValue;

/***********************
    Apertur time value , use in AE AV mode
    It's impossible list all Fno in enum
    So choose most close Fno.in enum
    and set real value in  structure "strAV.AvValue"
***********************/
typedef enum
{
    Fno_2,       //!<: Fno 2.0
    Fno_2_3,     //!<: Fno  2.3
    Fno_2_8,     //!<: Fno 2.8
    Fno_3_2,     //!<: Fno 3.2
    Fno_3_5,     //!<: Fno 3.5
    Fno_4_0,     //!<: Fno 4.0
    Fno_5_0,     //!<: Fno 5.0
    Fno_5_6,     //!<: Fno 5.6
    Fno_6_2,     //!<: Fno 6.2
    Fno_8_0,     //!<: Fno 8.0

    Fno_MAx
}eApetureValue ;


// AE Input/Output Structure
typedef enum
{
    AE_STATE_CREATE,         // CREATE , JUST CREATE
    AE_STATE_INIT,           //  INIT
    AE_STATE_NORMAL_PREVIEW, // normal AE
    AE_STATE_AFASSIST,       // aF assist mode, limit exposuret time
    AE_STATE_AELOCK,         // LOCK ae
    AE_STATE_CAPTURE,        //capture
    AE_STATE_CAPTURE_nEV,        //bracket AE use
    AE_STATE_CAPTURE_pEV,        //bracket AE use

    AE_STATE_MAX
}eAESTATE;


typedef struct
{
    eAESTATE  eAeState;   //ae state
    MUINT32    u4BlockCnt;                  //AE block count number in each window;
    void*     pAESatisticBuffer;
    void*     pAWBSatisticBuffer;
    void*     pAEHistogramBuffer;
    void*     pFlareHistogramBuffer;
    void*     pFaceAESatisticBuffer;
} strAEInput;

typedef struct
{
    MUINT8    uFlareRGain;   //128 base
    MUINT8    uFlareGGain;
    MUINT8    uFlareBGain;
    MUINT8    uFlare_B:7;         //[6:0] Flare B Offset
    MUINT8    bFlare_SIGN_B:1;    // [7] Sign B
    MUINT8    uFlare_G:7;         //[14:8] Flare G Offset
    MUINT8    bFlare_SIGN_G:1;    //[15] Sign G
    MUINT8    uFlare_R:7;         // [22:16] Flare R Offset
    MUINT8    bFlare_SIGN_R:1;    // [23] Sign R
} strFlareCFG;

typedef struct
{
    strEvSetting  EvSetting;
    MINT32         Bv;
    MUINT32        u4AECondition;
    MINT32         i4DeltaBV;
    MUINT32        u4ISO;          //correspoing ISO , only use in capture
    MUINT8         uFrameRate;     // Calculate the frame
    MBOOL          bAEStable;      // Only used in Preview/Movie
    MUINT32        u4StrobeWidth; //strobe trigger pulse width.
    MUINT32        u4FlashPosRatio; //
} strAEOutput;

//Flash Parameter Structure
typedef enum {
    RET_PF_NEXT,          //check return parameters and set EV or strobe settings.
    RET_PF_FINISH,        //preflash metering process is done and ready to enter still capture mode.

    RET_PF_STATE_MAX
}ePreFlashOState;

typedef enum {
    PF_FRM_VALID,
    PF_FRM_INVALID
}ePreFlashValidFrmState;

typedef struct{
	strEvSetting NFPrv;
	strEvSetting WFPrv[2];
	strEvSetting WFStill;
	MUINT32 u4RawLevel[4];
	MUINT32 u4PreFlashTime[2];
	MUINT32 u4MainFlashTime;
}strPreFlashParam;

typedef struct
{
    MUINT32 u4Time;
    MUINT32 u4Gain;
    MUINT32 u4GOverRGain;
    MUINT32 u4GOverBGain;
}strStrobePTbl;

typedef struct
{
    MUINT32 u4FlashOverEnvRatio;
    MUINT32 u4FlashCtRGain;
    MUINT32 u4FlashCtBGain;
    MUINT32 u4Base;
}strFlashAWBInfo;

struct AEPF_PARAMETER
{
    MUINT32 (*pPreFlashCbf)(MUINT32*);
    strStrobePTbl *pstrStoPTbl;
    MUINT32 *pu4StoWM;
    MUINT32 *pu4FNoTbl;
    MUINT32 u4NumOfAper;
};

typedef struct AEPF_PARAMETER struct_AEPF_Para;

struct AEPF_EXTRAINFO
{
    struct_AEPF_Para strAEPFPara;
    MUINT32 u4MixingRatio;
};
typedef struct AEPF_EXTRAINFO strPreFlashExtraInfo;


typedef struct
{
    struct_AEPF_Para strrAEPFPara;
}AEPF_PARAM_T;

typedef struct
{
    strPreFlashExtraInfo strAEPFExInfo;
}AEPF_EXTRAINFO_T;




typedef struct
{
   MUINT32 u4HighY;
   MUINT32 u4LowY;
   MUINT32 u4Maxbin;
   MUINT32 u4Brightest;
   MUINT32 u4Darkest;
   MUINT32 u4BrightHalf;
   MUINT32 u4DarkHalf;
   MUINT32 u4DownSideBrightest;
   MUINT32 u4FullBrightest;
}strHistInfo;//histogram information

typedef struct
{
    MUINT32  u4CWValue;
    MUINT32  u4Dir;
    MUINT32  u4GreenCount;
    MUINT32  u4FaceMean;
    MUINT32*  pu4Hist1;
    MUINT32*  pu4Hist2;
    MUINT32*  pu4Hist3;
    strHistInfo sHistInfo;
}strAEInterInfo;

//Low Pass filer filter
#define G_FILTER_TAPIZE  8
#define  LPF_BUFFER_SIZE  G_FILTER_TAPIZE

typedef struct
{
    MUINT32 u4Idx ;          //index of ring buffer
    MUINT32 u4valid;         //valid data in ring buffer
    MUINT32 pu4LPFBuffer[LPF_BUFFER_SIZE]; //ring low pass buffer
    MUINT32 u4LPFLevel;                   //low pass filter level ;

}strTimeLPF;


typedef struct
{
    MUINT32 u4LpfMin;          // min lpf that be used
    MUINT32 u4LpfMax;         /// max lpf that be used

//
//
//  max LPF  level~~~~~~|       ------------
//                      |      //:          :\\,
//                      |     // :          : \\,
//                      |    //  :          :  \\,
//  min LPF level~~~~~~~|---//------------------\\---
//                          p1  p2         p3 p4    delta_idx

    MUINT32 u4p1;              // p1 please reference chart , 8X delta index
    MUINT32 u4p2;
    MUINT32 u4p3;
    MUINT32 u4p4;
}strLpfConfig;

enum {
AE_TAG_EXPOSURE_TIME=0,
AE_TAG_GAIN,
AE_TAG_APERTURE,
AE_TAG_FRAME_RATE,
AE_TAG_PREVIEW_INDEX,
AE_TAG_CAPTURE_INDEX,
AE_TAG_AETABLE_ID,
AE_TAG_BV,
AE_TAG_AEMODE,
AE_TAG_EV_COM,
AE_TAG_FLICKER,
AE_TAG_BACKLIGH_HIT,
AE_TAG_BACKLIGH_EN,
AE_TAG_BACKLIGH_WEIGHTING,
AE_TAG_BACKLIGH_METERING,
AE_TAG_HISTSTRETCH_HIT,
AE_TAG_HISTSTRETCH_EN,
AE_TAG_HISTSTRETCH_WEIGHTING,
AE_TAG_HISTSTRETCH_METERING,
AE_TAG_ANTIOVER_HIT,
AE_TAG_ANTIOVER_EN,
AE_TAG_ANTIOVER_WEIGHTING,
AE_TAG_ANTIOVER_METERING,
AE_TAG_FACEAE_HIT,
AE_TAG_FACEAE_EN,
AE_TAG_SATURATION_HIT,
AE_TAG_SATURATION_EN,
AE_TAG_AEMETER,
AE_TAG_CWV,
AE_TAG_AVG,
AE_TAG_CENTRAL_Y,
AE_TAG_FACE_Y,
AE_TAG_WITH_FACE,
AE_TAG_ISO_SETTING,
AE_TAG_ISO_REAL,
AE_TAG_SATURATION_COUNT,
AE_TAG_HIST_INFO_HIGHY,
AE_TAG_HIST_INFO_LOWY,
AE_TAG_HIST_INFO_MAXBIN,
AE_TAG_HIST_INFO_BRIGHTEST,
AE_TAG_HIST_INFO_DARKEST,
AE_TAG_HIST_INFO_BRIGHTHALF,
AE_TAG_HIST_INFO_DARKHALF,
AE_TAG_HIST_INFO_DOWN_BRIGHTEST,
AE_TAG_HIST_INFO_FULL_BRIGHTEST,
AE_TAG_HIST_INFO_FLARE_PREVIEW,
AE_TAG_HIST_INFO_FLARE_CAPTURE,
AE_TAG_CWV_FINAL_TARGET,
AE_TAG_STAT_WIN00,
AE_TAG_STAT_WIN01,
AE_TAG_STAT_WIN02,
AE_TAG_STAT_WIN03,
AE_TAG_STAT_WIN04,
AE_TAG_STAT_WIN10,
AE_TAG_STAT_WIN11,
AE_TAG_STAT_WIN12,
AE_TAG_STAT_WIN13,
AE_TAG_STAT_WIN14,
AE_TAG_STAT_WIN20,
AE_TAG_STAT_WIN21,
AE_TAG_STAT_WIN22,
AE_TAG_STAT_WIN23,
AE_TAG_STAT_WIN24,
AE_TAG_STAT_WIN30,
AE_TAG_STAT_WIN31,
AE_TAG_STAT_WIN32,
AE_TAG_STAT_WIN33,
AE_TAG_STAT_WIN34,
AE_TAG_STAT_WIN40,
AE_TAG_STAT_WIN41,
AE_TAG_STAT_WIN42,
AE_TAG_STAT_WIN43,
AE_TAG_STAT_WIN44,
AE_TAG_STAT_BLOCK_CNT,
AE_TAG_PREVIEW_STATH_OFFSET,
AE_TAG_PREVIEW_STATV_OFFSET,
AE_TAG_PREVIEW_STAT_WIDTH,
AE_TAG_PREVIEW_STAT_HEIGHT,
AE_TAG_PREVIEW_HISTH_OFFSET,
AE_TAG_PREVIEW_HISTV_OFFSET,
AE_TAG_PREVIEW_HIST_WIDTH,
AE_TAG_PREVIEW_HIST_HEIGHT,
AE_TAG_ISP_GAIN,
AE_TAG_HIST_INFO_FLARE_P0,
AE_TAG_HIST_INFO_FLARE_P1,
AE_TAG_HIST_INFO_FLARE_P2,
AE_TAG_HIST_INFO_FLARE_P3,
AE_TAG_HIST_INFO_FLARE_P4,
AE_TAG_HIST_INFO_FLARE_P5,
AE_TAG_HIST_INFO_FLARE_P6,
AE_TAG_HIST_INFO_FLARE_P7,
AE_TAG_HIST_INFO_FLARE_P8,
AE_TAG_HIST_INFO_FLARE_P9,
AE_TAG_STROBE_MODE,
AE_TAG_STROBE_LENGTH,
AE_TAG_HIST_INFO_BIN_0,
AE_TAG_HIST_INFO_BIN_1,
AE_TAG_HIST_INFO_BIN_2,
AE_TAG_HIST_INFO_BIN_3,
AE_TAG_HIST_INFO_BIN_4,
AE_TAG_HIST_INFO_BIN_5,
AE_TAG_HIST_INFO_BIN_6,
AE_TAG_HIST_INFO_BIN_7,
AE_TAG_HIST_INFO_BIN_8,
AE_TAG_HIST_INFO_BIN_9,
AE_TAG_HIST_INFO_BIN_10,
AE_TAG_HIST_INFO_BIN_11,
AE_TAG_HIST_INFO_BIN_12,
AE_TAG_HIST_INFO_BIN_13,
AE_TAG_HIST_INFO_BIN_14,
AE_TAG_HIST_INFO_BIN_15,
AE_TAG_HIST_INFO_BIN_16,
AE_TAG_HIST_INFO_BIN_17,
AE_TAG_HIST_INFO_BIN_18,
AE_TAG_HIST_INFO_BIN_19,
AE_TAG_HIST_INFO_BIN_20,
AE_TAG_HIST_INFO_BIN_21,
AE_TAG_HIST_INFO_BIN_22,
AE_TAG_HIST_INFO_BIN_23,
AE_TAG_HIST_INFO_BIN_24,
AE_TAG_HIST_INFO_BIN_25,
AE_TAG_HIST_INFO_BIN_26,
AE_TAG_HIST_INFO_BIN_27,
AE_TAG_HIST_INFO_BIN_28,
AE_TAG_HIST_INFO_BIN_29,
AE_TAG_HIST_INFO_BIN_30,
AE_TAG_HIST_INFO_BIN_31,
AE_TAG_HIST_INFO_BIN_32,
AE_TAG_HIST_INFO_BIN_33,
AE_TAG_HIST_INFO_BIN_34,
AE_TAG_HIST_INFO_BIN_35,
AE_TAG_HIST_INFO_BIN_36,
AE_TAG_HIST_INFO_BIN_37,
AE_TAG_HIST_INFO_BIN_38,
AE_TAG_HIST_INFO_BIN_39,
AE_TAG_HIST_INFO_BIN_40,
AE_TAG_HIST_INFO_BIN_41,
AE_TAG_HIST_INFO_BIN_42,
AE_TAG_HIST_INFO_BIN_43,
AE_TAG_HIST_INFO_BIN_44,
AE_TAG_HIST_INFO_BIN_45,
AE_TAG_HIST_INFO_BIN_46,
AE_TAG_HIST_INFO_BIN_47,
AE_TAG_HIST_INFO_BIN_48,
AE_TAG_HIST_INFO_BIN_49,
AE_TAG_HIST_INFO_BIN_50,
AE_TAG_HIST_INFO_BIN_51,
AE_TAG_HIST_INFO_BIN_52,
AE_TAG_HIST_INFO_BIN_53,
AE_TAG_HIST_INFO_BIN_54,
AE_TAG_HIST_INFO_BIN_55,
AE_TAG_HIST_INFO_BIN_56,
AE_TAG_HIST_INFO_BIN_57,
AE_TAG_HIST_INFO_BIN_58,
AE_TAG_HIST_INFO_BIN_59,
AE_TAG_HIST_INFO_BIN_60,
AE_TAG_HIST_INFO_BIN_61,
AE_TAG_HIST_INFO_BIN_62,
AE_TAG_HIST_INFO_BIN_63,
AE_TAG_HDR_GAIN_THR,
AE_TAG_HDR_LOWLIGHTMODE,
AE_TAG_HDR_SATURATED_PERCENTAGE,
AE_TAG_HDR_SATURATED_PERCENTAGE_LEVEL2,
AE_TAG_HDR_SATURATED_PERCENTAGE_LEVEL3,
AE_TAG_HDR_PERCENTAGE_THR1,
AE_TAG_HDR_PERCENTAGE_THR2,
AE_TAG_HDR_PERCENTAGE_THR3,
AE_TAG_HDR_MUST_ENHANCE_HIGHLIGHT,
AE_TAG_HDR_GAIN_LEVEL1,
AE_TAG_HDR_GAIN_LEVEL2,
AE_TAG_HDR_GAIN_LEVEL3,
AE_TAG_HDR_MAX_EV_GAIN_LEVEL1,
AE_TAG_HDR_MAX_EV_GAIN_LEVEL2,
AE_TAG_HDR_MAX_EV_GAIN_LEVEL3,
AE_TAG_HDR_MAX_EV_GAIN_LEVEL4,
AE_TAG_HDR_LOWLIGHT_HIST_SHT,
AE_TAG_HDR_LOWLIGHT_HIGHTLIGHT,
AE_TAG_HDR_EV_SEL,
AE_TAG_HDR_STNR,
AE_TAG_FACE_HIGH_BOUND,
AE_TAG_FACE_LOW_BOUND,
AE_TAG_MAX
};

//////////////////////////////
//  enum of AE condition
//
//////////////////////////////
enum
{
    AE_CONDITION_NORMAL=0X00,
    AE_CONDITION_BACKLIGHT=BIT(0),
    AE_CONDITION_OVEREXPOSURE=BIT(1),
    AE_CONDITION_HIST_STRETCH=BIT(2),
    AE_CONDITION_SATURATION_CHECK=BIT(3),
    AE_CONDITION_FACEAE=BIT(4)
};

// AE algorithm parameter
typedef struct
{
    struct_AE_Para   strAEPara;      //AE algorithm parameter
//    struct_AE           strAEStatConfig;   //AE statistic configuration
} AE_PARAM_T;


//AE Sensor Config information

//initial AE input/out
typedef struct
{
    MUINT16 u4SensorWidth;       // Input sensor width to calculate the AE Block width
    MUINT16 u4SensorHeight;      // Input sensor height to calculate the AE Block height
    MUINT8  uExposureTimeUnit;   // us base
    MUINT8  uGainStep;           // 32 base
} AE_INITIAL_MODE_T;

typedef struct
{
    AE_INITIAL_MODE_T strPreviewAEInfo;
//   AE_INITIAL_MODE_T strAFAEInfo;
    AE_INITIAL_MODE_T strMovieAEInfo;
//    AE_INITIAL_MODE_T strCaptureAEInfo;
} AE_INITIAL_INPUT_T;

// FD AE
typedef struct
{
    MUINT32 u4FDAE_SUM[4];
    MUINT32 u4FDAE_COUNT[4];
} FD_AE_STAT_T;

//Handle AE input/output
typedef struct
{
    MUINT32 u4AEWindowInfo[25];
    MUINT32 u4AEHistogram[64];
    MUINT32 u4FlareHistogram[10];
    MUINT32 u4AEBlockCnt;
    FD_AE_STAT_T rFDAEStat;
} AE_STAT_T;

//AAA_OUTPUT_PARAM_T use strAEOutput
typedef struct
{
    MUINT32 u4Eposuretime;   //!<: Exposure time in ms
    MUINT32 u4AfeGain;       //!<: sensor gain
    MUINT32 u4IspGain;       //!<: raw gain
    MUINT8  uFrameRate;
    MUINT8  uIris;           //!<: Iris
    MUINT32 u4ISOSpeed;      //!<: ISO speed
    strFlareCFG FlareCFG;

    MUINT32 u4StrobeWidth; //strobe trigger pulse width.
    MUINT32 u4FlashPosRatio; //
}AE_MODE_CFG_T;

typedef struct
{
    AE_MODE_CFG_T rPreviewMode;
    AE_MODE_CFG_T rAFMode;
    AE_MODE_CFG_T rCaptureMode[3];
}AE_OUTPUT_T;

// FD AE statistics config

#define FDAE_WIN_NUM (4)

typedef struct
{
    MUINT32 u4FDAEWIN_EN;
    MUINT32 u4FDAEWINR;
    MUINT32 u4FDAEWINL;
    MUINT32 u4FDAEWIND;
    MUINT32 u4FDAEWINU;
} FDAE_STAT_CONFIG_T;

//AE Statistic window config
typedef struct
{
    MBOOL bAEHisEnable;
    MBOOL bAELumEnable;
    MBOOL bFlareHisEnable;
    MUINT16 AE_WinHOffset;
    MUINT16 AE_WinVOffset;
    MUINT16 AE_WinWidth;
    MUINT16 AE_WinHeight;
    MUINT16 AE_ACC_SEL;
    MUINT16 HIST_WinHOffset;
    MUINT16 HIST_WinVOffset;
    MUINT16 HIST_WinWidth;
    MUINT16 HIST_WinHeight;
    FDAE_STAT_CONFIG_T rFDAEStatConfig[FDAE_WIN_NUM];
} AE_STAT_CONFIG_T;

// for HDR information.
typedef struct
{
    MBOOL bSTNRMode;
    MINT32 i4HDR_EV_SEL;
}AE_HDR_INFO_T;

#endif

