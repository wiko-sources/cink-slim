//
#define LOG_TAG "IspTuningCustom"
#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (0)
#endif
//
#include <utils/Errors.h>
#include <cutils/log.h>
//
#define USE_CUSTOM_ISP_TUNING
#include "isp_tuning.h"
//
using namespace NSIspTuning;
//


/*******************************************************************************
*
*   rCamInfo
*       [in]    ISP Camera Info for RAW sensor. Its members are as below:
*
*           eCamMode:
*               ECamMode_Video          = 0, 
*               ECamMode_Online_Preview,
*               ECamMode_Online_Capture, 
*               ECamMode_Online_Capture_ZSD,
*               ECamMode_Offline_Capture_Pass1, 
*               ECamMode_Offline_Capture_Pass2, 
*               ECamMode_HDR_Cap_Pass1_SF,  //  Pass1: Single Frame
*               ECamMode_HDR_Cap_Pass1_MF1, //  Pass1: Multi Frame Stage1
*               ECamMode_HDR_Cap_Pass1_MF2, //  Pass1: Multi Frame Stage2
*               ECamMode_HDR_Cap_Pass2,     //  Pass2
*
*           eIdx_Scene:
*               SCENE_MODE_OFF,             // Disable scene mode equal Auto mode
*               SCENE_MODE_NORMAL,          // Normal mode
*               SCENE_MODE_ACTION,          // Action mode
*               SCENE_MODE_PORTRAIT,        // Portrait mode
*               SCENE_MODE_LANDSCAPE,       // Landscape
*               SCENE_MODE_NIGHTSCENE,      // Night Scene
*               SCENE_MODE_NIGHTPORTRAIT,   // Night Portrait
*               SCENE_MODE_THEATRE,         // Theatre mode
*               SCENE_MODE_BEACH,           // Beach mode
*               SCENE_MODE_SNOW,            // Snow mode
*               SCENE_MODE_SUNSET,          // Sunset mode
*               SCENE_MODE_STEADYPHOTO,     // Steady photo mode
*               SCENE_MODE_FIREWORKS,       // Fireworks mode
*               SCENE_MODE_SPORTS,          // Sports mode
*               SCENE_MODE_PARTY,           // Party mode
*               SCENE_MODE_CANDLELIGHT,     // Candle light mode
*
*           u4ISOValue:
*               ISO value to determine eISO.
*
*           eIdx_ISO:
*               eIDX_ISO_100, 
*               eIDX_ISO_200, 
*               eIDX_ISO_400, 
*               eIDX_ISO_800, 
*               eIDX_ISO_1600
*
*           i4CCT:
*               Correlated color temperature
*
*           eCCTIndex_CCM:
*               Correlated color temperature index for CCM
*                   eIDX_CCM_CCT_TL84
*                   eIDX_CCM_CCT_CWF
*                   eIDX_CCM_CCT_D65
*
*           u4ZoomRatio_x100:
*               zoom ratio (x100)
*
*           i4LightValue_x10:
*               light value (x10)
*
*   rIdxMgr:
*       [in]    The default ISP tuning index manager.
*       [out]   The ISP tuning index manager after customizing.
*
*
*******************************************************************************/
MVOID
IspTuningCustom::
evaluate_nvram_index(RAWIspCamInfo const& rCamInfo, IndexMgr& rIdxMgr)
{
    MBOOL fgRet = MFALSE;
    ECamMode_T const       eCamMode = rCamInfo.eCamMode;
    EIndex_Scene_T const eIdx_Scene = rCamInfo.eIdx_Scene;
    EIndex_ISO_T const     eIdx_ISO = rCamInfo.eIdx_ISO;
    MUINT32 const        u4ISOValue = rCamInfo.u4ISOValue;
    MUINT32 const             i4CCT = rCamInfo.i4CCT;
    MUINT32 const  u4ZoomRatio_x100 = rCamInfo.u4ZoomRatio_x100;
    MINT32 const   i4LightValue_x10 = rCamInfo.i4LightValue_x10;

    //  (0) We have:
    //      eCamMode, eScene, ......
//..............................................................................
    //  (1) Dump info. before customizing.
#if ENABLE_MY_LOG
    rCamInfo.dump();
#endif

#if 0
    LOGD("[+evaluate_nvram_index][before customizing]");
    rIdxMgr.dump();
#endif
//..............................................................................
    //  (2) Modify each index based on conditions.
    //
    //  setIdx_XXX() returns:
    //      MTURE: if successful
    //      MFALSE: if the input index is out of range.
    //
#if 0
    fgRet = rIdxMgr.setIdx_DM(XXX);
    fgRet = rIdxMgr.setIdx_DP(XXX);
    fgRet = rIdxMgr.setIdx_NR1(XXX);
    fgRet = rIdxMgr.setIdx_NR2(XXX);
    fgRet = rIdxMgr.setIdx_Saturation(XXX);
    fgRet = rIdxMgr.setIdx_Contrast(XXX);
    fgRet = rIdxMgr.setIdx_Hue(XXX);
    fgRet = rIdxMgr.setIdx_Gamma(XXX);
    fgRet = rIdxMgr.setIdx_EE(XXX);
#endif
//..............................................................................
    //  (3) Finally, dump info. after modifying.
#if 0
    LOGD("[-evaluate_nvram_index][after customizing]");
    rIdxMgr.dump();
#endif
}


/*******************************************************************************
* 
*******************************************************************************/
MVOID
IspTuningCustom::
refine_NR1(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_NR1_T& rNR1)
{
    //  (1) Check to see if it works or not.
    switch  (rCamInfo.eCamMode)
    {
    //  Normal
    case ECamMode_Online_Preview:
    case ECamMode_Video:
    case ECamMode_Online_Capture:
    case ECamMode_Online_Capture_ZSD:
    case ECamMode_Offline_Capture_Pass1:
    //   TODO: Add your code below...

    //20110808 Jouny	
    int curr_gnf,set_gnf;
	curr_gnf	= rNR1.nr_cfg1.bits.GNF;
	//LOGD("ISO:%d Original NR1_GNF:%d %d\n",rCamInfo.eIdx_ISO
	//	,rNR1.nr_cfg1.bits.GNF
	//	,curr_gnf);
	
	if(rCamInfo.u4ZoomRatio_x100>0)
	{
		if(rCamInfo.u4ZoomRatio_x100>=459)
		{
			set_gnf	= 16;	
		}
		else
		{
			set_gnf	= curr_gnf+rCamInfo.u4ZoomRatio_x100 * (16-curr_gnf)/459;
			
		}
		if(set_gnf>16)
		{
			set_gnf	= 16;
		}
		else if(set_gnf<1)
		{
			set_gnf	= 1;
		}
		rNR1.nr_cfg1.bits.GNF	= set_gnf;
	}
        break;

    //  HDR
    case ECamMode_HDR_Cap_Pass1_SF:
    //   TODO: Add your code below...

		switch(rCamInfo.eIdx_ISO)
		{
			case eIDX_ISO_100:
		    	rNR1.nr_cfg1.bits.GNF = rNR1.nr_cfg1.bits.GNF+2;
		    	if(rNR1.nr_cfg1.bits.GNF>15)
		    	{
		    		rNR1.nr_cfg1.bits.GNF = 15;
		    	}
		    	break;
			case eIDX_ISO_200:
		    	rNR1.nr_cfg1.bits.GNF = rNR1.nr_cfg1.bits.GNF+2;
		    	if(rNR1.nr_cfg1.bits.GNF>15)
		    	{
		    		rNR1.nr_cfg1.bits.GNF = 15;
		    	}
		    	break;
			case eIDX_ISO_400:
//		    	rNR1.nr_cfg1.bits.GNF = 15;
		    	break;
		    case eIDX_ISO_800:
			case eIDX_ISO_1600:	
//		    	rNR1.nr_cfg1.bits.GNF = 16;
		    	break;
			default:	
		    	//rNR1.nr_cfg1.bits.GNF = rNR1.nr_cfg1.bits.GNF;
				break;
		}

        break;

    case ECamMode_HDR_Cap_Pass1_MF1:
        rNR1.ctrl.bits.NR_EN = 0;
        break;

//  case ECamMode_Offline_Capture_Pass2:
//  case ECamMode_HDR_Cap_Pass1_MF2:
//  case ECamMode_HDR_Cap_Pass2:
    default:
    //  Usually, NR1 is disabled in capture pass2.
    //  Of course, you can do what you want.
#if 1
        ::memset(rNR1.set, 0, sizeof(ISP_NVRAM_NR1_T));
#endif
        break;
    }
}

/*******************************************************************************
*
*******************************************************************************/
MVOID
IspTuningCustom::
refine_DP(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_DP_T& rDP)
{
    if (ECamMode_FB_PostProcess_NR2_Only == rCamInfo.eCamMode) { // Disable DP in FB
        ::memset(rDP.set, 0, sizeof(ISP_NVRAM_DP_T));
    }

}


/*******************************************************************************
* 
*******************************************************************************/
MVOID
IspTuningCustom::
refine_NR2(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_NR2_T& rNR2)
{
    //  (1) Check to see if it works or not.
    if  ( ECamMode_Offline_Capture_Pass1 == rCamInfo.eCamMode )
    {   //  Usually, NR2 is disabled in capture pass1.
        //  Of course, you can do what you want.
#if 1
        ::memset(rNR2.set, 0, sizeof(ISP_NVRAM_NR2_T));
#endif
        return;
    }

    //  (2) HDR Mode.
    if  ( ECamMode_HDR_Cap_Pass2 == rCamInfo.eCamMode )
    {   //  TODO: Add your code below...
        rNR2.lce_gain.bits.LCE_LINK = 1;
        rNR2.lce_gain.bits.LCE_GAIN0 = 0x8;
        rNR2.lce_gain.bits.LCE_GAIN1 = 0xc;
        rNR2.lce_gain.bits.LCE_GAIN2 = 0xe;
        rNR2.lce_gain.bits.LCE_GAIN3 = 0x10;
        
        rNR2.lce_gain_div.bits.LCE_GAIN_DIV0 = 0x10;
        rNR2.lce_gain_div.bits.LCE_GAIN_DIV1 = 0xb;
        rNR2.lce_gain_div.bits.LCE_GAIN_DIV2 = 0x9;
        rNR2.lce_gain_div.bits.LCE_GAIN_DIV3 = 0x8;
        
		switch(rCamInfo.eIdx_ISO)
		{
			case eIDX_ISO_100:
		        rNR2.ctrl.bits.ENY = 1;
				rNR2.ctrl.bits.MODE = 1;
				rNR2.ctrl.bits.IIR_MODE = 1;
		    	rNR2.cfg2.bits.GNY = 8;
				rNR2.cfg2.bits.GNC = 8;
				rNR2.mode1_cfg1.bits.GNY_H = 5;
				rNR2.mode1_cfg1.bits.GNC_H = 8;
				
				rNR2.mode1_cfg1.bits.C_V_FLT2 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT3 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT4 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT5 = 1;
				
				rNR2.mode1_cfg2.bits.Y_H_FLT4 = 1;
				rNR2.mode1_cfg2.bits.Y_H_FLT5 = 1;
				rNR2.mode1_cfg2.bits.Y_H_FLT6 = 1;
				break;
			case eIDX_ISO_200:
		        rNR2.ctrl.bits.ENY = 1;
				rNR2.ctrl.bits.MODE = 1;
				rNR2.ctrl.bits.IIR_MODE = 1;
		    	rNR2.cfg2.bits.GNY = 8;
				rNR2.cfg2.bits.GNC = 8;
				rNR2.mode1_cfg1.bits.GNY_H = 5;
				rNR2.mode1_cfg1.bits.GNC_H = 8;
				
				rNR2.mode1_cfg1.bits.C_V_FLT2 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT3 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT4 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT5 = 1;
				
				rNR2.mode1_cfg2.bits.Y_H_FLT4 = 1;
				rNR2.mode1_cfg2.bits.Y_H_FLT5 = 1;
				rNR2.mode1_cfg2.bits.Y_H_FLT6 = 1;
				break;
			case eIDX_ISO_400:
		        rNR2.ctrl.bits.ENY = 1;
				rNR2.ctrl.bits.MODE = 1;
				rNR2.ctrl.bits.IIR_MODE = 1;
		    	rNR2.cfg2.bits.GNY = 8;
				rNR2.cfg2.bits.GNC = 8;
				rNR2.mode1_cfg1.bits.GNY_H = 5;
				rNR2.mode1_cfg1.bits.GNC_H = 8;
				
				rNR2.mode1_cfg1.bits.C_V_FLT2 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT3 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT4 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT5 = 1;
				
				rNR2.mode1_cfg2.bits.Y_H_FLT4 = 1;
				rNR2.mode1_cfg2.bits.Y_H_FLT5 = 1;
				rNR2.mode1_cfg2.bits.Y_H_FLT6 = 1;
				break;
			case eIDX_ISO_800:
			case eIDX_ISO_1600:	
		        rNR2.ctrl.bits.ENY = 1;
				rNR2.ctrl.bits.MODE = 1;
				rNR2.ctrl.bits.IIR_MODE = 1;
		    	rNR2.cfg2.bits.GNY = 8;
				rNR2.cfg2.bits.GNC = 8;
				rNR2.mode1_cfg1.bits.GNY_H = 5;
				rNR2.mode1_cfg1.bits.GNC_H = 8;
				
				rNR2.mode1_cfg1.bits.C_V_FLT2 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT3 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT4 = 1;
				rNR2.mode1_cfg1.bits.C_V_FLT5 = 1;
				
				rNR2.mode1_cfg2.bits.Y_H_FLT4 = 1;
				rNR2.mode1_cfg2.bits.Y_H_FLT5 = 1;
				rNR2.mode1_cfg2.bits.Y_H_FLT6 = 1;
				break;
			default:
			    /*
		        rNR2.ctrl.bits.ENY = rNR2.ctrl.bits.ENY;
				rNR2.ctrl.bits.MODE = rNR2.ctrl.bits.MODE;
				rNR2.ctrl.bits.IIR_MODE = rNR2.ctrl.bits.IIR_MODE;
		    	rNR2.cfg2.bits.GNY = rNR2.cfg2.bits.GNY;
				rNR2.cfg2.bits.GNC = rNR2.cfg2.bits.GNC;
				rNR2.mode1_cfg1.bits.GNY_H = rNR2.mode1_cfg1.bits.GNY_H;
				rNR2.mode1_cfg1.bits.GNC_H = rNR2.mode1_cfg1.bits.GNC_H;
				
				rNR2.mode1_cfg1.bits.C_V_FLT2 = rNR2.mode1_cfg1.bits.C_V_FLT2;
				rNR2.mode1_cfg1.bits.C_V_FLT3 = rNR2.mode1_cfg1.bits.C_V_FLT3;
				rNR2.mode1_cfg1.bits.C_V_FLT4 = rNR2.mode1_cfg1.bits.C_V_FLT4;
				rNR2.mode1_cfg1.bits.C_V_FLT5 = rNR2.mode1_cfg1.bits.C_V_FLT5;
				
				rNR2.mode1_cfg2.bits.Y_H_FLT4 = rNR2.mode1_cfg2.bits.Y_H_FLT4;
				rNR2.mode1_cfg2.bits.Y_H_FLT5 = rNR2.mode1_cfg2.bits.Y_H_FLT5;
				rNR2.mode1_cfg2.bits.Y_H_FLT6 = rNR2.mode1_cfg2.bits.Y_H_FLT6;
				*/
				break;
		}

        return;
    }

    //  (3) Face Beautifier Mode
    if (ECamMode_FB_PostProcess_NR2_Only == rCamInfo.eCamMode)
    {
        // CAM+0500H: NR2 Control Register
        rNR2.ctrl.val = 0x00000000;
        rNR2.ctrl.bits.ENY = 1;
        rNR2.ctrl.bits.ENC = 1;
        rNR2.ctrl.bits.MODE = 1;
        rNR2.ctrl.bits.IIR_MODE = 1;

        // CAM+0504H: NR2 Common Configuration Register 1
        rNR2.cfg1.val = 0x00000000;
        rNR2.cfg1.bits.Y_DP_MIN_TH = 0x14;
        rNR2.cfg1.bits.Y_DP_MAX_TH = 0x14;
        rNR2.cfg1.bits.Y_DP_EN = 0x01;
        rNR2.cfg1.bits.YUV444_MODE = 0;
        //rNR2.cfg1.bits.C_SM = 0;
        //rNR2.cfg1.bits.QEC = 0;

        // CAM+0508H: NR2 Configuration Register 2
        rNR2.cfg2.val = 0x00000000;
        rNR2.cfg2.bits.GNC = 0x03;
        rNR2.cfg2.bits.GNY = 0x08;
        rNR2.cfg2.bits.SC1 = 0x07;
        rNR2.cfg2.bits.SY1 = 0x07;
        rNR2.cfg2.bits.S3 = 0x02;
        rNR2.cfg2.bits.S2 = 0x02;
        rNR2.cfg2.bits.UV_SMPL = 0x02;
		
        // CAM+050CH: NR2 Configuration Register 3
        rNR2.pty.val = 0x00000000;
        rNR2.pty.bits.PTY4 = 0x3f;
        rNR2.pty.bits.PTY3 = 0x30;
        rNR2.pty.bits.PTY2 = 0x21;
        rNR2.pty.bits.PTY1 = 0x13;

        // CAM+0510H: NR2 Configuration Register 4
        rNR2.ptc.val = 0x00000000;
        rNR2.ptc.bits.PTC4 = 0x0f;
        rNR2.ptc.bits.PTC3 = 0x08;
        rNR2.ptc.bits.PTC2 = 0x08;
        rNR2.ptc.bits.PTC1 = 0x04;

        // CAM+0514H: NR2 Common Configuration Register 2
        rNR2.luma.val = 0x00000000;
        rNR2.luma.bits.LUMA_AVG = 0;
        rNR2.luma.bits.LUMA_LUTY5 = 0x01;
        rNR2.luma.bits.LUMA_LUTY4 = 0x06;
        rNR2.luma.bits.LUMA_LUTY3 = 0x0a;
        rNR2.luma.bits.LUMA_LUTY2 = 0x10;
        rNR2.luma.bits.LUMA_LUTY1 = 0x10;

        // CAM+0518H: NR2 LCE Configuration Register 1
        rNR2.lce_gain.val = 0x00000000;
        rNR2.lce_gain.bits.LCE_GAIN3 = 0x08;
        rNR2.lce_gain.bits.LCE_GAIN2 = 0x08;
        rNR2.lce_gain.bits.LCE_GAIN1 = 0x08;
        rNR2.lce_gain.bits.LCE_GAIN0 = 0x08;
        rNR2.lce_gain.bits.LCE_LINK = 0;

        // CAM+051CH: NR2 LCE Configuration Register 2
        rNR2.lce_gain_div.val = 0x00000000;
        rNR2.lce_gain_div.bits.LCE_GAIN_DIV3 = 0x10;
        rNR2.lce_gain_div.bits.LCE_GAIN_DIV2 = 0x10;
        rNR2.lce_gain_div.bits.LCE_GAIN_DIV1 = 0x10;
        rNR2.lce_gain_div.bits.LCE_GAIN_DIV0 = 0x10;

        // CAM+0520H: NR2 Mode1 Configuration Register 1
        rNR2.mode1_cfg1.val = 0x00000000;
        rNR2.mode1_cfg1.bits.GNC_H = 0x08;
        rNR2.mode1_cfg1.bits.GNY_H = 0x05;
        rNR2.mode1_cfg1.bits.Y_V_FLT4 = 0x03;
        rNR2.mode1_cfg1.bits.Y_V_FLT3 = 0x02;
        rNR2.mode1_cfg1.bits.Y_V_FLT2 = 0x01;
        rNR2.mode1_cfg1.bits.Y_V_FLT1 = 0x01;
        rNR2.mode1_cfg1.bits.C_V_FLT5 = 0x03;
        rNR2.mode1_cfg1.bits.C_V_FLT4 = 0x03;
        rNR2.mode1_cfg1.bits.C_V_FLT3 = 0x02;
        rNR2.mode1_cfg1.bits.C_V_FLT2 = 0x02;
        rNR2.mode1_cfg1.bits.C_V_FLT1 = 0x01;

        // CAM+0524H: NR2 Mode1 Configuration Register 2
        rNR2.mode1_cfg2.val = 0x00000000;
        rNR2.mode1_cfg2.bits.Y_H_FLT6 = 0x03;
        rNR2.mode1_cfg2.bits.Y_H_FLT5 = 0x02;
        rNR2.mode1_cfg2.bits.Y_H_FLT4 = 0x02;
        rNR2.mode1_cfg2.bits.Y_H_FLT3 = 0x01;
        rNR2.mode1_cfg2.bits.Y_H_FLT2 = 0x01;
        rNR2.mode1_cfg2.bits.Y_H_FLT1 = 0x01;

        // CAM+0528H: NR2 Mode1 Configuration Register 3
        rNR2.mode1_cfg3.val = 0x00000000;
        rNR2.mode1_cfg3.bits.H_PTY4 = 0x3f;
        rNR2.mode1_cfg3.bits.H_PTY3 = 0x30;
        rNR2.mode1_cfg3.bits.H_PTY2 = 0x21;
        rNR2.mode1_cfg3.bits.H_PTY1 = 0x13;
    }

}


/*******************************************************************************
* 
*******************************************************************************/
MVOID
IspTuningCustom::
refine_DM(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_DEMOSAIC_T& rDM)
{
    //  (1) Check to see if it works or not.
    if  ( ECamMode_Offline_Capture_Pass1 == rCamInfo.eCamMode )
        return; //  It does not work in capture pass1.

    //  (2) TODO: Add your code below...
}


/*******************************************************************************
* 
*******************************************************************************/
MVOID
IspTuningCustom::
refine_EE(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_EE_T& rEE)
{
    //  (1) Check to see if it works or not.
    if  ( ECamMode_Offline_Capture_Pass1 == rCamInfo.eCamMode )
        return; //  It does not work in capture pass1.

    //  (2) HDR Mode.
    if  ( ECamMode_HDR_Cap_Pass2 == rCamInfo.eCamMode )
    {   //  TODO: Add your code below...

        return;
    }

    //  (3) TODO: Add your code below...
}

/*******************************************************************************
*
*******************************************************************************/
MVOID
IspTuningCustom::
refine_Saturation(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_SATURATION_T& rSaturation)
{
    //  (1) Check to see if it works or not.
    if  ( ECamMode_Offline_Capture_Pass1 == rCamInfo.eCamMode )
        return; //  It does not work in capture pass1.

    //  (2) HDR Mode.
    if  ( ECamMode_HDR_Cap_Pass2 == rCamInfo.eCamMode )
    {
		MINT8 icTempG1 = 0, icTempG2 = 0, icTempG3 = 0, icTempG4 = 0, icTempG5 = 0;
		switch(rCamInfo.eIdx_ISO)
		{
			case eIDX_ISO_100:
		    	icTempG1 = 0x00;
		    	icTempG2 = 0x00;
		    	icTempG3 = 0x00;
		    	icTempG4 = 0x00;
		    	icTempG5 = 0x00;
		    	break;
			case eIDX_ISO_200:
		    	icTempG1 = 0x00;
		    	icTempG2 = 0x00;
		    	icTempG3 = 0x00;
		    	icTempG4 = 0x00;
		    	icTempG5 = 0x00;
		    	break;
			case eIDX_ISO_400:
		    	icTempG1 = 0x00;
		    	icTempG2 = 0x00;
		    	icTempG3 = 0x00;
		    	icTempG4 = 0x00;
		    	icTempG5 = 0x00;
		    	break;
		    case eIDX_ISO_800:
		    	icTempG1 = 0x00;
		    	icTempG2 = 0x00;
		    	icTempG3 = 0x00;
		    	icTempG4 = 0x00;
		    	icTempG5 = 0x00;
				break;
			case eIDX_ISO_1600:	
		    	icTempG1 = 0x00;
		    	icTempG2 = 0x00;
		    	icTempG3 = 0x00;
		    	icTempG4 = 0x00;
		    	icTempG5 = 0x00;
		    	break;
			default:	
		    	//icTempG1 = 0x00;
		    	//icTempG2 = 0x00;
		    	//icTempG3 = 0x00;
		    	//icTempG4 = 0x00;
		    	//icTempG5 = 0x00;
				break;
		}

		rSaturation.gain.bits.G1 = (rSaturation.gain.bits.G1 > icTempG1) ? (rSaturation.gain.bits.G1 - icTempG1) : 0;
		rSaturation.gain.bits.G2 = (rSaturation.gain.bits.G2 > icTempG2) ? (rSaturation.gain.bits.G2 - icTempG2) : 0;
		rSaturation.gain.bits.G3 = (rSaturation.gain.bits.G3 > icTempG3) ? (rSaturation.gain.bits.G3 - icTempG3) : 0;
		rSaturation.gain.bits.G4 = (rSaturation.gain.bits.G4 > icTempG4) ? (rSaturation.gain.bits.G4 - icTempG4) : 0;
		rSaturation.gain_ofs.bits.G5 = (rSaturation.gain_ofs.bits.G5 > icTempG5) ? (rSaturation.gain_ofs.bits.G5 - icTempG5) : 0;

        return;
    }

}

/*******************************************************************************
*
*******************************************************************************/
MVOID
IspTuningCustom::
refine_Contrast(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_CONTRAST_T& rContrast)
{
    //  (1) Check to see if it works or not.
    if  ( ECamMode_Offline_Capture_Pass1 == rCamInfo.eCamMode )
        return; //  It does not work in capture pass1.

    //  (2) HDR Mode.
    if  ( ECamMode_HDR_Cap_Pass2 == rCamInfo.eCamMode )
    {   //  TODO: Add your code below...

        return;
    }

    //  (3) TODO: Add your code below...
}

/*******************************************************************************
*
*******************************************************************************/
MVOID
IspTuningCustom::
refine_Hue(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_HUE_T& rHue)
{
    //  (1) Check to see if it works or not.
    if  ( ECamMode_Offline_Capture_Pass1 == rCamInfo.eCamMode )
        return; //  It does not work in capture pass1.

    //  (2) HDR Mode.
    if  ( ECamMode_HDR_Cap_Pass2 == rCamInfo.eCamMode )
    {   //  TODO: Add your code below...

        return;
    }

    //  (3) TODO: Add your code below...
}

/*******************************************************************************
* 
*******************************************************************************/
MVOID
IspTuningCustom::
refine_CCM(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_CCM_T& rCCM)
{
    //  (1) Check to see if it works or not.
    if  ( ECamMode_Offline_Capture_Pass1 == rCamInfo.eCamMode )
        return; //  It does not work in capture pass1.

    //  (2) TODO: Add your code below...
    if  ( ECamMode_Online_Preview == rCamInfo.eCamMode ||
          ECamMode_Video == rCamInfo.eCamMode ||
    	  ECamMode_Online_Capture == rCamInfo.eCamMode ||
    	  ECamMode_Online_Capture_ZSD == rCamInfo.eCamMode ||
    	  ECamMode_Offline_Capture_Pass2 == rCamInfo.eCamMode ||
    	  ECamMode_HDR_Cap_Pass1_SF == rCamInfo.eCamMode ||	// refine_CCM() should also be applied to HDR.
    	  ECamMode_HDR_Cap_Pass2 ==  rCamInfo.eCamMode)
          {

          }
}

/*******************************************************************************
*
*******************************************************************************/
MVOID
IspTuningCustom::
refine_OB(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_OB_T& rOB)
{
    switch (rCamInfo.eCamMode)
    {
    //  NORMAL
    case ECamMode_Online_Preview:
    case ECamMode_Video:
    case ECamMode_Online_Capture:
    case ECamMode_Online_Capture_ZSD:
    case ECamMode_Offline_Capture_Pass1:
    //  HDR
    case ECamMode_HDR_Cap_Pass1_SF:     //  HDR Pass1: Single Frame
    case ECamMode_HDR_Cap_Pass1_MF1:    //  HDR Pass1: Multi Frame Stage1
        // TODO: Add your code below...

        break;

    default:
        //  Neen't apply OB since it has been applied in pass1.
        ::memset(rOB.set, 0, sizeof(rOB));
        break;
    }

   
}

/*******************************************************************************
*
*******************************************************************************/
MVOID
IspTuningCustom::
refine_GammaECO(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_GAMMA_ECO_T& rGammaECO)
{
    // Enable gamma ECO
    //rGammaECO.gamma_eco_en.bits.GMA_ECO_EN = 1;
}

/*******************************************************************************
*
*******************************************************************************/
MVOID
IspTuningCustom::
refine_RGB2YCC_YOfst(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_RGB2YCC_YOFST_T& rRGB2YCC_YOfst)
{
    // Adjust Y offset
    //rRGB2YCC_YOfst.rgb2ycc_yofst.bits.YOFST = 3;
}

/*******************************************************************************
* 
*******************************************************************************/
MVOID
IspTuningCustom::
refine_ColorClip(RAWIspCamInfo const& rCamInfo, ISP_NVRAM_CCLIP_T& rColorClip)
{
/*
    // CCLIP_CON CAM+00CCH
    rColorClip.cclip_ctrl.bits.EN = 1;
    // CCLIP_GTC CAM+00D0H
    rColorClip.cclip_gtc.bits.CCLIP_TC = 0x332;
    // CCLIP_ADC CAM+00D4H
    rColorClip.cclip_adc.bits.CCLIP_D_TH1 = 0x200;
    rColorClip.cclip_adc.bits.CCLIP_D_SLOPE = 0x13;
    // CCLIP_BAC CAM+00D8H
    rColorClip.cclip_bac.bits.CCLIP_B_OFFSET = 0x014;
    rColorClip.cclip_bac.bits.CCLIP_B_SLOPE = 0x2;
    rColorClip.cclip_bac.bits.CCLIP_B_DIFF = 0x100;
*/
}

/*******************************************************************************
*
*******************************************************************************/
MVOID
IspTuningCustom::
prepare_edge_gamma(ISP_NVRAM_EDGE_GAMMA_T& rEGamma)
{
    rEGamma.ctrl.bits.ED_GM_EN = 1;
    rEGamma.cfg1.bits.EGAMMA_B1 = 0x45;
    rEGamma.cfg1.bits.EGAMMA_B2 = 0x72;
    rEGamma.cfg1.bits.EGAMMA_B3 = 0x90;
    rEGamma.cfg1.bits.EGAMMA_B4 = 0xA6;
    rEGamma.cfg2.bits.EGAMMA_B5 = 0xC4;
    rEGamma.cfg2.bits.EGAMMA_B6 = 0xD7;
    rEGamma.cfg2.bits.EGAMMA_B7 = 0xE6;
    rEGamma.cfg2.bits.EGAMMA_B8 = 0xF1;
    rEGamma.cfg3.bits.EGAMMA_B9 = 0xF5;
    rEGamma.cfg3.bits.EGAMMA_B10= 0xF9;
    rEGamma.cfg3.bits.EGAMMA_B11= 0xFC;
}


/*******************************************************************************
*
* eIdx_CCM_CCT_old:
*   [in] the previous color temperature index
*           eIDX_CCM_CCT_TL84
*           eIDX_CCM_CCT_CWF
*           eIDX_CCM_CCT_D65
*
* i4CCT:
*   [in] the current color temperature from 3A.
*
* i4FluorescentIndex:
*   [in] the current fluorescent index
*
* return:
*   [out] the current color temperature index
*           eIDX_CCM_CCT_TL84
*           eIDX_CCM_CCT_CWF
*           eIDX_CCM_CCT_D65
*
*******************************************************************************/
EIndex_CCM_CCT_T
IspTuningCustom::
evaluate_CCM_CCT_index  (
    EIndex_CCM_CCT_T const eIdx_CCM_CCT_old, 
    MINT32 const i4CCT, 
    MINT32 const i4FluorescentIndex
)   const
{
    MY_LOG(
        "[+evaluate_CCM_CCT_index]"
        "(eIdx_CCM_CCT_old, i4CCT, i4FluorescentIndex)=(%d, %d, %d)"
        , eIdx_CCM_CCT_old, i4CCT, i4FluorescentIndex
    );

    EIndex_CCM_CCT_T eIdx_CCM_CCT_new = eIdx_CCM_CCT_old;

//    -----------------|---|---|--------------|---|---|------------------
//                                THA TH1 THB              THC TH2  THD

    MINT32 const THA = 3318;
    MINT32 const TH1 = 3484;
    MINT32 const THB = 3667;
    MINT32 const THC = 4810;
    MINT32 const TH2 = 5050;
    MINT32 const THD = 5316;
    MINT32 const F_IDX_TH1 = 25;
    MINT32 const F_IDX_TH2 = -25;

    switch  (eIdx_CCM_CCT_old)
    {
    case eIDX_CCM_CCT_TL84:
        if  ( i4CCT < THB )
        {
            eIdx_CCM_CCT_new = eIDX_CCM_CCT_TL84;
        }
        else if ( i4CCT < THD )
        {
            if  ( i4FluorescentIndex < F_IDX_TH2 )
                eIdx_CCM_CCT_new = eIDX_CCM_CCT_CWF;
            else 
                eIdx_CCM_CCT_new = eIDX_CCM_CCT_TL84;
        }
        else
        {
            eIdx_CCM_CCT_new = eIDX_CCM_CCT_D65;
        }
        break;
    case eIDX_CCM_CCT_CWF:
        if  ( i4CCT < THA )
        {
            eIdx_CCM_CCT_new = eIDX_CCM_CCT_TL84;
        }
        else if ( i4CCT < THD )
        {
            if  ( i4FluorescentIndex > F_IDX_TH1 )
                eIdx_CCM_CCT_new = eIDX_CCM_CCT_TL84;
            else 
                eIdx_CCM_CCT_new = eIDX_CCM_CCT_CWF;
        }
        else 
        {
            eIdx_CCM_CCT_new = eIDX_CCM_CCT_D65;
        }
        break;
    case eIDX_CCM_CCT_D65:
        if  ( i4CCT > THC )
        {
	        eIdx_CCM_CCT_new = eIDX_CCM_CCT_D65;
        } 
        else if ( i4CCT > TH1 )
        {
            if(i4FluorescentIndex > F_IDX_TH2)
                eIdx_CCM_CCT_new = eIDX_CCM_CCT_TL84;
            else 
                eIdx_CCM_CCT_new = eIDX_CCM_CCT_CWF;
        }
        else 
        {
            eIdx_CCM_CCT_new = eIDX_CCM_CCT_TL84;
        }
        break;
    }

//#if ENABLE_MY_LOG
    if  ( eIdx_CCM_CCT_old != eIdx_CCM_CCT_new )
    {
        LOGD(
            "[-evaluate_CCM_CCT_index] CCM CCT Idx(old,new)=(%d,%d)"
            , eIdx_CCM_CCT_old, eIdx_CCM_CCT_new
        );
    }
//#endif
    return  eIdx_CCM_CCT_new;
}


/*******************************************************************************
*
* eIdx_Shading_CCT_old:
*   [in] the previous color temperature index
*           eIDX_Shading_CCT_ALight
*           eIDX_Shading_CCT_CWF
*           eIDX_Shading_CCT_D65
*
* i4CCT:
*   [in] the current color temperature from 3A.
*
*
* return:
*   [out] the current color temperature index
*           eIDX_Shading_CCT_ALight
*           eIDX_Shading_CCT_CWF
*           eIDX_Shading_CCT_D65
*
*******************************************************************************/
EIndex_Shading_CCT_T
IspTuningCustom::
evaluate_Shading_CCT_index  (
    EIndex_Shading_CCT_T const eIdx_Shading_CCT_old, 
    MINT32 const i4CCT,
    MINT32 const i4DaylightFluorescentIndex,
    MINT32 const i4DaylightProb,
    MINT32 const i4DaylightFluorescentProb,
    MINT32 const i4SceneLV
)   const
{
    MY_LOG(
        "[+evaluate_Shading_CCT_index]"
        "(eIdx_Shading_CCT_old, i4CCT, i4DaylightFluorescentIndex, i4DaylightProb, i4DaylightFluorescentProb, i4SceneLV)="
        "(%d, %d, %d, %d, %d, %d)"
        , eIdx_Shading_CCT_old, i4CCT, i4DaylightFluorescentIndex, i4DaylightProb, i4DaylightFluorescentProb, i4SceneLV
    );

    EIndex_Shading_CCT_T eIdx_Shading_CCT_new = eIdx_Shading_CCT_old;

//    -----------------|----|----|--------------|----|----|------------------
//                   THH2  TH2  THL2                   THH1  TH1  THL1

    MINT32 const THL1 = 3257;
    MINT32 const THH1 = 3484;
    MINT32 const TH1 = (THL1+THH1)/2; //(THL1 +THH1)/2
    MINT32 const THL2 = 4673;
    MINT32 const THH2 = 5155;
    MINT32 const TH2 = (THL2+THH2)/2;//(THL2 +THH2)/2

    switch  (eIdx_Shading_CCT_old)
    {
    case eIDX_Shading_CCT_ALight:
        if  ( i4CCT < THH1 )
        {
            eIdx_Shading_CCT_new = eIDX_Shading_CCT_ALight;
        }
        else if ( i4CCT <  TH2)
        {
            eIdx_Shading_CCT_new = eIDX_Shading_CCT_CWF;
        }
        else
        {
            eIdx_Shading_CCT_new = eIDX_Shading_CCT_D65;
        }
        break;
    case eIDX_Shading_CCT_CWF:
        if  ( i4CCT < THL1 )
        {
            eIdx_Shading_CCT_new = eIDX_Shading_CCT_ALight;
        }
        else if ( i4CCT < THH2 )
        {
            eIdx_Shading_CCT_new = eIDX_Shading_CCT_CWF;
        }
        else 
        {
            eIdx_Shading_CCT_new = eIDX_Shading_CCT_D65;
        }
        break;
    case eIDX_Shading_CCT_D65:
        if  ( i4CCT < TH1 )
        {
	     eIdx_Shading_CCT_new = eIDX_Shading_CCT_ALight;
        } 
        else if ( i4CCT < THL2 )
        {
            eIdx_Shading_CCT_new = eIDX_Shading_CCT_CWF;
        }
        else 
        {
            eIdx_Shading_CCT_new = eIDX_Shading_CCT_D65;
        }
        break;
    }

//#if ENABLE_MY_LOG
    if  ( eIdx_Shading_CCT_old != eIdx_Shading_CCT_new )
    {
        LOGD(
            "[-evaluate_Shading_CCT_index] Shading CCT Idx(old,new)=(%d,%d), i4CCT = %d\n"
            , eIdx_Shading_CCT_old, eIdx_Shading_CCT_new,i4CCT
        );
    }
//#endif
    return  eIdx_Shading_CCT_new;
}


/*******************************************************************************
* 
*******************************************************************************/
EIndex_ISO_T
IspTuningCustom::
map_ISO_value_to_index(MUINT32 const u4Iso) const
{
    if      ( u4Iso < 150 )
    {
        return  eIDX_ISO_100;
    }
    else if ( u4Iso < 300 )
    {
        return  eIDX_ISO_200;
    }
    else if ( u4Iso < 600 )
    {
        return  eIDX_ISO_400;
    }
    else if ( u4Iso < 900 )
    {
        return  eIDX_ISO_800;
    }
    return  eIDX_ISO_1600;
}


/*******************************************************************************
*
*   rCamInfo
*       [in]    ISP Camera Info for RAW sensor. Its members are as below:
*
*           eIdx_Scene:
*               SCENE_MODE_OFF,             // Disable scene mode equal Auto mode
*               SCENE_MODE_NORMAL,          // Normal mode
*               SCENE_MODE_ACTION,          // Action mode
*               SCENE_MODE_PORTRAIT,        // Portrait mode
*               SCENE_MODE_LANDSCAPE,       // Landscape
*               SCENE_MODE_NIGHTSCENE,      // Night Scene
*               SCENE_MODE_NIGHTPORTRAIT,   // Night Portrait
*               SCENE_MODE_THEATRE,         // Theatre mode
*               SCENE_MODE_BEACH,           // Beach mode
*               SCENE_MODE_SNOW,            // Snow mode
*               SCENE_MODE_SUNSET,          // Sunset mode
*               SCENE_MODE_STEADYPHOTO,     // Steady photo mode
*               SCENE_MODE_FIREWORKS,       // Fireworks mode
*               SCENE_MODE_SPORTS,          // Sports mode
*               SCENE_MODE_PARTY,           // Party mode
*               SCENE_MODE_CANDLELIGHT,     // Candle light mode
*
*           u4ISOValue:
*               ISO value to determine eISO.
*
*           eIdx_ISO:
*               eIDX_ISO_100, 
*               eIDX_ISO_200, 
*               eIDX_ISO_400, 
*               eIDX_ISO_800, 
*               eIDX_ISO_1600
*
*           i4CCT:
*               Correlated color temperature
*
*           eCCTIndex_CCM:
*               Correlated color temperature index for CCM
*                   eIDX_CCM_CCT_TL84
*                   eIDX_CCM_CCT_CWF
*                   eIDX_CCM_CCT_D65
*
*           u4ZoomRatio_x100:
*               zoom ratio (x100)
*
*           i4LightValue_x10:
*               light value (x10)
*
*
*******************************************************************************/
MBOOL
IspTuningCustom::
is_to_invoke_offline_capture(RAWIspCamInfo const& rCamInfo) const
{
#if 1
    EIndex_Scene_T const eIdx_Scene = rCamInfo.eIdx_Scene;
    EIndex_ISO_T const     eIdx_ISO = rCamInfo.eIdx_ISO;        //  ISO enum
    MUINT32 const        u4ISOValue = rCamInfo.u4ISOValue;      //  real ISO
    MUINT32 const             i4CCT = rCamInfo.i4CCT;
    MUINT32 const  u4ZoomRatio_x100 = rCamInfo.u4ZoomRatio_x100;
    MINT32 const   i4LightValue_x10 = rCamInfo.i4LightValue_x10;
#endif
#if 0
    switch  (eIdx_ISO)
    {
    case eIDX_ISO_100:
    case eIDX_ISO_200:
    case eIDX_ISO_400:
    case eIDX_ISO_800:
    case eIDX_ISO_1600:
    default:
        break;
    }
#endif
#if 0
		if(eIdx_ISO==eIDX_ISO_400 ||eIdx_ISO==eIDX_ISO_800 || eIdx_ISO==eIDX_ISO_1600)
		{
			return  MTRUE;
		}
		else
#endif			
    return  MTRUE;
}


/*******************************************************************************
* 
*******************************************************************************/


