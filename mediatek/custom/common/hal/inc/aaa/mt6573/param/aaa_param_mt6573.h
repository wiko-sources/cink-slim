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

#ifndef _AAA_PARAM_MT6573_H
#define _AAA_PARAM_MT6573_H

#include "../../../camera_custom_types.h"
#include "../../../camera_custom_nvram.h"
#include "../../../camera_custom_flashlight.h"
#include "../../aaa_error_code.h"
#include "../../aaa_feature.h"

typedef float          MFLOAT;
typedef double         MDOUBLE;
typedef void           MVOID;
typedef signed int     MRESULT;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef NULL
#define NULL 0
#endif

#define PC_SIMU  (0) // PC Simulation

#define BIT(pos) (1 << (pos))

#define FD_MAX_NO (11)

#define AAA_CYCLE_MAX (10)

typedef struct
{
    MUINT32 u4FieldID;
    MUINT32 u4FieldValue;
}  AAA_DEBUG_TAG_T;

typedef struct
{
    MUINT32 u4XOffset;
    MUINT32 u4YOffset;
    MUINT32 u4XWidth;
    MUINT32 u4YHeight;
} EZOOM_WIN_T;

typedef struct
{
	MBOOL  bFDon;
	MINT32 i4Width;
	MINT32 i4Height;

	struct
	{
		MBOOL  fgPrimary;
		MINT32 i4Left;
		MINT32 i4Up;
		MINT32 i4Right;
		MINT32 i4Bottom;
	} sWin[FD_MAX_NO];

} FD_INFO_T;

#include "ae_param_mt6573.h"
#include "af_param_mt6573.h"
#include "awb_param_mt6573.h"

// Used to deconstruct an object.
#define DESTROY_OBJECT( x )     \
{                               \
    if ( x )                    \
    {                           \
        delete x;               \
    }                           \
    x = NULL;                   \
}

// 3A state
typedef enum
{
    AAA_STATE_NONE = -3,    // Internal use:
    AAA_STATE_CREATE = -2,  // Internal use: set in App3A object constructor
    AAA_STATE_INIT = -1,    // Internal use: set in init3A()
    AAA_STATE_AUTO_FRAMERATE_PREVIEW = 0, // Parameter of set3AState(): PREVIEW (Auto Frame Rate) state
    AAA_STATE_MANUAL_FRAMERATE_PREVIEW,   // Parameter of set3AState(): PREVIEW (Manual Frame Rate) state
    AAA_STATE_AF,           // Parameter of set3AState(): AF state
    AAA_STATE_PRE_CAPTURE,  // Parameter of set3AState(): PRE_CAPTURE state (for pre-flash statistics)
    AAA_STATE_CAPTURE,      // Parameter of set3AState(): CAPTURE state (for capture AWB statistics)
    AAA_STATE_MOVIE_RECORD  // Parameter of set3AState(): MOVIE RECORD state
} AAA_STATE_T;

// Half-push state
typedef enum
{
    HALF_PUSH_STATE_NONE = -1,
    HALF_PUSH_STATE_ONE_SHOT_AEAWB = 0,
    HALF_PUSH_STATE_AF,
    HALF_PUSH_STATE_LOCK_3A
} HALF_PUSH_STATE_T;

// 3A cycle
typedef enum
{
    AAA_CYCLE_NONE         = 0,
    AAA_CYCLE_DUMMY        = BIT(0),
    AAA_CYCLE_AE           = BIT(1),
    AAA_CYCLE_AWB          = BIT(2),
    AAA_CYCLE_AF           = BIT(3),
    AAA_CYCLE_LCE          = BIT(4),
    AAA_CYCLE_AEAWB_CONFIG = BIT(5),
    AAA_CYCLE_LCE_CONFIG   = BIT(6),
    AAA_CYCLE_INFO_ASD     = BIT(7), // Send information to ASD (auto scene detect)
    AAA_CYCLE_MDE          = BIT(8)  // Proces MDE (motion detect engine)
} AAA_CYCLE_T;

// 3A statistics
typedef struct
{
    MUINT32 u4RawWidth;
    MUINT32 u4RawHeight;
    AE_STAT_T rAEStat;
    AWB_STAT_T rAWBStat;
    AF_STAT_T rAFStat;
} AAA_STAT_T;

// Frame-based input parameters for handle3A()
typedef struct
{
    AAA_STAT_T r3AStat;
	FOCUS_INFO_T sFocusInfo;
    FD_INFO_T sFDInfo;
    EZOOM_WIN_T sEZoom;
} AAA_FRAME_INPUT_PARAM_T;

// Frame-based output parameters for handle3A()
typedef struct
{
    MBOOL bUpdateAE; // Update AE setting: iris, exposure time, gain
    MBOOL bUpdateAWB; // Update AWB setting: AWB gain
    MBOOL bUpdateAF; // Update AF setting: target AF motor step
    MBOOL bAFDone; // TRUE: AF is finished
    MBOOL bUpdateAEStatConfig; // Update AE statistics config parameter
    MBOOL bUpdateAWBStatConfig; // Update AWB statistics config parameter
    MBOOL bUpdateAFStatConfig; // Update AF statistics config parameter
    MBOOL bReadyForCapture; // TRUE: 3A for capture is ready
    MINT32 u4WaitVDNum; // Delayed VD number for next 3A statistics
    AE_OUTPUT_T rAEOutput;
    AWB_OUTPUT_T rAWBOutput;
    AF_OUTPUT_T rAFOutput;
    AE_STAT_CONFIG_T rAEStatConfig;
    AWB_STAT_CONFIG_T rAWBStatConfig;
} AAA_OUTPUT_PARAM_T;

// 3A statistics config data structure
typedef struct
{
    AE_STAT_CONFIG_T rAEStatConfig;
    AWB_STAT_CONFIG_T rAWBStatConfig;
    AF_STAT_CONFIG_T  rAFStatConfig;
} AAA_STAT_CONFIG_T;

// 3A statistics config parameters
typedef struct
{
    AE_STAT_CONFIG_T rAEStatConfig;
    AWB_STAT_CONFIG_T rAWBStatConfig[LIB3A_AWB_MODE_NUM];
	AF_STAT_CONFIG_T  rAFStatConfig;
} AAA_STAT_CONFIG_PARAM_T, *P3A_STAT_CONFIG_PARAM_T;

// 3A non-NVRAM parameter
typedef struct
{
   MBOOL bEnableAE; // Enable AE
   MBOOL bEnableAF; // Enable AF
   MBOOL bEnableAWB; // Enable AWB
   MUINT32 u4AAACycle[AAA_CYCLE_MAX]; // 3A cycle
   AE_PARAM_T rAEParam; // AE algorithm parameter
   AWB_PARAM_T rAWBParam; // AWB algorithm parameter
   AF_PARAM_T rAFParam; // AF algorithm parameter
   AEPF_PARAM_T rAEPFParam; //AE preflash parameter
} AAA_PARAM_T, *P3A_PARAM_T;

// 3A init input parameters for init3A()
typedef struct
{
	NVRAM_CAMERA_3A_STRUCT r3ANVRAMData;
	NVRAM_LENS_PARA_STRUCT rAFNVRAMData;
	AAA_PARAM_T r3AParam;
	AAA_STAT_CONFIG_PARAM_T r3AStatConfigParam;
    AE_INITIAL_INPUT_T rAEIniPara;
	FOCUS_INFO_T sFocusInfo;
} AAA_INIT_INPUT_PARAM_T;

// 3A calibration data ==> removed later
typedef struct
{
   MBOOL bAWBCalDone;
   AWB_CALIBRATION_DATA_T rAWB;
} AAA_CALIBRATION_DATA_T;


// 3A debug info
#define AAATAG(module_id, tag, line_keep)   \
( (MINT32)                                  \
  ((MUINT32)(0x00000000) |                  \
   (MUINT32)((module_id & 0xff) << 24) |    \
   (MUINT32)((line_keep & 0x01) << 23) |    \
   (MUINT32)(tag & 0xffff))                 \
)

#define MODULE_NUM(total_module, tag_module)      \
((MINT32)                                         \
 ((MUINT32)(0x00000000) |                         \
  (MUINT32)((total_module & 0xff) << 16) |        \
  (MUINT32)(tag_module & 0xff))                   \
)

#define AAA_DEBUG_AE_MODULE_ID   0x0001
#define AAA_DEBUG_AF_MODULE_ID   0x0002
#define AAA_DEBUG_AWB_MODULE_ID  0x0003

#define AAA_DEBUG_KEYID 0xF0F1F2F3

#define AAA_DEBUG_HEADER_SIZE (5*4)

typedef struct
{
    MUINT32  u4KeyID;
    MUINT32  u4ModuleCount;
    MUINT32  u4AEDebugInfoOffset;
    MUINT32  u4AFDebugInfoOffset;
    MUINT32  u4AWBDebugInfoOffset;

    AE_DEBUG_INFO_T  rAEDebugInfo;
    AF_DEBUG_INFO_T  rAFDebugInfo;
    AWB_DEBUG_INFO_T rAWBDebugInfo;
} AAA_DEBUG_INFO_T;

// meta mode 3A data structure
typedef struct
{
    MUINT8 *puParaIn;
    MUINT8 *puParaOut;
    MUINT32 u4ParaInLen;
    MUINT32 u4ParaOutLen;
    MUINT32 *pu4RealParaOutLen;
} AAA_META_FEATURE_INFO_STRUCT;

// 3A EXIF info data structure
typedef struct
{
    MUINT32 u4FNumber; // Format: F2.8 = 28
    LIB3A_AE_MODE_T eAEMode;
    LIB3A_AE_ISO_SPEED_T eAEISOSpeed;
    LIB3A_AE_EVCOMP_T eAEComp;
    LIB3A_AE_METERING_MODE_T eAEMeterMode;
    LIB3A_AWB_MODE_T eAWBMode;
    MUINT32 u4CapExposureTime;
    MUINT32 u4FlashLightTimeus;
    MUINT32 u4RealISOValue;
} AAA_EXIF_INFO_T;


#endif

