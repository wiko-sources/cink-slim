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

#include "camera_AE_para_ov2655_yuv.h"
#include "camera_AF_para_ov2655_yuv.h"
#include "camera_AWB_para_ov2655_yuv.h"

// 3A cycle
static UINT32 g_u4AAACycle[AAA_CYCLE_MAX] = 
{
    AAA_CYCLE_AWB,
    AAA_CYCLE_AE,
    AAA_CYCLE_AF, 
    AAA_CYCLE_NONE,
    AAA_CYCLE_NONE,
    AAA_CYCLE_NONE,
    AAA_CYCLE_NONE,
    AAA_CYCLE_NONE,
    AAA_CYCLE_NONE,
    AAA_CYCLE_NONE
};

//____3A Non-NVRAM Parameter____
static AAA_PARAM_T OV2655_YUV_CAMERA_3A_PARA = 
{
    TRUE, // Enable AE
    FALSE, // Enable AF
    TRUE, // Enable AWB
    
    // 3A cycle
    {
        g_u4AAACycle[0],
        g_u4AAACycle[1],
        g_u4AAACycle[2],
        g_u4AAACycle[3],
        g_u4AAACycle[4],
        g_u4AAACycle[5],
        g_u4AAACycle[6],
        g_u4AAACycle[7],
        g_u4AAACycle[8],
        g_u4AAACycle[9],
    },
    
    // AE algorithm parameter
    g_rAEParam,    
    
    // AWB algorithm parameter
    g_rAWBParam,

    // AF algorithm parameter
	g_rAFParam,

	// AE preflash algorithm parameter
	g_rAEPFParam
};

//____3A Statistics Config Parameter____
static AAA_STAT_CONFIG_PARAM_T OV2655_YUV_CAMERA_3A_STAT_CONFIG_PARAM =
{
    g_rAEPreStatConfig,
    {
	    g_rAWBStatConfig[AWB_MODE_AUTO],
	    g_rAWBStatConfig[AWB_MODE_DAYLIGHT],
	    g_rAWBStatConfig[AWB_MODE_CLOUDY_DAYLIGHT],
	    g_rAWBStatConfig[AWB_MODE_SHADE],
	    g_rAWBStatConfig[AWB_MODE_TWILIGHT],
	    g_rAWBStatConfig[AWB_MODE_FLUORESCENT],
	    g_rAWBStatConfig[AWB_MODE_WARM_FLUORESCENT],
	    g_rAWBStatConfig[AWB_MODE_INCANDESCENT]
    },
    g_rAFStatConfig   
};


