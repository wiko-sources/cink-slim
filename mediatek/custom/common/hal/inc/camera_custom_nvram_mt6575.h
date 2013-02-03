#ifndef _CAMERA_CUSTOM_NVRAM_MT6575_H_
#define _CAMERA_CUSTOM_NVRAM_MT6575_H_

#include "MediaTypes.h"
#include "ispif_mt6575.h"
#include "CFG_Camera_File_Max_Size.h"

using namespace NS_MT6575ISP_NVRAM;
using namespace NS_MT6575ISP_EFFECT;

#define NVRAM_CAMERA_DEFECT_FILE_VERSION        1
#define NVRAM_CAMERA_SHADING_FILE_VERSION       1
#define NVRAM_CAMERA_PARA_FILE_VERSION          1
#define NVRAM_CAMERA_3A_FILE_VERSION            1
#define NVRAM_CAMERA_SENSOR_FILE_VERSION        1
#define NVRAM_CAMERA_LENS_FILE_VERSION          1

/*******************************************************************************
* defect
*******************************************************************************/
#define MAX_DEFECT_PRV_SIZE     (256)  //(INT32)
#define MAX_DEFECT_CAP_SIZE     (2048)  //5M sensor BPC ratio 0.1% = 5000 pixel (INT32)

typedef struct
{
    UINT32 Version;
    UINT32 SensorId;    // ID of sensor module
    UINT16 PreviewSize;
    UINT16 CaptureSize;
    UINT32 PreviewTable[MAX_DEFECT_PRV_SIZE];
    UINT32 CaptureTable1[MAX_DEFECT_CAP_SIZE];
    UINT8 CameraData[MAXIMUM_NVRAM_CAMERA_DEFECT_FILE_SIZE-12-4*MAX_DEFECT_PRV_SIZE-4*MAX_DEFECT_CAP_SIZE];
} ISP_DEFECT_STRUCT, *PISP_DEFECT_STRUCT;

typedef struct
{
   ISP_DEFECT_STRUCT Defect;
} NVRAM_CAMERA_DEFECT_STRUCT, *PNVRAM_CAMERA_DEFECT_STRUCT;

/*******************************************************************************
* shading
********************************************************************************/
#define MAX_SHADING_SIZE                (1024) //INT32 
#define MAX_SHADING_Capture_SIZE        (4096) //INT32 
#define MAX_SHADING_Preview_SIZE        (1600) //INT32 
#define MAX_SVD_SHADING_SIZE            (1024) //Byte
#define MAX_SENSOR_CAL_SIZE             (1024) //Byte
typedef struct
{
    UINT32 Version;
    UINT32 SensorId;            // ID of sensor module
    UINT16 PreviewSize;
    UINT16 CaptureSize;
    UINT16 PreviewSVDSize;
    UINT16 CaptureSVDSize;
    UINT32 PreviewTable[3][ MAX_SHADING_Preview_SIZE];
    UINT32 CaptureTable[3][ MAX_SHADING_Capture_SIZE];
    UINT8 PreviewSVDTable[3][MAX_SVD_SHADING_SIZE];
    UINT8 CaptureSVDTable[3][MAX_SVD_SHADING_SIZE];
    UINT8 SensorCalTable[MAX_SENSOR_CAL_SIZE];
    UINT8 CameraData[MAXIMUM_NVRAM_CAMERA_SHADING_FILE_SIZE-16-MAX_SHADING_Preview_SIZE*3*4-MAX_SHADING_Capture_SIZE*3*4-MAX_SVD_SHADING_SIZE*3*2-MAX_SENSOR_CAL_SIZE];
} ISP_SHADING_STRUCT, *PISP_SHADING_STRUCT;

typedef struct
{
   	ISP_SHADING_STRUCT  Shading;
} NVRAM_CAMERA_SHADING_STRUCT, *PNVRAM_CAMERA_SHADING_STRUCT;

/*******************************************************************************
* sensor
********************************************************************************/
// Sensor table
#define MAXIMUM_SENSOR_CCT_REG_NUMBER   100
#define MAXIMUM_SENSOR_ENG_REG_NUMBER   100

/*******************************************************************************
* 3A
********************************************************************************/

//____AE NVRAM____

typedef struct
{
    UINT32 u4MinGain;
    UINT32 u4MaxGain;
    UINT32 u4MiniISOGain;
    UINT32 u4GainStepUnit;
    UINT32 u4PreExpUnit;
    UINT32 u4PreMaxFrameRate;
    UINT32 u4VideoExpUnit;
    UINT32 u4VideoMaxFrameRate;
    UINT32 u4Video2PreRatio;    // 1x = 1024
    UINT32 u4CapExpUnit;
    UINT32 u4CapMaxFrameRate;
    UINT32 u4Cap2PreRatio;	    // 1x = 1024
    UINT32 u4LensFno;           // 10 Base
} AE_DEVICES_INFO_T;

//histogram control information
#define AE_CCT_STRENGTH_NUM (5)

typedef struct
{
   //histogram info
    UINT32 u4HistHighThres;                         // central histogram high threshold
    UINT32 u4HistLowThres;                          // central histogram low threshold
    UINT32 u4MostBrightRatio;                       // full histogram high threshold
    UINT32 u4MostDarkRatio;                         // full histogram low threshold
    UINT32 u4CentralHighBound;                      // central block high boundary
    UINT32 u4CentralLowBound;                       // central block low bounary
    UINT32 u4OverExpThres[AE_CCT_STRENGTH_NUM];     // over exposure threshold
    UINT32 u4HistStretchThres[AE_CCT_STRENGTH_NUM]; // histogram stretch trheshold
    UINT32 u4BlackLightThres[AE_CCT_STRENGTH_NUM];  // backlight threshold
} AE_HIST_CFG_T;

//strAETable AE table Setting
typedef struct
{
    BOOL   bEnableBlackLight;           // enable back light detector
    BOOL   bEnableHistStretch;          // enable histogram stretch
    BOOL   bEnableAntiOverExposure;     // enable anti over exposure
    BOOL   bEnableTimeLPF;              // enable time domain LPF for smooth converge
    BOOL   bEnableCaptureThres;         // enable capture threshold or fix flare offset

    UINT32 u4AETarget;                  // central weighting target
    UINT32 u4InitIndex;                 // AE initiail index

    UINT32 u4BackLightWeight;           // Back light weighting value
    UINT32 u4HistStretchWeight;         // Histogram weighting value
    UINT32 u4AntiOverExpWeight;         // Anti over exposure weighting value

    UINT32 u4BlackLightStrengthIndex;   // Black light threshold strength index
    UINT32 u4HistStretchStrengthIndex;  // Histogram stretch threshold strength index
    UINT32 u4AntiOverExpStrengthIndex;  // Anti over exposure threshold strength index
    UINT32 u4TimeLPFStrengthIndex;      // Smooth converge threshold strength index
    UINT32 u4LPFConvergeLevel[AE_CCT_STRENGTH_NUM];  //LPF converge support level

    UINT32 u4InDoorEV;                  // check the environment indoor/outdoor
    INT32 u4BVOffset;                  // Calibrate BV offset
    UINT32 u4PreviewFlareOffset;        // Fix preview flare offset
    UINT32 u4CaptureFlareOffset;        // Fix capture flare offset
    UINT32 u4CaptureFlareThres;         // Capture flare threshold
    UINT32 u4MaxCaptureFlareThres;  // for u4CaptureFlareThres used
    UINT32 u4FlatnessThres;              // 10 base for flatness condition.
    UINT32 u4FlatnessStrength;
} AE_CCT_CFG_T;                            // histogram control information

typedef struct
{
    AE_DEVICES_INFO_T rDevicesInfo;
    AE_HIST_CFG_T rHistConfig;
    AE_CCT_CFG_T rCCTConfig;
} AE_NVRAM_T;

//____AF NVRAM____

#define DEST_NUM (30)
#define ZOOM_NUM (1)

typedef struct
{
    INT32 i4InfPos;
    INT32 i4MacroPos;
} FOCUS_RANGE_T;

typedef struct
{
    INT32  i4NormalNum;
    INT32  i4MacroNum;
    INT32  i4Pos[DEST_NUM];
} AF_DEST_T;

typedef struct
{
    struct
    {
        AF_DEST_T sExactSrch;
    } sZoomDest[ZOOM_NUM];

    INT32  i4ZoomTable[ZOOM_NUM];	// offset from calibration

    INT32 i4AF_THRES_MAIN;
    INT32 i4AF_THRES_SUB;
    INT32 i4AF_THRES_OFFSET;
    INT32 i4AF_CURVE_SCORE;
    INT32 i4LV_THRES;
    // --- Matrix AF ---
    INT32 i4MATRIX_AF_DOF;
    INT32 i4MATRIX_AF_WIN_NUM;
    // --- AFC ---
    INT32 i4AFC_THRES_OFFSET;   // AFC blur degree
    INT32 i4AFC_STEP_SIZE;      // AFC speed / blur degree
    INT32 i4AFC_SPEED;          // AFC speed
    INT32 i4SCENE_CHANGE_THRES;	// AFC sensitive
    INT32 i4SCENE_CHANGE_CNT;   // AFC sensitive
    // --- AF window location ---
    INT32 i4SPOT_PERCENT_X;
    INT32 i4SPOT_PERCENT_Y;
    INT32 i4MATRIX_PERCENT_X;
    INT32 i4MATRIX_PERCENT_Y;
    INT32 i4MATRIX_LOC_OFFSET;
    // --- Tune Para ---
    INT32 i4TUNE_PARA1;
    INT32 i4TUNE_PARA2;
    INT32 i4TUNE_PARA3;

} AF_NVRAM_T;

//____AWB NVRAM____

// AWB gain
typedef struct
{
    UINT32 u4R; // R gain
    UINT32 u4G; // G gain
    UINT32 u4B; // B gain
} AWB_GAIN_T;

// XY coordinate
typedef struct
{
    INT32 i4X; // X
    INT32 i4Y; // Y
} XY_COORDINATE_T;

// Light area
typedef struct
{
    INT32 i4RightBound; // Right bound
    INT32 i4LeftBound;  // Left bound
    INT32 i4UpperBound; // Upper bound
    INT32 i4LowerBound; // Lower bound
} LIGHT_AREA_T;

// Preference color
typedef struct
{
    INT32 i4SliderValue; // Slider value
    INT32 i4OffsetThr;   // Offset threshold
} PREFERENCE_COLOR_T;

// AWB calibration data
typedef struct
{
    AWB_GAIN_T rCalGain;    // Calibration gain: WB gain of DNP (individual camera)
    AWB_GAIN_T rDefGain;    // Default calibration gain: WB gain of DNP (golden sample)
    AWB_GAIN_T rD65Gain;    // WB gain of D65 (golden sample)
} AWB_CALIBRATION_DATA_T;

// AWB light source XY coordinate
typedef struct
{
    XY_COORDINATE_T rHorizon;  // Horizon
    XY_COORDINATE_T rA;        // A
    XY_COORDINATE_T rTL84;     // TL84
    XY_COORDINATE_T rCWF;      // CWF
    XY_COORDINATE_T rDNP;      // DNP
    XY_COORDINATE_T rD65;      // D65
    XY_COORDINATE_T rD75;      // D75
    XY_COORDINATE_T rDF;       // Daylight fluorescent
} AWB_LIGHT_SOURCE_XY_COORDINATE_T;

// Rotation matrix parameter
typedef struct
{
    INT32 i4RotationAngle; // Rotation angle
    INT32 i4H11;           // cos
    INT32 i4H12;           // sin
    INT32 i4H21;           // -sin
    INT32 i4H22;           // cos
} AWB_ROTATION_MATRIX_T;

// Daylight locus parameter
typedef struct
{
    INT32 i4SlopeNumerator;   // Slope numerator
    INT32 i4SlopeDenominator; // Slope denominator
} AWB_DAYLIGHT_LOCUS_T;

// AWB light area
typedef struct
{
    LIGHT_AREA_T rTungsten;        // Tungsten
    LIGHT_AREA_T rWarmFluorescent; // Warm fluorescent
    LIGHT_AREA_T rFluorescent;     // Fluorescent
    LIGHT_AREA_T rCWF;             // CWF
    LIGHT_AREA_T rDaylight;        // Daylight
    LIGHT_AREA_T rShade;           // Shade
    LIGHT_AREA_T rDaylightFluorescent; // Daylight fluorescent
} AWB_LIGHT_AREA_T;

// PWB light area
typedef struct
{
    LIGHT_AREA_T rReferenceArea;   // Reference area
    LIGHT_AREA_T rDaylight;        // Daylight
    LIGHT_AREA_T rCloudyDaylight;  // Cloudy daylight
    LIGHT_AREA_T rShade;           // Shade
    LIGHT_AREA_T rTwilight;        // Twilight
    LIGHT_AREA_T rFluorescent;     // Fluorescent
    LIGHT_AREA_T rWarmFluorescent; // Warm fluorescent
    LIGHT_AREA_T rIncandescent;    // Incandescent
    LIGHT_AREA_T rGrayWorld; // for CCT use
} PWB_LIGHT_AREA_T;

// PWB default gain
typedef struct
{
    AWB_GAIN_T rDaylight;        // Daylight
    AWB_GAIN_T rCloudyDaylight;  // Cloudy daylight
    AWB_GAIN_T rShade;           // Shade
    AWB_GAIN_T rTwilight;        // Twilight
    AWB_GAIN_T rFluorescent;     // Fluorescent
    AWB_GAIN_T rWarmFluorescent; // Warm fluorescent
    AWB_GAIN_T rIncandescent;    // Incandescent
    AWB_GAIN_T rGrayWorld; // for CCT use
} PWB_DEFAULT_GAIN_T;

// AWB preference color
typedef struct
{
    PREFERENCE_COLOR_T rTungsten;        // Tungsten
    PREFERENCE_COLOR_T rWarmFluorescent; // Warm fluorescent
    PREFERENCE_COLOR_T rShade;           // Shade
    AWB_GAIN_T rDaylightWBGain;           // Daylight WB gain
    AWB_GAIN_T rPreferenceGain_Tungsten;            // Preference gain: tungsten
    AWB_GAIN_T rPreferenceGain_WarmFluorescent;     // Preference gain: warm fluorescent
    AWB_GAIN_T rPreferenceGain_Fluorescent;         // Preference gain: fluorescent
    AWB_GAIN_T rPreferenceGain_CWF;                 // Preference gain: CWF
    AWB_GAIN_T rPreferenceGain_Daylight;            // Preference gain: daylight
    AWB_GAIN_T rPreferenceGain_Shade;               // Preference gain: shade
    AWB_GAIN_T rPreferenceGain_DaylightFluorescent; // Preference gain: daylight fluorescent
} AWB_PREFERENCE_COLOR_T;

#define AWB_CCT_ESTIMATION_LIGHT_SOURCE_NUM (6)

// CCT estimation
typedef struct
{
    INT32 i4CCT[AWB_CCT_ESTIMATION_LIGHT_SOURCE_NUM];                // CCT
    INT32 i4RotatedXCoordinate[AWB_CCT_ESTIMATION_LIGHT_SOURCE_NUM]; // Rotated X coordinate
} AWB_CCT_ESTIMATION_T;


// AWB NVRAM structure
typedef struct
{
    AWB_CALIBRATION_DATA_T rCalData; // AWB calibration data
    AWB_LIGHT_SOURCE_XY_COORDINATE_T rOriginalXY; // Original XY coordinate of AWB light source
    AWB_LIGHT_SOURCE_XY_COORDINATE_T rRotatedXY; // Rotated XY coordinate of AWB light source
    AWB_ROTATION_MATRIX_T rRotationMatrix; // Rotation matrix parameter
    AWB_DAYLIGHT_LOCUS_T rDaylightLocus; // Daylight locus parameter
    AWB_LIGHT_AREA_T rAWBLightArea; // AWB light area
    PWB_LIGHT_AREA_T rPWBLightArea; // PWB light area
    PWB_DEFAULT_GAIN_T rPWBDefaultGain; // PWB default gain
    AWB_PREFERENCE_COLOR_T rPreferenceColor; // AWB preference color
    AWB_CCT_ESTIMATION_T rCCTEstimation; // CCT estimation
} AWB_NVRAM_T;

//____3A NVRAM____

//typedef unsigned char  UINT8;

typedef struct
{
    //data structure version, update once structure been modified.
    UINT32 u4Version;

    // ID of sensor module
    UINT32 SensorId;

    //data content
    AE_NVRAM_T rAENVRAM;
    AWB_NVRAM_T rAWBNVRAM;

    //SSS(reserved unused spaces(bytes)) = total-used;,
    //ex. SSS = 4096-sizeof(UINT32)--sizeof(NVRAM_AAA_T)-sizeof(NVRAM_bbb_T);
    //    UINT8 reserved[MAXIMUM_NVRAM_CAMERA_3A_FILE_SIZE-sizeof(UINT32)-sizeof(AE_NVRAM_T)-sizeof(AF_NVRAM_T)-sizeof(AWB_NVRAM_T)];
    UINT8 reserved[MAXIMUM_NVRAM_CAMERA_3A_FILE_SIZE-sizeof(UINT32)-sizeof(AE_NVRAM_T)-sizeof(AWB_NVRAM_T)];
} NVRAM_CAMERA_3A_T, *PNVRAM_CAMERA_3A_T;

#define NVRAM_CAMERA_3A_STRUCT NVRAM_CAMERA_3A_T
#define PNVRAM_CAMERA_3A_STRUCT PNVRAM_CAMERA_3A_T

/*******************************************************************************
* camera parameter
********************************************************************************/
#define NVRAM_SHADING_TBL_NUM           3
#define NVRAM_OB_TBL_NUM                1
#define NVRAM_DM_TBL_NUM                7
#define NVRAM_DP_TBL_NUM                4
#define NVRAM_NR1_TBL_NUM               11
#define NVRAM_NR2_TBL_NUM               16
#define NVRAM_EE_TBL_NUM                12
#define NVRAM_SAT_TBL_NUM               7
#define NVRAM_CONTRAST_TBL_NUM          9
#define NVRAM_HUE_TBL_NUM               3
#define NVRAM_CCM_TBL_NUM               3
#define NVRAM_GAMMA_TBL_NUM             1

// camera common parameters and sensor parameters
typedef struct
{
    UINT32 CommReg[64];// 0~30 reserve for compatiblilty with WCP1 setting; 
                                      // [31] for table defect control 0x0154h
                                      // [32] for defect table address 0x0158h
} ISP_NVRAM_COMMON_STRUCT, *PISP_NVRAM_COMMON_STRUCT;

typedef struct ISP_NVRAM_REG_INDEX_STRUCT
{
    UINT8 Shading;
    UINT8 OB;
    UINT8 DM;
    UINT8 DP;   //AutoDefect;
    UINT8 NR1;
    UINT8 NR2;
    UINT8 EE;
    UINT8 Saturation;
    UINT8 Contrast;
    UINT8 Hue;
    UINT8 CCM;
    UINT8 Gamma;
}	ISP_NVRAM_REG_INDEX_T, *PISP_NVRAM_REG_INDEX_T;

typedef struct
{
    ISP_NVRAM_REG_INDEX_T       Idx;
    ISP_NVRAM_SHADING_T         Shading[NVRAM_SHADING_TBL_NUM];
    ISP_NVRAM_OB_T              OB[NVRAM_OB_TBL_NUM];
    ISP_NVRAM_DEMOSAIC_T        DM[NVRAM_DM_TBL_NUM];
    ISP_NVRAM_DP_T              DP[NVRAM_DP_TBL_NUM];
    ISP_NVRAM_NR1_T             NR1[NVRAM_NR1_TBL_NUM];
    ISP_NVRAM_NR2_T             NR2[NVRAM_NR2_TBL_NUM];
    ISP_NVRAM_EE_T              EE[NVRAM_EE_TBL_NUM];
    ISP_NVRAM_SATURATION_T      Saturation[NVRAM_SAT_TBL_NUM];
    ISP_NVRAM_CONTRAST_T        Contrast[NVRAM_CONTRAST_TBL_NUM];
    ISP_NVRAM_HUE_T             Hue[NVRAM_HUE_TBL_NUM];
    ISP_NVRAM_CCM_T             CCM[NVRAM_CCM_TBL_NUM];
    ISP_NVRAM_GAMMA_T           Gamma[NVRAM_GAMMA_TBL_NUM];
} ISP_NVRAM_REGISTER_STRUCT, *PISP_NVRAM_REGISTER_STRUCT;

typedef struct
{
    ISP_NVRAM_PCA_T         Config;
    ISP_NVRAM_PCA_LUTS_T    PCA_LUTs;
} ISP_NVRAM_PCA_STRUCT, *PISP_NVRAM_PCA_STRUCT;

typedef union
{
    struct  {
        UINT32                      Version;
        UINT32                      SensorId;    // ID of sensor module
        ISP_NVRAM_COMMON_STRUCT     ISPComm;
        ISP_NVRAM_REGISTER_STRUCT   ISPRegs;
        ISP_NVRAM_PCA_STRUCT        ISPPca;
    };
    UINT8   Data[MAXIMUM_NVRAM_CAMERA_PARA_FILE_SIZE];
} NVRAM_CAMERA_ISP_PARAM_STRUCT, *PNVRAM_CAMERA_ISP_PARAM_STRUCT;

//FIXME: remove if possible.
typedef NVRAM_CAMERA_ISP_PARAM_STRUCT   NVRAM_CAMERA_PARA_STRUCT;
typedef PNVRAM_CAMERA_ISP_PARAM_STRUCT  PNVRAM_CAMERA_PARA_STRUCT;


class IspNvramRegMgr
{
public:
    IspNvramRegMgr(ISP_NVRAM_REGISTER_STRUCT*const pIspNvramRegs)
        : m_rRegs(*pIspNvramRegs)
        , m_rIdx(pIspNvramRegs->Idx)
    {}
    virtual ~IspNvramRegMgr() {}

public:
    enum EIndexNum
    {
        NUM_Shading     =   NVRAM_SHADING_TBL_NUM, 
        NUM_OB          =   NVRAM_OB_TBL_NUM, 
        NUM_DM          =   NVRAM_DM_TBL_NUM, 
        NUM_DP          =   NVRAM_DP_TBL_NUM, 
        NUM_NR1         =   NVRAM_NR1_TBL_NUM, 
        NUM_NR2         =   NVRAM_NR2_TBL_NUM, 
        NUM_EE          =   NVRAM_EE_TBL_NUM, 
        NUM_Saturation  =   NVRAM_SAT_TBL_NUM, 
        NUM_Contrast    =   NVRAM_CONTRAST_TBL_NUM, 
        NUM_Hue         =   NVRAM_HUE_TBL_NUM, 
        NUM_CCM         =   NVRAM_CCM_TBL_NUM, 
        NUM_Gamma       =   NVRAM_GAMMA_TBL_NUM, 
    };

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Index.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:    ////    Set Index
    inline bool setIdx(UINT8 &rIdxTgt, UINT8 const IdxSrc, EIndexNum const Num)
    {
        if  (IdxSrc < Num)
        {
            rIdxTgt = IdxSrc;
            return  true;
        }
        return  false;
    }
public:     ////    Set Index
    inline bool setIdx_Shading(UINT8 const idx)    { return setIdx(m_rIdx.Shading, idx, NUM_Shading); }
    inline bool setIdx_OB(UINT8 const idx)         { return setIdx(m_rIdx.OB, idx, NUM_OB); }
    inline bool setIdx_DM(UINT8 const idx)         { return setIdx(m_rIdx.DM, idx, NUM_DM); }
    inline bool setIdx_DP(UINT8 const idx)         { return setIdx(m_rIdx.DP, idx, NUM_DP); }
    inline bool setIdx_NR1(UINT8 const idx)        { return setIdx(m_rIdx.NR1, idx, NUM_NR1); }
    inline bool setIdx_NR2(UINT8 const idx)        { return setIdx(m_rIdx.NR2, idx, NUM_NR2); }
    inline bool setIdx_EE(UINT8 const idx)         { return setIdx(m_rIdx.EE, idx, NUM_EE); }
    inline bool setIdx_Saturation(UINT8 const idx) { return setIdx(m_rIdx.Saturation, idx, NUM_Saturation); }
    inline bool setIdx_Contrast(UINT8 const idx)   { return setIdx(m_rIdx.Contrast, idx, NUM_Contrast); }
    inline bool setIdx_Hue(UINT8 const idx)        { return setIdx(m_rIdx.Hue, idx, NUM_Hue); }
    inline bool setIdx_CCM(UINT8 const idx)        { return setIdx(m_rIdx.CCM, idx, NUM_CCM); }
    inline bool setIdx_Gamma(UINT8 const idx)      { return setIdx(m_rIdx.Gamma, idx, NUM_Gamma); }
public:     ////    Get Index
    inline UINT8 getIdx_Shading()   const { return m_rIdx.Shading; }
    inline UINT8 getIdx_OB()        const { return m_rIdx.OB; }
    inline UINT8 getIdx_DM()        const { return m_rIdx.DM; }
    inline UINT8 getIdx_DP()        const { return m_rIdx.DP; }
    inline UINT8 getIdx_NR1()       const { return m_rIdx.NR1; }
    inline UINT8 getIdx_NR2()       const { return m_rIdx.NR2; }
    inline UINT8 getIdx_EE()        const { return m_rIdx.EE; }
    inline UINT8 getIdx_Saturation()const { return m_rIdx.Saturation; }
    inline UINT8 getIdx_Contrast()  const { return m_rIdx.Contrast; }
    inline UINT8 getIdx_Hue()       const { return m_rIdx.Hue; }
    inline UINT8 getIdx_CCM()       const { return m_rIdx.CCM; }
    inline UINT8 getIdx_Gamma()     const { return m_rIdx.Gamma; }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    inline ISP_NVRAM_SHADING_T&     getShading() { return m_rRegs.Shading[getIdx_Shading()]; }
    inline ISP_NVRAM_OB_T&          getOB() { return m_rRegs.OB[getIdx_OB()]; }
    inline ISP_NVRAM_DEMOSAIC_T&    getDM() { return m_rRegs.DM[getIdx_DM()]; }
    inline ISP_NVRAM_DP_T&          getDP() { return m_rRegs.DP[getIdx_DP()]; }
    inline ISP_NVRAM_NR1_T&         getNR1() { return m_rRegs.NR1[getIdx_NR1()]; }
    inline ISP_NVRAM_NR2_T&         getNR2() { return m_rRegs.NR2[getIdx_NR2()]; }
    inline ISP_NVRAM_EE_T&          getEE() { return m_rRegs.EE[getIdx_EE()]; }
    inline ISP_NVRAM_SATURATION_T&  getSaturation() { return m_rRegs.Saturation[getIdx_Saturation()]; }
    inline ISP_NVRAM_CONTRAST_T&    getContrast() { return m_rRegs.Contrast[getIdx_Contrast()]; }
    inline ISP_NVRAM_HUE_T&         getHue() { return m_rRegs.Hue[getIdx_Hue()]; }
    inline ISP_NVRAM_CCM_T&         getCCM() { return m_rRegs.CCM[getIdx_CCM()]; }
    inline ISP_NVRAM_GAMMA_T&       getGamma() { return m_rRegs.Gamma[getIdx_Gamma()]; }

private:    ////    Data Members.
    ISP_NVRAM_REGISTER_STRUCT&      m_rRegs;
    ISP_NVRAM_REG_INDEX_STRUCT&     m_rIdx;
};


#define CAL_GET_DEFECT_FLAG     0x01
#define CAL_GET_3ANVRAM_FLAG    0x02
#define CAL_GET_SHADING_FLAG    0x04
#define CAL_GET_PARA_FLAG       0x08
#define CAL_DATA_LOAD           0x6C6F6164//"load"
#define CAL_DATA_UNLOAD         0x00000000
#define CAL_SHADING_TYPE_SENSOR 0x216D746B//"!mtk"
#define CAL_SHADING_TYPE_ISP    0x3D6D746B//"=mtk"

typedef struct
{
    PNVRAM_CAMERA_DEFECT_STRUCT     pCameraDefect;
    PNVRAM_CAMERA_SHADING_STRUCT    pCameraShading;
    PNVRAM_CAMERA_ISP_PARAM_STRUCT  pCameraPara;
    AWB_GAIN_T 						rCalGain;
} GET_SENSOR_CALIBRATION_DATA_STRUCT, *PGET_SENSOR_CALIBRATION_DATA_STRUCT;

/*******************************************************************************
*
********************************************************************************/
typedef enum
{
    CAMERA_NVRAM_DATA_ISP = 0, 
    CAMERA_NVRAM_DATA_PARA = 0, //  remove if possible (the same as CAMERA_NVRAM_DATA_ISP)
    CAMERA_NVRAM_DATA_3A,
    CAMERA_NVRAM_DATA_SHADING,
    CAMERA_NVRAM_DATA_DEFECT,
    CAMERA_NVRAM_DATA_SENSOR,
    CAMERA_NVRAM_DATA_LENS,
    CAMERA_DATA_3A_PARA,
    CAMERA_DATA_3A_STAT_CONFIG_PARA,
    CAMERA_DATA_TYPE_NUM
} CAMERA_DATA_TYPE_ENUM;

typedef enum
{
    GET_CAMERA_DATA_NVRAM,
    GET_CAMERA_DATA_DEFAULT,
    SET_CAMERA_DATA_NVRAM,
} MSDK_CAMERA_NVRAM_DATA_CTRL_CODE_ENUM;

/*******************************************************************************
*
********************************************************************************/

typedef struct
{
    UINT32 Version;
    FOCUS_RANGE_T rFocusRange;
    AF_NVRAM_T    rAFNVRAM;
    UINT8 reserved[MAXIMUM_NVRAM_CAMERA_LENS_FILE_SIZE-sizeof(UINT32)-sizeof(FOCUS_RANGE_T)-sizeof(AF_NVRAM_T)];
} NVRAM_LENS_PARA_STRUCT, *PNVRAM_LENS_PARA_STRUCT;




#endif // _CAMERA_CUSTOM_NVRAM_MT6575_H_

