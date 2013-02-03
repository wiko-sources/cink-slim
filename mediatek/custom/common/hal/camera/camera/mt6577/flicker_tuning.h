#ifndef _FLICKER_TUNING_H_
#define _FLICKER_TUNING_H_

/*******************************************************************************
* Flicker detection
*******************************************************************************/
FlickerThresholdSetting_T const&  
getFlickerThresPara()
{
    static FlickerThresholdSetting_T const inst = {
        u4FlickerPoss1  : 9,          // impossible flicker
        u4FlickerPoss2  : 11,        // maybe flicker exist
        u4FlickerFreq1  : 34,        // flicker frequency detect 
        u4FlickerFreq2  : 39,        // flicker frequency detect
        u4ConfidenceLevel1  : 3,  // flicker confidence level
        u4ConfidenceLevel2  : 3,  // flicker confidence level
        u4ConfidenceLevel3  : 3,  // flicker confidence level
    };
    return inst;
}

/*******************************************************************************
* 
*******************************************************************************/
#endif //  _CFG_TUNING_H_

