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

#ifndef _AE_TUNING_CUSTOM_MT6573_H
#define _AE_TUNING_CUSTOM_MT6573_H

#include "../param/aaa_param_mt6573.h"

static strAEParamCFG g_AEParasetting = 
{
	FALSE,		  // bEnableSaturationCheck
	TRUE,		  // bEnableFaceAE
	FALSE,		  // bFaceAEAvoidOverExp;

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

static strAFPlineInfo g_strAFPLineTable =     // from high lv to low lv
{
1,                  // 1 enable the AF Pline, 0 disable the AF Pline (bypass)
{
{73, 30},       // {LV value, AF frame rate}
{65, 25},  
{55, 17}, 
{45, 17}, 
{0, 17},
}
}; 

static MUINT32 g_strEVValueArray[17] =
{
1024,  // EV   0
1100,  // EV   0.3
1161,  // EV   0.5
1250,  // EV   0.7
1394,  // EV   1.0
1550,  // EV   1.3
1742,  // EV   1.5
1850,  // EV   1.7
2066,  // EV   2.0
  850,  // EV -0.3
  773,  // EV -0.5
  700,  // EV -0.7
  623,  // EV -1.0
  550,  // EV -1.3
  492,  // EV -1.5
  425,  // EV -1.7
  375,  // EV -2.0
};

// total 24 sets
static strAEMOVE  g_AEMoveTable[] =
{
    {-20,   25,    20}, //   mean below -2  move increase 20 index
    {-15,   33,    15}, //   -2~-1.6
    {-15,   40,    12}, //   -1.6~-1.3
    {-10,   50,    9}, //   -1.3~-1
    { -8,   57,     7}, //   -1~-0.8
    { -5,   71,     4}, //   -0.8~-0.5
    { -4,   75,     3}, //   -0.5~-0.4
    { -3,   81,     2}, //   -0.4~-0.3
    { -2,   87,     1}, //   -0.3~-0.2
    { -1,   93,     1}, //   -0.2~-0.1
    {   0,  100,     0}, //   -0.1~0
    {   1,  106,     0}, //     0~0.1
    {   2,  114,    -1}, //    0.1~0.2       move decrease 1 index
    {   3,  123,    -1}, //    0.2~0.3
    {   4,  131,    -2}, //    0.3~0.4    
    {   5,  141,    -3}, //    0.4~0.5    
    {   7,  162,    -4}, //    0.5~0.7    
    {   9,  186,    -6}, //    0.7~0.9    
    { 10,  200,   -8}, //    0.9~1.0
    { 13,  246,   -9}, //    1.0~1.3
    { 16,  303,   -12}, //    1.3~1.6
    { 20,  400,   -15}, //    1.6~2       move decrease 10  index
    { 25,  566,   -19}, //    2~2.5       move decrease 20  index
    { 30,  800,   -24}, //    2.5~3      move decrease 30  index
};

// for HDR parameters
static strAEHDRTable  g_AEHDRInfoTable =
{
//    100,     // i2GainThr                  // 10 base
    40,     // i2GainThr                  // 10 base
     2,     // iLowLightMode
    25,     // iSaturated_percentage   // 10 base
    50,     // i2Saturated_percentage_Level2;   // 10 base
    70,     // i2Saturated_percentage_Level3;   // 10 base
    60,     // iPercentage_thr1          // 10 base
    15,     // iPercentage_thr2          // 10 base
     5,     // iPercentage_thr3          // 10 base
     0,     // iMust_enhance_highlight
      2,     // iGAIN_LEVEL1
      4,     // iGAIN_LEVEL2
      7,     // iGAIN_LEVEL3
      2,     // iMAX_EV_GAIN_LEVEL1
      1,     // iMAX_EV_GAIN_LEVEL2
      0,     // iMAX_EV_GAIN_LEVEL3
      0,     // iMAX_EV_GAIN_LEVEL4
      2,     // lowlight_histogram_shift
};

// for AE limiter
static strAELimiterTable g_AELimiterDataTable = 
{
    2,    //  iLEVEL1_GAIN
    10,  //  iLEVEL2_GAIN
    18,  //  iLEVEL3_GAIN   
    28,  //  iLEVEL4_GAIN   
    40,  //  iLEVEL5_GAIN   
    50,  //  iLEVEL6_GAIN    
    0,    //  iLEVEL1_TARGET_DIFFERENCE
    4,    //  iLEVEL2_TARGET_DIFFERENCE
    7,    //  iLEVEL3_TARGET_DIFFERENCE
    7,    //  iLEVEL4_TARGET_DIFFERENCE
    7,    //  iLEVEL5_TARGET_DIFFERENCE
    7,    //  iLEVEL6_TARGET_DIFFERENCE
    2,    //  iLEVEL1_GAINH
    -2,  //  iLEVEL1_GAINL    
    6,    //  iLEVEL2_GAINH
    -7,  //  iLEVEL2_GAINL    
    8,    // iLEVEL3_GAINH
    -9,  //  iLEVEL3_GAINL    
    10,  //  iLEVEL4_GAINH
    -12, // iLEVEL4_GAINL    
    12,   // iLEVEL5_GAINH
    -16, // iLEVEL5_GAINL    
    15,   // iLEVEL6_GAINH
    -20, // iLEVEL6_GAINL    
    1,     // iGAIN_DIFFERENCE_LIMITER
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
&g_strEVValueArray[0],
g_AEMoveTable,
g_AEHDRInfoTable,
g_AELimiterDataTable,
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
   127,//127, //AE_WinWidth : 1280/2/5 - 1 = 127  count from zero
   95,//95, //AE_WinHeight : 960/2/5 - 1 = 95 count from zero
   0, //AE_ACC_SEL
   0, //HIST_WinHOffset : No Vertical Offset
   0, //HIST_WinVOffset : No Horizontal Offset
   80,//40, //HIST_WinWidth : 1280/16 = 80
   120,//60, //HIST_WinHeight : 960/8 = 120
   {
       { // FD AE window 1
           1,   // u4FDAEWIN_EN
           319, // u4FDAEWINR = (1280/4 - 1)
           0,   // u4FDAEWINL
           239, // u4FDAEWIND = (960/4 - 1)
           0    // u4FDAEWINU
       },
       { // FD AE window 2
           1,   // u4FDAEWIN_EN
           319, // u4FDAEWINR = (1280/4 - 1)
           0,   // u4FDAEWINL
           239, // u4FDAEWIND = (960/4 - 1)
           0    // u4FDAEWINU
       },
       { // FD AE window 3
           1,   // u4FDAEWIN_EN
           319, // u4FDAEWINR = (1280/4 - 1)
           0,   // u4FDAEWINL
           239, // u4FDAEWIND = (960/4 - 1)
           0    // u4FDAEWINU
       },
       { // FD AE window 4
           1,   // u4FDAEWIN_EN
           319, // u4FDAEWINR = (1280/4 - 1)
           0,   // u4FDAEWINL
           239, // u4FDAEWIND = (960/4 - 1)
           0    // u4FDAEWINU
       }
   }
};

static AE_STAT_CONFIG_T g_rAEMovieStatConfig =
{
   1, //bAEHisEnable : enable Histogram
   1, //bAELumEnable : enable luminance
   1, //bFlareHisEnable : enable Flare Histogram
   0, //AE_WinHOffset : No Horizontal Offset
   0, //AE_WinVOffset : No Vertical Offset
   127,//127, //AE_WinWidth : 1280/2/5 - 1 = 127  count from zero
   95,//95, //AE_WinHeight : 960/2/5 - 1 = 95 count from zero
   0, //AE_ACC_SEL
   0, //HIST_WinHOffset : No Vertical Offset
   0, //HIST_WinVOffset : No Horizontal Offset
   80,//40, //HIST_WinWidth : 1280/16 = 80
   120,//60, //HIST_WinHeight : 960/8 = 120
   {
       { // FD AE window 1
           1,   // u4FDAEWIN_EN
           319, // u4FDAEWINR = (1280/4 - 1)
           0,   // u4FDAEWINL
           239, // u4FDAEWIND = (960/4 - 1)
           0    // u4FDAEWINU
       },
       { // FD AE window 2
           1,   // u4FDAEWIN_EN
           319, // u4FDAEWINR = (1280/4 - 1)
           0,   // u4FDAEWINL
           239, // u4FDAEWIND = (960/4 - 1)
           0    // u4FDAEWINU
       },
       { // FD AE window 3
           1,   // u4FDAEWIN_EN
           319, // u4FDAEWINR = (1280/4 - 1)
           0,   // u4FDAEWINL
           239, // u4FDAEWIND = (960/4 - 1)
           0    // u4FDAEWINU
       },
       { // FD AE window 4
           1,   // u4FDAEWIN_EN
           319, // u4FDAEWINR = (1280/4 - 1)
           0,   // u4FDAEWINL
           239, // u4FDAEWIND = (960/4 - 1)
           0    // u4FDAEWINU
       }
   }
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

