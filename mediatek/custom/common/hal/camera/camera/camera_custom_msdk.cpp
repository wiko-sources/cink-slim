// seanlin test \mediatek\custom\common\hal\camera\camera\camera_custom_msdk.cpp
//#include "MediaLog.h"
#include "camera_custom_msdk.h"
#include "kd_imgsensor_define.h"
//#include "msdk_isp_exp.h"
//#include "camera_tuning_para.h"
//#include "msdk_sensor_exp.h"

#include "camera_custom_sensor.h"

#include <utils/Errors.h>
#include <utils/Log.h>
#include <fcntl.h>

#undef LOG_TAG
#define LOG_TAG "CAM_CUS_MSDK"

#define CAM_MSDK_LOG(fmt, arg...)    LOGD(LOG_TAG " "fmt, ##arg)
#define CAM_MSDK_ERR(fmt, arg...)    LOGE(LOG_TAG "Err: %5d: "fmt, __LINE__, ##arg)



MSDK_SENSOR_INIT_FUNCTION_STRUCT *pstSensorInitFunc = NULL;
MSDK_LENS_INIT_FUNCTION_STRUCT LensInitFunc[MAX_NUM_OF_SUPPORT_LENS] =
{ {0,0,{0},NULL},
  {0,0,{0},NULL},
  {0,0,{0},NULL},
  {0,0,{0},NULL},
};

MUINT32 gMainLensIdx;
MUINT32 gSubLensIdx;

#if 0
void GetDSCSupportInfo(PDSC_INFO_STRUCT pDSCSupportInfo)
{
	memcpy(pDSCSupportInfo, &DSCSupportInfo, sizeof(DSC_INFO_STRUCT));
}	/* GetDSCSupportInfo() */

void GETCameraDeviceSupportInfo(PDEVICE_INFO_STRUCT pDeviceSupportInfo)
{
	memcpy(pDeviceSupportInfo, &DeviceSupportInfo, sizeof(DEVICE_INFO_STRUCT));
} /* GETCameraDeviceSupportInfo() */
#endif

/*******************************************************************************
*
********************************************************************************/
MUINT32 cameraCustomInit()
{
    GetSensorInitFuncList(&pstSensorInitFunc);
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
void GetCameraDefaultPara(MUINT32 SensorId,
						  PNVRAM_CAMERA_PARA_STRUCT pCameraParaDefault,
						  PNVRAM_CAMERA_3A_STRUCT pCamera3ANVRAMDefault,
						  PNVRAM_CAMERA_SHADING_STRUCT pCameraShadingDefault,
						  PNVRAM_CAMERA_DEFECT_STRUCT pCameraDefectDefault,
						  P3A_PARAM_T pCamera3AParam,
						  P3A_STAT_CONFIG_PARAM_T pCamera3AStatConfigParam)
{
    MUINT32 i;

    if (NULL == pstSensorInitFunc[0].getCameraDefault)
    {
        CAM_MSDK_LOG("[GetCameraDefaultPara]: uninit yet\n\n");
        return; //not initial pstSensorInitFunc yet
    }
    
    for (i=0;i<MAX_NUM_OF_SUPPORT_SENSOR;i++)
    {
        if (SensorId == pstSensorInitFunc[i].SensorId)
        {
            break;
        }
    }

    if (MAX_NUM_OF_SUPPORT_SENSOR == i)
    {
        //return 1; //no match sensorId
    }

	if (pCameraParaDefault!=NULL)
        pstSensorInitFunc[i].getCameraDefault(CAMERA_NVRAM_DATA_PARA,(VOID*)pCameraParaDefault,sizeof(NVRAM_CAMERA_PARA_STRUCT));

	if (pCamera3ANVRAMDefault != NULL)
	    pstSensorInitFunc[i].getCameraDefault(CAMERA_NVRAM_DATA_3A,(VOID*)pCamera3ANVRAMDefault,sizeof(NVRAM_CAMERA_3A_STRUCT));

	if (pCameraShadingDefault!=NULL)
	    pstSensorInitFunc[i].getCameraDefault(CAMERA_NVRAM_DATA_SHADING,(VOID*)pCameraShadingDefault,sizeof(NVRAM_CAMERA_SHADING_STRUCT));

	if (pCameraDefectDefault!=NULL)
	    pstSensorInitFunc[i].getCameraDefault(CAMERA_NVRAM_DATA_DEFECT,(VOID*)pCameraDefectDefault,sizeof(NVRAM_CAMERA_DEFECT_STRUCT));

    if (pCamera3AParam!=NULL)
	    pstSensorInitFunc[i].getCameraDefault(CAMERA_DATA_3A_PARA,(VOID*)pCamera3AParam,sizeof(AAA_PARAM_T));

    if (pCamera3AStatConfigParam!=NULL)
	    pstSensorInitFunc[i].getCameraDefault(CAMERA_DATA_3A_STAT_CONFIG_PARA,(VOID*)pCamera3AStatConfigParam,sizeof(AAA_STAT_CONFIG_PARAM_T));

} /* GetCameraNvramValue() */

MUINT32 GetCameraCalData(MUINT32 SensorId, MUINT32* pGetSensorCalData)
{
    MINT32 result = 0xFF;
    MUINT32 i;
//    CAM_MSDK_LOG("GetCameraCalData(MainSensorIdx=%d) Enter\n",SensorId);
    for (i=0;i<MAX_NUM_OF_SUPPORT_SENSOR;i++)
    {
        if (SensorId == pstSensorInitFunc[i].SensorId)
        {
//            CAM_MSDK_LOG("[i]=%d \n",i);
            break;
        }
    }    
    if (pstSensorInitFunc[i].getCameraCalData != NULL)
    {
        result = pstSensorInitFunc[i].getCameraCalData(pGetSensorCalData);
    }
    else
    {        
        CAM_MSDK_LOG("[GetCameraCalData]: uninit yet\n\n");
    }
    return result;
}

/*******************************************************************************
*
********************************************************************************/
MUINT32 LensCustomInit()
{
    GetLensInitFuncList(&LensInitFunc[0]); 
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
void GetLensDefaultPara(PNVRAM_LENS_PARA_STRUCT pLensParaDefault)
{
	MUINT32 i;

    MUINT32 LensId = LensInitFunc[gMainLensIdx].LensId;

    if (LensInitFunc[0].getLensDefault == NULL)
    {
        CAM_MSDK_LOG("[GetLensDefaultPara]: uninit yet\n\n");
        return;
    }
	
    for (i=0; i<MAX_NUM_OF_SUPPORT_LENS; i++)
    {
        if (LensId == LensInitFunc[i].LensId)
        {
            break;
        }
    }

	if (pLensParaDefault != NULL)
	{
        LensInitFunc[i].getLensDefault((VOID*)pLensParaDefault, sizeof(NVRAM_LENS_PARA_STRUCT));
	}

}

/*******************************************************************************
*
********************************************************************************/
MUINT32 LensCustomSetIndex(MUINT32 a_u4CurrIdx)
{
    gMainLensIdx = a_u4CurrIdx;
    
    return 0; 
}

/*******************************************************************************
*
********************************************************************************/
MUINT32 LensCustomGetInitFunc(MSDK_LENS_INIT_FUNCTION_STRUCT *a_pLensInitFunc)
{
    if (a_pLensInitFunc != NULL) {
        memcpy(a_pLensInitFunc, &LensInitFunc[0], sizeof(MSDK_LENS_INIT_FUNCTION_STRUCT) * MAX_NUM_OF_SUPPORT_LENS);                
        return 0; 
    }
    return -1; 
}


