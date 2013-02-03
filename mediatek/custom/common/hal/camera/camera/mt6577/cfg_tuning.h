#ifndef _CFG_TUNING_H_
#define _CFG_TUNING_H_

#include <cutils/properties.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <utils/Errors.h>	// For errno.
#include <cutils/xlog.h>	// For XLOG?().

#define ENABLE_HDR_AE_DEBUG_INFO	0	// This is only temporary solution, when When Koli's DebugInfo is ready, this can be removed.
#if (ENABLE_HDR_AE_DEBUG_INFO)
	#define MAX_LOG_BUF_SIZE	5000
	static unsigned char GS_ucLogBuf[MAX_LOG_BUF_SIZE];	// Buffer to put log message. Will be outputed to file.
	static char* pucLogBufPosition = NULL;	// A pointer pointing to some position in the GS_ucLogBuf[].
	static unsigned int S_u4RunningNumber = 0;	// Record execution counts.


unsigned int DumpToFile(char *fname, unsigned char *pbuf, unsigned int size);
#endif	// ENABLE_HDR_AE_DEBUG_INFO.


/*******************************************************************************
* CRZ for Video
*******************************************************************************/
TuningParam_CRZ_T const&
getParam_CRZ_Video()
{
    static TuningParam_CRZ_T inst = {
        uUpScaleCoeff   : 8,    //  [5 bits; 1~19] Up sample coeff. choose > 12 may get undesirable result, '8' is recommended.
        uDnScaleCoeff   : 15,   //  [5 bits; 1~19] Down sample coeff. '1':blur,'19':sharpest; '15' is recommended.
    };
    char value[32] = {'\0'};
    property_get("Tuning.Vio.up", value, "8");
    inst.uUpScaleCoeff = atoi(value); 
    property_get("Tuning.Vio.dw", value, "15");
    inst.uDnScaleCoeff = atoi(value);   
    return inst;
}

/*******************************************************************************
* CRZ for Preview
*******************************************************************************/
TuningParam_CRZ_T const&
getParam_CRZ_Preview()
{
    static TuningParam_CRZ_T inst = {
        uUpScaleCoeff   : 8,    //  [5 bits; 1~19] Up sample coeff. choose > 12 may get undesirable result, '8' is recommended.
        uDnScaleCoeff   : 15,   //  [5 bits; 1~19] Down sample coeff. '1':blur,'19':sharpest; '15' is recommended.
    };
    char value[32] = {'\0'};
    property_get("Tuning.Prv.up", value, "8");
    inst.uUpScaleCoeff = atoi(value); 
    property_get("Tuning.Prv.dw", value, "15");
    inst.uDnScaleCoeff = atoi(value); 
    return inst;
}

/*******************************************************************************
* CRZ for Capture
*******************************************************************************/
TuningParam_CRZ_T const&
getParam_CRZ_Capture()
{
    static TuningParam_CRZ_T inst = {
        uUpScaleCoeff   : 8,    //  [5 bits; 1~19] Up sample coeff. choose > 12 may get undesirable result, '8' is recommended.
        uDnScaleCoeff   : 15,   //  [5 bits; 1~19] Down sample coeff. '1':blur,'19':sharpest; '15' is recommended.
    };
    char value[32] = {'\0'}; 
    property_get("Tuning.Cap.up", value, "8");
    inst.uUpScaleCoeff = atoi(value); 
    property_get("Tuning.Cap.dw", value, "15");
    inst.uDnScaleCoeff = atoi(value);    
    return inst;
}

/*******************************************************************************
* PRZ for quick view (post view)
*******************************************************************************/
TuningParam_PRZ_T const&
getParam_PRZ_QuickView()
{
    static TuningParam_PRZ_T inst = {
        uUpScaleCoeff   : 8,    //  [5 bits; 1~19] Up sample coeff. choose > 12 may get undesirable result, '8' is recommended.
        uDnScaleCoeff   : 15,   //  [5 bits; 1~19] Down sample coeff. '1':blur,'19':sharpest; '15' is recommended.
        uEEHCoeff       : 0,    //  [4 bits] The strength for horizontal edge.
        uEEVCoeff       : 0,    //  [4 bits] The strength for vertial edge.
    };
    char value[32] = {'\0'}; 
    property_get("Tuning.Quv.up", value, "8");
    inst.uUpScaleCoeff = atoi(value); 
    property_get("Tuning.Quv.dw", value, "15");
    inst.uDnScaleCoeff = atoi(value);  
    property_get("Tuning.Quv.eeh", value, "0");
    inst.uEEHCoeff = atoi(value); 
    property_get("Tuning.Quv.eev", value, "0");
    inst.uEEVCoeff = atoi(value);  
    return inst;
}

/*******************************************************************************
* ATV
*******************************************************************************/
MINT32 get_atv_input_data()
{
	MINT32 AtvInputdata = 1;
	return AtvInputdata;
}


/*******************************************************************************
*  Get FD threshold    Default: 5       0/1/2/3/4/  5  /6/7/8/9/10/
                              loose  (Hight DR/FPR),   --> strict  (Low DR/FPR)
*******************************************************************************/
MINT8 get_fdvt_threshold()  
{
	MINT8 FeatureThreshold = 7;    
	return FeatureThreshold;
}


/*******************************************************************************
*  Get SD threshold    Default: 5   -->    Count Index
*******************************************************************************/

MINT8 get_SD_threshold()
{
	MINT8 SmileThreshold = 5;
	return SmileThreshold;
}

/*******************************************************************************
* ATV disp delay time
*******************************************************************************/
MINT32 get_atv_disp_delay(MINT32 mode)
{
    return ((ATV_MODE_NTSC == mode)?ATV_MODE_NTSC_DELAY:((ATV_MODE_PAL == mode)?ATV_MODE_PAL_DELAY:0));
};

/*******************************************************************************
*  Get ASD threshold
*******************************************************************************/
ASDThreshold_T const&
get_ASD_threshold()
{
	static ASDThreshold_T const inst = {
	     s2IdxWeightBlAe : 1,           /*0~10 */
	     s2IdxWeightBlScd : 8,        /*0~10 */
	     s2IdxWeightLsAe: 1,           /*0~10 */
	    	s2IdxWeightLsAwb: 1,       /*0~10 */
	    	s2IdxWeightLsAf: 1,            /*0~10 */
	    	s2IdxWeightLsScd: 2,        /*0~10 */ 
	    	u1TimeWeightType: 1,       /*0~1 */ 
	    	u1TimeWeightRange: 10,  /*1~10 */ 
	    	s2EvLoThrNight: 50,           /* 50 -> Lv=5 */
	    	s2EvHiThrNight: 80,            /* 70 -> Lv=7 */
	    	s2EvLoThrOutdoor: 90,     /*110 -> Lv=11 */
	    	s2EvHiThrOutdoor: 130,    /*140 -> Lv=14 */
	    	u1ScoreThrNight: 60,          /* 40~100 */
	    	u1ScoreThrBacklit: 50,       /* 40~100 */
	    	u1ScoreThrPortrait: 50,      /* 40~100 */
	    	u1ScoreThrLandscape: 55,   /* 40~100 */
	    	boolBacklitLockEnable: true,
	    	s2BacklitLockEvDiff: -10,   /* -50~0 */
     };	
	return inst;
}



/*******************************************************************************
* HDR exposure setting
*******************************************************************************/
MVOID getHDRExpSetting(const HDRExpSettingInputParam_T& rInput, HDRExpSettingOutputParam_T& rOutput)
{
    // Tuning parameters
    MUINT32 u4MaxHDRExpTimeInUS = 200000; // Manually set, no longer than 0.5s (unit: us)
    MUINT32 u4MaxSafeHDRExpTimeInUS = 31250; // Manually set, no longer than 0.5s (unit: us)
    MUINT32 u4MaxHDRSensorGain = 4848; //Manually set, no larger than max gain in normal capture 
    MUINT32 u4TimeMode = 1; // 0:Depend on default AE parameters; 1: Manually set
    MUINT32 u4GainMode = 1; // 0:Depend on default AE parameters; 1: Manually set
	double dfTargetTopEV =  1.5; // Target EV of long exposure image
	double dfSafeTargetTopEV = 0.5; // Target EV of long exposure image
	double dfTargetBottomEV = -2; // Target EV of short exposure image
	double dfTopEVBound = 0.2; // Long exposure image should be at least "dfTopEVBound" EV or would be discarded (2 frame case)
//  MBOOL bGainEnable = MTRUE; // True: Enable long exposure image to increase sensor gain; False: Disable
    MBOOL bGain0EVLimit = MFALSE;  // True: Limit the gain of 0EV and short exposure image; False: Keep it
    //Add by ChingYi
    // Tuning parameters
//	MUINT32 TargetToneMaxExposureTime = 100000; //Decide the exposure time start to decrease target tone
//	MUINT32 TargetToneCurve = 10000; //Decide the curve to decide target tone
//	MUINT32 TargetToneIn = 150; //Decide the curve to decide target tone

    // Temporary parameters
    MUINT32 u4MaxExpTimeInUS;
    MUINT32 u4MaxSensorGain;
	MUINT32 i;
	double dfRemainGain[3];
	double dfGainDiff[2];
	double dfTopGain     = pow(2, dfTargetTopEV);
	double dfSafeTopGain = pow(2, dfSafeTargetTopEV);
	double dfBottomGain  = pow(2, dfTargetBottomEV);

	if (u4TimeMode == 0) {
		u4MaxExpTimeInUS = rInput.u4MaxAEExpTimeInUS; // Depend on default AE parameters
    }
	else {
		u4MaxExpTimeInUS = u4MaxHDRExpTimeInUS; // Manually set
    }

	if (u4GainMode == 0) {
		u4MaxSensorGain = rInput.u4MaxAESensorGain; // Depend on default AE parameters
	}
	else {
		u4MaxSensorGain = u4MaxHDRSensorGain; // Manually set

	    if (u4MaxSensorGain > rInput.u4MaxSensorAnalogGain) {  
			u4MaxSensorGain = rInput.u4MaxSensorAnalogGain;   
		}
	}

    // Final output without any limitation
	if(rInput.u4SensorGain0EV > u4MaxHDRSensorGain)
		rOutput.u4ExpTimeInUS[2] = static_cast<MUINT32>(rInput.u4ExpTimeInUS0EV * dfSafeTopGain + 0.5);   //High ISO case: Long exposurerInput.u4ExpTimeInUS0EV;	
	else
		rOutput.u4ExpTimeInUS[2] = static_cast<MUINT32>(rInput.u4ExpTimeInUS0EV * dfTopGain + 0.5);   //Low ISO case: Long exposurerInput.u4ExpTimeInUS0EV;
    rOutput.u4ExpTimeInUS[1] = rInput.u4ExpTimeInUS0EV; // 0EV
    rOutput.u4ExpTimeInUS[0] = static_cast<MUINT32>(rInput.u4ExpTimeInUS0EV * dfBottomGain + 0.5); // Short exposure
	rOutput.u4SensorGain[2] = rInput.u4SensorGain0EV;
	rOutput.u4SensorGain[1] = rInput.u4SensorGain0EV;
	rOutput.u4SensorGain[0] = rInput.u4SensorGain0EV;

	if (bGain0EVLimit == MTRUE) {// Limit 0EV or even short exposure image's gain but keep the brightness
		for (MUINT32 i = 0; i < 2; i++) {
			dfRemainGain[i] = 1;

			if (rOutput.u4SensorGain[i] > u4MaxHDRSensorGain) {
				dfRemainGain[i] = static_cast<double>(rOutput.u4SensorGain[i]) / u4MaxHDRSensorGain;
				rOutput.u4SensorGain[i] = u4MaxHDRSensorGain;
			}

			if (dfRemainGain[i] > 1) {
				rOutput.u4ExpTimeInUS[i] = static_cast<MUINT32>(rOutput.u4ExpTimeInUS[i] * dfRemainGain[i] + 0.5);

				if (rOutput.u4ExpTimeInUS[i] > u4MaxHDRExpTimeInUS) { //It means that we cannot keep original brightness, so we decide to keep original setting 
					rOutput.u4ExpTimeInUS[1] = rInput.u4ExpTimeInUS0EV; // 0EV
					rOutput.u4ExpTimeInUS[0] = static_cast<MUINT32>(rInput.u4ExpTimeInUS0EV * dfBottomGain + 0.5); // Short exposure
					rOutput.u4SensorGain[1]   = rInput.u4SensorGain0EV;
					rOutput.u4SensorGain[0]   = rInput.u4SensorGain0EV;
				}
			}
		}
	}

//	if (bGainEnable == MFALSE) {
//		u4MaxSensorGain  = rOutput.u4SensorGain[1]; // MaxSensor gain should be equal to 0EV
//	}

	dfRemainGain[2] = 1;
	if(rInput.u4ExpTimeInUS0EV>u4MaxSafeHDRExpTimeInUS)
       u4MaxSafeHDRExpTimeInUS = rInput.u4ExpTimeInUS0EV;

	if (rOutput.u4ExpTimeInUS[2] > u4MaxSafeHDRExpTimeInUS) {
		dfRemainGain[2] = static_cast<double>(rOutput.u4ExpTimeInUS[2]) / u4MaxSafeHDRExpTimeInUS;
		rOutput.u4ExpTimeInUS[2] =  u4MaxSafeHDRExpTimeInUS;
	}

	if (dfRemainGain[2] > 1) {
		rOutput.u4SensorGain[2] = static_cast<MUINT32>(rOutput.u4SensorGain[2] * dfRemainGain[2] + 0.5);
		if (rOutput.u4SensorGain[2] > u4MaxSensorGain) {
			dfRemainGain[2] = static_cast<double>(rOutput.u4SensorGain[2]) / u4MaxSensorGain;
			rOutput.u4SensorGain[2] = u4MaxSensorGain;
		}
		else
			dfRemainGain[2] = 1;
	}

	if (dfRemainGain[2] > 1) {
		rOutput.u4ExpTimeInUS[2] = static_cast<MUINT32>(rOutput.u4ExpTimeInUS[2] * dfRemainGain[2] + 0.5);

		if (rOutput.u4ExpTimeInUS[2] >u4MaxExpTimeInUS)
		{
			dfRemainGain[2] = static_cast<double>(rOutput.u4ExpTimeInUS[2]) / u4MaxExpTimeInUS;
			rOutput.u4ExpTimeInUS[2]  = u4MaxExpTimeInUS;
		}
	}

	dfGainDiff[0] = static_cast<double>(rOutput.u4SensorGain[0]*rOutput.u4ExpTimeInUS[0]) / rOutput.u4SensorGain[1] / rOutput.u4ExpTimeInUS[1];
	dfGainDiff[1] = static_cast<double>(rOutput.u4SensorGain[2]*rOutput.u4ExpTimeInUS[2]) / rOutput.u4SensorGain[1] / rOutput.u4ExpTimeInUS[1];

	rOutput.u1FlareOffset[1] = rInput.u1FlareOffset0EV;
	if(rOutput.u1FlareOffset[1]>4)
       rOutput.u1FlareOffset[1] = 4;

	rOutput.u1FlareOffset[0] = static_cast<MUINT8>(rOutput.u1FlareOffset[1] * dfGainDiff[0] + 0.5);
	rOutput.u1FlareOffset[2] = static_cast<MUINT8>(rOutput.u1FlareOffset[1] * dfGainDiff[1] + 0.5);

	if(rOutput.u1FlareOffset[2]>63)
         rOutput.u1FlareOffset[2] = 63; //hardware limitation --> may lead to alignment issue

	double dfTopGainBound = pow(2, dfTopEVBound);
	if (dfTopGainBound < dfGainDiff[1]) {
		rOutput.u4OutputFrameNum = 3;
    }
	else {
		rOutput.u4OutputFrameNum = 2;
	}

	//Add by ChingYi
//	if(rOutput.u4ExpTimeInUS[2] > TargetToneMaxExposureTime && rOutput.u4OutputFrameNum == 3)
//		rOutput.u4TargetTone = TargetToneIn - (rOutput.u4ExpTimeInUS[2]-TargetToneMaxExposureTime)/TargetToneCurve;
//	else
//		rOutput.u4TargetTone = TargetToneIn;
	rOutput.u4TargetTone = 150;

	rOutput.u4FinalGainDiff[0] = static_cast<MUINT32>(1024 / dfGainDiff[0] + 0.5);
	rOutput.u4FinalGainDiff[1] = static_cast<MUINT32>(1024 / dfGainDiff[1] + 0.5);


#if 0 // for debug only: adb logcat HdrCustome:V *:S
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "HdrCustome"

    XLOGD("System Paramters\n");
    XLOGD("0EV Exposure Time = %d\n0EV Sensor Gain = %d\n", rInput.u4ExpTimeInUS0EV, rInput.u4SensorGain0EV);
	XLOGD("Max Exposure Time 0EV = %d\nMaxSensor Gain 0EV = %d\n", rInput.u4MaxAEExpTimeInUS, rInput.u4MaxAESensorGain);
	XLOGD("Max Exposure Time Sensor = %d\nMaxSensor Gain Sensor = %d\n", u4MaxHDRExpTimeInUS, u4MaxHDRSensorGain);
    XLOGD("Final Max Exposure Time = %d\nFinal MaxSensor Gain = %d\n", u4MaxExpTimeInUS, u4MaxSensorGain);

    XLOGD("\nTuning Paramters\n");
	XLOGD("Target Top EV = %f\nTarget Bottom EV = %f\n", dfTargetTopEV, dfTargetBottomEV);
	XLOGD("dfTopGainBound = %f\n",dfTopGainBound);
   XLOGD("bGain0EVLimit = %s\n", (bGain0EVLimit ? "true" : "false"));

	XLOGD("\nOutput Paramters\n");
	for (i = 0; i < 3; i++) {
		XLOGD("Final Frame %d ExposureTime = %d\nSensorGain = %d\n", i, rOutput.u4ExpTimeInUS[i], rOutput.u4SensorGain[i]);
    }
    XLOGD("Final EVdiff[0] = %d\nFinal EVdiff[1] = %d\n", rOutput.u4FinalGainDiff[0], rOutput.u4FinalGainDiff[1]);
	XLOGD("OutputFrameNum = %d\n", rOutput.u4OutputFrameNum);
	XLOGD("Final FlareOffsetOut[0]= %d\nFinal FlareOffsetOut[1]= %d\nFinal FlareOffsetOut[2]= %d\n", rOutput.u1FlareOffset[0], rOutput.u1FlareOffset[1], rOutput.u1FlareOffset[2]);
    XLOGD("Final TargetTone1 = %d\n", rOutput.u4TargetTone);	
#endif	// for debug only: adb logcat HdrCustome:V *:S

#if (ENABLE_HDR_AE_DEBUG_INFO)	// Save HDR AE debug info to a file.
{
	// Increase 4-digit running number (range: 1 ~ 9999).
	if (S_u4RunningNumber >= 9999)
		S_u4RunningNumber = 1;
	else
		S_u4RunningNumber++;
		
	pucLogBufPosition = (char*)GS_ucLogBuf;
    ::sprintf(pucLogBufPosition, "< No.%04d > ----------------------------------------------------------------------\n", S_u4RunningNumber);
    pucLogBufPosition += strlen(pucLogBufPosition);
    ::sprintf(pucLogBufPosition, "[System Paramters]\n");
    pucLogBufPosition += strlen(pucLogBufPosition);
    ::sprintf(pucLogBufPosition, "0EV Exposure Time = %d\n0EV Sensor Gain = %d\n", rInput.u4ExpTimeInUS0EV, rInput.u4SensorGain0EV);
    pucLogBufPosition += strlen(pucLogBufPosition);
    ::sprintf(pucLogBufPosition, "Max Exposure Time Sensor= %d\nMaxSensor Gain Sensor= %d\n", rInput.u4MaxAEExpTimeInUS, rInput.u4MaxAESensorGain);
    pucLogBufPosition += strlen(pucLogBufPosition);
    ::sprintf(pucLogBufPosition, "Max Exposure Time Manual= %d\nMaxSensor Gain Manual= %d\n", u4MaxHDRExpTimeInUS, u4MaxHDRSensorGain);
    pucLogBufPosition += strlen(pucLogBufPosition);
    ::sprintf(pucLogBufPosition, "Max Exposure Time = %d\nMaxSensor Gain = %d\n", u4MaxExpTimeInUS, u4MaxSensorGain);
    pucLogBufPosition += strlen(pucLogBufPosition);

    ::sprintf(pucLogBufPosition, "\n[Tuning Paramters]\n");
    pucLogBufPosition += strlen(pucLogBufPosition);
	::sprintf(pucLogBufPosition, "Target Top EV = %f\nTarget Bottom EV = %f\n", dfTargetTopEV, dfTargetBottomEV);
    pucLogBufPosition += strlen(pucLogBufPosition);
	::sprintf(pucLogBufPosition, "dfTopGainBound = %f\n",dfTopGainBound);
    pucLogBufPosition += strlen(pucLogBufPosition);
	::sprintf(pucLogBufPosition, "bGain0EVLimit = %s\n", (bGain0EVLimit ? "true" : "false"));
    pucLogBufPosition += strlen(pucLogBufPosition);

	::sprintf(pucLogBufPosition, "\n[Output Paramters]\n");
    pucLogBufPosition += strlen(pucLogBufPosition);
	for (i = 0; i < 3; i++) {
		::sprintf(pucLogBufPosition, "Final Frame %d ExposureTime = %d\nSensorGain = %d\n", i, rOutput.u4ExpTimeInUS[i], rOutput.u4SensorGain[i]);
	    pucLogBufPosition += strlen(pucLogBufPosition);
    }
    ::sprintf(pucLogBufPosition, "Final EVdiff[0] = %d\nFinal EVdiff[1] = %d\n", rOutput.u4FinalGainDiff[0], rOutput.u4FinalGainDiff[1]);
    pucLogBufPosition += strlen(pucLogBufPosition);
	::sprintf(pucLogBufPosition, "OutputFrameNum = %d\n", rOutput.u4OutputFrameNum);
    pucLogBufPosition += strlen(pucLogBufPosition);
	::sprintf(pucLogBufPosition, "Final FlareOffsetOut[0]= %d\nFinal FlareOffsetOut[1]= %d\nFinal FlareOffsetOut[2]= %d\n", rOutput.u1FlareOffset[0], rOutput.u1FlareOffset[1], rOutput.u1FlareOffset[2]);
    pucLogBufPosition += strlen(pucLogBufPosition);
	::sprintf(pucLogBufPosition, "Final TargetTone= %d\n", rOutput.u4TargetTone);
    pucLogBufPosition += strlen(pucLogBufPosition);

	char szFileName[100];
	//::sprintf(szFileName, "sdcard/Photo/%04d_HDR_ExposureSetting.txt", S_u4RunningNumber);	// For ALPS.GB2.
	::sprintf(szFileName, "sdcard/DCIM/Camera/%04d_HDR_ExposureSetting.txt", S_u4RunningNumber);	// For ALPS.ICS.
	DumpToFile(szFileName, (unsigned char *)GS_ucLogBuf, MAX_LOG_BUF_SIZE);

}
#endif	// ENABLE_HDR_AE_DEBUG_INFO

}

/*******************************************************************************
* Get PCA LUT for face beautifier
*******************************************************************************/
FB_PCA_LUT_T&  getFBPCALut()
{
    static FB_PCA_LUT_T rFBPcaLut;

        return (rFBPcaLut);

}



/*******************************************************************************
*
********************************************************************************/
#if (ENABLE_HDR_AE_DEBUG_INFO)
unsigned int DumpToFile(
    char *fname,
    unsigned char *pbuf, 
    unsigned int size
)
{
    int nw, cnt = 0;
    unsigned int written = 0;


    XLOGD("[DumpToFile] S_u4RunningNumber: %d.\n", S_u4RunningNumber);
    XLOGD("[DumpToFile] opening file [%s]\n", fname);
	FILE* pFp = fopen(fname, "a+t");	// a+: Opens for reading and appending. t: Open in text (translated) mode.
    if (pFp < 0) {
        XLOGE("[DumpToFile] failed to create file [%s]: %s", fname, strerror(errno));
        return 0x80000000;
    }

    XLOGD("[DumpToFile] writing %d bytes to file [%s]\n", size, fname);
    while (written < size) {
        nw = fwrite(pbuf + written, sizeof( char ), size - written, pFp);
        if (nw < 0) {
            XLOGE("[DumpToFile] failed to write to file [%s]: %s", fname, strerror(errno));
            break;
        }
        written += nw;
        cnt++;
    }
    XLOGD("[DumpToFile] done writing %d bytes to file [%s] in %d passes\n", size, fname, cnt);
    fclose(pFp);

    return 0;
    
}
#endif	// ENABLE_HDR_AE_DEBUG_INFO.

/*******************************************************************************
* Refine capture ISP RAW gain
*******************************************************************************/
MVOID  refineCaptureISPRAWGain(MUINT32 u4SensorGain, MUINT32& u4RAWGain_R, MUINT32& u4RAWGain_Gr, MUINT32& u4RAWGain_Gb, MUINT32& u4RAWGain_B)
{
#if 0
    XLOGD("[refineCaptureISPRAWGain] u4SensorGain=%d, u4RAWGain_R=%d, u4RAWGain_Gr=%d, u4RAWGain_Gb=%d, u4RAWGain_B=%d\n",
          u4SensorGain, u4RAWGain_R, u4RAWGain_Gr, u4RAWGain_Gb, u4RAWGain_B);
#endif
}

#endif //  _CFG_TUNING_H_

