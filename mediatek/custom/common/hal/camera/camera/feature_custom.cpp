#define LOG_TAG "feature_custom"
//
#include <utils/Errors.h>
#include <cutils/log.h>
//
#define USE_CAMERA_FEATURE_MACRO 1  //define before "camera_feature.h"
#include "camera_feature.h"


/*******************************************************************************
* MACRO Define: Scene Independent
*******************************************************************************/
#define GETFINFO_RAW_MAIN()      \
        _GETFINFO_SCENE_INDEP("[Custom RAW Main]")
#define END_GETFINFO_RAW_MAIN()  \
    _END_GETFINFO_SCENE_INDEP("[Custom RAW Main]")
#define GETFINFO_RAW_SUB()      \
        _GETFINFO_SCENE_INDEP("[Custom RAW Sub]")
#define END_GETFINFO_RAW_SUB()  \
    _END_GETFINFO_SCENE_INDEP("[Custom RAW Sub]")
#define GETFINFO_YUV_MAIN()      \
        _GETFINFO_SCENE_INDEP("[Custom YUV Main]")
#define END_GETFINFO_YUV_MAIN()  \
    _END_GETFINFO_SCENE_INDEP("[Custom YUV Main]")
#define GETFINFO_YUV_SUB()      \
        _GETFINFO_SCENE_INDEP("[Custom YUV Sub]")
#define END_GETFINFO_YUV_SUB()  \
    _END_GETFINFO_SCENE_INDEP("[Custom YUV Sub]")


/*******************************************************************************
* MACRO Define: Config Feature
*******************************************************************************/
#define CHECK_FID_SI
#define CONFIG_FEATURE  CONFIG_FEATURE_SI


namespace NSFeature {
namespace NSCustom  {


#include "cfg_ftbl_custom_raw_main.h"
PF_GETFINFO_SCENE_INDEP_T
GetFInfo_RAW_Main()
{
#ifndef CUSTOM_FEATURE_RAW_MAIN
    #define CUSTOM_FEATURE_RAW_MAIN 0
#endif
#if (CUSTOM_FEATURE_RAW_MAIN==0)
    return  NULL;
#else
    return  NSRAW::NSMain::GetFInfo;
#endif
}


#include "cfg_ftbl_custom_raw_sub.h"
PF_GETFINFO_SCENE_INDEP_T
GetFInfo_RAW_Sub()
{
#ifndef CUSTOM_FEATURE_RAW_SUB
    #define CUSTOM_FEATURE_RAW_SUB 0
#endif
#if (CUSTOM_FEATURE_RAW_SUB==0)
    return  NULL;
#else
    return  NSRAW::NSSub::GetFInfo;
#endif
}


#include "cfg_ftbl_custom_yuv_main.h"
PF_GETFINFO_SCENE_INDEP_T
GetFInfo_YUV_Main()
{
#ifndef CUSTOM_FEATURE_YUV_MAIN
    #define CUSTOM_FEATURE_YUV_MAIN 0
#endif
#if (CUSTOM_FEATURE_YUV_MAIN==0)
    return  NULL;
#else
    return  NSYUV::NSMain::GetFInfo;
#endif
}


#include "cfg_ftbl_custom_yuv_sub.h"
PF_GETFINFO_SCENE_INDEP_T
GetFInfo_YUV_Sub()
{
#ifndef CUSTOM_FEATURE_YUV_SUB
    #define CUSTOM_FEATURE_YUV_SUB 0
#endif
#if (CUSTOM_FEATURE_YUV_SUB==0)
    return  NULL;
#else
    return  NSYUV::NSSub::GetFInfo;
#endif
}


};  //  namespace NSCustom
};  //  namespace NSFeature

