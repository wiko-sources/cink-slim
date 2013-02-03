#ifndef _ISP_TUNING_H_
#define _ISP_TUNING_H_


namespace NSIspTuning
{


/*******************************************************************************
* 
*******************************************************************************/
typedef enum MERROR_ENUM
{
    MERR_OK         = 0, 
    MERR_UNKNOWN    = 0x80000000, // Unknown error
    MERR_UNSUPPORT, 
    MERR_BAD_PARAM, 
    MERR_BAD_CTRL_CODE, 
    MERR_BAD_FORMAT, 
    MERR_BAD_ISP_DRV, 
    MERR_BAD_NVRAM_DRV, 
    MERR_BAD_SENSOR_DRV, 
    MERR_BAD_SYSRAM_DRV, 
    MERR_SET_ISP_REG, 
    MERR_NO_MEM, 
    MERR_NO_SYSRAM_MEM, 
    MERR_NO_RESOURCE, 
    MERR_CUSTOM_DEFAULT_INDEX_NOT_FOUND, 
    MERR_CUSTOM_NOT_READY, 
    MERR_PREPARE_HW, 
    MERR_APPLY_TO_HW
} MERROR_ENUM_T;


/*******************************************************************************
* Operation Mode
*******************************************************************************/
typedef enum
{
    EOperMode_Normal    = 0, 
    EOperMode_PureRaw, 
    EOperMode_Meta, 
} EOperMode_T;


/*******************************************************************************
* 
*******************************************************************************/
typedef enum
{
    //  NORMAL
    ECamMode_Video              = 0, 
    ECamMode_Online_Preview,
    ECamMode_Online_Capture, 
    ECamMode_Online_Capture_ZSD,
    ECamMode_Offline_Capture_Pass1, 
    ECamMode_Offline_Capture_Pass2, 
    //  HDR
    ECamMode_HDR_Cap_Pass1_SF,  //  Pass1: Single Frame
    ECamMode_HDR_Cap_Pass1_MF1, //  Pass1: Multi Frame Stage1
    ECamMode_HDR_Cap_Pass1_MF2, //  Pass1: Multi Frame Stage2
    ECamMode_HDR_Cap_Pass2,     //  Pass2
    // YUV2JPG
    ECamMode_YUV2JPG_Scalado,   // YUV -> JPG (Scalado): disable EE, NR2, PCA, YCCGO
    ECamMode_YUV2JPG_ZSD,       // YUV -> JPG (ZSD): enable NR2, disable EE, PCA, YCCGO
    // Face Beautifier
    ECamMode_FB_PostProcess_NR2_Only, // do NR2, disable EE, PCA, and YCCGO
    ECamMode_FB_PostProcess_PCA_Only  // do PCA, disable EE, NR2, and YCCGO
} ECamMode_T;


/*******************************************************************************
* 
*******************************************************************************/
typedef enum
{
    ESensorRole_Main = 0,   //   Main Sensor
    ESensorRole_Sub,        //   Sub Sensor
    ESensorRole_ATV, 
}   ESensorRole_T;


typedef enum
{
    ESensorType_RAW = 0,    //  RAW Sensor
    ESensorType_YUV,        //  YUV Sensor
}   ESensorType_T;


};  //  NSIspTuning


/*******************************************************************************
* 
*******************************************************************************/
#ifdef  USE_CUSTOM_ISP_TUNING

    #include "camera_feature.h"
    using namespace NSFeature;
    #include "isp_tuning_debug.h"
    #include "isp_tuning_cam_info.h"
    #include "isp_tuning_idx.h"
    #include "isp_tuning_custom.h"

#endif  //  USE_CUSTOM_ISP_TUNING
/*******************************************************************************
* 
*******************************************************************************/


#endif //  _ISP_TUNING_H_

