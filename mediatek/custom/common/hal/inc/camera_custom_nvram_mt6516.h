#ifndef _CAMERA_CUSTOM_NVRAM_MT6516_H_
#define _CAMERA_CUSTOM_NVRAM_MT6516_H_

#include "MediaTypes.h"

//#if defined(MT6516)
//#include "ispif_mt6516.h"
//#endif

#define MAXIMUM_NVRAM_CAMERA_DEFECT_FILE_SIZE		       29744 //BYTE 81920
#define MAXIMUM_NVRAM_CAMERA_SHADING_FILE_SIZE		29744 //BYTE 81920--> 16000 for meta tool limitation
#define MAXIMUM_NVRAM_CAMERA_PARA_FILE_SIZE			4096
#define MAXIMUM_NVRAM_CAMERA_3A_FILE_SIZE			4096
#define MAXIMUM_NVRAM_CAMERA_SENSOR_FILE_SIZE		4096

#define NVRAM_CAMERA_DEFECT_FILE_VERSION 		1
#define NVRAM_CAMERA_SHADING_FILE_VERSION 		1
#define NVRAM_CAMERA_PARA_FILE_VERSION 			2
#define NVRAM_CAMERA_3A_FILE_VERSION 			4
#define NVRAM_CAMERA_SENSOR_FILE_VERSION 		1
#define NVRAM_CAMERA_LENS_FILE_VERSION 			1

//#if defined(MT6516)
#define MT6516ISP_LSC_REGISTERCNT 11
#define MT6516ISP_NR1_REGISTERCNT 16
#define MT6516ISP_NR2_REGISTERCNT 4
#define MT6516ISP_EE_REGISTERCNT 9
#define MT6516ISP_YCCGO_REGISTERCNT 7
#define MT6516ISP_CCM_REGISTERCNT 3
#define MT6516ISP_GAMMA_REGISTERCNT 5
//#endif

/*******************************************************************************
* defect
*******************************************************************************/
#define MAX_DEFECT_PRV_SIZE  	(256)  //(INT32)
#define MAX_DEFECT_CAP_SIZE  	(2048)  //5M sensor BPC ratio 0.1% = 5000 pixel (INT32)

typedef struct
{
    UINT32 Version;
    UINT32 SensorId;		// ID of sensor module
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
#define MAX_SHADING_SIZE 	(1024)  //INT32 
#define MAX_SVD_SHADING_SIZE 	(512)  //Byte
#define MAX_SENSOR_CAL_SIZE     (1024) //Byte
typedef struct
{
    UINT32 Version;
    UINT32 SensorId;		// ID of sensor module
    UINT16 PreviewSize;
    UINT16 CaptureSize;
    UINT16 PreviewSVDSize;
    UINT16 CaptureSVDSize;
    UINT32 PreviewTable[3][MAX_SHADING_SIZE];
    UINT32 CaptureTable[3][MAX_SHADING_SIZE];
    UINT8 PreviewSVDTable[3][MAX_SVD_SHADING_SIZE];
    UINT8 CaptureSVDTable[3][MAX_SVD_SHADING_SIZE];
    UINT8 SensorCalTable[MAX_SENSOR_CAL_SIZE];
    UINT8 CameraData[MAXIMUM_NVRAM_CAMERA_SHADING_FILE_SIZE-16-MAX_SHADING_SIZE*4*2*3 -MAX_SVD_SHADING_SIZE*2*3-MAX_SENSOR_CAL_SIZE];
} ISP_SHADING_STRUCT, *PISP_SHADING_STRUCT;

typedef struct
{
   	ISP_SHADING_STRUCT	Shading;
} NVRAM_CAMERA_SHADING_STRUCT, *PNVRAM_CAMERA_SHADING_STRUCT;

/*******************************************************************************
* sensor
********************************************************************************/
// Sensor table
#define MAXIMUM_SENSOR_CCT_REG_NUMBER	100
#define MAXIMUM_SENSOR_ENG_REG_NUMBER	100

#if 0   //@Sean, defined in kd_imgsensor_definition.h 
typedef struct
{
	UINT32	Addr;
	UINT32	Para;
} SENSOR_REG_STRUCT;

typedef struct
{
	UINT32 Version;
	UINT32 SensorId;		// ID of sensor module
    SENSOR_REG_STRUCT   SensorEngReg[MAXIMUM_SENSOR_ENG_REG_NUMBER];
    SENSOR_REG_STRUCT   SensorCCTReg[MAXIMUM_SENSOR_CCT_REG_NUMBER];
    UINT8 CameraData[MAXIMUM_NVRAM_CAMERA_SENSOR_FILE_SIZE-4-sizeof(SENSOR_REG_STRUCT)*(MAXIMUM_SENSOR_ENG_REG_NUMBER+MAXIMUM_SENSOR_CCT_REG_NUMBER)];
} NVRAM_SENSOR_DATA_STRUCT, *PNVRAM_SENSOR_DATA_STRUCT;
#endif 

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
    INT32 i4AFC_THRES_OFFSET;		// AFC blur degree
    INT32 i4AFC_STEP_SIZE;			// AFC speed / blur degree
    INT32 i4AFC_SPEED;			// AFC speed
    INT32 i4SCENE_CHANGE_THRES;		// AFC sensitive
    INT32 i4SCENE_CHANGE_CNT;		// AFC sensitive
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
//    UINT8 reserved[2048-sizeof(UINT32)-sizeof(AE_NVRAM_T)-sizeof(AF_NVRAM_T)-sizeof(AWB_NVRAM_T)];
    UINT8 reserved[2048-sizeof(UINT32)-sizeof(AE_NVRAM_T)-sizeof(AWB_NVRAM_T)];
} NVRAM_CAMERA_3A_T, *PNVRAM_CAMERA_3A_T;

#define NVRAM_CAMERA_3A_STRUCT NVRAM_CAMERA_3A_T
#define PNVRAM_CAMERA_3A_STRUCT PNVRAM_CAMERA_3A_T

#if 0
// camera 3A parameters
#define MAX_ISO_LUT_NO 3
#define	AF_MAX_DEF_PATH_NO		(3)	// AF PATH NO in NVRAM
#define	AF_MAX_TABLE_NO			(24)
#define AF_MZ_DOF_MAX_NO 		(8)
#define AF_MZ_DOF_LUT_IDX_NO 	(3)
#define AE_TOTAL_GAMMA_NO 5
#define GAMMA_FAST_LUT_NO 64

typedef enum
{
	AF_PARAM_VERSION=0,
	AF_PARAM_PEAK_THRE,
	AF_PARAM_NOFOCUS_THRE,
	AF_PARAM_STOP_THRE,
	AF_PARAM_HYSTERESIS_THRE,
	AF_PARAM_HYSTERESIS_DIR,
	AF_PARAM_SLOW_MOVE_MAX_STEP,
	AF_PARAM_SLOW_BACK_MAX_STEP,
	AF_PARAM_WAIT_STABLE_THRE2,
	AF_PARAM_WAIT_STABLE_THRE3,
	AF_PARAM_STARTUP_POS,
	AF_PARAM_STARTUP_WAIT_FRAME,
	AF_PARAM_SEARCH_OPTION,
	// for continuous AF
	AF_PARAM_CONT_DD_STEP_RATIO,
	AF_PARAM_CONT_DD_RANGE_THRE,
	AF_PARAM_CONT_FV_DELTA,	//0828NEW
	AF_PARAM_CONT_FV_BLUR,	//0828NEW
	AF_PARAM_CONT_FV_DELTA_HIGH,
	AF_PARAM_CONT_FV_DELTA_LOW,
	AF_PARAM_CONT_AE_DELTA_HIGH,
	AF_PARAM_CONT_AE_DELTA_LOW,
	AF_PARAM_CONT_WAIT_STABLE_AE,
	AF_PARAM_CONT_WAIT_STABLE_FV_HIGH,
	AF_PARAM_CONT_WAIT_STABLE_FV_LOW,
	AF_PARAM_CONT_WAIT_STABLE_FREEZE,
	AF_PARAM_CONT_JUMP_COUNT_THRE,
	AF_PARAM_CONT_WAIT_STABLE_MAX,
	// for MZ AF
	AF_PARAM_MZ_MAX_INF_DOF,
	AF_PARAM_MZ_MAX_MACRO_DOF,
	// for continuous AF append
	AF_PARAM_CONT_PAN_LUMA_DELTA, // 0832NEW
	AF_PARAM_CONT_PAN_HOLD_FRAME, // 0832NEW
	AF_PARAM_CONT_PAN_OVER_SPEEDUP, // 0832NEW
	AF_PARAM_CONT_WINDOW_CONFIG_X,// 0832NEW
	AF_PARAM_CONT_WINDOW_CONFIG_Y,// 0832NEW
	AF_PARAM_CONT_STOP_SEARCH_CONFIG,// 0836NEW
	// for single/multi window AF append
	AF_PARAM_WIN_SIZE_LEVEL_CONFIG,// 0836NEW
	// END TAG
	AF_PARAM_END_TAG,
	AF_PARAM_MAX_NO	= 38
} AF_PARAM_ENUM;

enum
{
	AE_SELECT_PREIVEW=0,
	AE_SELECT_VIDEO,
	AE_SELECT_NIGHT,
	AE_SELECT_MODE_NO
};

//AE
/* ISO priority */
typedef struct
{
	UINT16	IsoGain[MAX_ISO_LUT_NO];
	UINT16	IsoValue[MAX_ISO_LUT_NO];
}aeIsoLutStruct, MSDK_CCT_AE_ISO_LUT_STRUCT, *PMSDK_CCT_AE_ISO_LUT_STRUCT;

typedef struct
{
	UINT8 target_mean;
	UINT8 init_expo_idx;
	/* capture flare offset cal */
	UINT8 flare_fix_flag;
	UINT8	flare_fix_offset;
	UINT8	flare_search_dn_ratio;
	char	flare_min_offset;
	/* preview & video gamma no */
	UINT8 preview_gamma_no;
	UINT8 preview_flare_offset;
	/* gamma mean */
	UINT8 indoor_outdoor_ev;
	UINT8 indoor_min_gamma_mean;
	UINT8 outdoor_max_gamma_mean;
	/* sky detection */
	UINT8 sky_ev;
	UINT8 sky_mean;
	UINT8 sky_max_window_mean;
	/* max under expo bin */
	UINT8 min_under_expo_bin;
	/* lowlight gamma limit */
	UINT8 lowlight_ev;
	UINT8 lowlight_min_target_mean;
	/* enable flag */
	UINT8 non_over_exposure_stretch_enabled;
	UINT8 full_dark_background_check_enabled;
	UINT8 backlight_stretch_enabled;
	/* fast gamma lut for gamma mean */
	UINT8 gamma_lut[AE_TOTAL_GAMMA_NO][GAMMA_FAST_LUT_NO];
	UINT8 grass_detect_enable;/* grass detection flag */
	UINT8 reserve_2;/* padding for even size */
}aeMeteringCommStruct, MSDK_CCT_AE_METERING_COMM_STRUCT, *PMSDK_CCT_AE_METERING_COMM_STRUCT;

typedef struct
{
	/* method select */
	UINT8 aeSmoothMethod;/* 0:disabled, 1:enable */
	UINT8 aeSmoothFilterTape;/* media filter tape no, NA for curve like enable */
	/* normal */
	UINT8 aeSmoothNormalLastWeight;/* 0~128 */
	UINT8 aeSmoothNormalMaxStep;/* 1~4, NA for curve like enable */
	/* fast */
	UINT8 aeSmoothFastLastWeight;/* 0~128, NA for curve like enable */
	UINT8 aeSmoothFastMaxStep;/* 5~40, NA for curve like enable */
	UINT8 aeSmoothFastMaxChangeStep;/* 5~40, NA for curve like enable */
	UINT8 aeSmoothFastCheckCount;/* 0~3 */
	UINT8 aeSmoothFastBackCheckCount;/* 0~3, NA for curve like enable */
	UINT8 aeSmoothFastBackCount;/* 0~3, NA for curve like enable */
	UINT8 aeSmoothFastTransitionCount;/* 0~3, NA for curve like enable */
	UINT8 aeSmoothNormalToFastTh;/* 1~4 */
	UINT8 aeSmoothFastToNormalTh;/* 5~15*/
	/* 6328 new smooth mode */
	UINT8 aeSmoothFastSmoothEnable;/* 0: disable, 1:enable */
	UINT8 aeSmoothNormalSpeedEnable;/* 0: disable, 1:enable */
	UINT8 aeSmoothMovingStopCheckEnable;/* 0: disable, 1:enable */
	UINT8 aeSmoothMovingStopNormalCheckCount;/* 1~4 check count */
	UINT8 aeSmoothMovingStopFastCheckCount;/* 1~4 check count */
	UINT8 reserve_1;/* padding for even size */
	UINT8 reserve_2;/* padding for even size */
}aeSmoothModeStruct;

typedef struct
{
	aeIsoLutStruct aeIsoPara;/* 2*6=12 */
	aeMeteringCommStruct aeMeteringComm;/* 22+5*64=342 */
	aeSmoothModeStruct ae_smooth[AE_SELECT_MODE_NO];/* 3*20=60 */
	UINT8 reserve_1[66];/* 480-12-342-60=66 */
} nvram_ae_para_struct, NVRAM_AE_PARA_STRUCT, *PNVRAM_AE_PARA_STRUCT;

//AWB
typedef struct
{
    UINT32      reg[30];//0x280~0x2F8
	UINT16	LightSource[7][4];	/* { Rgain, GRgain, Bgain, GBgain} */
	                                                        /*A, TL84,CWF,D65,D75,Flash,Manual*/
	UINT8        AWB_speed_fast;    /*awb converge ratio in 1 step in fast state
	                                                      when gain difference is higher than AWB_thres_fast, 128 base*/
	UINT8        AWB_speed_mid;     /*awb converge ratio in 1 step in fast state
	                                                      when gain difference is between AWB_thres_mid and AWB_thres_fast, 128 base*/
	UINT8        AWB_speed_slow;    /*awb converge ratio in 1 step in fast state
	                                                      when gain difference is smaller than AWB_thres_mid , 128 base*/
	UINT8        AWB_thres_fast;      /*gain difference for AWB_speed_fast*/
	UINT8        AWB_thres_mid;      /*gain difference for AWB_speed_mid*/
	UINT8        AWBPreference[3][3];  /*A, TL8, Daylight; RcompRatio,GcompRatio,BcompRatio*/
	UINT8        change_state_gain_thr;  /*gain difference between target & current to enter change state*/
	UINT8        change_state_frame_thr; /*frame number to enter change state when gain difference is over threshold*/
	UINT8        stable_state_gain_thr;  /*gain difference between target & current to enter stable state*/
	UINT8        stable_state_frame_thr;  /*frame number to enter stable state when gain difference is smaller than threshold*/
	UINT8        finetune_state_gain_thr; /*gain difference between target & current to enter finetune state*/
	UINT8        finetune_state_frame_thr;/*frame number to enter finetuen state when gain difference is over threshold*/
	UINT8        awbSmFrameNum;  /*Frame number used to smooth current target gain*/
	UINT8        AWBPreferEn;    /*Enable awb preference**/
	UINT8        MWBFixedWinEn;  /*0: fixed gain for MWB, 1:fixed window for MWB*/
	UINT8        ALightMethod;//0: Use TL84, 1: Use Small A box
    UINT8        awbSmallAWinEn; //use small a window
    UINT8        awbSmallTL84WinEn; //use small TL84 window
    UINT8        awbSmallCWFWinEn; //use small CWF window
    UINT8        awbSmallDWinEn; //use small Day window
    UINT8        awbCaptureFastConvergeEn;//capture fast converge
	UINT8        reserved[25];
}awb_v2_para_struct, AWB_V2_PARA_STRUCT, *PAWB_V2_PARA_STRUCT;

typedef struct
{
	UINT8       LightSource[3][3][3];
	UINT8       CCMPreferEn;
}ccm_para_struct, CCM_PARA_STRUCT, *PCCM_PARA_STRUCT;

typedef struct
{
	awb_v2_para_struct				AWB;
	ccm_para_struct                 CCM;
}nvram_awb_para_struct, NVRAM_AWB_PARA_STRUCT, *PNVRAM_AWB_PARA_STRUCT;

// AF

typedef struct {
	UINT16		af_calibration_offset;
	UINT16		af_calibration_data;
} nvram_lens_calibration_struct;

typedef struct
{
	UINT16		af_table_num;
	UINT16		af_home_idx;
	UINT16		af_macro_idx;
	UINT16		af_infinite_idx;
	UINT16		af_hyper_pos;
	UINT16		af_me_home_pos;
	UINT16		af_me_macro_pos;
	UINT16		af_filter_type;
} af_table_range_struct;

typedef struct
{
	UINT16	pos;
	UINT16	distance;
} af_table_step_struct;

typedef struct {
	af_table_range_struct	table_range;
	af_table_step_struct	table_step[AF_MAX_TABLE_NO];
} nvram_af_table_info_struct;

typedef struct
{
   	UINT16 dof_lut_size;
   	UINT16 dof_lut[AF_MZ_DOF_MAX_NO][AF_MZ_DOF_LUT_IDX_NO];/* 0: step, 1:macro, 2:inf */
}nvram_af_mz_dof_info_struct;

typedef struct
{
	nvram_lens_calibration_struct 	lens_calibration;			// 1 words
	nvram_af_table_info_struct		table_info[AF_MAX_DEF_PATH_NO]; 	// provide 4+24 words * 3 = 84 words
	nvram_af_mz_dof_info_struct		dof_table_info;/* 2+3*2*8 = 50 bytes = 25 words */
	UINT16							para_info[AF_PARAM_MAX_NO];	// 32/2 = 16 words + 3 words
} nvram_af_para_struct, NVRAM_AF_PARA_STRUCT, *PNVRAM_AF_PARA_STRUCT;

typedef struct
{
	UINT32 Version;
    NVRAM_AE_PARA_STRUCT	AE;
	NVRAM_AWB_PARA_STRUCT	AWB;
	NVRAM_AF_PARA_STRUCT	AF;
	UINT8 CameraData[MAXIMUM_NVRAM_CAMERA_3A_FILE_SIZE-4-sizeof(NVRAM_AE_PARA_STRUCT)-sizeof(NVRAM_AWB_PARA_STRUCT)-sizeof(NVRAM_AF_PARA_STRUCT)];
} NVRAM_CAMERA_3A_STRUCT, *PNVRAM_CAMERA_3A_STRUCT;
#endif

/*******************************************************************************
* camera parameter
********************************************************************************/
// camera common parameters and sensor parameters
typedef struct
{
    UINT32 CommReg[64];// 0~30 reserve for compatiblilty with WCP1 setting; 
                                      // [31] for table defect control 0x0154h
                                      // [32] for defect table address 0x0158h
} ISP_COMMON_PARA_STRUCT, *PISP_COMMON_PARA_STRUCT;

typedef struct
{
    UINT8 Shading;
    UINT8 NR1;
    UINT8 NR2;
    UINT8 Edge;
    UINT8 AutoDefect;
    UINT8 Saturation;
    UINT8 Contrast;
    UINT8 CCM;
    UINT8 Gamma;
    UINT8 Reserved;
}	ISP_TUNING_INDEX_STRUCT, *PISP_TUNING_INDEX_STRUCT;

#define NVRAM_SHADING_TBL_NUM            3
#define NVRAM_NR1_TBL_NUM                    7
#define NVRAM_NR2_TBL_NUM                    7
#define NVRAM_EDGE_TBL_NUM                  7
#define NVRAM_DEFECT_TBL_NUM              3
#define NVRAM_SAT_TBL_NUM                    7
#define NVRAM_CONTRAST_TBL_NUM          3
#define NVRAM_CCM_TBL_NUM                    3
#define NVRAM_GAMMA_TBL_NUM               5

typedef struct
{
    ISP_TUNING_INDEX_STRUCT Idx;
    UINT32 ShadingReg[NVRAM_SHADING_TBL_NUM][MT6516ISP_LSC_REGISTERCNT];//binning
    UINT32 NR1Reg[NVRAM_NR1_TBL_NUM][MT6516ISP_NR1_REGISTERCNT];
    UINT32 NR2Reg[NVRAM_NR2_TBL_NUM][MT6516ISP_NR2_REGISTERCNT];
    UINT32 EdgeReg[NVRAM_EDGE_TBL_NUM][MT6516ISP_EE_REGISTERCNT];
    UINT32 AutoDefect[NVRAM_DEFECT_TBL_NUM][MT6516ISP_NR1_REGISTERCNT]; //AD is in NR1
    UINT32 Saturation[NVRAM_SAT_TBL_NUM][MT6516ISP_YCCGO_REGISTERCNT]; //YCCGO ENC3/Y1-4/G1-5
    UINT32 Contrast[NVRAM_CONTRAST_TBL_NUM][MT6516ISP_YCCGO_REGISTERCNT]; //YCCGO  ENY3/OFSTY/GAINY
    UINT32 CCM[NVRAM_CCM_TBL_NUM][MT6516ISP_CCM_REGISTERCNT];
    UINT32 Gamma[NVRAM_GAMMA_TBL_NUM][MT6516ISP_GAMMA_REGISTERCNT];
} ISP_TUNING_PARA_STRUCT, *PISP_TUNING_PARA_STRUCT;

typedef struct
{
	UINT32 TargetTime;/*minimum exposure time, unit:(us)*/
	UINT32 ShutterDelayTime; /*shutter delay time*/
}	SHUTTER_DELAY_STRUCT, *PSHUTTER_DELAY_STRUCT;

typedef struct
{
	UINT32 Version;
	UINT32 SensorId;		// ID of sensor module
    ISP_COMMON_PARA_STRUCT  ISPComm;
    ISP_TUNING_PARA_STRUCT  ISPTuning;
    SHUTTER_DELAY_STRUCT MShutter;
    UINT8 CameraData[MAXIMUM_NVRAM_CAMERA_PARA_FILE_SIZE-8-sizeof(ISP_COMMON_PARA_STRUCT)-sizeof(ISP_TUNING_PARA_STRUCT)-sizeof(SHUTTER_DELAY_STRUCT)];
} NVRAM_CAMERA_PARA_STRUCT, *PNVRAM_CAMERA_PARA_STRUCT;

#define CAL_GET_DEFECT_FLAG  0x01
#define CAL_GET_3ANVRAM_FLAG 0x02
#define CAL_GET_SHADING_FLAG 0x04
#define CAL_GET_PARA_FLAG    0x08
#define CAL_DATA_LOAD           0x6C6F6164//"load"
#define CAL_DATA_UNLOAD         0x00000000
#define CAL_SHADING_TYPE_SENSOR 0x216D746B//"!mtk"
#define CAL_SHADING_TYPE_ISP    0x3D6D746B//"=mtk"

typedef struct
{
	PNVRAM_CAMERA_DEFECT_STRUCT pCameraDefect;
	PNVRAM_CAMERA_SHADING_STRUCT pCameraShading;
	PNVRAM_CAMERA_PARA_STRUCT pCameraPara;
	PNVRAM_CAMERA_3A_STRUCT pCamera3ANVRAMData;
} GET_SENSOR_CALIBRATION_DATA_STRUCT, *PGET_SENSOR_CALIBRATION_DATA_STRUCT;

#if 0 //@Sean, defined in kd_imgsensor_definition.h 
#define MAX_SHADING_DATA_TBL ((MAX_SENSOR_CAL_SIZE-8)/4)
typedef struct
{
	UINT32 DataFormat;
	UINT32 DataSize;
	UINT32 ShadingData[MAX_SHADING_DATA_TBL];
} SET_SENSOR_CALIBRATION_DATA_STRUCT, *PSET_SENSOR_CALIBRATION_DATA_STRUCT;
#endif 

/*******************************************************************************
*
*******************************************************************************/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WinMo Structure WinMo Structure WinMo Structure WinMo Structure WinMo Structure camera common parameters and sensor parameters
typedef struct
{
    UINT32 CommReg[64];
} WINMO_ISP_COMMON_PARA_STRUCT, *PWINMO_ISP_COMMON_PARA_STRUCT;

typedef struct
{
    UINT8 Shading;
    UINT8 NR1;
    UINT8 NR2;
    UINT8 Edge;
    UINT8 AutoDefect;
    UINT8 Saturation;
    UINT8 Contrast;
    UINT8 Reserved;
} WINMO_ISP_TUNING_INDEX_STRUCT, *PWINMO_ISP_TUNING_INDEX_STRUCT;

typedef struct
{
    WINMO_ISP_TUNING_INDEX_STRUCT Idx;
    UINT32 ShadingReg[3][5];//binning       //ok
    UINT32 NR1Reg[7][11];
    UINT32 NR2Reg[7][5];
    UINT32 EdgeReg[7][3];
    UINT32 AutoDefect[3][5];
    UINT32 Saturation[7][4];
    UINT32 Contrast[3][3];
} WINMO_ISP_TUNING_PARA_STRUCT, *PWINMO_ISP_TUNING_PARA_STRUCT;

typedef struct
{
    UINT32 TargetTime;/*minimum exposure time, unit:(us)*/
    UINT32 ShutterDelayTime; /*shutter delay time*/
} WINMO_SHUTTER_DELAY_STRUCT, *PWINMO_SHUTTER_DELAY_STRUCT;

typedef struct
{
    UINT32 Version;
    WINMO_ISP_COMMON_PARA_STRUCT  ISPComm;
    WINMO_ISP_TUNING_PARA_STRUCT  ISPTuning;
    WINMO_SHUTTER_DELAY_STRUCT MShutter;
    UINT8 CameraData[MAXIMUM_NVRAM_CAMERA_PARA_FILE_SIZE-4-sizeof(ISP_COMMON_PARA_STRUCT)-sizeof(ISP_TUNING_PARA_STRUCT)-sizeof(SHUTTER_DELAY_STRUCT)];
} WINMO_NVRAM_CAMERA_PARA_STRUCT, *PWINMO_NVRAM_CAMERA_PARA_STRUCT;

//WinMo NVRAM data structure define end
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*******************************************************************************
*
********************************************************************************/
typedef enum
{
    CAMERA_NVRAM_DATA_PARA,
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

} NVRAM_LENS_PARA_STRUCT, *PNVRAM_LENS_PARA_STRUCT;




#endif // _CAMERA_CUSTOM_NVRAM_MT6516_H_

