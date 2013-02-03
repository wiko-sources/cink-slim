#ifndef _CAMERA_CUSTOM_EEPROM_H_
#define _CAMERA_CUSTOM_EEPROM_H_

#if defined(MT6573)
#include "camera_custom_eeprom_mt6573.h"
#endif

#if defined(MT6575)
#include "camera_custom_eeprom_mt6575.h"
#endif
#if defined(MT6575T)||defined(MT6577) 
#include "camera_custom_eeprom_mt6575.h"
#endif

typedef enum
{
    EEPROM_NONE = 0,
    EEPROM_USED      
} EEPROM_TYPE_ENUM;

EEPROM_TYPE_ENUM EEPROMInit(void);
unsigned int EEPROMDeviceName(char* DevName);


#endif //_CAMERA_CUSTOM_EEPROM_H_
