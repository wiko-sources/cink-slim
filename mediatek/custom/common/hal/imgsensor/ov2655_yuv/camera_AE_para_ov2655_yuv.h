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

#ifndef _CAMERA_AE_PARA_OV2655_YUV_H
#define _CAMERA_AE_PARA_OV2655_YUV_H

#include "MTK3ACommon.h"
#include "camera_AE_PLineTable_ov2655_yuv.h"

static strAEParamCFG g_AEParasetting = 
{
	FALSE,		  // bEnableSaturationCheck
	FALSE,		  // bEnableFaceAE
	TRUE,		  // bFaceAEAvoidOverExp;

	256,		  // u4BackLightStrength : strength of backlight condtion
	256,		  // u4OverExpStrength : strength of anti over exposure
	256,		  // u4HistStretchStrength : strength of  histogram stretch
	0,			  // u4SmoothLevel : time LPF smooth level , internal use
	4,			  // u4TimeLPFLevel : time LOW pass filter level
	
	AE_BLOCK_NO,  // u4BockXNum : AE X block number;
	AE_BLOCK_NO,  // u4BockYNum : AE Yblock number;
	20, 		  // u4SatBlockCheckLow : saturation block check , low thres
	50, 		  // u4SatBlockCheckHigh : sturation  block check , hight thres
	50, 		  // u4SatBlockAdjustFactor : adjust factore , to adjust central weighting target value
	
	20, 		  // u4FaceYLowBound;
	50, 		  // u4FaceYHighBound;
};

static strWeightTable  g_Weight_Matrix =
{
AE_WEIGHTING_CENTRALWEIGHT,
{
{22, 22, 22,  22, 22},
{1 , 50, 50,  50, 1 },
{34, 84, 191, 84, 34},
{1,  50, 50,  50, 1 },
{22, 22, 22,  22, 22},
}
};

static strWeightTable  g_Weight_Spot =
{
AE_WEIGHTING_SPOT,
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
}
};

static strWeightTable  g_Weight_Average =
{
AE_WEIGHTING_AVERAGE,
{
{1, 1, 1, 1, 1},
{1, 1, 1, 1, 1},
{1, 1, 1, 1, 1},
{1, 1, 1, 1, 1},
{1, 1, 1, 1, 1},
}
};

static strAFPlineInfo g_strAFPLineTable[] =     // from high lv to low lv
{
{73, 30},       // {LV value, AF frame rate}
{65, 25},  
{55, 17}, 
{45, 17}, 
{0, 17},
}; 

static struct_AE_Para strAEInitPara =
{
//g_AE_ThresCFG,
//g_AE_HistCFG,
//g_AE_DeviceInfo,
g_AEParasetting,
// PLINE TABLE
g_AEModePLineMapping,
g_strAEPlineTable,
//WEIGHTING TABLE
{
&g_Weight_Matrix,
&g_Weight_Spot,
&g_Weight_Average,
NULL
},
g_strAFPLineTable,
};

static AE_PARAM_T g_rAEParam =
{
	strAEInitPara
};

//____AE Statistics Config____
static AE_STAT_CONFIG_T g_rAEPreStatConfig = 
{
   1, //bAEHisEnable : enable Histogram
   1, //bAELumEnable : enable luminance
   1, //bFlareHisEnable : enable Flare Histogram
   0, //AE_WinHOffset : No Horizontal Offset
   0, //AE_WinVOffset : No Vertical Offset
   79,//AE_WinWidth : 800/2/5 - 1 = 79  count from zero
   58,//AE_WinHeight : 592/2/5 - 1 = 58 count from zero
   0, //HIST_WinHOffset : No Vertical Offset
   0, //HIST_WinVOffset : No Horizontal Offset
   50, //HIST_WinWidth : 800/16 = 80
   74,//HIST_WinHeight : 592/8 = 74
};

static AE_STAT_CONFIG_T g_rAEMovieStatConfig = 
{
   1, //bAEHisEnable : enable Histogram
   1, //bAELumEnable : enable luminance
   1, //bFlareHisEnable : enable Flare Histogram
   0, //AE_WinHOffset : No Horizontal Offset
   0, //AE_WinVOffset : No Vertical Offset
   79,//AE_WinWidth : 800/2/5 - 1 = 79 count from zero
   58,//AE_WinHeight : 592/2/5 - 1 = 58 count from zero
   0, //HIST_WinHOffset : No Vertical Offset
   0, //HIST_WinVOffset : No Horizontal Offset
   50, //HIST_WinWidth : 800/16 = 80
   74,//HIST_WinHeight : 592/8 = 74
};

//strobe power table.
//yusu/091230
#if 1 //fixed POWER LED strrngth
static strStrobePTbl g_stoPTbl[]={  {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {12  ,256 ,500	,322},
                                    {0   ,   0,  0,    0}};
#else
static strStrobePTbl g_stoPTbl[]={  {1   ,24  ,470	,346},
                                    {2   ,48  ,478	,331},
                                    {3   ,75  ,481	,328},
                                    {4   ,101 ,481	,331},
                                    {5   ,127 ,481	,324},
                                    {6   ,156 ,485	,325},
                                    {7   ,181 ,489	,325},
                                    {8   ,207 ,490	,326},
                                    {9   ,233 ,490	,321},
                                    {10  ,256 ,491	,320},
                                    {11  ,283 ,495	,323},
                                    {12  ,307 ,500	,322},
                                    {13  ,329 ,503	,317},
                                    {14  ,349 ,511	,316},
                                    {15  ,366 ,517	,311},
                                    {16  ,383 ,521	,307},
                                    {17  ,396 ,534	,301},
                                    {18  ,406 ,538	,295},
                                    {19  ,411 ,548	,287},
                                    {20  ,415 ,561	,275},
                                    {0   ,   0,  0,    0}};
#endif

//strobe capture weighting matrix
static UINT32 g_stoWM[] = {22, 22,  22,  22, 22,
                     1, 50,  50,  50,  1,
                    34, 84, 191,  84, 34,
                     1, 50,  50,  50,  1,
                    22, 22,  22,  22, 22 };

//lens iris FNo array.
static UINT32 g_stoFNo[] = {28};

// preflash initial parameters
static struct_AEPF_Para strAEPFInitPara =
{
    NULL,
    g_stoPTbl,
    g_stoWM,
    g_stoFNo,
    (sizeof(g_stoFNo)/sizeof(UINT32)),
};

static AEPF_PARAM_T g_rAEPFParam =
{
    strAEPFInitPara
};
#endif

