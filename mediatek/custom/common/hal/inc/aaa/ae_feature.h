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

#ifndef _LIB3A_AE_FEATURE_H
#define _LIB3A_AE_FEATURE_H

// AE command ID: 0x1000 ~
typedef enum
{
    LIB3A_AE_CMD_ID_SET_AE_MODE                         = 0x1000, // Set AE Mode
    LIB3A_AE_CMD_ID_SET_AE_EVCOMP                     = 0x1001, // Set AE EV compensation
    LIB3A_AE_CMD_ID_SET_AE_METERING_MODE       = 0x1002, // Set AE metering mode
    LIB3A_AE_CMD_ID_SET_AE_ISOSPEED                   = 0x1003, // Set AE ISO speed
    LIB3A_AE_CMD_ID_SET_AE_STROBE_MODE            = 0x1004, // Set AE strobe mode
    LIB3A_AE_CMD_ID_SET_AE_REDEYE_MODE            = 0x1005,  // Set AE sedeye mode
    LIB3A_AE_CMD_ID_SET_AE_FLICKER_MODE           = 0x1006,  // Set Flicker frequency
    LIB3A_AE_CMD_ID_SET_AE_FRAMERATE_MODE     = 0x1007,   // Set Frame rate mode     
    LIB3A_AE_CMD_ID_AE_FPS_MAX                             =  0x1008,
    LIB3A_AE_CMD_ID_AE_FPS_MIN			         =  0x1009,
    LIB3A_AE_CMD_ID_AE_SUPPORT_FPS_NUM            = 0x100A,
    LIB3A_AE_CMD_ID_AE_SUPPORT_FPS_RANGE	  = 0x100B,
    LIB3A_AE_CMD_ID_SET_AE_FLASHLIGHT_TYPE      = 0x100C, // Set AE Flashlight type for strobe
    LIB3A_AE_CMD_ID_SET_AE_FLICKER_AUTO_MODE = 0x100D,
    LIB3A_AE_CMD_ID_AE_PREVIEW_MODE                  = 0x100E,  //default is the auto mode
}LIB3A_AE_CMD_ID_T;

// AE mode definition
typedef enum                            // ev mode , auto , TV,AV,SV
{
    LIB3A_AE_MODE_UNSUPPORTED          = -1,
    LIB3A_AE_MODE_OFF                           =  0,            // disable AE
    LIB3A_AE_MODE_AUTO                        =  1,           // auto mode   full auto ,EV ISO LCE .. is inactive
    LIB3A_AE_MODE_PROGRAM                  =  2,           // AE program mode , allow set EV ISO LCE ....
    LIB3A_AE_MODE_TV                             =  3,           // AE TV mode
    LIB3A_AE_MODE_AV                             =  4,           // AE AV mode
    LIB3A_AE_MODE_SV                             =  5,           // AE SV mode
    LIB3A_AE_MODE_VIDEO                       =  6,           // Video mode AE
    LIB3A_AE_MODE_NIGHT                       =  7,           // Night Scene mode
    LIB3A_AE_MODE_ACTION                     =  8,           // AE Action mode
    LIB3A_AE_MODE_BEACH                       =  9,           // AE beach mode
    LIB3A_AE_MODE_CANDLELIGHT           = 10,           // AE Candlelight mode
    LIB3A_AE_MODE_FIREWORKS               = 11,           // AE firework mode
    LIB3A_AE_MODE_LANDSCAPE               = 12,           // AE landscape mode
    LIB3A_AE_MODE_PORTRAIT		          = 13,			 // AE portrait mode
    LIB3A_AE_MODE_NIGHT_PORTRAIT     = 14,           // AE night portrait mode
    LIB3A_AE_MODE_PARTY                       = 15,           // AE party mode
    LIB3A_AE_MODE_SNOW                        = 16,           // AE snow mode
    LIB3A_AE_MODE_SPORTS                     = 17,           // AE sport mode
    LIB3A_AE_MODE_STEADYPHOTO          = 18,           // AE steadyphoto mode
    LIB3A_AE_MODE_SUNSET                     = 19,           // AE sunset mode
    LIB3A_AE_MODE_THEATRE                   = 20,           // AE theatre mode
    LIB3A_AE_MODE_ISO_ANTI_SHAKE     = 21,           // AE ISO anti shake mode
    LIB3A_AE_MODE_BRACKET_AE             = 22,
    LIB3A_AE_MODE_AUTO_PANORAMA     = 23,
    LIB3A_AE_MODE_HDR                           = 24,
    LIB3A_AE_MODE_TOTAL_NUM,
    LIB3A_AE_MODE_MIN = LIB3A_AE_MODE_OFF,
    LIB3A_AE_MODE_MAX = LIB3A_AE_MODE_HDR
} LIB3A_AE_MODE_T;

// AE EV compensation
typedef enum                            // enum  for evcompensate
{
    LIB3A_AE_EV_COMP_UNSUPPORTED = -1,
    LIB3A_AE_EV_COMP_00          =  0,           // Disable EV compenate
    LIB3A_AE_EV_COMP_03          =  1,           // EV compensate 0.3
    LIB3A_AE_EV_COMP_05          =  2,           // EV compensate 0.5
    LIB3A_AE_EV_COMP_07          =  3,           // EV compensate 0.7
    LIB3A_AE_EV_COMP_10          =  4,           // EV compensate 1.0
    LIB3A_AE_EV_COMP_13          =  5,           // EV compensate 1.3
    LIB3A_AE_EV_COMP_15          =  6,           // EV compensate 1.5
    LIB3A_AE_EV_COMP_17          =  7,           // EV compensate 1.7
    LIB3A_AE_EV_COMP_20          =  8,           // EV compensate 2.0
    LIB3A_AE_EV_COMP_25          =  9,           // EV compensate 2.5
    LIB3A_AE_EV_COMP_30          =  10,           // EV compensate 3.0
    LIB3A_AE_EV_COMP_35          =  11,           // EV compensate 3.5
    LIB3A_AE_EV_COMP_40          =  12,           // EV compensate 4.0
    LIB3A_AE_EV_COMP_n03         = 13,           // EV compensate -0.3
    LIB3A_AE_EV_COMP_n05         = 14,           // EV compensate -0.5
    LIB3A_AE_EV_COMP_n07         = 15,           // EV compensate -0.7
    LIB3A_AE_EV_COMP_n10         = 16,           // EV compensate -1.0
    LIB3A_AE_EV_COMP_n13         = 17,           // EV compensate -1.3
    LIB3A_AE_EV_COMP_n15         = 18,           // EV compensate -1.5
    LIB3A_AE_EV_COMP_n17         = 19,           // EV compensate -1.7
    LIB3A_AE_EV_COMP_n20         = 20,           // EV compensate -2.0
    LIB3A_AE_EV_COMP_n25         = 21,           // EV compensate -2.5
    LIB3A_AE_EV_COMP_n30         = 22,           // EV compensate -3.0
    LIB3A_AE_EV_COMP_n35         = 23,           // EV compensate -3.5
    LIB3A_AE_EV_COMP_n40         = 24,           // EV compensate -4.0
    LIB3A_AE_EV_COMP_TOTAL_NUM,
    LIB3A_AE_EV_COMP_MIN = LIB3A_AE_EV_COMP_00,
    LIB3A_AE_EV_COMP_MAX = LIB3A_AE_EV_COMP_n40
}LIB3A_AE_EVCOMP_T;

// AE metering mode
typedef enum                                  // enum for metering
{
    LIB3A_AE_METERING_MODE_UNSUPPORTED    = -1,
    LIB3A_AE_METERING_MODE_CENTER_WEIGHT,           // CENTER WEIGHTED MODE
    LIB3A_AE_METERING_MODE_SOPT,                    // SPOT MODE
    LIB3A_AE_METERING_MODE_AVERAGE,                 // AVERAGE MODE
    LIB3A_AE_METERING_MODE_MULTI,                   // MULTI MODE
    LIB3A_AE_METERING_MODE_NUM,
    LIB3A_AE_METERING_MODE_MIN = LIB3A_AE_METERING_MODE_CENTER_WEIGHT,
    LIB3A_AE_METERING_MODE_MAX = LIB3A_AE_METERING_MODE_MULTI
}LIB3A_AE_METERING_MODE_T;

// AE ISO speed
typedef enum
{
    LIB3A_AE_ISO_SPEED_UNSUPPORTED =     -1,
    LIB3A_AE_ISO_SPEED_AUTO                =      0,
    LIB3A_AE_ISO_SPEED_50                     =     50,
    LIB3A_AE_ISO_SPEED_60                     =     60,
    LIB3A_AE_ISO_SPEED_64                     =     64,
    LIB3A_AE_ISO_SPEED_80                     =     80,    // 5
    LIB3A_AE_ISO_SPEED_100                   =    100,
    LIB3A_AE_ISO_SPEED_150                   =    150,    
    LIB3A_AE_ISO_SPEED_200                   =    200,
    LIB3A_AE_ISO_SPEED_300                   =    300,    
    LIB3A_AE_ISO_SPEED_400                   =    400,
    LIB3A_AE_ISO_SPEED_600                   =    600,    
    LIB3A_AE_ISO_SPEED_800                   =    800,
    LIB3A_AE_ISO_SPEED_1600                 =   1600,   // 10
    LIB3A_AE_ISO_SPEED_3200                 =   3200,
    LIB3A_AE_REAL_ISO_SPEED_100         =  10000,
    LIB3A_AE_REAL_ISO_SPEED_150         =  15000,
    LIB3A_AE_REAL_ISO_SPEED_200         =  20000,
    LIB3A_AE_REAL_ISO_SPEED_300         =  30000,  // 15
    LIB3A_AE_REAL_ISO_SPEED_400         =  40000,
    LIB3A_AE_REAL_ISO_SPEED_600         =  60000,
    LIB3A_AE_REAL_ISO_SPEED_800         =  80000,
    LIB3A_AE_REAL_ISO_SPEED_1600       =  160000,
    LIB3A_AE_ISO_SPEED_TOTAL_NUM=19,
    LIB3A_AE_ISO_SPEED_MIN = LIB3A_AE_ISO_SPEED_AUTO,
    LIB3A_AE_ISO_SPEED_MAX = LIB3A_AE_REAL_ISO_SPEED_1600
}LIB3A_AE_ISO_SPEED_T;

// AE strobe mode
typedef enum
{
    LIB3A_AE_STROBE_MODE_UNSUPPORTED = -1,
    LIB3A_AE_STROBE_MODE_AUTO        =  0,
    LIB3A_AE_STROBE_MODE_SLOWSYNC    =  0, //NOW DO NOT SUPPORT SLOW SYNC, TEMPERALLY THE SAME WITH AUTO
    LIB3A_AE_STROBE_MODE_FORCE_ON    =  1,
    LIB3A_AE_STROBE_MODE_FORCE_OFF   =  2,
    LIB3A_AE_STROBE_MODE_FORCE_TORCH =  3,
    LIB3A_AE_STROBE_MODE_REDEYE      =  4,
    LIB3A_AE_STROBE_MODE_TOTAL_NUM,
    LIB3A_AE_STROBE_MODE_MIN = LIB3A_AE_STROBE_MODE_AUTO,
    LIB3A_AE_STROBE_MODE_MAX = LIB3A_AE_STROBE_MODE_FORCE_TORCH //cotta-- modified to TORCH for strobe in video
}LIB3A_AE_STROBE_MODE_T;

// AE red eye mode
typedef enum
{
    LIB3A_AE_REDEYE_MODE_UNSUPPORTED = -1,
    LIB3A_AE_REDEYE_MODE_OFF,
    LIB3A_AE_REDEYE_MODE_ON,
    LIB3A_AE_REDEYE_MODE_TOTAL_NUM,
    LIB3A_AE_REDEYE_MODE_MIN = LIB3A_AE_REDEYE_MODE_OFF,
    LIB3A_AE_REDEYE_MODE_MAX = LIB3A_AE_REDEYE_MODE_ON
}LIB3A_AE_REDEYE_MODE_T;

// AE set flicker mode
typedef enum
{
    LIB3A_AE_FLICKER_MODE_UNSUPPORTED = -1,
    LIB3A_AE_FLICKER_MODE_60HZ,
    LIB3A_AE_FLICKER_MODE_50HZ,
    LIB3A_AE_FLICKER_MODE_AUTO,    // No support in MT6516
    LIB3A_AE_FLICKER_MODE_OFF,     // No support in MT6516
    LIB3A_AE_FLICKER_MODE_TOTAL_NUM,
    LIB3A_AE_FLICKER_MODE_MIN = LIB3A_AE_FLICKER_MODE_60HZ,
    LIB3A_AE_FLICKER_MODE_MAX = LIB3A_AE_FLICKER_MODE_OFF
}LIB3A_AE_FLICKER_MODE_T;

// AE set frame rate mode   //10base
typedef enum
{
    LIB3A_AE_FRAMERATE_MODE_UNSUPPORTED = -1,
    LIB3A_AE_FRAMERATE_MODE_DYNAMIC = 0,
    LIB3A_AE_FRAMERATE_MODE_04FPS = 40,
    LIB3A_AE_FRAMERATE_MODE_05FPS = 50,
    LIB3A_AE_FRAMERATE_MODE_15FPS = 150,
    LIB3A_AE_FRAMERATE_MODE_30FPS = 300,
//    LIB3A_AE_FRAMERATE_MODE_60FPS = 60,
    LIB3A_AE_FRAMERATE_MODE_TOTAL_NUM,
    LIB3A_AE_FRAMERATE_MODE_MIN = LIB3A_AE_FRAMERATE_MODE_DYNAMIC,
    LIB3A_AE_FRAMERATE_MODE_MAX = LIB3A_AE_FRAMERATE_MODE_30FPS
}LIB3A_AE_FRAMERATE_MODE_T;

// for flicker detection algorithm used only.
typedef enum
{
    LIB3A_AE_FLICKER_AUTO_MODE_UNSUPPORTED = -1,
//    LIB3A_AE_FLICKER_AUTO_MODE_CONTINUOUS,    	
    LIB3A_AE_FLICKER_AUTO_MODE_50HZ,
    LIB3A_AE_FLICKER_AUTO_MODE_60HZ,
    LIB3A_AE_FLICKER_AUTO_MODE_OFF,
    LIB3A_AE_FLICKER_AUTO_MODE_MIN = LIB3A_AE_FLICKER_AUTO_MODE_50HZ,
    LIB3A_AE_FLICKER_AUTO_MODE_MAX = LIB3A_AE_FLICKER_AUTO_MODE_OFF
}LIB3A_AE_FLICKER_AUTO_MODE_T;

// for preview mode selection used.
typedef enum
{
    LIB3A_AE_PREVIEW_MODE_UNSUPPORTED = -1,
    LIB3A_AE_PREVIEW_AUTO_MODE = 0,
    LIB3A_AE_PREVIEW_ZSD_MODE,
    LIB3A_AE_PREVIEW_MODE_MIN = LIB3A_AE_PREVIEW_AUTO_MODE,
    LIB3A_AE_PREVIEW_MODE_MAX = LIB3A_AE_PREVIEW_ZSD_MODE
}LIB3A_AE_PREVIEW_MODE_T;

#endif
