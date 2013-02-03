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

#ifndef _MTK_3A_ERRCODE_H
#define _MTK_3A_ERRCODE_H

///////////////////////////////////////////////////////////////////////////////                     
//!  Error code formmat is:
//!
//!  Bit 31~24 is global, each module must follow it, bit 23~0 is defined by module
//!  | 31(1 bit) |30-24(7 bits) |         23-0   (24 bits)      |
//!  | Indicator | Module ID    |   Module-defined error Code   |
//!  
//!  Example 1:
//!  | 31(1 bit) |30-24(7 bits) |   23-16(8 bits)   | 15-0(16 bits) |
//!  | Indicator | Module ID    | group or sub-mod  |    Err Code   |
//! 
//!  Example 2:
//!  | 31(1 bit) |30-24(7 bits) | 23-12(12 bits)| 11-8(8 bits) | 7-0(16 bits)  |
//!  | Indicator | Module ID    |   line number |    group     |    Err Code   |
//! 
//!  Indicator  : 0 - success, 1 - error
//!  Module ID  : module ID, defined below
//!  Extended   : module dependent, but providee macro to add partial line info
//!  Err code   : defined in each module's public include file,
//!               IF module ID is MODULE_COMMON, the errocode is
//!               defined here
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//! Error code type definition
///////////////////////////////////////////////////////////////////////////
typedef MINT32 MRESULT;

///////////////////////////////////////////////////////////////////////////
//! Helper macros to define error code
///////////////////////////////////////////////////////////////////////////
#define ERROR_CODE_DEFINE(modid, errid)           \
  ((MINT32)                              \
    ((MUINT32)(0x80000000) |             \
     (MUINT32)((modid & 0x7f) << 24) |   \
     (MUINT32)(errid & 0xffff))          \
  )

#define OK_CODE_DEFINE(modid, okid)             \
  ((MINT32)                              \
    ((MUINT32)(0x00000000) |             \
     (MUINT32)((modid & 0x7f) << 24) |   \
     (MUINT32)(okid & 0xffff))           \
  )

///////////////////////////////////////////////////////////////////////////
//! Helper macros to check error code
///////////////////////////////////////////////////////////////////////////
#define SUCCEEDED(Status)   ((MRESULT)(Status) >= 0)
#define FAILED(Status)      ((MRESULT)(Status) < 0)

#define MODULE_MTK_3A (0) // Temp value

#define MTK3A_OKCODE(errid)         OK_CODE_DEFINE(MODULE_MTK_3A, errid)
#define MTK3A_ERRCODE(errid)        ERROR_CODE_DEFINE(MODULE_MTK_3A, errid)


// 3A error code
#define S_3A_OK                  MTK3A_OKCODE(0x0000)     

#define E_3A_NEED_OVER_WRITE     MTK3A_ERRCODE(0x0001)
#define E_3A_NULL_OBJECT         MTK3A_ERRCODE(0x0002)
#define E_3A_WRONG_STATE         MTK3A_ERRCODE(0x0003)
#define E_3A_WRONG_CMD_ID        MTK3A_ERRCODE(0x0004)
#define E_3A_WRONG_CMD_PARAM     MTK3A_ERRCODE(0x0005)

#define E_3A_ERR                 MTK3A_ERRCODE(0x0100)

// AE error code
#define S_AE_OK                     MTK3A_OKCODE(0x0000)     
#define S_AE_CANNOT_FIND_TABLE      MTK3A_OKCODE(0x0101)

#define E_AE_WRONG_STATE            MTK3A_ERRCODE(0x0101)
#define E_AE_NULL_AE_TABLE          MTK3A_ERRCODE(0x0102)
#define E_AE_DISALBE                MTK3A_ERRCODE(0x0103)
#define E_AE_HIST_ROI_ERR           MTK3A_ERRCODE(0x0104)
#define E_AE_NOMATCH_TABLE          MTK3A_ERRCODE(0x0105)
#define E_AE_ISO_NOT_SUPPORT        MTK3A_ERRCODE(0x0106)
#define E_AE_CAPTURE_AE_WRONG       MTK3A_ERRCODE(0x0107)
#define E_AE_PARAMETER_ERROR        MTK3A_ERRCODE(0x0108)
#define E_AE_NULL_BUFFER            MTK3A_ERRCODE(0x0109)

// AF error code
#define S_AF_OK                     MTK3A_OKCODE(0x0000)     
#define E_AF_BUSY                   MTK3A_ERRCODE(0x0201)
#define E_AF_INPUT                  MTK3A_ERRCODE(0x0202)
#define E_AF_NOSUPPORT              MTK3A_ERRCODE(0x0203)
#define E_AF_MCU_STOP_ERR           MTK3A_ERRCODE(0x0204)
#define E_AF_MCU_PARA_ERR           MTK3A_ERRCODE(0x0205)
#define E_AF_FOCUS_MOVE_ERR         MTK3A_ERRCODE(0x0206)
#define E_AF_ZOOM_MOVE_ERR          MTK3A_ERRCODE(0x0207)

#define E_AF_MCU_NULL               MTK3A_ERRCODE(0x0208)
#define E_AF_DRV_NULL               MTK3A_ERRCODE(0x0209)

#define E_AF_STEP_SIZE              MTK3A_ERRCODE(0x0210)
#define E_AF_SPOT_IDX               MTK3A_ERRCODE(0x0211)
#define E_AF_GPIO_ERR               MTK3A_ERRCODE(0x0212)
#define E_AF_WINSET_ERR             MTK3A_ERRCODE(0x0213)

// AWB error code
#define S_AWB_OK                     MTK3A_OKCODE(0x0000)     
#define E_AWB_DISABLE                MTK3A_ERRCODE(0x0301)
#define E_AWB_STATISTIC_ERROR        MTK3A_ERRCODE(0x0302)
#define E_AWB_UNSUPPORT_MODE         MTK3A_ERRCODE(0x0303)
#define E_AWB_STATISTIC_CONFIG_ERROR MTK3A_ERRCODE(0x0304)
#define E_AWB_PARAMETER_ERROR        MTK3A_ERRCODE(0x0305)
#define E_AWB_NULL_POINTER           MTK3A_ERRCODE(0x0306)
#define E_STROBE_AWB_PARAMETER_ERROR MTK3A_ERRCODE(0x0307)
#endif

