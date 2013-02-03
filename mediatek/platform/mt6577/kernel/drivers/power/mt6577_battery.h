#ifndef MT6577_BATTERY_H
#define MT6577_BATTERY_H

#include <linux/ioctl.h>

/*****************************************************************************
 *  BATTERY VOLTAGE
 ****************************************************************************/
#define PRE_CHARGE_VOLTAGE                  3200
#define SYSTEM_OFF_VOLTAGE                  3400  
#define CONSTANT_CURRENT_CHARGE_VOLTAGE     4100  
#define CONSTANT_VOLTAGE_CHARGE_VOLTAGE     4200  
#define CV_DROPDOWN_VOLTAGE                 4000
#define CHARGER_THRESH_HOLD                 4300
#define BATTERY_UVLO_VOLTAGE                2700

/*****************************************************************************
 *  BATTERY TIMER
 ****************************************************************************/
//#define MAX_CHARGING_TIME                   8*60*60 	// 8hr
//#define MAX_CHARGING_TIME                   12*60*60 	// 12hr
#define MAX_CHARGING_TIME                   24*60*60 	// 24hr
#define MAX_POSTFULL_SAFETY_TIME       		1*30*60 	// 30mins
#define MAX_PreCC_CHARGING_TIME         	1*30*60  	// 0.5hr
#define MAX_CV_CHARGING_TIME              	3*60*60 	// 3hr
//#define MAX_CV_CHARGING_TIME              	24*60*60 	// 24hr
#define MUTEX_TIMEOUT                       5000
#define BAT_TASK_PERIOD                     10 			// 10sec
#define g_free_bat_temp 					1000 		// 1 s

/*****************************************************************************
 *  BATTERY Protection
 ****************************************************************************/
#define Battery_Percent_100    100
#define charger_OVER_VOL	    1
#define BATTERY_UNDER_VOL		2
#define BATTERY_OVER_TEMP		3
#define ADC_SAMPLE_TIMES        5

/*****************************************************************************
 *  Pulse Charging State
 ****************************************************************************/
#define  CHR_PRE                        0x1000
#define  CHR_CC                         0x1001 
#define  CHR_TOP_OFF                    0x1002 
#define  CHR_POST_FULL                  0x1003
#define  CHR_BATFULL                    0x1004 
#define  CHR_ERROR                      0x1005

/*****************************************************************************
 *  Type define
 ****************************************************************************/
typedef unsigned int       WORD;
typedef enum {
    CHARGER_UNKNOWN = 0,
    STANDARD_HOST,          // USB : 450mA
    CHARGING_HOST,
    NONSTANDARD_CHARGER,    // AC : 450mA~1A 
    STANDARD_CHARGER,       // AC : ~1A
} CHARGER_TYPE;

typedef enum
{
	USB_SUSPEND = 0,
	USB_UNCONFIGURED,
	USB_CONFIGURED
}usb_state_enum;

/*****************************************************************************
 *  Extern Function
 ****************************************************************************/
#ifdef CONFIG_MTK_SMART_BATTERY

extern void wake_up_bat(void);
extern unsigned long BAT_Get_Battery_Voltage(int polling_mode);

#else

#define wake_up_bat()			do {} while (0)
#define BAT_Get_Battery_Voltage()	({ 0; })

#endif

#endif

