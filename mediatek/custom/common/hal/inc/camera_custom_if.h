#ifndef _CAMERA_CUSTOM_IF_
#define _CAMERA_CUSTOM_IF_
//
#include "camera_custom_types.h"
//
namespace NSCamCustom
{
//
//
enum EDevId
{
    eDevId_ImgSensor0, 
    eDevId_ImgSensor1, 
};

/*******************************************************************************
* Sensor Input Data Bit Order
*   Return:
*       0   : raw data input [9:2]
*       1   : raw data input [7:0]
*       -1  : error
*******************************************************************************/
MINT32  getSensorInputDataBitOrder(EDevId const eDevId);

/*******************************************************************************
* Sensor Pixel Clock Inverse in PAD side.
*   Return:
*       0   : no inverse
*       1   : inverse
*       -1  : error
*******************************************************************************/
MINT32  getSensorPadPclkInv(EDevId const eDevId);

/*******************************************************************************
* Image Sensor Orientation
*******************************************************************************/
typedef struct SensorOrientation_S
{
    MUINT32 u4Degree_0;     //  main sensor in degree (0, 90, 180, 270)
    MUINT32 u4Degree_1;     //  sub  sensor in degree (0, 90, 180, 270)
} SensorOrientation_T;

SensorOrientation_T const&  getSensorOrientation();

/*******************************************************************************
* Return fake orientation for front sensor in degree 0/180 or not
*******************************************************************************/
MBOOL isRetFakeSubOrientation();

/*******************************************************************************
* Auto flicker detection
*******************************************************************************/
typedef struct FlickerThresholdSetting_S
{
    MUINT32 u4FlickerPoss1;         // impossible flicker
    MUINT32 u4FlickerPoss2;         // maybe flicker exist
    MUINT32 u4FlickerFreq1;         // flicker frequency detect 
    MUINT32 u4FlickerFreq2;         // flicker frequency detect
    MUINT32 u4ConfidenceLevel1;   // flicker confidence level
    MUINT32 u4ConfidenceLevel2;   // flicker confidence level
    MUINT32 u4ConfidenceLevel3;   // flicker confidence level
}FlickerThresholdSetting_T;

FlickerThresholdSetting_T const&  getFlickerThresPara();

/*******************************************************************************
* MDP
*******************************************************************************/
typedef struct TuningParam_CRZ_S
{
    MUINT8  uUpScaleCoeff;  //  [5 bits; 1~19] Up sample coeff. choose > 12 may get undesirable result, '8' is recommended.
    MUINT8  uDnScaleCoeff;  //  [5 bits; 1~19] Down sample coeff. '15' is recommended.
} TuningParam_CRZ_T;

typedef struct TuningParam_PRZ_S
{
    MUINT8  uUpScaleCoeff;  //  [5 bits; 1~19] Up sample coeff. choose > 12 may get undesirable result, '8' is recommended.
    MUINT8  uDnScaleCoeff;  //  [5 bits; 1~19] Down sample coeff. '15' is recommended.
    MUINT8  uEEHCoeff;      //  [4 bits] The strength for horizontal edge.
    MUINT8  uEEVCoeff;      //  [4 bits] The strength for vertial edge.
} TuningParam_PRZ_T;

TuningParam_CRZ_T const&  getParam_CRZ_Video();
TuningParam_CRZ_T const&  getParam_CRZ_Preview();
TuningParam_CRZ_T const&  getParam_CRZ_Capture();
TuningParam_PRZ_T const&  getParam_PRZ_QuickView();

//
/*******************************************************************************
* Dynamic Frame Rate for Video
******************************************************************************/
typedef struct VdoDynamicFrameRate_S
{
    MUINT32 EVThresNormal;
    MUINT32 EVThresNight;
    MBOOL   isEnableDFps;
} VdoDynamicFrameRate_T;

VdoDynamicFrameRate_T const& getParamVdoDynamicFrameRate();


/*******************************************************************************
* Custom EXIF (Imgsensor-related)
*******************************************************************************/
typedef struct SensorExifInfo_S
{
    MUINT32 uFLengthNum;
    MUINT32 uFLengthDenom;
    
} SensorExifInfo_T;

SensorExifInfo_T const& getParamSensorExif();

//
/*******************************************************************************
* Custom EXIF
******************************************************************************/
#define SET_EXIF_TAG_STRING(tag,str) \
    if (strlen((const char*)str) <= 32) { \
        strcpy((char *)pexifApp1Info->tag, (const char*)str); }
        
typedef struct customExifInfo_s {
    unsigned char strMake[32];
    unsigned char strModel[32];
    unsigned char strSoftware[32];
} customExifInfo_t;

MINT32 custom_SetExif(void **ppCustomExifTag);
MUINT32 custom_GetFlashlightGain10X(void);  //cotta : added for high current solution

/*******************************************************************************
* Get the LCM Physical Orientation, the LCM physical orientation 
* will be defined in ProjectConfig.mk 
*******************************************************************************/
MUINT32 getLCMPhysicalOrientation();
/*******************************************************************************
* ATV
*******************************************************************************/
MINT32 get_atv_input_data();

/*******************************************************************************
* FD Threshold
*******************************************************************************/
MINT8 get_fdvt_threshold();

/*******************************************************************************
* SD Threshold:  Default: 5 
*******************************************************************************/
MINT8 get_SD_threshold();

/*******************************************************************************
*  Get Face beautify blur level Default: 4   1~4
*******************************************************************************/
MINT8 get_FB_BlurLevel();

/*******************************************************************************
*  Get Face beautify NR cycle number Default: 4   2~6
*******************************************************************************/
MINT8 get_FB_NRTime();

/*******************************************************************************
*  Get Face beautify Color Target mode Default: 2   2:white  0:red
*******************************************************************************/
MINT8 get_FB_ColorTarget();

/*******************************************************************************
* ATV disp delay time
*******************************************************************************/

#define ATV_MODE_NTSC 30000
#define ATV_MODE_PAL  25000

#ifdef MTK_MT5192
//unit: us
#define ATV_MODE_NTSC_DELAY 5000
#define ATV_MODE_PAL_DELAY  10000

#else 
#ifdef MTK_MT5193
//unit: us
#define ATV_MODE_NTSC_DELAY 18000
#define ATV_MODE_PAL_DELAY  26000
#else
//unit: us
#define ATV_MODE_NTSC_DELAY 0
#define ATV_MODE_PAL_DELAY  0
#endif

#endif

MINT32 get_atv_disp_delay(MINT32 mode);

/*******************************************************************************
* ASD Threshold
*******************************************************************************/

typedef struct ASDThreshold_S
{
	MINT16 s2IdxWeightBlAe;
     MINT16 s2IdxWeightBlScd;    
	MINT16 s2IdxWeightLsAe;        
  	MINT16 s2IdxWeightLsAwb;
  	MINT16 s2IdxWeightLsAf;    
     MINT16 s2IdxWeightLsScd;
     MUINT8 u1TimeWeightType;
     MUINT8 u1TimeWeightRange;
     MINT16 s2EvLoThrNight;
     MINT16 s2EvHiThrNight;
     MINT16 s2EvLoThrOutdoor;
     MINT16 s2EvHiThrOutdoor;
     MUINT8 u1ScoreThrNight;
     MUINT8 u1ScoreThrBacklit;
     MUINT8 u1ScoreThrPortrait;
     MUINT8 u1ScoreThrLandscape;
     MBOOL boolBacklitLockEnable;
     MINT16 s2BacklitLockEvDiff;  
}ASDThreshold_T;

ASDThreshold_T const&  get_ASD_threshold();

/*******************************************************************************
* HDR exposure setting
*******************************************************************************/
typedef struct HDRExpSettingInputParam_S
{
    MUINT32 u4MaxSensorAnalogGain; // 1x=1024
    MUINT32 u4MaxAEExpTimeInUS;    // unit: us
    MUINT32 u4MaxAESensorGain;     // 1x=1024
    MUINT32 u4ExpTimeInUS0EV;      // unit: us
    MUINT32 u4SensorGain0EV;       // 1x=1024
    MUINT8  u1FlareOffset0EV;
    MUINT32 u4Histogram[64];
} HDRExpSettingInputParam_T;

typedef struct HDRExpSettingOutputParam_S
{
    MUINT32 u4OutputFrameNum;     // Output frame number (2 or 3)
    MUINT32 u4ExpTimeInUS[3];     // unit: us; [0]-> short exposure; [1]: 0EV; [2]: long exposure
    MUINT32 u4SensorGain[3];      // 1x=1204; [0]-> short exposure; [1]: 0EV; [2]: long exposure
    MUINT8  u1FlareOffset[3];     // [0]-> short exposure; [1]: 0EV; [2]: long exposure
    MUINT32 u4FinalGainDiff[2];   // 1x=1024; [0]: Between short exposure and 0EV; [1]: Between 0EV and long exposure
    MUINT32 u4TargetTone; //Decide the curve to decide target tone     
} HDRExpSettingOutputParam_T;

MVOID getHDRExpSetting(const HDRExpSettingInputParam_T& rInput, HDRExpSettingOutputParam_T& rOutput);

/*******************************************************************************
* PCA LUT for face beautifier
*******************************************************************************/
enum { PCA_BIN_NUM = 180 };
typedef struct {
    MUINT8  y_gain;
    MUINT8  sat_gain;
    MUINT8  hue_shift;
    MUINT8  reserved;
} FB_PCA_BIN_T;
//
//
typedef struct {
    FB_PCA_BIN_T lut[PCA_BIN_NUM];
} FB_PCA_LUT_T;

FB_PCA_LUT_T&  getFBPCALut();

/*******************************************************************************
* Refine capture ISP RAW gain
*******************************************************************************/
MVOID  refineCaptureISPRAWGain(MUINT32 u4SensorGain, MUINT32& u4RAWGain_R, MUINT32& u4RAWGain_Gr, MUINT32& u4RAWGain_Gb, MUINT32& u4RAWGain_B);

};  //NSCamCustom
#endif  //  _CAMERA_CUSTOM_IF_

