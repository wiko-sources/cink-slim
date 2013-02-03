#ifndef _CAMERA_CUSTOM_NVRAM_H_
#define _CAMERA_CUSTOM_NVRAM_H_


#if defined(MT6516)
#include "camera_custom_nvram_mt6516.h"
#endif

#if defined(MT6573)
#include "camera_custom_nvram_mt6573.h"
#endif

#if defined(MT6575)||defined(MT6577)
#include "camera_custom_nvram_mt6575.h"
#endif

#endif // _CAMERA_CUSTOM_NVRAM_H_

