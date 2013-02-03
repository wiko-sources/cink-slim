/*
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   mt6577_bat.c
 *
 * Project:
 * --------
 *   MT6577 
 *
 * Description:
 * ------------
 *   MT6577 Linux Uboot power scheme
 *
 * Author:
 * -------
 *   James Lo 		(MTK02485)
 *
 ****************************************************************************/
#include <common.h>
#include <asm/io.h>

//#define CFG_POWER_CHARGING

#ifdef CFG_POWER_CHARGING
#include <asm/arch/mt65xx_typedefs.h>
#include <asm/arch/mt6577_pmic6329.h>
#include <asm/arch/mt65xx_leds.h> 
#include <asm/arch/mt6577_rtc.h>
#include <asm/arch/mt65xx_disp_drv.h>
#include <asm/arch/mt65xx_logo.h>
#include <asm/arch/mt6577_sleep.h>
#include <asm/arch/boot_mode.h>
#include <asm/arch/mtk_wdt.h>
#include <asm/arch/mtk_key.h>

#include <cust_battery.h>

#include <asm/arch/mt6577_gpio.h>

#undef printf

//#define CONFIG_DEBUG_MSG
#define GPT_TIMER

#include "fan5405.h"

/*****************************************************************************
 *  Type define
 ****************************************************************************/
typedef unsigned int       WORD;

typedef enum
{
    USB_SUSPEND = 0,
    USB_UNCONFIGURED,
    USB_CONFIGURED
} usb_state_enum;

///////////////////////////////////////////////////////////////////////////////////////////
//// Extern Functions
///////////////////////////////////////////////////////////////////////////////////////////
extern kal_bool upmu_is_chr_det(void);
extern int mt6577_detect_powerkey(void);
extern int PMIC_IMM_GetOneChannelValue(int dwChannel, int deCount);

//extern kal_bool meta_mode_check(void);

extern CHARGER_TYPE mt_charger_type_detection(void);
extern CHARGER_TYPE hw_charger_type_detection(void);

/*****************************************************************************
 *  BATTERY VOLTAGE
 ****************************************************************************/
#define BATTERY_LOWVOL_THRESOLD             3450
#define CHR_OUT_CURRENT                     100

/*****************************************************************************
 *  BATTERY TIMER
 ****************************************************************************/
//#define MAX_CHARGING_TIME                   12*60*60    // 12hr
#define MAX_CHARGING_TIME                   24*60*60    // 24hr
#define MAX_POSTFULL_SAFETY_TIME            1*30*60     // 30mins
#define MAX_PreCC_CHARGING_TIME             1*30*60     // 0.5hr
#define MAX_CV_CHARGING_TIME                3*60*60     // 3hr
#define BAT_TASK_PERIOD                     1           // 1sec
//#define BL_SWITCH_TIMEOUT                   1*6         // 6s  
#ifdef ANIMATION_NEW
#define BL_SWITCH_TIMEOUT                   3*6         // 6s   by Yu
#else
#define BL_SWITCH_TIMEOUT                   1*6         // 6s
#endif

#define POWER_ON_TIME                       4*1         // 0.5s

/*****************************************************************************
 *  BATTERY Protection
 ****************************************************************************/
#define charger_OVER_VOL                    1
#define ADC_SAMPLE_TIMES                    5

/*****************************************************************************
 *  Pulse Charging State
 ****************************************************************************/
#define  CHR_PRE                            0x1000
#define  CHR_CC                             0x1001 
#define  CHR_TOP_OFF                        0x1002 
#define  CHR_POST_FULL                      0x1003
#define  CHR_BATFULL                        0x1004 
#define  CHR_ERROR                          0x1005

///////////////////////////////////////////////////////////////////////////////////////////
//// Smart Battery Structure
///////////////////////////////////////////////////////////////////////////////////////////
#define UINT32 unsigned long
#define UINT16 unsigned short
#define UINT8 unsigned char

typedef struct 
{
    kal_bool   	bat_exist;
    kal_bool   	bat_full;  
    kal_bool   	bat_low;  
    UINT32      bat_charging_state;
    UINT32      bat_vol;            
    kal_bool    charger_exist;   
    UINT32      pre_charging_current;
    UINT32      charging_current;
    INT32      charger_vol;        
    UINT32      charger_protect_status; 
    UINT32      ISENSE;                
    UINT32      ICharging;
    INT32      temperature;
    UINT32      total_charging_time;
    UINT32      PRE_charging_time;
    UINT32      CC_charging_time;
    UINT32      TOPOFF_charging_time;
    UINT32      POSTFULL_charging_time;
    UINT32      charger_type;
    UINT32      PWR_SRC;
    UINT32      SOC;
    UINT32      ADC_BAT_SENSE;
    UINT32      ADC_I_SENSE;
} PMU_ChargerStruct;

typedef enum 
{
    PMU_STATUS_OK = 0,
    PMU_STATUS_FAIL = 1,
} PMU_STATUS;

/////////////////////////////////////////////////////////////////////
//// Global Variable
/////////////////////////////////////////////////////////////////////
static CHARGER_TYPE CHR_Type_num = CHARGER_UNKNOWN;
PMU_ChargerStruct BMT_status;

static unsigned short batteryVoltageBuffer[BATTERY_AVERAGE_SIZE];
static unsigned short batteryCurrentBuffer[BATTERY_AVERAGE_SIZE];
static unsigned short batterySOCBuffer[BATTERY_AVERAGE_SIZE];
static int batteryIndex = 0;
static int batteryVoltageSum = 0;
static int batteryCurrentSum = 0;
static int batterySOCSum = 0;
kal_bool g_bat_full_user_view = KAL_FALSE;
kal_bool g_Battery_Fail = KAL_FALSE;
kal_bool batteryBufferFirst = KAL_FALSE;

int V_PRE2CC_THRES = 3400;
int V_CC2TOPOFF_THRES = 4050;
int V_compensate_EVB = 80;

int g_HW_Charging_Done = 0;
int g_Charging_Over_Time = 0;

int g_SW_CHR_OUT_EN = 0;
int g_bl_on = 1;

int g_thread_count = 10;

// HW CV algorithm
unsigned int CHR_CON_0 = 0x7002FA00;
unsigned int CHR_CON_1 = 0x7002FA04;
unsigned int CHR_CON_2 = 0x7002FA08;
unsigned int CHR_CON_4 = 0x7002FA10;
unsigned int CHR_CON_6 = 0x7002FA18;
unsigned int CHR_CON_9 = 0x7002FA24;
unsigned int CHR_CON_10 = 0x7002FA28;
unsigned int PMIC_RESERVE_CON1 = 0x7002FE84;
volatile unsigned int save_value = 0x0;
volatile unsigned int CSDAC_DAT_MAX = 255;
volatile unsigned int CSDAC_DAT = 0;
volatile unsigned int VBAT_CV_DET = 0x0;
volatile unsigned int CS_DET = 0x0;
//int g_sw_cv_enable = 0;

int CHARGING_FULL_CURRENT = 130;	// mA on phone

int g_bat_temperature_pre=0;

int gADC_BAT_SENSE_temp=0;
int gADC_I_SENSE_temp=0;
int gADC_I_SENSE_offset=0;

kal_uint32 g_eco_version = 0;
#define PMIC6329_E1_CID_CODE    0x0029
int g_E1_vbat_sense = 0;

int g_R_BAT_SENSE = R_BAT_SENSE;
int g_R_I_SENSE = R_I_SENSE;
int g_R_CHARGER_1 = R_CHARGER_1;
int g_R_CHARGER_2 = R_CHARGER_2;

/*****************************************************************************
 * EM
****************************************************************************/
int g_BatteryAverageCurrent = 0;

/*****************************************************************************
 * USB-IF
****************************************************************************/
int g_usb_state = USB_UNCONFIGURED;
int g_temp_CC_value = Cust_CC_0MA;

/*****************************************************************************
 * Logging System
****************************************************************************/
int g_chr_event = 0;
int bat_volt_cp_flag = 0;
//int Enable_BATDRV_LOG = 0;
int Enable_BATDRV_LOG = 1;

/***************************************************
 * UBOOT 
****************************************************/
int prog = 25;
int prog_temp = 0;
int prog_first = 1;
int g_HW_stop_charging = 0;
int bl_switch_timer = 0;
int bat_volt_check_point = 0;
int getVoltFlag = 0;
int low_bat_boot_display=0;
int charger_ov_boot_display = 0;

kal_bool bl_switch = KAL_FALSE;
kal_bool user_view_flag = KAL_FALSE;

int vbat_compensate_cp = 4185; //4185mV
int vbat_compensate_value = 80; //80mV

/********************************************** 
 * Battery Temprature Parameters and functions    
 ***********************************************/
typedef struct{
    INT32 BatteryTemp;
    INT32 TemperatureR;
} BATT_TEMPERATURE;

/* convert register to temperature  */
INT16 BattThermistorConverTemp(INT32 Res)
{
    int i = 0;
    INT32 RES1 = 0, RES2 = 0;
    INT32 TBatt_Value = -200, TMP1 = 0, TMP2 = 0;

#if defined(BAT_NTC_10_TDK_1)		
BATT_TEMPERATURE Batt_Temperature_Table[] = {
 {-20,95327},
 {-15,71746},
 {-10,54564},
 { -5,41813},
 {  0,32330},
 {  5,25194},
 { 10,19785},
 { 15,15651},
 { 20,12468},
 { 25,10000},
 { 30,8072},
 { 35,6556},
 { 40,5356},
 { 45,4401},
 { 50,3635},
 { 55,3019},
 { 60,2521}
};
#endif

#if defined(BAT_NTC_10_SEN_1)		
BATT_TEMPERATURE Batt_Temperature_Table[] = {
 {-20,74354},
 {-15,57626},
 {-10,45068},
 { -5,35548},
 {  0,28267},
 {  5,22650},
 { 10,18280},
 { 15,14855},
 { 20,12151},
 { 25,10000},
 { 30,8279},
 { 35,6892},
 { 40,5768},
 { 45,4852},
 { 50,4101},
 { 55,3483},
 { 60,2970}
};
#endif

#if (BAT_NTC_10 == 1)
    BATT_TEMPERATURE Batt_Temperature_Table[] = {
        {-20,68237},
        {-15,53650},
        {-10,42506},
        { -5,33892},
        {  0,27219},
        {  5,22021},
        { 10,17926},
        { 15,14674},
        { 20,12081},
        { 25,10000},
        { 30,8315},
        { 35,6948},
        { 40,5834},
        { 45,4917},
        { 50,4161},
        { 55,3535},
        { 60,3014}		
    };
#endif

#if (BAT_NTC_47 == 1)
    BATT_TEMPERATURE Batt_Temperature_Table[] = {
        {-20,483954},
        {-15,360850},
        {-10,271697},
        { -5,206463},
        {  0,158214},
        {  5,122259},
        { 10,95227},
        { 15,74730},
        { 20,59065},
        { 25,47000},
        { 30,37643},
        { 35,30334},
        { 40,24591},
        { 45,20048},
        { 50,16433},
        { 55,13539},
        { 60,11210}		
    };
#endif

    if (Enable_BATDRV_LOG == 1) {
        printf("###### %d <-> %d ######\r\n", Batt_Temperature_Table[9].BatteryTemp, 
            Batt_Temperature_Table[9].TemperatureR);
    }

    if(Res >= Batt_Temperature_Table[0].TemperatureR)
    {
        #ifdef CONFIG_DEBUG_MSG_NO_BQ27500
        printf("Res >= %d\n", Batt_Temperature_Table[0].TemperatureR);
        #endif
        TBatt_Value = -20;
    }
    else if(Res <= Batt_Temperature_Table[16].TemperatureR)
    {
        #ifdef CONFIG_DEBUG_MSG_NO_BQ27500
        printf("Res <= %d\n", Batt_Temperature_Table[16].TemperatureR);
        #endif
        TBatt_Value = 60;
    }
    else
    {
        RES1 = Batt_Temperature_Table[0].TemperatureR;
        TMP1 = Batt_Temperature_Table[0].BatteryTemp;
        
        for (i = 0; i <= 16; i++)
        {
            if(Res >= Batt_Temperature_Table[i].TemperatureR)
            {
                RES2 = Batt_Temperature_Table[i].TemperatureR;
                TMP2 = Batt_Temperature_Table[i].BatteryTemp;
                break;
            }
            else
            {
                RES1 = Batt_Temperature_Table[i].TemperatureR;
                TMP1 = Batt_Temperature_Table[i].BatteryTemp;
            }
        }
        
        TBatt_Value = (((Res - RES2) * TMP1) + ((RES1 - Res) * TMP2)) / (RES1-RES2);
    }
    
    #ifdef CONFIG_DEBUG_MSG_NO_BQ27500
    printf("BattThermistorConverTemp() : TBatt_Value = %d\n",TBatt_Value);
    #endif
    
    return TBatt_Value;    
}

/* convert ADC_bat_temp_volt to register */
INT16 BattVoltToTemp(UINT32 dwVolt)
{
    INT32 TRes;
    INT32 dwVCriBat = (TBAT_OVER_CRITICAL_LOW * RBAT_PULL_UP_VOLT) / (TBAT_OVER_CRITICAL_LOW + RBAT_PULL_UP_R); //~2000mV
    INT32 sBaTTMP = -100;
    
    if(dwVolt > dwVCriBat)
        TRes = TBAT_OVER_CRITICAL_LOW;
    else
        TRes = (RBAT_PULL_UP_R*dwVolt) / (RBAT_PULL_UP_VOLT-dwVolt);        
        
    /* convert register to temperature */
    sBaTTMP = BattThermistorConverTemp(TRes);
    
    #ifdef CONFIG_DEBUG_MSG_NO_BQ27500
    printf("BattVoltToTemp() : TBAT_OVER_CRITICAL_LOW = %d\n", TBAT_OVER_CRITICAL_LOW);
    printf("BattVoltToTemp() : RBAT_PULL_UP_VOLT = %d\n", RBAT_PULL_UP_VOLT);
    printf("BattVoltToTemp() : dwVolt = %d\n", dwVolt);
    printf("BattVoltToTemp() : TRes = %d\n", TRes);
    printf("BattVoltToTemp() : sBaTTMP = %d\n", sBaTTMP);
    #endif
    
    return sBaTTMP;
}

//////////////////////////////////////////////////////
//// Pulse Charging Algorithm 
//////////////////////////////////////////////////////
kal_bool pmic_chrdet_status(void)
{
    if( upmu_is_chr_det() == KAL_TRUE )    
    {
        return KAL_TRUE;
    }
    else
    {
        printf("[pmic_chrdet_status] No charger\r\n");
        return KAL_FALSE;
    }
}

void fan5405_set_ac_current(void)
{
	if (Enable_BATDRV_LOG == 1) {
		printk("[BATTERY:fan5405] fan5405_set_ac_charging_current \r\n");	
	}	
	//fan5405_config_interface_liao(0x01,0xb8);

    #if 0
    //set the current to 1.25A,
    //1). 0x06h->0x70h  // set safety register first,
    //2). 0x01h->0xF8h
    //3). 0x02h->0x8Eh
    //4). 0x04h->0x79h
    //5). 0x05h->0x04h
    fan5405_config_interface_liao(0x06,0x70);
    fan5405_config_interface_liao(0x01,0xF8);
    fan5405_config_interface_liao(0x02,0x8E);
    fan5405_config_interface_liao(0x04,0x79);
    fan5405_config_interface_liao(0x05,0x04);
    #endif

    #if 1
    //set the current to 650mA,
    //1). 0x06h->0x10h  // set safety register first,
    //2). 0x01h->0xF8h
    //3). 0x02h->0x8Eh
    //4). 0x04h->0x19h
    //5). 0x05h->0x04h
    fan5405_config_interface_liao(0x06,0x10); //set ISAFE
    fan5405_config_interface_liao(0x01,0xF8);
    fan5405_config_interface_liao(0x02,0x8E);
    fan5405_config_interface_liao(0x04,0x19); //set IOCHARGE
    fan5405_config_interface_liao(0x05,0x04);
    #endif
}

void select_charging_curret_fan5405()
{
    if ( BMT_status.charger_type == STANDARD_HOST ) 
    {        
        g_temp_CC_value = USB_CHARGER_CURRENT;	
        fan5405_config_interface_liao(0x01,0x78);
		if (Enable_BATDRV_LOG == 1) {
			printf("[BATTERY:fan5405] BMT_status.charger_type == STANDARD_HOST \r\n");	
		}
    } 
    else if (BMT_status.charger_type == NONSTANDARD_CHARGER) 
    {   
        g_temp_CC_value = USB_CHARGER_CURRENT;
		fan5405_config_interface_liao(0x01,0x78);
		if (Enable_BATDRV_LOG == 1) {
			printf("[BATTERY:fan5405] BMT_status.charger_type == NONSTANDARD_CHARGER \r\n");	
		}
    } 
    else if (BMT_status.charger_type == STANDARD_CHARGER) 
    {
        g_temp_CC_value = AC_CHARGER_CURRENT;
        fan5405_set_ac_current();
        if (Enable_BATDRV_LOG == 1) {
			printf("[BATTERY:fan5405] BMT_status.charger_type == STANDARD_CHARGER \r\n");	
		}
    }
    else if (BMT_status.charger_type == CHARGING_HOST) 
    {
        g_temp_CC_value = AC_CHARGER_CURRENT;
        fan5405_set_ac_current();
        if (Enable_BATDRV_LOG == 1) {
			printf("[BATTERY:fan5405] BMT_status.charger_type == CHARGING_HOST \r\n");	
		}
    }
    else 
    {
        g_temp_CC_value = USB_CHARGER_CURRENT;
		fan5405_config_interface_liao(0x01,0x78);
		if (Enable_BATDRV_LOG == 1) {
			printf("[BATTERY:fan5405] default set g_temp_CC_value = USB_CHARGER_CURRENT \r\n");	
		}
    }
}

static unsigned int temp_init_flag = 0;
void ChargerHwInit_fan5405(void)
{
	if (Enable_BATDRV_LOG == 1) {
		printk("[MT6577 BAT_probe] ChargerHwInit\n" );
	}

	if(temp_init_flag==0)
	{
	    fan5405_config_interface_liao(0x06,0x70);
	    fan5405_config_interface_liao(0x00,0x80);
	    fan5405_config_interface_liao(0x01,0xb1);
	    fan5405_config_interface_liao(0x02,0x8e);
	    fan5405_config_interface_liao(0x05,0x04);
	    fan5405_config_interface_liao(0x04,0x19);
	    temp_init_flag =1;	
	}
	else
	{
    	fan5405_config_interface_liao(0x00,0x80);
	}
}

int gpio_number   = GPIO162;
int gpio_off_mode = GPIO_MODE_GPIO;
int gpio_off_dir  = GPIO_DIR_OUT;
int gpio_off_out  = GPIO_OUT_ONE;
int gpio_on_mode  = GPIO_MODE_GPIO;
int gpio_on_dir   = GPIO_DIR_OUT;
int gpio_on_out   = GPIO_OUT_ZERO;

void pchr_turn_off_charging_fan5405 (void)
{
    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY] pchr_turn_off_charging_fan5405 !\r\n");
    }
    
    mt_set_gpio_mode(gpio_number,gpio_off_mode);  //57 mode0
    mt_set_gpio_dir(gpio_number,gpio_off_dir);
    mt_set_gpio_out(gpio_number,gpio_off_out);

	fan5405_config_interface_liao(0x01,0xbc);	  
}

void pchr_turn_on_charging_fan5405 (void)
{

    mt_set_gpio_mode(gpio_number,gpio_on_mode);  //57 mode0
    mt_set_gpio_dir(gpio_number,gpio_on_dir);
    mt_set_gpio_out(gpio_number,gpio_on_out);

    if ( BMT_status.bat_charging_state == CHR_ERROR ) 
    {
        //if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] Charger Error, turn OFF charging !\r\n");
        //}
        pchr_turn_off_charging_fan5405();
    }
    else
    {
        ChargerHwInit_fan5405();
        
        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] pchr_turn_on_charging_fan5405 !\r\n");
        }
        
        select_charging_curret_fan5405();
        
        if( g_temp_CC_value == Cust_CC_0MA)
        {
            pchr_turn_off_charging_fan5405();
            printf("[BATTERY] g_temp_CC_value == Cust_CC_0MA !\r\n");
        }
        else
        {		
            if (Enable_BATDRV_LOG == 1) {
				printf("[BATTERY] charger enable !\r\n");
			}     
        }
    }    	
}

int BAT_CheckPMUStatusReg(void)
{ 
    if( upmu_is_chr_det() == KAL_TRUE )
    {
        BMT_status.charger_exist = TRUE;
    }
    else
    {   
        BMT_status.charger_exist = FALSE;
        
        BMT_status.total_charging_time = 0;
        BMT_status.PRE_charging_time = 0;
        BMT_status.CC_charging_time = 0;
        BMT_status.TOPOFF_charging_time = 0;
        BMT_status.POSTFULL_charging_time = 0;
        
        BMT_status.bat_charging_state = CHR_PRE;        
        
        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] BAT_CheckPMUStatusReg : charger loss \n");
        }
        
        return PMU_STATUS_FAIL;
    }  
    
    return PMU_STATUS_OK;
}

int g_Get_I_Charging(void)
{
	kal_int32 ADC_BAT_SENSE_tmp[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	kal_int32 ADC_BAT_SENSE_sum=0;
	kal_int32 ADC_BAT_SENSE=0;
	kal_int32 ADC_I_SENSE_tmp[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	kal_int32 ADC_I_SENSE_sum=0;
	kal_int32 ADC_I_SENSE=0;	
	int repeat=20;
	int i=0;
	int j=0;
	kal_int32 temp=0;
	int ICharging=0;	

	for(i=0 ; i<repeat ; i++)
	{
		ADC_BAT_SENSE_tmp[i] = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
		ADC_I_SENSE_tmp[i] = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL,1);
	
		ADC_BAT_SENSE_sum += ADC_BAT_SENSE_tmp[i];
		ADC_I_SENSE_sum += ADC_I_SENSE_tmp[i];	
	}

	//sorting	BAT_SENSE 
	for(i=0 ; i<repeat ; i++)
	{
		for(j=i; j<repeat ; j++)
		{
			if( ADC_BAT_SENSE_tmp[j] < ADC_BAT_SENSE_tmp[i] )
			{
				temp = ADC_BAT_SENSE_tmp[j];
				ADC_BAT_SENSE_tmp[j] = ADC_BAT_SENSE_tmp[i];
				ADC_BAT_SENSE_tmp[i] = temp;
			}
		}
	}
	if (Enable_BATDRV_LOG == 1) {
		printf("[g_Get_I_Charging:BAT_SENSE]\r\n");	
		for(i=0 ; i<repeat ; i++ )
		{
			printf("%d,", ADC_BAT_SENSE_tmp[i]);
		}
		printf("\r\n");
	}

	//sorting	I_SENSE 
	for(i=0 ; i<repeat ; i++)
	{
		for(j=i ; j<repeat ; j++)
		{
			if( ADC_I_SENSE_tmp[j] < ADC_I_SENSE_tmp[i] )
			{
				temp = ADC_I_SENSE_tmp[j];
				ADC_I_SENSE_tmp[j] = ADC_I_SENSE_tmp[i];
				ADC_I_SENSE_tmp[i] = temp;
			}
		}
	}
	if (Enable_BATDRV_LOG == 1) {
		printf("[g_Get_I_Charging:I_SENSE]\r\n");	
		for(i=0 ; i<repeat ; i++ )
		{
			printf("%d,", ADC_I_SENSE_tmp[i]);
		}
		printf("\r\n");
	}
		
	ADC_BAT_SENSE_sum -= ADC_BAT_SENSE_tmp[0];
	ADC_BAT_SENSE_sum -= ADC_BAT_SENSE_tmp[1];
	ADC_BAT_SENSE_sum -= ADC_BAT_SENSE_tmp[18];
	ADC_BAT_SENSE_sum -= ADC_BAT_SENSE_tmp[19];		
	ADC_BAT_SENSE = ADC_BAT_SENSE_sum / (repeat-4);

	if (Enable_BATDRV_LOG == 1) {
		printf("[g_Get_I_Charging] ADC_BAT_SENSE=%d\r\n", ADC_BAT_SENSE);
	}

	ADC_I_SENSE_sum -= ADC_I_SENSE_tmp[0];
	ADC_I_SENSE_sum -= ADC_I_SENSE_tmp[1];
	ADC_I_SENSE_sum -= ADC_I_SENSE_tmp[18];
	ADC_I_SENSE_sum -= ADC_I_SENSE_tmp[19];
	ADC_I_SENSE = ADC_I_SENSE_sum / (repeat-4);

	if (Enable_BATDRV_LOG == 1) {
		printf("[g_Get_I_Charging] ADC_I_SENSE(Before)=%d\r\n", ADC_I_SENSE);
	}
	
	ADC_I_SENSE += gADC_I_SENSE_offset;

	if (Enable_BATDRV_LOG == 1) {
		printf("[g_Get_I_Charging] ADC_I_SENSE(After)=%d\r\n", ADC_I_SENSE);
	}

	BMT_status.ADC_BAT_SENSE = ADC_BAT_SENSE;
	BMT_status.ADC_I_SENSE = ADC_I_SENSE;
	
	if(ADC_I_SENSE > ADC_BAT_SENSE)
	{
    	ICharging = (ADC_I_SENSE - ADC_BAT_SENSE)*10/R_CURRENT_SENSE;
	}
	else
	{
    	ICharging = 0;
	}

	return ICharging;
}

void BAT_Vbat_Compensate(void)
{
    if( (upmu_is_chr_det() == KAL_TRUE) && (g_bat_full_user_view != KAL_TRUE) )
    {
        if(BMT_status.ADC_BAT_SENSE <= vbat_compensate_cp)
        {
            printf("[vbat compensate before] BMT_status.bat_vol = %d\r\n", BMT_status.ADC_BAT_SENSE);
            BMT_status.ADC_BAT_SENSE = BMT_status.ADC_BAT_SENSE - vbat_compensate_value;
            printf("[vbat compensate after ] BMT_status.bat_vol = %d\r\n", BMT_status.ADC_BAT_SENSE);
        }
    }    
}

void BAT_GetVoltage_fan5405(void)
{ 
    int bat_temperature_volt=0;
    
    /* Get V_BAT_SENSE */
    if (g_chr_event == 0) 
    {    	
        BMT_status.ADC_BAT_SENSE = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL, 1);		
    } 
    else 
    {
        /* Just charger in/out event, same as I_sense */
        g_chr_event = 0;		
        BMT_status.ADC_BAT_SENSE = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL, 1);
    }
    if (batteryBufferFirst)
    {
    BAT_Vbat_Compensate();
    }
    else
    {
        if( (upmu_is_chr_det() == KAL_TRUE) && (g_bat_full_user_view != KAL_TRUE) )
        {
            if(BMT_status.ADC_BAT_SENSE <= vbat_compensate_cp)
            {
                printf("[vbat first compensate before] BMT_status.bat_vol = %d\r\n", BMT_status.ADC_BAT_SENSE);
                BMT_status.ADC_BAT_SENSE = BMT_status.ADC_BAT_SENSE + 20;
                printf("[vbat first compensate after ] BMT_status.bat_vol = %d\r\n", BMT_status.ADC_BAT_SENSE);
            }
        }
    }
    BMT_status.bat_vol = BMT_status.ADC_BAT_SENSE;

	if (g_eco_version == PMIC6329_E1_CID_CODE)
	{	
		g_E1_vbat_sense = BMT_status.ADC_BAT_SENSE;
		if (Enable_BATDRV_LOG == 1) {
			printf("[Charger_E1] Get g_E1_vbat_sense = %d\r\n", g_E1_vbat_sense);
		}
	}
	
    /* Get V_I_SENSE */	
    //BMT_status.ADC_I_SENSE = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL, 1);
	//BMT_status.ADC_I_SENSE += gADC_I_SENSE_offset;	
	
    /* Get V_Charger */
    BMT_status.charger_vol = PMIC_IMM_GetOneChannelValue(AUXADC_CHARGER_VOLTAGE_CHANNEL, 5);
    BMT_status.charger_vol = BMT_status.charger_vol / 100;
        
	/* Get V_BAT_Temperature */
	bat_temperature_volt = PMIC_IMM_GetOneChannelValue(AUXADC_TEMPERATURE_CHANNEL,5);			
	if(bat_temperature_volt == 0)
	{
		BMT_status.temperature = g_bat_temperature_pre;
		if (Enable_BATDRV_LOG == 1) {
			printf("[BATTERY] Warning !! bat_temperature_volt == 0, restore temperature value\n\r");
		}
	}
	else
	{
		BMT_status.temperature = BattVoltToTemp(bat_temperature_volt);
		g_bat_temperature_pre = BMT_status.temperature; 	
	}	
    
    /* Calculate the charging current */
    //if(BMT_status.ADC_I_SENSE > BMT_status.ADC_BAT_SENSE)
    //    BMT_status.ICharging = (BMT_status.ADC_I_SENSE - BMT_status.ADC_BAT_SENSE) * 10 / R_CURRENT_SENSE;
    //else
    //    BMT_status.ICharging = 0;    
    //BMT_status.ICharging = g_Get_I_Charging();
    
    //if (Enable_BATDRV_LOG == 1) {
    //    printf("[BATTERY:ADC] VCHR:%d BAT_SENSE:%d I_SENSE:%d Current:%d\n", BMT_status.charger_vol,
    //    BMT_status.ADC_BAT_SENSE, BMT_status.ADC_I_SENSE, BMT_status.ICharging);
    //}
    
    g_BatteryAverageCurrent = BMT_status.ICharging;
}

void BAT_GetVoltage_notbat_fan5405(void)
{   
    /* Get V_BAT_SENSE */
    if (g_chr_event == 0) 
    {    	
        BMT_status.ADC_BAT_SENSE = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL, 1);		
    } 
    else 
    {
        /* Just charger in/out event, same as I_sense */
        g_chr_event = 0;		
        BMT_status.ADC_BAT_SENSE = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL, 1);
    }
    BMT_status.bat_vol = BMT_status.ADC_BAT_SENSE;

	if (g_eco_version == PMIC6329_E1_CID_CODE)
	{	
		g_E1_vbat_sense = BMT_status.ADC_BAT_SENSE;
		if (Enable_BATDRV_LOG == 1) {
			printf("[Charger_E1] Get g_E1_vbat_sense = %d\r\n", g_E1_vbat_sense);
		}
	}
	
    /* Get V_I_SENSE */	
    BMT_status.ADC_I_SENSE = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL, 1);
	BMT_status.ADC_I_SENSE += gADC_I_SENSE_offset;	
	
    /* Get V_Charger */
    BMT_status.charger_vol = PMIC_IMM_GetOneChannelValue(AUXADC_CHARGER_VOLTAGE_CHANNEL, 2);
    BMT_status.charger_vol = BMT_status.charger_vol / 100;              			
    
    /* Calculate the charging current */
    if(BMT_status.ADC_I_SENSE > BMT_status.ADC_BAT_SENSE)
        BMT_status.ICharging = (BMT_status.ADC_I_SENSE - BMT_status.ADC_BAT_SENSE) * 10 / R_CURRENT_SENSE;
    else
        BMT_status.ICharging = 0;    
    
    //if (Enable_BATDRV_LOG == 1) {
        printf("[BAT_GetVoltage_notbat] VCHR:%d BAT_SENSE:%d I_SENSE:%d Current:%d\n", BMT_status.charger_vol,
        BMT_status.ADC_BAT_SENSE, BMT_status.ADC_I_SENSE, BMT_status.ICharging);
    //}
    
    g_BatteryAverageCurrent = BMT_status.ICharging;
}

UINT32 BattVoltToPercent(UINT16 dwVoltage)
{
    UINT32 m = 0;
    UINT32 VBAT1 = 0, VBAT2 = 0;
    UINT32 bPercntResult = 0, bPercnt1 = 0, bPercnt2 = 0;
    
    //if (Enable_BATDRV_LOG == 1) {
    //    printf("###### 100 <-> voltage : %d ######\r\n", Batt_VoltToPercent_Table[10].BattVolt);
    //}
    
    if(dwVoltage <= Batt_VoltToPercent_Table[0].BattVolt)
    {
        bPercntResult = Batt_VoltToPercent_Table[0].BattPercent;
        return bPercntResult;
    }
    else if (dwVoltage >= Batt_VoltToPercent_Table[10].BattVolt)
    {
        bPercntResult = Batt_VoltToPercent_Table[10].BattPercent;		
        return bPercntResult;
    }
    else
    {        
        VBAT1 = Batt_VoltToPercent_Table[0].BattVolt;
        bPercnt1 = Batt_VoltToPercent_Table[0].BattPercent;
        for(m = 1; m <= 10; m++)
        {
            if(dwVoltage <= Batt_VoltToPercent_Table[m].BattVolt)
            {
                VBAT2 = Batt_VoltToPercent_Table[m].BattVolt;
                bPercnt2 = Batt_VoltToPercent_Table[m].BattPercent;
                break;
            }
            else
            {
                VBAT1 = Batt_VoltToPercent_Table[m].BattVolt;
                bPercnt1 = Batt_VoltToPercent_Table[m].BattPercent;    
            }
        }
    }
    
    bPercntResult = ( ((dwVoltage - VBAT1) * bPercnt2) + ((VBAT2 - dwVoltage) * bPercnt1) ) / (VBAT2 - VBAT1);    
    
    return bPercntResult;
}

int BAT_CheckBatteryStatus_fan5405(void)
{
    int BAT_status = PMU_STATUS_OK;
    int i = 0;
    int ret_check_I_charging = 0;
    int j = 0;
    int sw_chr_out_flag = 0;
    int repeat_times = 10;
	int iii = 0;
    
    /* Get Battery Information */
    BAT_GetVoltage_fan5405();

    #if 0        
    if ((upmu_is_chr_det() == KAL_TRUE) && (g_HW_Charging_Done == 0) &&
        (BMT_status.bat_charging_state != CHR_ERROR) &&
        (BMT_status.bat_charging_state != CHR_TOP_OFF))
    {
        if ((BMT_status.total_charging_time % 10) == 0)
        {		
            g_HW_stop_charging = 1;		
            //printf("Disable charging 1s\n");		
        }
        else
        {
            g_HW_stop_charging = 0;
            //printf("Charging 1s\n");
        }
    }
    else
    {
        g_HW_stop_charging = 0;
        //printf("SW CV mode do not dis-charging 1s\n");
    }
    #endif
    
    /* Re-calculate Battery Percentage (SOC) */    
    BMT_status.SOC = BattVoltToPercent(BMT_status.bat_vol);
    if (Enable_BATDRV_LOG == 1) {
        printf("===> %d , %d (%d)\r\n", BMT_status.SOC, BMT_status.bat_vol, BATTERY_AVERAGE_SIZE);
    }
        
    if (bat_volt_cp_flag == 0) 
    {
        bat_volt_cp_flag = 1;
        bat_volt_check_point = BMT_status.SOC;
    }
    /* User smooth View when discharging : end */
    
    /**************** Averaging : START ****************/        
    if (!batteryBufferFirst)
    {
        batteryBufferFirst = KAL_TRUE;
        
        for (i=0; i<BATTERY_AVERAGE_SIZE; i++) {
            batteryVoltageBuffer[i] = BMT_status.bat_vol;            
            batteryCurrentBuffer[i] = BMT_status.ICharging;            
            batterySOCBuffer[i] = BMT_status.SOC;
        }
        
        batteryVoltageSum = BMT_status.bat_vol * BATTERY_AVERAGE_SIZE;
        batteryCurrentSum = BMT_status.ICharging * BATTERY_AVERAGE_SIZE;        
        batterySOCSum = BMT_status.SOC * BATTERY_AVERAGE_SIZE;
    }
    
    batteryVoltageSum -= batteryVoltageBuffer[batteryIndex];
    batteryVoltageSum += BMT_status.bat_vol;
    batteryVoltageBuffer[batteryIndex] = BMT_status.bat_vol;
    
    batteryCurrentSum -= batteryCurrentBuffer[batteryIndex];
    batteryCurrentSum += BMT_status.ICharging;
    batteryCurrentBuffer[batteryIndex] = BMT_status.ICharging;
    
    if (BMT_status.bat_full)
        BMT_status.SOC = 100;
    if (g_bat_full_user_view)
        BMT_status.SOC = 100;
    
    batterySOCSum -= batterySOCBuffer[batteryIndex];
    batterySOCSum += BMT_status.SOC;
    batterySOCBuffer[batteryIndex] = BMT_status.SOC;
    
    BMT_status.bat_vol = batteryVoltageSum / BATTERY_AVERAGE_SIZE;
    BMT_status.ICharging = batteryCurrentSum / BATTERY_AVERAGE_SIZE;    
    BMT_status.SOC = batterySOCSum / BATTERY_AVERAGE_SIZE;
    
    batteryIndex++;
    if (batteryIndex >= BATTERY_AVERAGE_SIZE)
        batteryIndex = 0;
    /**************** Averaging : END ****************/
    
    if( BMT_status.SOC == 100 ) {
        BMT_status.bat_full = KAL_TRUE;   
    }
    
    /**************** For UBOOT : Start ****************/
    if (low_bat_boot_display == 0)
    {
    
        /* SOC only UP when charging */
        if ( BMT_status.SOC > bat_volt_check_point ) {
            bat_volt_check_point = BMT_status.SOC;		
        }

#if 1        
        /* UBOOT charging LED */
        if ( (bat_volt_check_point >= 90)  || (user_view_flag == KAL_TRUE) ) {
            leds_battery_full_charging();
        } else if(bat_volt_check_point <= 10) {
            leds_battery_low_charging();
        } else {
            leds_battery_medium_charging();
        }
#endif

#if 0        
        /* UBOOT charging animation */
        if ( (BMT_status.bat_full) || (user_view_flag == KAL_TRUE) ) 
        {		
            if(g_bl_on == 1)
            {	
                mt65xx_disp_show_battery_full();	
            }
            user_view_flag = KAL_TRUE;
        } 
        else 
        {	
            if ( (bat_volt_check_point>=0) && (bat_volt_check_point<25) )
            {
                prog_temp = 0;
            }
            else if ( (bat_volt_check_point>=25) && (bat_volt_check_point<50) )
            {
                prog_temp = 25;
            }
            else if ( (bat_volt_check_point>=50) && (bat_volt_check_point<75) )
            {
                prog_temp = 50;
            }
            else if ( (bat_volt_check_point>=75) && (bat_volt_check_point<100) )
            {
                prog_temp = 75;
            }
            else
            {
                prog_temp = 100;
            }
            
            if (prog_first == 1)
            {
                prog = prog_temp;
                prog_first = 0;
            }
            if(g_bl_on == 1)
            {	
                mt65xx_disp_show_battery_capacity(prog);
            }
            prog += 25;
            if (prog > 100) prog = prog_temp;
        }
#endif

#if 0
        /* UBOOT charging idle mode */
        if (!bl_switch) {
            mt6577_sleep(500, KAL_FALSE);
            mt65xx_disp_power(TRUE);
            bl_switch_timer++;
            mt65xx_backlight_on();
            g_bl_on = 1;				
        }	
        if (mt6577_detect_key(BACKLIGHT_KEY)) { 
            bl_switch = KAL_FALSE;
            bl_switch_timer = 0;
            g_bl_on = 1;
            printf("[BATTERY] mt65xx_backlight_on\r\n");
        }	
        if (bl_switch_timer > BL_SWITCH_TIMEOUT) {
            bl_switch = KAL_TRUE;
            bl_switch_timer = 0;
            mt65xx_backlight_off();
            mt65xx_disp_power(FALSE);
            g_bl_on = 0;
            
            // fill the screen with a whole black image
            mt65xx_disp_fill_rect(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT, 0x0);
            mt65xx_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
            
            printf("[BATTERY] mt65xx_backlight_off\r\n");
        }
#endif

    }

    /**************** For UBOOT : End ****************/	

    //if (Enable_BATDRV_LOG == 1) {
    printf("[BATTERY:AVG(%d,%dmA)] BatTemp:%d Vbat:%d VBatSen:%d SOC:%d ChrDet:%d Vchrin:%d Icharging:%d(%d) ChrType:%d \r\n", 
    BATTERY_AVERAGE_SIZE, CHARGING_FULL_CURRENT, BMT_status.temperature ,BMT_status.bat_vol, BMT_status.ADC_BAT_SENSE, BMT_status.SOC, 
    upmu_is_chr_det(), BMT_status.charger_vol, BMT_status.ICharging, g_BatteryAverageCurrent, CHR_Type_num );  
    //}  
    
    //if (Enable_BATDRV_LOG == 1) {
    //    printf("[BATTERY] CON_9:%x, CON10:%x\n\r", INREG16(CHR_CON_9), INREG16(CHR_CON_10));
    //}
    
	//for(iii=0x21 ; iii<0x3F ; iii++)
	//{
		//printf("Bank0[0x%x]=0x%x, ", iii, upmu_get_reg_value(iii));
	//}
	
    /* Protection Check : start*/
    //BAT_status = BAT_CheckPMUStatusReg();
    //if(BAT_status != PMU_STATUS_OK)
    //    return PMU_STATUS_FAIL;                  
        
	#if (BAT_TEMP_PROTECT_ENABLE == 1)
    if ((BMT_status.temperature <= MIN_CHARGE_TEMPERATURE) || 
        (BMT_status.temperature == ERR_CHARGE_TEMPERATURE))
    {
        printf(  "[BATTERY] Battery Under Temperature or NTC fail !!\n\r");                
		BMT_status.bat_charging_state = CHR_ERROR;
        return PMU_STATUS_FAIL;       
    }
	#endif		    	
	if (BMT_status.temperature >= MAX_CHARGE_TEMPERATURE)
    {
        printf(  "[BATTERY] Battery Over Temperature !!\n\r");                
		BMT_status.bat_charging_state = CHR_ERROR;
        return PMU_STATUS_FAIL;       
    }
    
    if (upmu_is_chr_det() == KAL_TRUE)
    {
		#if (V_CHARGER_ENABLE == 1)
        if (BMT_status.charger_vol <= V_CHARGER_MIN )
        {
            printf(  "[BATTERY]Charger under voltage!!\r\n");                    
            BMT_status.bat_charging_state = CHR_ERROR;
            return PMU_STATUS_FAIL;        
        }
		#endif		
        if ( BMT_status.charger_vol >= V_CHARGER_MAX )
        {
            printf(  "[BATTERY]Charger over voltage !!\r\n");                    
            BMT_status.charger_protect_status = charger_OVER_VOL;
            BMT_status.bat_charging_state = CHR_ERROR;
            return PMU_STATUS_FAIL;        
        }
    }
    /* Protection Check : end*/
    
    if (upmu_is_chr_det() == KAL_TRUE)
    {
        if ((BMT_status.bat_vol < RECHARGING_VOLTAGE) && (BMT_status.bat_full) && (g_HW_Charging_Done == 1))	
        {
            //if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] Check Battery Re-charging & turn on charging!!\n\r");                
            //}
            BMT_status.bat_full = KAL_FALSE;    
            g_bat_full_user_view = KAL_TRUE;
            BMT_status.bat_charging_state = CHR_CC;
            
            g_HW_Charging_Done = 0;         
            
            pchr_turn_on_charging_fan5405();
        }		
    }
    
    return PMU_STATUS_OK;
}

PMU_STATUS BAT_BatteryStatusFailAction(void)
{
    if (Enable_BATDRV_LOG == 1) {
        printf(  "[BATTERY] BAD Battery status... Charging Stop !!\n\r");            
    }
    
    BMT_status.total_charging_time = 0;
    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.POSTFULL_charging_time = 0;

    /*  Disable charger */
    pchr_turn_off_charging_fan5405();
    
    //g_sw_cv_enable = 0;
    
    return PMU_STATUS_OK;
}

PMU_STATUS BAT_ChargingOTAction(void)
{    
    printf(  "[BATTERY] Charging over time !!\n\r");            
    
    BMT_status.bat_full = KAL_TRUE;
    BMT_status.total_charging_time = 0;
    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.POSTFULL_charging_time = 0;
    
    g_HW_Charging_Done = 1;	
    g_Charging_Over_Time = 1;
    
    /*  Disable charger*/
    pchr_turn_off_charging_fan5405();
    
    //g_sw_cv_enable = 0;
    
    return PMU_STATUS_OK;
}

PMU_STATUS BAT_BatteryFullAction(void)
{
    //if (Enable_BATDRV_LOG == 1) {    
    printf(  "[BATTERY] Battery full !!\n\r");            
    //}
    
    BMT_status.bat_full = KAL_TRUE;
    BMT_status.total_charging_time = 0;
    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.POSTFULL_charging_time = 0;
    
    g_HW_Charging_Done = 1;

    /*  Disable charger */
    //pchr_turn_off_charging_fan5405();
    
    //g_sw_cv_enable = 0;
    
    return PMU_STATUS_OK;
}

extern BOOT_ARGUMENT *g_boot_arg;

void check_battery_exist(void)
{
    kal_uint32 baton_count = 0;
	int mode = 0;

    baton_count += upmu_chr_get_baton_undet();
    baton_count += upmu_chr_get_baton_undet();
    baton_count += upmu_chr_get_baton_undet();
        
    mode = g_boot_arg->boot_mode &= 0x000000FF;
    if (g_boot_arg->maggic_number == BOOT_ARGUMENT_MAGIC)
    {        
    if( baton_count >= 3)
    {
            if( (mode == META_BOOT) || (mode == ATE_FACTORY_BOOT) )
            {
                printf("[BATTERY] boot mode = %d, bypass battery check\n", mode);
            }
            else
            {
                printf("[BATTERY] Battery is not exist, power off FAN5405 and system (%d) 1\n", baton_count);
                pchr_turn_off_charging_fan5405();
                mt6577_power_off();        
            }
        }        
    }
    else
    {        
        if( baton_count >= 3)
        {
            printf("[BATTERY] Battery is not exist, power off FAN5405 and system (%d) 2\n", baton_count);
        pchr_turn_off_charging_fan5405();
        mt6577_power_off();        
    }
}
}

int get_pmic_flag=0;

void BAT_thread_fan5405(void)
{
    int BAT_status = 0;
	kal_uint32 tmp32;
    kal_uint32 fan5405_status=0;
       
    printf("[BATTERY] mtk_wdt_restart()\n");
    mtk_wdt_restart();
       
    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY] LOG. %d---------------------------------------------------------------------\n", TBAT_OVER_CRITICAL_LOW);
    }

    if ((upmu_is_chr_det() == KAL_TRUE))
    {
        check_battery_exist();
    }

    printf("[BATTERY] SET TMR_RST");
    fan5405_config_interface_liao(0x00,0x80);

	if(get_pmic_flag == 0)
	{
		/* get pmic version */
		/* Low part of CID */
	    tmp32=upmu_get_cid0();
	    g_eco_version |= tmp32;
	    /* High part of CID */
	    tmp32=upmu_get_cid1();
	    g_eco_version |= (tmp32 << 8);
		if (g_eco_version == PMIC6329_E1_CID_CODE)
	    {
	        printf("[Charger_E1] Get PMIC version = E1\n");
			upmu_chr_vcdt_lv_vth(0); // VCDT_LV=4.2V
			printf("[Charger_E1] Set VCDT_LV=4.2V\n");
		}
		else
		{
			printf("[Battery] Get PMIC version > E1\n");
		}

		get_pmic_flag = 1;
	}
	
	/* If charger does not exist */
   // if ( upmu_is_chr_det() == KAL_FALSE )
    if(upmu_chr_get_vcdt_hv_det() == 1 || (upmu_is_chr_det() == KAL_FALSE))
    {
        bmt_charger_ov_check();
        BMT_status.charger_type = CHARGER_UNKNOWN;
        BMT_status.bat_full = KAL_FALSE;
        g_bat_full_user_view = KAL_FALSE;
        g_usb_state = USB_UNCONFIGURED;
        
        g_HW_Charging_Done = 0;
        g_Charging_Over_Time = 0;
        
        printf("[BATTERY] No Charger, Power OFF !?\n");

        pchr_turn_off_charging_fan5405();
        
        printf("[BATTERY] mt6577_power_off !!\n");
        #ifndef NO_POWER_OFF
        mt6577_power_off();
        #endif
        while(1);
    }
    
    /* Check Battery Status */
    BAT_status = BAT_CheckBatteryStatus_fan5405();
    if( BAT_status == PMU_STATUS_FAIL )
        g_Battery_Fail = KAL_TRUE;
    else
        g_Battery_Fail = KAL_FALSE;
    
    /* No Charger */
    if(BAT_status == PMU_STATUS_FAIL || g_Battery_Fail)    
    {
        BAT_BatteryStatusFailAction();
    }
    
    /* Battery Full */
    //else if (BMT_status.bat_full)
    /* HW charging done, real stop charging */
    else if (g_HW_Charging_Done == 1)
    {   
        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] Battery real full. \n");
        }
        BAT_BatteryFullAction();		
    }
    
    /* Charging Overtime, can not charging */
    else if (g_Charging_Over_Time == 1)
    {
        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] Charging Over Time. \n");
        }
        pchr_turn_off_charging_fan5405();
    }
    
    /* Battery Not Full and Charger exist : Do Charging */
    else
    {
        if(BMT_status.total_charging_time >= MAX_CHARGING_TIME)
        {
            BMT_status.bat_charging_state = CHR_BATFULL;
            BAT_ChargingOTAction();
            return;
        }
        
        if ( BMT_status.TOPOFF_charging_time >= MAX_CV_CHARGING_TIME )
        {
            if (Enable_BATDRV_LOG == 1) {
                printf("BMT_status.TOPOFF_charging_time >= %d \r\n", MAX_CV_CHARGING_TIME);
            }
            BMT_status.bat_charging_state = CHR_BATFULL;
            BAT_BatteryFullAction();										
            return;
        }

        //printf("[BATTERY] DUMP-start \n");    
        fan5405_dump_register();
        //printf("[BATTERY] DUMP-end \n");
                
        fan5405_status = fan5405_get_chip_status();		
        /* check battery full */
    	if( fan5405_status == 0x2 )
    	{
    		BMT_status.bat_charging_state = CHR_BATFULL;
    		BMT_status.bat_full = KAL_TRUE;
    		BMT_status.total_charging_time = 0;
    		BMT_status.PRE_charging_time = 0;
    		BMT_status.CC_charging_time = 0;
    		BMT_status.TOPOFF_charging_time = 0;
    		BMT_status.POSTFULL_charging_time = 0;	
    		g_HW_Charging_Done = 1;			
    		//pchr_turn_off_charging_fan5405();
    		printk("[BATTERY:fan5405] Battery real full and disable charging (%ld) \n", fan5405_status); 
    		return;
    	}

        /* Charging flow begin */
    	BMT_status.total_charging_time += BAT_TASK_PERIOD;
    	pchr_turn_on_charging_fan5405();		
    	if (Enable_BATDRV_LOG == 1) {
        		printk(  "[BATTERY:fan5405] Total charging timer=%ld, fan5405_status=%ld \n\r", 
    				BMT_status.total_charging_time, fan5405_status);    
        	}
            
        }
    
    g_SW_CHR_OUT_EN = 1;
        
}

void bmt_charger_ov_check(void)
{
     if(upmu_chr_get_vcdt_hv_det() == 1)
     {
	      pchr_turn_off_charging_fan5405();
              printf("[bmt_charger_ov_check]uboot charger ov, turn off charging\r\n");
              while(1)             
	     {  
                 if(charger_ov_boot_display == 0)
	         {
		   mt65xx_disp_power(TRUE);
		   //mt65xx_disp_show_charger_ov_logo(); 
		   mt65xx_disp_wait_idle();
		   charger_ov_boot_display = 1;
		   printf("uboot charger ov, Set low brightness\r\n");
		   mt65xx_leds_brightness_set(6, 20);
	         }
                BMT_status.charger_vol = PMIC_IMM_GetOneChannelValue(AUXADC_CHARGER_VOLTAGE_CHANNEL, 5);
                BMT_status.charger_vol = BMT_status.charger_vol / 100;
                 if (BMT_status.charger_vol < 4000) //charger out detection        
		 {             
		    #ifndef NO_POWER_OFF                
		    mt6577_power_off();              
		    #endif             
		    while(1);            
		  } 
                  #ifdef GPT_TIMER                  
		  mt6577_sleep(500, KAL_FALSE);          
		  #else              
		  tmo = get_timer(0);			  
		  while(get_timer(tmo) <= 500 /* ms */);            
		  #endif        
              }
     }    
}

void uboot_charging_display()
{
    #define BATTERY_BAR 25
    
    if ( (BMT_status.bat_full) || (user_view_flag == KAL_TRUE) ) 
    {		
        if(g_bl_on == KAL_TRUE)
        {	
            mt65xx_disp_show_battery_full();	
        }
        user_view_flag = KAL_TRUE;
    } 
    else 
    {	
        prog_temp = (bat_volt_check_point/BATTERY_BAR) * BATTERY_BAR;
        
        if (prog_first == 1)
        {
            prog = prog_temp;
            prog_first = 0;
        }
        if(g_bl_on == 1)
        {	
#ifdef ANIMATION_NEW	
            mt65xx_disp_show_battery_capacity(bat_volt_check_point); // Charging Animation
#else	
            mt65xx_disp_show_battery_capacity(prog);
#endif

        }
        prog += BATTERY_BAR;
        if (prog > 100) prog = prog_temp;
    }

    /* UBOOT charging idle mode */
    if (!bl_switch) {
        mt65xx_disp_power(TRUE);
        bl_switch_timer++;
        mt65xx_backlight_on();
        g_bl_on = 1;				
    }	
    
    if (bl_switch_timer > BL_SWITCH_TIMEOUT) {
        bl_switch = KAL_TRUE;
        bl_switch_timer = 0;
        mt65xx_backlight_off();
        mt65xx_disp_power(FALSE);
        g_bl_on = 0;
        printf("[BATTERY] mt65xx_backlight_off\r\n");
    }
}

void batdrv_init(void)
{
    int i = 0;
    int ret = 0;
    
    /* Initialization BMT Struct */
    for (i=0; i<BATTERY_AVERAGE_SIZE; i++) {
        batteryCurrentBuffer[i] = 0;
        batteryVoltageBuffer[i] = 0; 
        batterySOCBuffer[i] = 0;
    }
    batteryVoltageSum = 0;
    batteryCurrentSum = 0;
    batterySOCSum = 0;
    
    BMT_status.bat_exist = 1;       /* phone must have battery */
    BMT_status.charger_exist = 0;   /* for default, no charger */
    BMT_status.bat_vol = 0;
    BMT_status.ICharging = 0;
    BMT_status.temperature = 0;
    BMT_status.charger_vol = 0;
    //BMT_status.total_charging_time = 0;
    BMT_status.total_charging_time = 1;
    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.POSTFULL_charging_time = 0;
    
    BMT_status.bat_charging_state = CHR_PRE;

    /* init i2c0 interface*/
    ret = i2c0_v1_init();
    printf("Init I2C0: %s(%d)\n", ret ? "FAIL" : "OK", ret);

    if ((upmu_is_chr_det() == KAL_TRUE))
    {
        printf("fan5405 hw init\n");        
        fan5405_hw_init();        
        fan5405_dump_register();
    }
    #if defined(CHARGER_IC_GPIO_RESETTING)
    gpio_number   = cust_gpio_number;
    gpio_off_mode = cust_gpio_off_mode;
    gpio_off_dir  = cust_gpio_off_dir;
    gpio_off_out  = cust_gpio_off_out;
    gpio_on_mode  = cust_gpio_on_mode;
    gpio_on_dir   = cust_gpio_on_dir;
    gpio_on_out   = cust_gpio_on_out;
    #endif
    printf("[UBOOT][Charger IC] GPIO setting : %d,%d,%d,%d,%d,%d,%d\n",
    gpio_number, gpio_off_mode, gpio_off_dir, gpio_off_out, 
    gpio_on_mode, gpio_on_dir, gpio_on_out);
     
    upmu_chr_vcdt_hv_vth(0xA);		//VCDT_HV_VTH, 6.5V
		   
    printf("[BATTERY] batdrv_init : Done\n");
}

void mt65xx_bat_init(void)
{    
    UINT32 i;
    long tmo;
    long tmo2;
    BOOL checked = FALSE;
    BOOL print_msg = FALSE;
    int ret = 0;	
    int press_pwrkey_count=0, loop_count = 0;	
    BOOL pwrkey_ready = false;
    BOOL back_to_charging_animation_flag = false;
     
    #if (CHARGING_PICTURE == 1)
    mt65xx_disp_enter_charging_state();
    #else
    mt65xx_disp_show_boot_logo();
    #endif	
    
    if ((upmu_is_chr_det() == KAL_TRUE))
    {
        check_battery_exist();
    }
    
    sc_mod_init();
    batdrv_init();
    
    BMT_status.bat_full = FALSE;
	BAT_GetVoltage_notbat_fan5405();
	BAT_GetVoltage_notbat_fan5405();
   
    if ( BMT_status.bat_vol > RECHARGING_VOLTAGE ) {
        user_view_flag = KAL_TRUE;
    } else {
        user_view_flag = KAL_FALSE;
    }
    
     if (mt6577_detect_powerkey())	  
         pwrkey_ready = true;
     else
         pwrkey_ready = false;

    /* Boot with Charger */
    if ((upmu_is_chr_det() == KAL_TRUE))	
    {
		CHR_Type_num = mt_charger_type_detection();
        BMT_status.charger_type = CHR_Type_num;
		//BMT_status.charger_type = NONSTANDARD_CHARGER;		
		
        printf("[Battery] turn on charging first for sw workaround \n");
        pchr_turn_on_charging_fan5405();        
		
        while (1) 
        {
            upmu_chr_chrwdt_td(0x0);				// CHRWDT_TD, 4s, check me
            upmu_chr_chrwdt_int_en(1);				// CHRWDT_INT_EN, check me
            upmu_chr_chrwdt_en(1); 					// CHRWDT_EN, check me
            upmu_chr_chrwdt_flag_wr(1);				// CHRWDT_FLAG, check me
            upmu_chr_vcdt_hv_enable(1);		//VCDT_HV_EN
            
            //add charger ov detection
            bmt_charger_ov_check();

            if (rtc_boot_check(true) || meta_mode_check() || (pwrkey_ready == true) 
            	|| mtk_wdt_boot_check()==WDT_BY_PASS_PWK_REBOOT || g_boot_arg->boot_reason==BR_TOOL_BY_PASS_PWK )			
            {
                // Low Battery Safety Booting
                //pchr_turn_off_charging_fan5405();
                BMT_status.bat_vol = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
                printf("check VBAT=%d mV with %d mV\n", BMT_status.bat_vol, BATTERY_LOWVOL_THRESOLD);
                pchr_turn_on_charging_fan5405();

#if 1				
                while ( BMT_status.bat_vol < BATTERY_LOWVOL_THRESOLD )
                {	
                    if (low_bat_boot_display == 0)
                    {
                        mt65xx_disp_power(TRUE);
                        mt65xx_backlight_off();
                        printf("Before mt6516_disp_show_low_battery\r\n");
                        mt65xx_disp_show_low_battery();
                        printf("After mt6516_disp_show_low_battery\r\n");
                        mt65xx_disp_wait_idle();
                        printf("After mt6516_disp_wait_idle\r\n");
                        
                        low_bat_boot_display = 1;												
                        
                        printf("Set low brightness\r\n");
                        mt65xx_leds_brightness_set(6, 20);
                    }
                    
                    rtc_boot_check(false);
                    BAT_thread_fan5405();
                    printf("-");
                    
                    #ifdef GPT_TIMER						
                        if (g_bl_on == KAL_TRUE)
                            mt6577_sleep(1000, KAL_FALSE);
                        else
                            mt6577_sleep(1000, KAL_TRUE);
                    #else
                        tmo2 = get_timer(0);            
                        while(get_timer(tmo2) <= 1000 /* ms */);                    
                    #endif			
                    
                    if((pwrkey_ready ==true) & mt6577_detect_powerkey()==0 )
        		    {
        		        back_to_charging_animation_flag = TRUE;
        		        break;
        		    }
                    else
                    {
                      back_to_charging_animation_flag = false;
                    }

                    //pchr_turn_off_charging_fan5405();
                    BMT_status.bat_vol = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
                    printf("VBAT=%d < %d\n", BMT_status.bat_vol, BATTERY_LOWVOL_THRESOLD);
                    pchr_turn_on_charging_fan5405();
                }
                 
                if(back_to_charging_animation_flag == false)
		        {
                    mt65xx_disp_power(KAL_TRUE);
                
                    if (g_boot_mode != ALARM_BOOT)
                    {
                        mt65xx_disp_show_boot_logo();
                        
                        // update twice here to ensure the LCM is ready to show the
                        // boot logo before turn on backlight, OR user may glimpse
                        // at the previous battery charging screen
                        mt65xx_disp_show_boot_logo();
                        mt65xx_disp_wait_idle();
                    }
                    else
                    {
                        printf("[BATTERY] Power off alarm trigger! Boot Linux Kernel!!\n\r");
                        
                        // fill the screen with a whole black image
                        mt65xx_disp_fill_rect(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT, 0x0);
                        mt65xx_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
                        mt65xx_disp_wait_idle();
                    }
                
                    printf("Restore brightness\r\n");
                    mt65xx_leds_brightness_set(6, 255);
                    mt65xx_backlight_on();
#endif				
                
                    //pchr_turn_off_charging_fan5405();
                    
                    sc_mod_exit();
                    return;
                }

                back_to_charging_animation_flag = false;
	            low_bat_boot_display = 0;

            }
            else
            {
                //printf("[BATTERY] U-BOOT Charging !! \n\r");  
            }
            
            if(print_msg==FALSE)
            {	
                #ifdef ANIMATION_NEW 	
                BAT_CheckBatteryStatus_fan5405();  
                #endif
                uboot_charging_display();
                printf("[BATTERY] Charging !! Press Power Key to Booting !!! \n\r");				
                print_msg = TRUE;
            }
                        
            if (g_thread_count >= 5)  //change for charger ov 
            {
                g_thread_count = 1;
                BAT_thread_fan5405();
                printf(".");		
            }
            else
            {
                g_thread_count++;
            }
            
            #ifdef GPT_TIMER		
                if (g_bl_on == KAL_TRUE)
                    mt6577_sleep(200, KAL_FALSE);
                else
                    mt6577_sleep(200, KAL_TRUE);
            #else
                tmo = get_timer(0);			
                while(get_timer(tmo) <= 200 /* ms */);
            #endif		
            
             if (loop_count++ == 60) loop_count = 0;

             if (mt6577_detect_key(BACKLIGHT_KEY) || (!mt6577_detect_powerkey() && press_pwrkey_count > 0))
             {
                bl_switch = false;
                bl_switch_timer = 0;
                g_bl_on = true;
                printf("[BATTERY] mt65xx_backlight_on\r\n");
             }

	         if (mt6577_detect_powerkey())
             { 
		        press_pwrkey_count++;
	            printf("[BATTERY] press_pwrkey_count = %d, POWER_ON_TIME = %d\n", press_pwrkey_count, POWER_ON_TIME);
	         }
             else
             { 
        		press_pwrkey_count = 0;
             }
			 
            if (press_pwrkey_count > POWER_ON_TIME)	
                pwrkey_ready = true;
            else
                pwrkey_ready = false;
			
            /* if (mt6577_detect_key(BACKLIGHT_KEY) || press_pwrkey_count > 0)
                    {
                        bl_switch = false;
                        bl_switch_timer = 0;
                        g_bl_on = true;
                        printf("[BATTERY] mt65xx_backlight_on\r\n");
                    }*/
#ifdef ANIMATION_NEW         
            if (((loop_count % 1) == 0) && bl_switch == KAL_FALSE) // 3 --> 1 Yu
            {
                if ((BMT_status.total_charging_time % 9) == 0)
                {
                    tmo = get_timer(0);			
                    while(get_timer(tmo) <= 50 /* ms */);
                }
                uboot_charging_display();
            }
#else  
            if (((loop_count % 5) == 0) && bl_switch == false) // update charging screen
            {
                if (Enable_BATDRV_LOG == 1)
                {
                 printf("[BATTERY] loop_count = %d\n", loop_count);
                }
                uboot_charging_display();
            }  
#endif			
        } 	    
    }
    else
    {
        bmt_charger_ov_check();
        
        if ( (rtc_boot_check(false)||mtk_wdt_boot_check()==WDT_BY_PASS_PWK_REBOOT) && BMT_status.bat_vol >= BATTERY_LOWVOL_THRESOLD)
        //if (BMT_status.bat_vol >= BATTERY_LOWVOL_THRESOLD)
        {
            printf("[BATTERY] battery voltage(%dmV) >= CLV ! Boot Linux Kernel !! \n\r",BMT_status.bat_vol);
            sc_mod_exit();
            return;
        }
        else
        {
            printf("[BATTERY] battery voltage(%dmV) <= CLV ! Can not Boot Linux Kernel !! \n\r",BMT_status.bat_vol);
            pchr_turn_off_charging_fan5405();
            #ifndef NO_POWER_OFF
            mt6577_power_off();
            #endif			
            while(1)
            {
            	printf("If you see the log, please check with RTC power off API\n\r");
            }
        }
    }
    
    sc_mod_exit();
    return;
}

#else

#include <asm/arch/mt65xx_typedefs.h>

void mt65xx_bat_init(void)
{
    printf("[BATTERY] Skip mt65xx_bat_init !!\n\r");
    printf("[BATTERY] If you want to enable power off charging, \n\r");
    printf("[BATTERY] Please #define CFG_POWER_CHARGING!!\n\r");
}

//kal_bool pmic_chrdet_status(void)
//{
//   return KAL_FALSE;
//}

#endif
