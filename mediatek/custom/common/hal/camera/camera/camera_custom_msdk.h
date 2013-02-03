#ifndef __CAMERA_CUSTOM_MSDK_H
#define __CAMERA_CUSTOM_MSDK_H

#include "aaa_param.h"
#include "camera_custom_nvram.h"
#include "camera_custom_lens.h"


//FIXME , Should not export function outside. 
extern MSDK_LENS_INIT_FUNCTION_STRUCT LensInitFunc[MAX_NUM_OF_SUPPORT_LENS];
extern MUINT32 gMainLensIdx;
extern MUINT32 gSubLensIdx;

/*******************************************************************************
*
********************************************************************************/
MUINT32 cameraCustomInit();
void GetCameraDefaultPara(MUINT32 SensorId,
						  PNVRAM_CAMERA_PARA_STRUCT pCameraParaDefault,
						  PNVRAM_CAMERA_3A_STRUCT pCamera3ANVRAMDefault,
						  PNVRAM_CAMERA_SHADING_STRUCT pCameraShadingDefault,
						  PNVRAM_CAMERA_DEFECT_STRUCT pCameraDefectDefault,
						  P3A_PARAM_T pCamera3AParam,
						  P3A_STAT_CONFIG_PARAM_T pCamera3AStatConfigParam);


MUINT32 GetCameraCalData(MUINT32 SensorId, MUINT32* pGetSensorCalData); // sl 11041000
MUINT32 LensCustomInit();
MUINT32 LensCustomSetIndex(MUINT32 a_u4CurrIdx); 
MUINT32 LensCustomGetInitFunc(MSDK_LENS_INIT_FUNCTION_STRUCT *a_pLensInitFunc); 
void GetLensDefaultPara(PNVRAM_LENS_PARA_STRUCT pLensParaDefault);


#endif // __CAMERA_CUSTOM_MSDK_H


