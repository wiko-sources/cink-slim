//
#define LOG_TAG "IspTuningCustom_user"
#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif
//
#include <utils/Errors.h>
#include <cutils/log.h>
//
#define USE_CUSTOM_ISP_TUNING
#include "isp_tuning.h"
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Local Define
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#define NVRAM_IDX_EE_SHARPNESS_USR_LOW      10  //  Customizable
#define NVRAM_IDX_EE_SHARPNESS_USR_HIGH     11  //  Customizable
#define NVRAM_IDX_SATURATION_USR_LOW        5   //  Customizable
#define NVRAM_IDX_SATURATION_USR_HIGH       6   //  Customizable


namespace NSIspTuning
{


/*******************************************************************************
* EE
*******************************************************************************/
template <>
MUINT32
IspTuningCustom::
map_user_setting_to_nvram_index<ISP_NVRAM_EE_T>(
    MUINT8 const u8Idx_nvram_current,
    IspUsrSelectLevel_T const& rUsr
)
{
    STATIC_CHECK(NVRAM_IDX_EE_SHARPNESS_USR_LOW<=NVRAM_IDX_EE_SHARPNESS_USR_HIGH, BAD_NVRAM_IDX_EE_SHARPNESS_USR_LOW_HIGH);
    STATIC_CHECK(NVRAM_IDX_EE_SHARPNESS_USR_LOW <NVRAM_EE_TBL_NUM, NVRAM_IDX_EE_SHARPNESS_USR_LOW_OVER_RANGE);
    STATIC_CHECK(NVRAM_IDX_EE_SHARPNESS_USR_HIGH<NVRAM_EE_TBL_NUM, NVRAM_IDX_EE_SHARPNESS_USR_HIGH_OVER_RANGE);
    //--------------------------------------------------------------------------
    EIndex_Isp_Edge_T const eIdx_Edge = rUsr.eIdx_Edge;
    //--------------------------------------------------------------------------
    //
    MUINT8 u8Idx_EE = u8Idx_nvram_current;
    switch ( eIdx_Edge )
    {
    case ISP_EDGE_LOW:
        u8Idx_EE = NVRAM_IDX_EE_SHARPNESS_USR_LOW;
        break;
    case ISP_EDGE_HIGH:
        u8Idx_EE = NVRAM_IDX_EE_SHARPNESS_USR_HIGH;
        break;
    case ISP_EDGE_MIDDLE:
    default:
        //  By default, we use the current nvram index of EE
        //  for the middle level of user setting of sharpness.
        break;
    }
    return  u8Idx_EE;
}


/*******************************************************************************
* Saturation
*******************************************************************************/
template <>
MUINT32
IspTuningCustom::
map_user_setting_to_nvram_index<ISP_NVRAM_SATURATION_T>(
    MUINT8 const u8Idx_nvram_current,
    IspUsrSelectLevel_T const& rUsr
)
{
    STATIC_CHECK(NVRAM_IDX_SATURATION_USR_LOW<=NVRAM_IDX_SATURATION_USR_HIGH, BAD_NVRAM_IDX_SATURATION_USR_LOW_HIGH);
    STATIC_CHECK(NVRAM_IDX_SATURATION_USR_LOW <NVRAM_SAT_TBL_NUM, NVRAM_IDX_SATURATION_USR_LOW_OVER_RANGE);
    STATIC_CHECK(NVRAM_IDX_SATURATION_USR_HIGH<NVRAM_SAT_TBL_NUM, NVRAM_IDX_SATURATION_USR_HIGH_OVER_RANGE);
    //--------------------------------------------------------------------------
    EIndex_Isp_Saturation_T const   eIdx_Sat = rUsr.eIdx_Sat;
    //--------------------------------------------------------------------------
    //
    MUINT8 u8Idx_Sat = u8Idx_nvram_current;
    switch ( eIdx_Sat )
    {
    case ISP_SAT_LOW:
        u8Idx_Sat = NVRAM_IDX_SATURATION_USR_LOW;
        break;
    case ISP_SAT_HIGH:
        u8Idx_Sat = NVRAM_IDX_SATURATION_USR_HIGH;
        break;
    case ISP_SAT_MIDDLE:
    default:
        //  By default, we use the current nvram index of saturation
        //  for the middle level of user setting of saturation.
        break;
    }
    return  u8Idx_Sat;
}


/*******************************************************************************
* Hue
*******************************************************************************/
template <>
MUINT32
IspTuningCustom::
map_user_setting_to_nvram_index<ISP_NVRAM_HUE_T>(
    MUINT8 const u8Idx_nvram_current,
    IspUsrSelectLevel_T const& rUsr
)
{
    EIndex_Isp_Hue_T const  eIdx_Hue = rUsr.eIdx_Hue;
    //--------------------------------------------------------------------------
    //
    //  By default, the level of user setting of hue is the nvram index.
    MUINT8 u8Idx_Hue = eIdx_Hue;
    return  u8Idx_Hue;
}


/*******************************************************************************
* Contrast
*******************************************************************************/
template <>
MUINT32
IspTuningCustom::
map_user_setting_to_nvram_index<ISP_NVRAM_CONTRAST_T>(
    MUINT8 const u8Idx_nvram_current,
    IspUsrSelectLevel_T const& rUsr
)
{
    STATIC_CHECK(NUM_OF_ISP_CONTRAST*NUM_OF_ISP_BRIGHT<=NVRAM_CONTRAST_TBL_NUM, NVRAM_CONTRAST_TBL_NUM);
    //--------------------------------------------------------------------------
    EIndex_Isp_Contrast_T const   eIdx_Contrast = rUsr.eIdx_Contrast;
    EIndex_Isp_Brightness_T const eIdx_Bright   = rUsr.eIdx_Bright;
    //--------------------------------------------------------------------------
    //
    //  By default, the level of user setting of contrast & brightness
    //  determines the nvram index.
    //  [Contrast + Brightness]
    MUINT8 u8Idx_Contrast = eIdx_Bright * NUM_OF_ISP_CONTRAST
                          + eIdx_Contrast;
    return  u8Idx_Contrast;
}


/*******************************************************************************
*
*******************************************************************************/
};  //NSIspTuning


