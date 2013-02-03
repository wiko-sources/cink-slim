#ifndef _CFG_FTBL_CUSTOM_YUV_SUB_H_
#define _CFG_FTBL_CUSTOM_YUV_SUB_H_


//  1: Enable the custom-specific features of YUV-Sub sensor.
//  0: Disable the custom-specific features of YUV-Sub sensor.
#define CUSTOM_FEATURE_YUV_SUB 1


namespace NSYUV     {
namespace NSSub    {
GETFINFO_YUV_SUB()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //..........................................................................

#if 1
	// AF Lamp
	CONFIG_FEATURE(FID_AF_LAMP, 
		BY_DEFAULT( AF_LAMP_OFF ), 
		AF_LAMP_OFF
	)
#endif

#if 0
    //  Scene Mode
    CONFIG_FEATURE(FID_SCENE_MODE, 
        BY_DEFAULT(SCENE_MODE_OFF), 
        SCENE_MODE_OFF, SCENE_MODE_NIGHTSCENE
    )
#endif
    //..........................................................................
#if 0
    //  Effect
    CONFIG_FEATURE(FID_COLOR_EFFECT, 
        BY_DEFAULT(MEFFECT_OFF), 
        MEFFECT_OFF,        MEFFECT_MONO,       MEFFECT_SEPIA, 
        MEFFECT_NEGATIVE,   MEFFECT_SEPIAGREEN, MEFFECT_SEPIABLUE
    )
#endif
    //..........................................................................
#if 1
    //  Capture Mode
    CONFIG_FEATURE(FID_CAPTURE_MODE, 
        BY_DEFAULT(CAPTURE_MODE_NORMAL), 
        CAPTURE_MODE_NORMAL,     CAPTURE_MODE_BURST_SHOT, 
        CAPTURE_MODE_SMILE_SHOT
    )
#endif
    //..........................................................................
#if 0
    //  Capture Size
    CONFIG_FEATURE(FID_CAP_SIZE, 
        BY_DEFAULT(CAPTURE_SIZE_2560_1920), 
        CAPTURE_SIZE_320_240,   CAPTURE_SIZE_640_480, 
        CAPTURE_SIZE_1024_768,  CAPTURE_SIZE_1280_960, 
        CAPTURE_SIZE_1600_1200, CAPTURE_SIZE_2048_1536, 
        CAPTURE_SIZE_2560_1920
    )
#endif
    //..........................................................................
#if 0
    //  Preview Size
    CONFIG_FEATURE(FID_PREVIEW_SIZE, 
        BY_DEFAULT(PREVIEW_SIZE_320_240), 
        PREVIEW_SIZE_176_144, PREVIEW_SIZE_320_240, 
        PREVIEW_SIZE_352_288, PREVIEW_SIZE_480_368,
        PREVIEW_SIZE_640_480, PREVIEW_SIZE_720_480, PREVIEW_SIZE_800_480, 
        PREVIEW_SIZE_864_480, PREVIEW_SIZE_1280_720
    )
#endif
	//..........................................................................
#if 0
	//	Video Preview Size
	CONFIG_FEATURE(FID_VIDEO_PREVIEW_SIZE, 
		BY_DEFAULT(VIDEO_PREVIEW_SIZE_640_480), 
		VIDEO_PREVIEW_SIZE_640_480, VIDEO_PREVIEW_SIZE_800_600
	)
#endif
    //..........................................................................
#if 0
    //  Frame Rate
    CONFIG_FEATURE(FID_FRAME_RATE, 
        BY_DEFAULT(FRAME_RATE_300FPS), 
        FRAME_RATE_150FPS, FRAME_RATE_300FPS
    )
#endif
    //..........................................................................
#if 0
    //  Frame Rate Range
    CONFIG_FEATURE(FID_FRAME_RATE_RANGE, 
        BY_DEFAULT(FRAME_RATE_RANGE_5_30_FPS), 
        FRAME_RATE_RANGE_5_30_FPS
    )
#endif
    //..........................................................................
#if 0
    //  Focus Distance Normal
    CONFIG_FEATURE(FID_FOCUS_DIST_NORMAL, 
        BY_DEFAULT(FOCUS_DIST_N_10CM), 
        FOCUS_DIST_N_10CM
    )
#endif
    //..........................................................................
#if 0
    //  Focus Distance Macro
    CONFIG_FEATURE(FID_FOCUS_DIST_MACRO, 
        BY_DEFAULT(FOCUS_DIST_M_5CM), 
        FOCUS_DIST_M_5CM
    )
#endif
    //..........................................................................
#if 0
    //  AE Flicker
    CONFIG_FEATURE(FID_AE_FLICKER, 
        BY_DEFAULT(AE_FLICKER_MODE_50HZ), 
        AE_FLICKER_MODE_60HZ, AE_FLICKER_MODE_50HZ
    )
#endif
    //..........................................................................
#if 1
    //  EIS
    CONFIG_FEATURE(FID_EIS, 
        BY_DEFAULT(EIS_OFF), 
        EIS_OFF
    )
#endif
    //..........................................................................
#if 1
    //	ZSD
    CONFIG_FEATURE(FID_ZSD, 
        BY_DEFAULT(ZSD_OFF), 
        ZSD_OFF, ZSD_OFF
    )
#endif    
    //==========================================================================
    //..........................................................................
#if 0
    //  AE ISO
    CONFIG_FEATURE(FID_AE_ISO, 
        BY_DEFAULT(AE_ISO_AUTO), 
        AE_ISO_AUTO
    )
#endif
    //..........................................................................
//------------------------------------------------------------------------------
END_GETFINFO_YUV_SUB()
};  //  namespace NSSub
};  //  namespace NSYUV


#endif // _CFG_FTBL_CUSTOM_YUV_SUB_H_

