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

#ifndef _LIB3A_AAA_FEATURE_H
#define _LIB3A_AAA_FEATURE_H

#include "ae_feature.h"
#include "af_feature.h"
#include "awb_feature.h"

// 3A module UI command set
typedef struct
{
    // 3A command
    // AE command
    LIB3A_AE_MODE_T          eAEMode;
    LIB3A_AE_EVCOMP_T        eAEComp;
    LIB3A_AE_METERING_MODE_T eAEMeterMode;
    LIB3A_AE_ISO_SPEED_T     eAEISOSpeed;
    LIB3A_AE_STROBE_MODE_T   eAEStrobeMode;
    LIB3A_AE_REDEYE_MODE_T   eAERedeyeMode;
    LIB3A_AE_FLICKER_MODE_T  eAEFlickerMode;
    int eAEFrameRateMode;
    // AF command
    LIB3A_AF_MODE_T          eAFMode;
    LIB3A_AF_METER_T         eAFMeter;
    LIB3A_AF_ZONE_T          eAFZone;
    // AWB command
    LIB3A_AWB_MODE_T         eAWBMode;
} AAA_CMD_SET_T;

#endif
