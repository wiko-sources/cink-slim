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

#ifndef _AWB_TUNING_CUSTOM_MT6575_H
#define _AWB_TUNING_CUSTOM_MT6575_H

#include "../param/aaa_param_mt6575.h"

//____AWB Algorithm Parameters____

static AWB_PARAM_T g_rAWBParam =
{
	// Chip dependent parameter
	{
	    512, // u4AWBGainOutputScaleUnit: 1.0x = 512 for MT6575
	    2047, // u4AWBGainOutputUpperLimit: format 2.9 (11 bit) for MT6575
	    128  // i4RotationMatrixUnit: 1.0x = 128 for MT6575
	},

    // AWB Light source probability look-up table (Max: 100; Min: 0)
	{
        AWB_LV_INDEX_NUM, // i4SizeX: horizontal dimension
	    AWB_LIGHT_NUM, // i4SizeY: vertical dimension
	    // LUT
		{ // LV0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
			{100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  66,  33,   1,   1,   1,   1,   1,   1,   1}, // Tungsten
			{100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  66,  33,   1,   1,   1,   1,   1,   1,   1}, // Warm fluorescent
			{100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  66,  33,   1,   1,   1,   1,   1,   1,   1}, // Fluorescent
			{100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  66,  33,   1,   1,   1,   1,   1,   1,   1}, // CWF
			{100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100}, // Daylight
			{100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  66,  33,   1,   1,   1,   1,   1},  // Shade
			{100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  66,  33,   1,   1,   1,   1,   1,   1,   1}  // Daylight fluorescent
		},
	},
    
	// AWB convergence parameter
	{
        10, // i4Speed: Convergence speed: (0 ~ 100)
        225 // i4StableThr: Stable threshold ((currentRgain - targetRgain)^2 + (currentBgain - targetBgain)^2), WB gain format: 4.9
	},

    // AWB daylight locus target offset ratio LUT for tungsten and fluorescent0
	{
	    AWB_DAYLIGHT_LOCUS_NEW_OFFSET_INDEX_NUM, // i4Size: LUT dimension
		{// LUT: use daylight locus new offset (0~10000) as index to get daylight locus target offset ratio (0~100)
         // 0  500 1000 1500 2000 2500 3000 3500 4000 4500 5000 5500 6000 6500 7000 7500 8000 8500 9000 9500 10000
	       50,  50,  50,  50,  50,  50,  50,  50,  50,  50,  50,  55,  60,  65,  70,  75,  80,  85,  90,  95,  100
		}
	},

	// AWB green offset threshold for fluorescent0
	{
	    AWB_DAYLIGHT_LOCUS_OFFSET_INDEX_NUM, // i4Size: LUT dimension
		{// LUT: use daylight locus offset (0~10000) as index to get green offset threshold
         // 0  500 1000 1500 2000 2500 3000 3500 4000 4500 5000 5500 6000  6500  7000  7500  8000  8500  9000  9500 10000
	      600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 750, 1000, 1250, 1500, 1750, 1750, 1750, 1750, 1750
		}
	},

    // AWB light source weight LUT for tungsten light
	{
        AWB_TUNGSTEN_MAGENTA_OFFSET_INDEX_NUM, // i4Size: LUT dimension
		{// LUT: use magenta offset (0~1000) as index to get tungsten weight (x/256)
	     //  0  100  200  300  400  500  600  700  800  900 1000
	       256, 256, 256, 256, 256, 256, 256, 128,  64,  32,  16
		}
	},

    // AWB light source weight LUT for fluorescent 0
	{
        AWB_WARM_FLUORESCENT_GREEN_OFFSET_INDEX_NUM, // i4Size: LUT dimension
		{// LUT: use green offset (0~2000) as index to get fluorescent0 weight (x/256)
	     //  0  200  400  600  800 1000 1200 1400 1600 1800 2000
	       256, 256, 256, 256, 128,  64,  32,  16,  16,  16,  16
		}
	},

    // AWB light source weight LUT for shade light
	{
        AWB_SHADE_MAGENTA_OFFSET_INDEX_NUM, // i4MagentaLUTSize: Magenta LUT dimension
		{// MagentaLUT: use magenta offset (0~1000) as index to get shade light weight (x/256)
	     //  0  100  200  300  400  500  600  700 800 900 1000
	       256, 256, 256, 256, 128,  64,  32,  16, 16, 16, 16
		},
	    AWB_SHADE_GREEN_OFFSET_INDEX_NUM, // i4GreenLUTSize: Green LUT dimension
		{// GreenLUT: use green offset (0~1000) as index to get shade light weight (x/256)
	     //  0  100  200  300  400  500  600  700 800 900 1000
	       256, 256, 256, 256, 256, 128,  64,  32, 16, 16, 16
		}
	},

	// One-shot AWB parameter
	{
	    MTRUE,
	    10, // LV 1.0
	    50  // LV 5.0
	}
}; 

//____AWB Statistics Config____
static AWB_STAT_CONFIG_T g_rAWBStatConfig[LIB3A_AWB_MODE_NUM] =
{ 
	// AWB_MODE_AUTO
    {
	    // AWB Sum Window Config Register
	    0, // u4AWBSUM_WIND: no use
	    0, // u4AWBSUM_WINU: no use
		0, // u4AWBSUM_WINR: no use
	    0, // u4AWBSUM_WINL: no use

		// AWB Control Register
	    3,   // u4PAXEL_YL
	    254, // u4PAXEL_RGBH  
		0,   // u4AWBDM_DEBUG: disable 
	    2,   // u4SMAREA_NO: for preview mode  
	    1,   // u4SMAREA_EN: enable 
	    0,   // u4COLOREDGE_EN: disable 
	    0,   // u4NEUTRAL_EN: disable 
	    1,   // u4AWB_EN: enable 

	    // AWB Threshold Config
	    0, // u4NEUTRAL_TH: no use 
	    0, // u4CEDGEY_TH: no use 
		0, // u4CEDGEX_TH: no use 

		// AWB Color Space H1 Config Register
	    0,  // i4AWBH12: 0/128
	    128, // i4AWBH11: 128/128

		// AWB Color Space H2 Config Register
	    128, // u4AWBH22: 128/128
	    0,  // u4AWBH21: 0/128

		// AWB Color Edge Window Horizontal Config Register
	    0, // u4AWBCE_WINR: no use
	    0, // u4AWBCE_WINL: no use

	    // AWB Color Edge Window Vertical Config Register
	    0, // u4AWBCE_WIND: no use
	    0, // u4AWBCE_WINU: no use

		// AWB XY Window Horizontal/Vertical Config Register
	    // WINR
	    {
	       -128,//-110, // u4AWBXY_WINR0: right bound (tungsten)
	       -128,//-110, // u4AWBXY_WINR1: right bound (F0)
	         57,// 30, // u4AWBXY_WINR2: right bound (F1)
	         57,// 30, // u4AWBXY_WINR3: right bound (F2)
	        325,// 230, // u4AWBXY_WINR4: right bound (Day)
	        685,// 580, // u4AWBXY_WINR5: right bound (Shade)
	          0, // u4AWBXY_WINR6: right bound (Daylight Fluorescent)
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINRB: no use
	    },
	    // WINL
	    {
	        -778,//-770, // u4AWBXY_WINL0: left bound (tungsten)
	        -778,//-770, // u4AWBXY_WINL1: left bound (F0)
	        -128,//-110, // u4AWBXY_WINL2: left bound (F1)
	        -128,//-110, // u4AWBXY_WINL3: left bound (F2)
	          57,//30, // u4AWBXY_WINL4: left bound (Day)
	         325,//230, // u4AWBXY_WINL5: left bound (Shade)
	           0, // u4AWBXY_WINL6: left bound (Daylight Fluorescent)
	           0, // u4AWBXY_WINL7: no use
	           0, // u4AWBXY_WINL8: no use
	           0, // u4AWBXY_WINL9: no use
	           0, // u4AWBXY_WINLA: no use
	           0  // u4AWBXY_WINLB: no use
	    },
	    // WIND
	    {
	        -230,//-220, // u4AWBXY_WINU0: upper bound (tungsten)
	        -330,//-320, // u4AWBXY_WINU1: upper bound (F0)
	        -220,//-195, // u4AWBXY_WINU2: upper bound (F1)
	        -327,//-320, // u4AWBXY_WINU3: upper bound (F2)
	        -201,//-170, // u4AWBXY_WINU4: upper bound (Day)
	        -197,//-170, // u4AWBXY_WINU5: upper bound (Shade)
	           0, // u4AWBXY_WINU6: upper bound (Daylight Fluorescent)
	           0, // u4AWBXY_WINU7: no use
	           0, // u4AWBXY_WINU8: no use
	           0, // u4AWBXY_WINU9: no use
	           0, // u4AWBXY_WINUA: no use
	           0  // u4AWBXY_WINUB: no use
	    }, 
	    // WINU
	    {
	        -330,//-320, // u4AWBXY_WIND0: lower bound (tungsten)
	        -450,//-420, // u4AWBXY_WIND1: lower bound (F0)
	        -327,//-320, // u4AWBXY_WIND2: lower bound (F1)
	        -419,//-395, // u4AWBXY_WIND3: lower bound (F2)
	        -361,//-370, // u4AWBXY_WIND4: lower bound (Day)
	        -357,//-370, // u4AWBXY_WIND5: lower bound (Shade)
	           0, // u4AWBXY_WIND6: lower bound (Daylight Fluorescent)
	           0, // u4AWBXY_WIND7: no use
	           0, // u4AWBXY_WIND8: no use
	           0, // u4AWBXY_WIND9: no use
	           0, // u4AWBXY_WINDA: no use
	           0  // u4AWBXY_WINDB: no use
	    }
	},
	// AWB_MODE_DAYLIGHT
    {
	    // AWB Sum Window Config Register
	    0, // u4AWBSUM_WIND: no use
	    0, // u4AWBSUM_WINU: no use
		0, // u4AWBSUM_WINR: no use
	    0, // u4AWBSUM_WINL: no use

		// AWB Control Register
	    3,   // u4PAXEL_YL
	    254, // u4PAXEL_RGBH  
		0,   // u4AWBDM_DEBUG: disable 
	    2,   // u4SMAREA_NO: for preview mode  
	    1,   // u4SMAREA_EN: enable 
	    0,   // u4COLOREDGE_EN: disable 
	    0,   // u4NEUTRAL_EN: disable 
	    1,   // u4AWB_EN: enable 

	    // AWB Threshold Config
	    0, // u4NEUTRAL_TH: no use 
	    0, // u4CEDGEY_TH: no use 
		0, // u4CEDGEX_TH: no use 

		// AWB Color Space H1 Config Register
	    2,  // i4AWBH12: 0/128
	    128, // i4AWBH11: 128/128

		// AWB Color Space H2 Config Register
	    128, // u4AWBH22: 128/128
	    -2,  // u4AWBH21: 0/128

		// AWB Color Edge Window Horizontal Config Register
	    0, // u4AWBCE_WINR: no use
	    0, // u4AWBCE_WINL: no use

	    // AWB Color Edge Window Vertical Config Register
	    0, // u4AWBCE_WIND: no use
	    0, // u4AWBCE_WINU: no use

		// AWB XY Window Horizontal/Vertical Config Register
	    // WINR
	    {
	        160, // u4AWBXY_WINR0: neutral area
	        600, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINRB: no use
	    },
	    // WINL
	    {
	         70, // u4AWBXY_WINR0: neutral area
	       -760, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINLB: no use
	    },
	    // WIND
	    {
	       -230, // u4AWBXY_WINR0: neutral area
	       -210, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINUB: no use
	    }, 
	    // WINU
	    {
	       -330, // u4AWBXY_WINR0: neutral area
	       -430, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINDB: no use
	    }
	},	
	// AWB_MODE_CLOUDY_DAYLIGHT
    {
	    // AWB Sum Window Config Register
	    0, // u4AWBSUM_WIND: no use
	    0, // u4AWBSUM_WINU: no use
		0, // u4AWBSUM_WINR: no use
	    0, // u4AWBSUM_WINL: no use

		// AWB Control Register
	    3,   // u4PAXEL_YL
	    254, // u4PAXEL_RGBH  
		0,   // u4AWBDM_DEBUG: disable 
	    2,   // u4SMAREA_NO: for preview mode  
	    1,   // u4SMAREA_EN: enable 
	    0,   // u4COLOREDGE_EN: disable 
	    0,   // u4NEUTRAL_EN: disable 
	    1,   // u4AWB_EN: enable 

	    // AWB Threshold Config
	    0, // u4NEUTRAL_TH: no use 
	    0, // u4CEDGEY_TH: no use 
		0, // u4CEDGEX_TH: no use 

		// AWB Color Space H1 Config Register
	    2,  // i4AWBH12: 0/128
	    128, // i4AWBH11: 128/128

		// AWB Color Space H2 Config Register
	    128, // u4AWBH22: 128/128
	    -2,  // u4AWBH21: 0/128

		// AWB Color Edge Window Horizontal Config Register
	    0, // u4AWBCE_WINR: no use
	    0, // u4AWBCE_WINL: no use

	    // AWB Color Edge Window Vertical Config Register
	    0, // u4AWBCE_WIND: no use
	    0, // u4AWBCE_WINU: no use

		// AWB XY Window Horizontal/Vertical Config Register
	    // WINR
	    {
	        250, // u4AWBXY_WINR0: neutral area
	        600, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINRB: no use
	    },
	    // WINL
	    {
	        160, // u4AWBXY_WINR0: neutral area
	       -760, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINLB: no use
	    },
	    // WIND
	    {
	       -240, // u4AWBXY_WINR0: neutral area
	       -210, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINUB: no use
	    }, 
	    // WINU
	    {
	       -340, // u4AWBXY_WINR0: neutral area
	       -430, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINDB: no use
	    }
	},	
	// AWB_MODE_SHADE
    {
	    // AWB Sum Window Config Register
	    0, // u4AWBSUM_WIND: no use
	    0, // u4AWBSUM_WINU: no use
		0, // u4AWBSUM_WINR: no use
	    0, // u4AWBSUM_WINL: no use

		// AWB Control Register
	    3,   // u4PAXEL_YL
	    254, // u4PAXEL_RGBH  
		0,   // u4AWBDM_DEBUG: disable 
	    2,   // u4SMAREA_NO: for preview mode  
	    1,   // u4SMAREA_EN: enable 
	    0,   // u4COLOREDGE_EN: disable 
	    0,   // u4NEUTRAL_EN: disable 
	    1,   // u4AWB_EN: enable 

	    // AWB Threshold Config
	    0, // u4NEUTRAL_TH: no use 
	    0, // u4CEDGEY_TH: no use 
		0, // u4CEDGEX_TH: no use 

		// AWB Color Space H1 Config Register
	    2,  // i4AWBH12: 0/128
	    128, // i4AWBH11: 128/128

		// AWB Color Space H2 Config Register
	    128, // u4AWBH22: 128/128
	    -2,  // u4AWBH21: 0/128

		// AWB Color Edge Window Horizontal Config Register
	    0, // u4AWBCE_WINR: no use
	    0, // u4AWBCE_WINL: no use

	    // AWB Color Edge Window Vertical Config Register
	    0, // u4AWBCE_WIND: no use
	    0, // u4AWBCE_WINU: no use

		// AWB XY Window Horizontal/Vertical Config Register
	    // WINR
	    {
	        340, // u4AWBXY_WINR0: neutral area
	        600, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINRB: no use
	    },
	    // WINL
	    {
	        250, // u4AWBXY_WINR0: neutral area
	       -760, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINLB: no use
	    },
	    // WIND
	    {
	       -240, // u4AWBXY_WINR0: neutral area
	       -210, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINUB: no use
	    }, 
	    // WINU
	    {
	       -340, // u4AWBXY_WINR0: neutral area
	       -430, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINDB: no use
	    }
	},	
	// AWB_MODE_TWILIGHT
    {
	    // AWB Sum Window Config Register
	    0, // u4AWBSUM_WIND: no use
	    0, // u4AWBSUM_WINU: no use
		0, // u4AWBSUM_WINR: no use
	    0, // u4AWBSUM_WINL: no use

		// AWB Control Register
	    3,   // u4PAXEL_YL
	    254, // u4PAXEL_RGBH  
		0,   // u4AWBDM_DEBUG: disable 
	    2,   // u4SMAREA_NO: for preview mode  
	    1,   // u4SMAREA_EN: enable 
	    0,   // u4COLOREDGE_EN: disable 
	    0,   // u4NEUTRAL_EN: disable 
	    1,   // u4AWB_EN: enable 

	    // AWB Threshold Config
	    0, // u4NEUTRAL_TH: no use 
	    0, // u4CEDGEY_TH: no use 
		0, // u4CEDGEX_TH: no use 

		// AWB Color Space H1 Config Register
	    2,  // i4AWBH12: 0/128
	    128, // i4AWBH11: 128/128

		// AWB Color Space H2 Config Register
	    128, // u4AWBH22: 128/128
	    -2,  // u4AWBH21: 0/128

		// AWB Color Edge Window Horizontal Config Register
	    0, // u4AWBCE_WINR: no use
	    0, // u4AWBCE_WINL: no use

	    // AWB Color Edge Window Vertical Config Register
	    0, // u4AWBCE_WIND: no use
	    0, // u4AWBCE_WINU: no use

		// AWB XY Window Horizontal/Vertical Config Register
	    // WINR
	    {
	        160, // u4AWBXY_WINR0: neutral area
	        600, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINRB: no use
	    },
	    // WINL
	    {
	         70, // u4AWBXY_WINR0: neutral area
	       -760, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINLB: no use
	    },
	    // WIND
	    {
	       -230, // u4AWBXY_WINR0: neutral area
	       -210, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINUB: no use
	    }, 
	    // WINU
	    {
	       -330, // u4AWBXY_WINR0: neutral area
	       -430, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINDB: no use
	    }
	},	
	// AWB_MODE_FLUORESCENT
    {
	    // AWB Sum Window Config Register
	    0, // u4AWBSUM_WIND: no use
	    0, // u4AWBSUM_WINU: no use
		0, // u4AWBSUM_WINR: no use
	    0, // u4AWBSUM_WINL: no use

		// AWB Control Register
	    3,   // u4PAXEL_YL
	    254, // u4PAXEL_RGBH  
		0,   // u4AWBDM_DEBUG: disable 
	    2,   // u4SMAREA_NO: for preview mode  
	    1,   // u4SMAREA_EN: enable 
	    0,   // u4COLOREDGE_EN: disable 
	    0,   // u4NEUTRAL_EN: disable 
	    1,   // u4AWB_EN: enable 

	    // AWB Threshold Config
	    0, // u4NEUTRAL_TH: no use 
	    0, // u4CEDGEY_TH: no use 
		0, // u4CEDGEX_TH: no use 

		// AWB Color Space H1 Config Register
	    2,  // i4AWBH12: 0/128
	    128, // i4AWBH11: 128/128

		// AWB Color Space H2 Config Register
	    128, // u4AWBH22: 128/128
	    -2,  // u4AWBH21: 0/128

		// AWB Color Edge Window Horizontal Config Register
	    0, // u4AWBCE_WINR: no use
	    0, // u4AWBCE_WINL: no use

	    // AWB Color Edge Window Vertical Config Register
	    0, // u4AWBCE_WIND: no use
	    0, // u4AWBCE_WINU: no use

		// AWB XY Window Horizontal/Vertical Config Register
	    // WINR
	    {
	        190, // u4AWBXY_WINR0: neutral area
	        600, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINRB: no use
	    },
	    // WINL
	    {
	       -120, // u4AWBXY_WINR0: neutral area
	       -760, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINLB: no use
	    },
	    // WIND
	    {
	       -236, // u4AWBXY_WINR0: neutral area
	       -210, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINUB: no use
	    }, 
	    // WINU
	    {
	       -380, // u4AWBXY_WINR0: neutral area
	       -430, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINDB: no use
	    }
	},		
	// AWB_MODE_WARM_FLUORESCENT
    {
	    // AWB Sum Window Config Register
	    0, // u4AWBSUM_WIND: no use
	    0, // u4AWBSUM_WINU: no use
		0, // u4AWBSUM_WINR: no use
	    0, // u4AWBSUM_WINL: no use

		// AWB Control Register
	    3,   // u4PAXEL_YL
	    254, // u4PAXEL_RGBH  
		0,   // u4AWBDM_DEBUG: disable 
	    2,   // u4SMAREA_NO: for preview mode  
	    1,   // u4SMAREA_EN: enable 
	    0,   // u4COLOREDGE_EN: disable 
	    0,   // u4NEUTRAL_EN: disable 
	    1,   // u4AWB_EN: enable 

	    // AWB Threshold Config
	    0, // u4NEUTRAL_TH: no use 
	    0, // u4CEDGEY_TH: no use 
		0, // u4CEDGEX_TH: no use 

		// AWB Color Space H1 Config Register
	    2,  // i4AWBH12: 0/128
	    128, // i4AWBH11: 128/128

		// AWB Color Space H2 Config Register
	    128, // u4AWBH22: 128/128
	    -2,  // u4AWBH21: 0/128

		// AWB Color Edge Window Horizontal Config Register
	    0, // u4AWBCE_WINR: no use
	    0, // u4AWBCE_WINL: no use

	    // AWB Color Edge Window Vertical Config Register
	    0, // u4AWBCE_WIND: no use
	    0, // u4AWBCE_WINU: no use

		// AWB XY Window Horizontal/Vertical Config Register
	    // WINR
	    {
	       -120, // u4AWBXY_WINR0: neutral area
	        600, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINRB: no use
	    },
	    // WINL
	    {
	       -400, // u4AWBXY_WINR0: neutral area
	       -760, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINLB: no use
	    },
	    // WIND
	    {
	       -236, // u4AWBXY_WINR0: neutral area
	       -210, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINUB: no use
	    }, 
	    // WINU
	    {
	       -380, // u4AWBXY_WINR0: neutral area
	       -430, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINDB: no use
	    }
	},	
	// AWB_MODE_INCANDESCENT
    {
	    // AWB Sum Window Config Register
	    0, // u4AWBSUM_WIND: no use
	    0, // u4AWBSUM_WINU: no use
		0, // u4AWBSUM_WINR: no use
	    0, // u4AWBSUM_WINL: no use

		// AWB Control Register
	    3,   // u4PAXEL_YL
	    254, // u4PAXEL_RGBH  
		0,   // u4AWBDM_DEBUG: disable 
	    2,   // u4SMAREA_NO: for preview mode  
	    1,   // u4SMAREA_EN: enable 
	    0,   // u4COLOREDGE_EN: disable 
	    0,   // u4NEUTRAL_EN: disable 
	    1,   // u4AWB_EN: enable 

	    // AWB Threshold Config
	    0, // u4NEUTRAL_TH: no use 
	    0, // u4CEDGEY_TH: no use 
		0, // u4CEDGEX_TH: no use 

		// AWB Color Space H1 Config Register
	    2,  // i4AWBH12: 0/128
	    128, // i4AWBH11: 128/128

		// AWB Color Space H2 Config Register
	    128, // u4AWBH22: 128/128
	    -2,  // u4AWBH21: 0/128

		// AWB Color Edge Window Horizontal Config Register
	    0, // u4AWBCE_WINR: no use
	    0, // u4AWBCE_WINL: no use

	    // AWB Color Edge Window Vertical Config Register
	    0, // u4AWBCE_WIND: no use
	    0, // u4AWBCE_WINU: no use

		// AWB XY Window Horizontal/Vertical Config Register
	    // WINR
	    {
	       -120, // u4AWBXY_WINR0: neutral area
	        600, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINRB: no use
	    },
	    // WINL
	    {
	       -360, // u4AWBXY_WINR0: neutral area
	       -760, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINLB: no use
	    },
	    // WIND
	    {
	       -230, // u4AWBXY_WINR0: neutral area
	       -210, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINUB: no use
	    }, 
	    // WINU
	    {
	       -330, // u4AWBXY_WINR0: neutral area
	       -430, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINDB: no use
	    }
	},
	// AWB_MODE_GRAYWORLD
    {
	    // AWB Sum Window Config Register
	    0, // u4AWBSUM_WIND: no use
	    0, // u4AWBSUM_WINU: no use
		0, // u4AWBSUM_WINR: no use
	    0, // u4AWBSUM_WINL: no use

		// AWB Control Register
	    3,   // u4PAXEL_YL
	    254, // u4PAXEL_RGBH  
		0,   // u4AWBDM_DEBUG: disable 
	    2,   // u4SMAREA_NO: for preview mode  
	    1,   // u4SMAREA_EN: enable 
	    0,   // u4COLOREDGE_EN: disable 
	    0,   // u4NEUTRAL_EN: disable 
	    1,   // u4AWB_EN: enable 

	    // AWB Threshold Config
	    0, // u4NEUTRAL_TH: no use 
	    0, // u4CEDGEY_TH: no use 
		0, // u4CEDGEX_TH: no use 

		// AWB Color Space H1 Config Register
	    2,  // i4AWBH12: 0/128
	    128, // i4AWBH11: 128/128

		// AWB Color Space H2 Config Register
	    128, // u4AWBH22: 128/128
	    -2,  // u4AWBH21: 0/128

		// AWB Color Edge Window Horizontal Config Register
	    0, // u4AWBCE_WINR: no use
	    0, // u4AWBCE_WINL: no use

	    // AWB Color Edge Window Vertical Config Register
	    0, // u4AWBCE_WIND: no use
	    0, // u4AWBCE_WINU: no use

		// AWB XY Window Horizontal/Vertical Config Register
	    // WINR
	    {
	       -120, // u4AWBXY_WINR0: neutral area
	        600, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINRB: no use
	    },
	    // WINL
	    {
	       -360, // u4AWBXY_WINR0: neutral area
	       -760, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINLB: no use
	    },
	    // WIND
	    {
	       -230, // u4AWBXY_WINR0: neutral area
	       -210, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINUB: no use
	    }, 
	    // WINU
	    {
	       -330, // u4AWBXY_WINR0: neutral area
	       -430, // u4AWBXY_WINR1: reference area
	          0, // u4AWBXY_WINR2: no use
	          0, // u4AWBXY_WINR3: no use
	          0, // u4AWBXY_WINR4: no use
	          0, // u4AWBXY_WINR5: no use
	          0, // u4AWBXY_WINR6: no use
	          0, // u4AWBXY_WINR7: no use
	          0, // u4AWBXY_WINR8: no use
	          0, // u4AWBXY_WINR9: no use
	          0, // u4AWBXY_WINRA: no use
	          0  // u4AWBXY_WINDB: no use
	    }
	}
};

#endif

