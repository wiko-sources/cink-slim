#ifndef _KD_CAMERA_HW_H_
#define _KD_CAMERA_HW_H_
 

#if defined(MT6577)

//#include <mach/mt6575_pll.h>
#include <mach/mt6577_gpio.h>
#include <mach/mt6577_gpt.h>
#include <mach/mt6577_pm_ldo.h>
#include <mach/mt6577_typedefs.h>

//Power 
#define CAMERA_POWER_VCAM_A MT65XX_POWER_LDO_VCAMA
#define CAMERA_POWER_VCAM_D MT65XX_POWER_LDO_VCAMD
#define CAMERA_POWER_VCAM_A2 MT65XX_POWER_LDO_VCAM_AF
#define CAMERA_POWER_VCAM_D2 MT65XX_POWER_LDO_VCAM_IO

//FIXME, should defined in DCT tool 
#ifndef GPIO_CAMERA_CMRST_PIN 
#define GPIO_CAMERA_CMRST_PIN GPIO221
#endif 
//
#ifndef GPIO_CAMERA_CMRST_PIN_M_GPIO
#define GPIO_CAMERA_CMRST_PIN_M_GPIO GPIO_MODE_00
#endif 
//
#ifndef GPIO_CAMERA_CMPDN_PIN 
#define GPIO_CAMERA_CMPDN_PIN GPIO228
#endif 
//
#ifndef GPIO_CAMERA_CMPDN_PIN_M_GPIO
#define GPIO_CAMERA_CMPDN_PIN_M_GPIO  GPIO_MODE_00 
#endif 
//MUX with CMFLASH
#ifndef GPIO_CAMERA_CMRST1_PIN
#define GPIO_CAMERA_CMRST1_PIN GPIO230
#endif
//
#ifndef GPIO_CAMERA_CMRST1_PIN_M_GPIO
#define GPIO_CAMERA_CMRST1_PIN_M_GPIO GPIO_MODE_00
#endif
//mux with pwm4_6x
#ifndef GPIO_CAMERA_CMPDN1_PIN
#define GPIO_CAMERA_CMPDN1_PIN GPIO220
#endif
//
#ifndef GPIO_CAMERA_CMPDN1_PIN_M_GPIO
#define GPIO_CAMERA_CMPDN1_PIN_M_GPIO GPIO_MODE_00
#endif
//
#ifndef GPIO_CAMERA_2_CMRST_PIN 
#define GPIO_CAMERA_2_CMRST_PIN GPIO34
#endif 
//
#ifndef GPIO_CAMERA_2_CMRST_PIN_M_GPIO
#define GPIO_CAMERA_2_CMRST_PIN_M_GPIO GPIO_MODE_00
#endif 
//
#ifndef GPIO_CAMERA_2_CMPDN_PIN 
#define GPIO_CAMERA_2_CMPDN_PIN GPIO38
#endif 
//
#ifndef GPIO_CAMERA_2_CMPDN_PIN_M_GPIO
#define GPIO_CAMERA_2_CMPDN_PIN_M_GPIO  GPIO_MODE_00 
#endif 

//i2c id for sensor device, MT6575_evb, the I2C is attached on 1
#define IMG_SENSOR_I2C_GROUP_ID 1
#else 
#error Error!!!, Forget to define the MACRO for Sensor Power and I2C control
#endif 
#endif 
