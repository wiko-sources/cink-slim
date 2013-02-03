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
#include <asm/arch/mtk_wdt.h>
#include <asm/arch/mtk_key.h>

#include <cust_battery.h>

#undef printf

//#define CONFIG_DEBUG_MSG
#define GPT_TIMER

#ifdef MTK_NCP1851_SUPPORT
#include "ncp1851.h"
#include <asm/arch/mt6577_gpio.h>
#endif
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
#ifdef MTK_NCP1851_SUPPORT
#define BATTERY_LOWVOL_THRESOLD             3450
#else
#define BATTERY_LOWVOL_THRESOLD             3450
#endif
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
#define BL_SWITCH_TIMEOUT                   1*6         // 6s
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
    UINT32      charger_vol;
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

int g_battery_thermal_throttling_flag=0;
int battery_cmd_thermal_test_mode=0;
int battery_cmd_thermal_test_mode_value=0;
int g_battery_tt_check_flag=0;
#ifdef MTK_JEITA_SUPPORT
int g_battery_jeita_protection_flag = 0;
int g_battery_jeita_protection_flag_prev = 0;
#endif

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
kal_bool bl_switch = KAL_FALSE;
kal_bool user_view_flag = KAL_FALSE;

/**********************************************
 * Battery Temprature Parameters and functions
 ***********************************************/
typedef struct{
    INT32 BatteryTemp;
    INT32 TemperatureR;
} BATT_TEMPERATURE;

#ifdef MTK_NCP1851_SUPPORT
int PMIC_IMM_GetBatChannelValue(int deCount)
{
    kal_int32 adc_result = 0;
    adc_result = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL, deCount);
    return adc_result;
}
#endif

/* convert register to temperature  */
INT16 BattThermistorConverTemp(INT32 Res)
{
    int i = 0;
    INT32 RES1 = 0, RES2 = 0;
    INT32 TBatt_Value = -200, TMP1 = 0, TMP2 = 0;

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

void select_charging_curret()
{
    if ( BMT_status.charger_type == STANDARD_HOST )
    {
        g_temp_CC_value = USB_CHARGER_CURRENT;

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] STANDARD_HOST CC mode charging : %d\r\n", USB_CHARGER_CURRENT);
        }
    }
    else if (BMT_status.charger_type == NONSTANDARD_CHARGER)
    {
        g_temp_CC_value = USB_CHARGER_CURRENT;

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] NONSTANDARD_CHARGER CC mode charging : %d\r\n", USB_CHARGER_CURRENT); // USB HW limitation
        }
    }
    else if (BMT_status.charger_type == STANDARD_CHARGER)
    {
        g_temp_CC_value = AC_CHARGER_CURRENT;

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] STANDARD_CHARGER CC mode charging : %d\r\n", AC_CHARGER_CURRENT);
        }
    }
    else if (BMT_status.charger_type == CHARGING_HOST)
    {
        g_temp_CC_value = AC_CHARGER_CURRENT;

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] CHARGING_HOST CC mode charging : %d\r\n", AC_CHARGER_CURRENT);
        }
    }
    else
    {
        g_temp_CC_value = Cust_CC_70MA;

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] Default CC mode charging : %d\r\n", Cust_CC_70MA);
        }
    }
}

#ifdef MTK_JEITA_SUPPORT
void set_ac_current_JEITA(void)
{
    if(g_battery_thermal_throttling_flag == 1)
    {
        if((BMT_status.temperature >= MAX_CHARGE_TEMPERATURE) || (BMT_status.temperature <= MIN_CHARGE_TEMPERATURE))
        {
            return; //Charging will be disabled by thermal protection
        }
        else if(g_battery_jeita_protection_flag_prev == 1)
        {
            g_temp_CC_value -= 4; //reduce 400mA
            printf("[BATTERY:ncp1851] battery temperature (%d), WARM(%d), reduce charging current by 400mA \r\n", BMT_status.temperature, WARM_CHARGE_TEMPERATURE);            
        }
        else if(g_battery_jeita_protection_flag_prev == 2)
        {
            g_temp_CC_value -= 4; //reduce 400mA
            printf("[BATTERY:ncp1851] battery temperature (%d), CHILLY(%d), reduce charging current by 400mA \r\n", BMT_status.temperature, WARM_CHARGE_TEMPERATURE);                        
        }			
    }
}
#endif

#ifdef MTK_NCP1851_SUPPORT
void ncp1851_set_ac_current(void)
{
    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY:ncp1851] ncp1851_set_ac_current \r\n");
    }

#ifdef MTK_JEITA_SUPPORT
    set_ac_current_JEITA();	
#endif

    if((g_temp_CC_value > Cust_CC_1600MA) || (g_temp_CC_value < Cust_CC_400MA))
    {
        printf("[BATTERY:ncp1851] invalid current selected (%ld), use 500mA \r\n", g_temp_CC_value);
	 ncp1851_set_ichg(1);
    }
    else
    {
        ncp1851_set_ichg(g_temp_CC_value);
    }
}

void ncp1851_set_low_chg_current(void)
{
    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY:ncp1851] ncp1851_set_low_chg_current \r\n");
    }
    ncp1851_set_iinlim(0x0); //IN current limit at 100mA	
    ncp1851_set_iinset_pin_en(0x0); //Input current limit and AICL control by I2C
    ncp1851_set_iinlim_en(0x1); //enable I2C input current limit
    ncp1851_set_aicl_en(0x0); //disable AICL
    ncp1851_set_ichg(0x0); //charging current limit at 400mA
}

void select_charging_curret_ncp1851(void)
{
    if ( BMT_status.charger_type == STANDARD_HOST )
    {
        g_temp_CC_value = USB_CHARGER_CURRENT;
        ncp1851_set_iinlim(0x1); //IN current limit at 500mA		
        ncp1851_set_iinset_pin_en(0x0); //Input current limit and AICL control by I2C
        ncp1851_set_iinlim_en(0x1); //enable input current limit
        ncp1851_set_aicl_en(0x0); //disable AICL
        ncp1851_set_ac_current();

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY:ncp1851] STANDARD_HOST CC mode charging : %d\r\n", USB_CHARGER_CURRENT);
        }
    }
    else if (BMT_status.charger_type == NONSTANDARD_CHARGER)
    {
        g_temp_CC_value = AC_CHARGER_CURRENT;
        ncp1851_set_iinlim(0x3); //IN current limit at 1.5A		
        ncp1851_set_iinset_pin_en(0x0); //Input current limit and AICL control by I2C
        ncp1851_set_iinlim_en(0x1); //enable input current limit
        ncp1851_set_aicl_en(0x1); //enable AICL
        ncp1851_set_ac_current();

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY:ncp1851] NONSTANDARD_CHARGER CC mode charging : %d\r\n", AC_CHARGER_CURRENT); // USB HW limitation
        }
    }
    else if (BMT_status.charger_type == STANDARD_CHARGER)
    {
        g_temp_CC_value = AC_CHARGER_CURRENT;
        ncp1851_set_iinlim(0x3); //IN current limit at 1.5A        		
        ncp1851_set_iinset_pin_en(0x0); //Input current limit and AICL control by I2C
        ncp1851_set_iinlim_en(0x1); //enable input current limit
        ncp1851_set_aicl_en(0x1); //enable AICL
        ncp1851_set_ac_current();

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY:ncp1851] STANDARD_CHARGER CC mode charging : %d\r\n", AC_CHARGER_CURRENT);
        }
    }
    else if (BMT_status.charger_type == CHARGING_HOST)
    {
        g_temp_CC_value = AC_CHARGER_CURRENT;
        ncp1851_set_iinlim(0x3); //IN current limit at 1.5A        		
        ncp1851_set_iinset_pin_en(0x0); //Input current limit and AICL control by I2C
        ncp1851_set_iinlim_en(0x1); //enable input current limit
        ncp1851_set_aicl_en(0x1); //enable AICL
        ncp1851_set_ac_current();

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY:ncp1851] CHARGING_HOST CC mode charging : %d\r\n", AC_CHARGER_CURRENT);
        }
    }
    else
    {
        g_temp_CC_value = Cust_CC_500MA;
//        ncp1851_set_low_chg_current();
        ncp1851_set_iinlim(0x1); //IN current limit at 500mA
        ncp1851_set_iinset_pin_en(0x0); //Input current limit and AICL control by I2C
        ncp1851_set_iinlim_en(0x1); //enable input current limit
        ncp1851_set_aicl_en(0x0); //disable AICL
        ncp1851_set_ac_current();

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY:ncp1851] Default CC mode charging : %d\r\n", Cust_CC_500MA);
        }
    }
}
#endif

void ChargerHwInit(void)
{
    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY] ChargerHwInit\n" );
    }

	upmu_chr_chrwdt_en(1); 			//CHRWDT_EN
	upmu_chr_chrwdt_td(0x3);		//CHRWDT_TD, 32s
	upmu_chr_vcdt_mode(0);			//VCDT_MODE
	upmu_chr_vcdt_hv_enable(1);		//VCDT_HV_EN

	//upmu_chr_bc11_bb_ctrl(1);		//BC11_BB_CTRL, check me
	upmu_chr_bc11_rst(1);			//BC11_RST

	upmu_chr_csdac_mode(1);			//CSDAC_MODE
	upmu_chr_vbat_ov_en(1);			//VBAT_OV_EN
	upmu_chr_vbat_ov_vth(0x0);		//VBAT_OV_VTH, 4.3V
	upmu_chr_baton_en(1);			//BATON_EN

	//Tim, for TBAT
	upmu_auxadc_buf_pwd_b(1);		//RG_BUF_PWD_B
	upmu_chr_baton_ht_en(1);		//BATON_HT_EN

	upmu_chr_ulc_det_en(1);			// RG_ULC_DET_EN=1
	upmu_chr_low_ich_db(1);			// RG_LOW_ICH_DB=000001'b
}

#ifdef MTK_NCP1851_SUPPORT
void ChargerHwInit_ncp1851(void)
{
	kal_uint32 ncp1851_status;

	if (Enable_BATDRV_LOG == 1) {
		printf("[BATTERY:ncp1851] ChargerHwInit_ncp1851\n" );
	}

       ncp1851_status = ncp1851_get_chip_status();

       upmu_chr_bc11_rst(0x1);		  //BC11_RST

       ncp1851_set_otg_en(0x0);
#if defined(USE_NCP1851_NTC) //use 2 NTC for MT6329 & NCP1851
//       upmu_chr_baton_en(0);
       ncp1851_set_ntc_en(0x1);
#else
       upmu_chr_baton_ht_en(0x1);	   //BATON_HT_EN
       ncp1851_set_ntc_en(0x0);
#endif
       ncp1851_set_trans_en(0);
       ncp1851_set_tj_warn_opt(0x1);
       ncp1851_set_jeita_opt(0x1);
       ncp1851_set_int_mask(0x0); //disable all interrupt
       ncp1851_set_tchg_rst(0x1); //reset charge timer
#ifdef NCP1851_PWR_PATH
       ncp1851_set_pwr_path(0x1);
#else
       ncp1851_set_pwr_path(0x0);
#endif

#ifdef MTK_JEITA_SUPPORT
       if((g_battery_thermal_throttling_flag == 1) && (g_battery_jeita_protection_flag_prev == 1))
       {
           ncp1851_set_ctrl_vbat(0x1C); //reduce VCHG to 4.0V
           printf("[BATTERY:ncp1851] battery temperature (%d), WARM(%d), reduce VCHG to 4.0V\n", BMT_status.temperature, WARM_CHARGE_TEMPERATURE);           
       }
       else if((g_battery_thermal_throttling_flag == 1) && (g_battery_jeita_protection_flag_prev == 2))
       {
           ncp1851_set_ctrl_vbat(0x1C); //reduce VCHG to 4.0V       
           printf("[BATTERY:ncp1851] battery temperature (%d), CHILLY(%d), reduce VCHG to 4.0V\n", BMT_status.temperature, CHILLY_CHARGE_TEMPERATURE);                  
       }
       else
#endif
       if((ncp1851_status == 0x8) || (ncp1851_status == 0x9) || (ncp1851_status == 0xA)) //WEAK WAIT, WEAK SAFE, WEAK CHARGE
           ncp1851_set_ctrl_vbat(0x1C); //VCHG = 4.0V
       else if(ncp1851_status == 0x4)
           ncp1851_set_ctrl_vbat(0x28); //VCHG = 4.3V to decrease charge time
       else
           ncp1851_set_ctrl_vbat(0x24); //VCHG = 4.2V

       ncp1851_set_ieoc(0x6); // terminate current = 250mA

       if((BMT_status.charger_type != STANDARD_HOST) && (BMT_status.charger_type != CHARGER_UNKNOWN))
           ncp1851_set_iweak(0x3); //weak charge current = 300mA
       else
           ncp1851_set_iweak(0x1); //weak charge current = 100mA

       if(BMT_status.charger_type == STANDARD_HOST)
           ncp1851_set_ctrl_vfet(0x3); // VFET = 3.4V
       else
           ncp1851_set_ctrl_vfet(0x5); // VFET = 3.6V

       ncp1851_set_vchred(0x2); //reduce 200mV (JEITA)
       ncp1851_set_ichred(0x0); //reduce 400mA (JEITA)
       ncp1851_set_batcold(0x5);
       ncp1851_set_bathot(0x3);
       ncp1851_set_batchilly(0x0);
       ncp1851_set_batwarm(0x0);
}
#endif

void pchr_turn_off_charging (void)
{
    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY] pchr_turn_off_charging !\r\n");
    }

    upmu_chr_chrwdt_int_en(0);				// CHRWDT_INT_EN
	upmu_chr_chrwdt_en(0); 					// CHRWDT_EN
	upmu_chr_chrwdt_flag_wr(0);				// CHRWDT_FLAG
	upmu_chr_csdac_enable(0);				// CSDAC_EN
	upmu_chr_enable(0);						// CHR_EN
	upmu_chr_hw_cv_en(0);					// RG_HWCV_EN

	if ( BMT_status.bat_charging_state != CHR_TOP_OFF)
	{
		gADC_BAT_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
		gADC_I_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL,1);
		gADC_BAT_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
		gADC_I_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL,1);
		gADC_BAT_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
		gADC_I_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL,1);
		gADC_BAT_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
		gADC_I_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL,1);
		gADC_BAT_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
		gADC_I_SENSE_temp = PMIC_IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL,1);
		if (Enable_BATDRV_LOG == 1) {
			printf("[BATTERY] gADC_BAT_SENSE_temp=%d, gADC_I_SENSE_temp=%d\n\r", gADC_BAT_SENSE_temp, gADC_I_SENSE_temp);
		}
		gADC_I_SENSE_offset = gADC_BAT_SENSE_temp - gADC_I_SENSE_temp;
	}
}

#ifdef MTK_NCP1851_SUPPORT
void pchr_turn_off_charging_ncp1851 (void)
{
	if (Enable_BATDRV_LOG == 1) {
		printf("[BATTERY:ncp1851] pchr_turn_off_charging_ncp1851!\r\n");
	}

       ncp1851_set_chg_en(0x0);
}
#endif

void pchr_turn_on_charging (void)
{
    if ( BMT_status.bat_charging_state == CHR_ERROR )
    {
        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] Charger Error, turn OFF charging !\r\n");
        }
        pchr_turn_off_charging();
    }
    else
    {
        ChargerHwInit();

        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY] pchr_turn_on_charging !\r\n");
        }

        select_charging_curret();

        if( g_temp_CC_value == Cust_CC_0MA)
        {
            pchr_turn_off_charging();
        }
        else
        {
            upmu_chr_baton_tdet_en(0);				// check me

			upmu_chr_vbat_cs_vth(g_temp_CC_value);	// CS_VTH

			upmu_chr_csdac_dly(0x4);				// CSDAC_DLY
			upmu_chr_csdac_stp(0x1);				// CSDAC_STP
			upmu_chr_csdac_stp_inc(0x1);			// CSDAC_STP_INC
			upmu_chr_csdac_stp_dec(0x2);			// CSDAC_STP_DEC
			upmu_chr_cs_en(1);						// CS_EN, check me

			if (g_eco_version == PMIC6329_E1_CID_CODE)
			{
			//upmu_chr_hw_cv_en(1);					// RG_HWCV_EN
			}
			else
			{
				upmu_chr_hw_cv_en(1);					// RG_HWCV_EN
			}

			upmu_chr_vbat_cv_en(1);					// CV_EN
			upmu_chr_vbat_cv_vth(0x0);				// VBAT_CV_VTH

			upmu_chr_chrwdt_td(0x3);				// CHRWDT_TD, 32s, check me
			upmu_chr_chrwdt_int_en(1);				// CHRWDT_INT_EN, check me
			upmu_chr_chrwdt_en(1); 					// CHRWDT_EN, check me
			upmu_chr_chrwdt_flag_wr(1);				// CHRWDT_FLAG, check me
			//upmu_chr_get_chrwdt_out();				// CHRWDT_OUT, check me

			upmu_chr_csdac_enable(1);				// CSDAC_EN
			upmu_chr_enable(1);						// CHR_EN

        }
    }

	if (Enable_BATDRV_LOG == 1) {
		printf("[BATTERY] Reg[0x21]=0x%x, Reg[0x25]=0x%x, Reg[0x2E]=0x%x\r\n",
			upmu_get_reg_value(0x21), upmu_get_reg_value(0x25), upmu_get_reg_value(0x2E) );
	}

}

#ifdef MTK_NCP1851_SUPPORT
void pchr_turn_on_charging_ncp1851 (void)
{
	if ( BMT_status.bat_charging_state == CHR_ERROR )
	{
		if (Enable_BATDRV_LOG == 1) {
			printf("[BATTERY:ncp1851] Charger Error, turn OFF charging !\r\n");
		}
		BMT_status.total_charging_time = 0;
		pchr_turn_off_charging_ncp1851();
		//Set SPM = 0, for lenovo platform, GPIO83
		mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO83, GPIO_OUT_ZERO);
	}
	else
	{
 		ChargerHwInit_ncp1851();

		upmu_chr_baton_tdet_en(0);				// check me

		if (Enable_BATDRV_LOG == 1) {
			printf("[BATTERY:ncp1851] pchr_turn_on_charging_ncp1851 !\r\n");
		}

		select_charging_curret_ncp1851();

		if( g_temp_CC_value == Cust_CC_0MA)
		{
		    pchr_turn_off_charging_ncp1851();
                  //Set SPM = 0, for lenovo platform, GPIO83
                  mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
                  mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
                  mt_set_gpio_out(GPIO83, GPIO_OUT_ZERO);
		}
              else
              {
                  ncp1851_set_chg_en(0x1); // charger enable
                  if (Enable_BATDRV_LOG == 1) {
                      printf("[BATTERY:ncp1851] charger enable !\r\n");
                  }
              }
	}
}
#endif

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

void BAT_GetVoltage(void)
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
    BMT_status.ICharging = g_Get_I_Charging();

    //if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY:ADC] VCHR:%d BAT_SENSE:%d I_SENSE:%d Current:%d\n", BMT_status.charger_vol,
        BMT_status.ADC_BAT_SENSE, BMT_status.ADC_I_SENSE, BMT_status.ICharging);
    //}

    g_BatteryAverageCurrent = BMT_status.ICharging;
}

void BAT_GetVoltage_notbat(void)
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

    if (Enable_BATDRV_LOG == 1) {
        printf("###### 100 <-> voltage : %d ######\r\n", Batt_VoltToPercent_Table[10].BattVolt);
    }

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

int BAT_CheckBatteryStatus(void)
{
    int BAT_status = PMU_STATUS_OK;
    int i = 0;
    int ret_check_I_charging = 0;
    int j = 0;
    int sw_chr_out_flag = 0;
    int repeat_times = 10;
	int iii = 0;

    /* Get Battery Information */
    BAT_GetVoltage();

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

#if 1
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

#if 1
        /* UBOOT charging idle mode */
        if (!bl_switch) {
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

    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY] CON_9:%x, CON10:%x\n\r", INREG16(CHR_CON_9), INREG16(CHR_CON_10));
    }

	for(iii=0x21 ; iii<0x3F ; iii++)
	{
		//printf("Bank0[0x%x]=0x%x, ", iii, upmu_get_reg_value(iii));
	}

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
            printf("[BATTERY] Battery Re-charging !!\n\r");
            //}
            BMT_status.bat_full = KAL_FALSE;
            g_bat_full_user_view = KAL_TRUE;
            BMT_status.bat_charging_state = CHR_CC;

            g_HW_Charging_Done = 0;

        }
    }

    return PMU_STATUS_OK;
}

#ifdef MTK_NCP1851_SUPPORT
int g_uboot_anime_on = 0;
void BAT_UbootAnimationControl(kal_bool display)
{
    #define BATTERY_BAR 25

    if (low_bat_boot_display == 0)
    {
        /* UBOOT charging animation */
        if(display == KAL_TRUE)
        {
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
				return;
            }
			
            if ( (g_HW_Charging_Done == 1) || (user_view_flag == KAL_TRUE) )
            {
                if(g_bl_on == 1)
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
                    mt65xx_disp_show_battery_capacity(prog);
                }
                prog += BATTERY_BAR;
                if (prog > 100) prog = prog_temp;
            }
        }
        else
        {
            mt65xx_backlight_off();
            mt65xx_disp_power(FALSE);
            printf("[BATTERY] mt65xx_backlight_off due to (display == KAL_FALSE)\r\n");
        }
    }
#ifdef MTK_NCP1851_SUPPORT
    else
    {
        if(display == KAL_TRUE)
        {
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
                printf("[BATTERY] mt65xx_backlight_off\r\n");
            }
        }
        else
        {
            mt65xx_backlight_off();
            mt65xx_disp_power(FALSE);
            printf("[BATTERY] mt65xx_backlight_off due to (display == KAL_FALSE)\r\n");
        }
    }
#endif
}

int BAT_CheckBatteryStatus_ncp1851(void)
{
    int BAT_status = PMU_STATUS_OK;
    int i = 0;
    int bat_temperature_volt=0;
#if defined(USE_ncp1851_TS_MONITOR)
    kal_uint32 ncp1851_ts_status = 0;
#endif
    /* Get Battery Information : start --------------------------------------------------------------------------*/

    /* Get V_BAT_SENSE */
    BMT_status.ADC_I_SENSE = PMIC_IMM_GetBatChannelValue(5);
    BMT_status.bat_vol = BMT_status.ADC_I_SENSE;

    /* Get V_I_SENSE */
    BMT_status.ADC_BAT_SENSE = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL, 5); //system voltage

    /* Get V_Charger */
    BMT_status.charger_vol = PMIC_IMM_GetOneChannelValue(AUXADC_CHARGER_VOLTAGE_CHANNEL,5);
    BMT_status.charger_vol = BMT_status.charger_vol / 100;

    /* Get V_BAT_Temperature */
    //Tim, for TBAT
    upmu_auxadc_buf_pwd_b(0x1);    //RG_BUF_PWD_B
    upmu_auxadc_adc_pwd_b(0x1);
    upmu_chr_baton_tdet_en(1);
    mt6577_sleep(50, KAL_FALSE);
    bat_temperature_volt = PMIC_IMM_GetOneChannelValue(AUXADC_TEMPERATURE_CHANNEL,5);
    if(bat_temperature_volt == 0)
    {
        BMT_status.temperature = g_bat_temperature_pre;
        if (Enable_BATDRV_LOG == 1) {
            printf("[BATTERY:ncp1851] Warning !! bat_temperature_volt == 0, restore temperature value\n\r");
        }
    }
    else
    {
        BMT_status.temperature = BattVoltToTemp(bat_temperature_volt);
        g_bat_temperature_pre = BMT_status.temperature;
    }

    if((g_battery_tt_check_flag == 0) && (BMT_status.temperature < MAX_CHARGE_TEMPERATURE) && (BMT_status.temperature > MIN_CHARGE_TEMPERATURE))
    {
        g_battery_thermal_throttling_flag = 1;
        printf("[BATTERY:ncp1851] Enable battery TT (%d)\n\r", BMT_status.temperature);
        g_battery_tt_check_flag = 1;
    }

    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY:ADC:ncp1851] VCHR:%ld BAT_SENSE:%ld I_SENSE:%ld TBAT:%ld TBAT_vol:%ld BatTT:%d\n",
        BMT_status.charger_vol, BMT_status.ADC_BAT_SENSE, BMT_status.ADC_I_SENSE, BMT_status.temperature, bat_temperature_volt, g_battery_thermal_throttling_flag);
    }

    BMT_status.ICharging = 0; //cannot measure adaptor charing current when using power path

    g_BatteryAverageCurrent = BMT_status.ICharging;
    /* Get Battery Information : end --------------------------------------------------------------------------*/

    /* Re-calculate Battery Percentage (SOC) */
    BMT_status.SOC = BattVoltToPercent(BMT_status.bat_vol);

    /* User smooth View when discharging : start */
    if((upmu_is_chr_det() == KAL_FALSE) || (BMT_status.charger_type == STANDARD_HOST))
    {
        if (BMT_status.bat_vol >= RECHARGING_VOLTAGE) {
            BMT_status.SOC = 100;
            BMT_status.bat_full = KAL_TRUE;
        }
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

    if(low_bat_boot_display == 0)
    {
	/* SOC only UP when charging */
	if ( BMT_status.SOC > bat_volt_check_point ) {
		bat_volt_check_point = BMT_status.SOC;
	}
	
	/* UBOOT charging LED */
	if ( (bat_volt_check_point >= 90)  || (user_view_flag == KAL_TRUE) ) {
		leds_battery_full_charging();
	} else if(bat_volt_check_point <= 10) {
		leds_battery_low_charging();
	} else {
		leds_battery_medium_charging();
	}
    }

    /**************** For UBOOT : End ****************/

    //if (Enable_BATDRV_LOG == 1) {
    printf("[BATTERY:AVG:ncp1851(%d,%dmA)] BatTemp:%d Vbat:%d VBatSen:%d SOC:%d ChrDet:%d Vchrin:%d Icharging:%d(%d) ChrType:%d \r\n",
    BATTERY_AVERAGE_SIZE, CHARGING_FULL_CURRENT, BMT_status.temperature ,BMT_status.bat_vol, BMT_status.ADC_BAT_SENSE, BMT_status.SOC,
    upmu_is_chr_det(), BMT_status.charger_vol, BMT_status.ICharging, g_BatteryAverageCurrent, CHR_Type_num );
    //}

	/* Protection Check : start*/
    BAT_status = BAT_CheckPMUStatusReg();
    if(BAT_status != PMU_STATUS_OK)
        return PMU_STATUS_FAIL;

    #if !defined(USE_NCP1851_NTC)
    #if (BAT_TEMP_PROTECT_ENABLE == 1)
    if ((BMT_status.temperature <= MIN_CHARGE_TEMPERATURE) ||
        (BMT_status.temperature == ERR_CHARGE_TEMPERATURE))
    {
        printf(  "[BATTERY:ncp1851] Battery Under Temperature or NTC fail !!\n\r");
        BMT_status.bat_charging_state = CHR_ERROR;
        return PMU_STATUS_FAIL;
    }
    #endif
	if (BMT_status.temperature >= MAX_CHARGE_TEMPERATURE)
    {
        printf(  "[BATTERY:ncp1851] Battery Over Temperature !!\n\r");
        BMT_status.bat_charging_state = CHR_ERROR;
        return PMU_STATUS_FAIL;
    }
    #endif

#ifdef MTK_JEITA_SUPPORT
    if(g_battery_thermal_throttling_flag == 1) 
    {
        if(g_battery_jeita_protection_flag_prev == 1)
        {
            if(BMT_status.temperature <= WARM_CHARGE_TEMPERATURE - TEMPERATURE_DEBOUNCE)			
            g_battery_jeita_protection_flag_prev = 0;			 
        }
        else if(g_battery_jeita_protection_flag_prev == 2)
        {
            if(BMT_status.temperature >= CHILLY_CHARGE_TEMPERATURE + TEMPERATURE_DEBOUNCE)			
            g_battery_jeita_protection_flag_prev = 0;			         
        }
    
        if(BMT_status.temperature >= WARM_CHARGE_TEMPERATURE)        	
            g_battery_jeita_protection_flag = 1; //WARM
        else if(BMT_status.temperature <= CHILLY_CHARGE_TEMPERATURE)
            g_battery_jeita_protection_flag = 2; //CHILLY
        else
            g_battery_jeita_protection_flag = 0;

	if(g_battery_jeita_protection_flag != 0)
            g_battery_jeita_protection_flag_prev = g_battery_jeita_protection_flag;
    }	
#endif	

    if( upmu_is_chr_det() == KAL_TRUE)
    {
        #if (V_CHARGER_ENABLE == 1)
        if (BMT_status.charger_vol <= V_CHARGER_MIN )
        {
            printf(  "[BATTERY:ncp1851]Charger under voltage!!\r\n");
            BMT_status.bat_charging_state = CHR_ERROR;
            return PMU_STATUS_FAIL;
        }
        #endif
        if ( BMT_status.charger_vol >= V_CHARGER_MAX )
        {
            printf(  "[BATTERY:ncp1851]Charger over voltage !!\r\n");
            ncp1851_read_register(7); //read 0x07-bit[7] to check if VIN > VINOV
            BMT_status.charger_protect_status = charger_OVER_VOL;
            BMT_status.bat_charging_state = CHR_ERROR;
            return PMU_STATUS_FAIL;
        }
    }
    /* Protection Check : end*/

#if 1
    if( upmu_is_chr_det() == KAL_TRUE)
    {
        if((BMT_status.bat_vol < RECHARGING_VOLTAGE) && (BMT_status.bat_full) && (g_HW_Charging_Done == 1) && (!g_Battery_Fail) )
        {
            if (Enable_BATDRV_LOG == 1) {
            	printf("[BATTERY:ncp1851] Battery Re-charging !!\n\r");
            }
            BMT_status.bat_full = KAL_FALSE;
            g_bat_full_user_view = KAL_TRUE;
            BMT_status.bat_charging_state = CHR_CC;

            g_HW_Charging_Done = 0;

            ncp1851_set_chg_en(0x0); //just for toggling CHR_EN to start a new charge sequence
            mt6577_sleep(1000, KAL_FALSE);
        }
    }
#endif

    //Reset error status if no error condition detected this turn
    BMT_status.charger_protect_status = 0;
    BMT_status.bat_charging_state = CHR_CC;

    return PMU_STATUS_OK;
}
#endif

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
    pchr_turn_off_charging();

    //g_sw_cv_enable = 0;

    return PMU_STATUS_OK;
}

PMU_STATUS BAT_ChargingOTAction(void)
{
    printf(  "[BATTERY] Charging over 12 hr stop !!\n\r");

    BMT_status.bat_full = KAL_TRUE;
    BMT_status.total_charging_time = 0;
    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.POSTFULL_charging_time = 0;

    g_HW_Charging_Done = 1;
    g_Charging_Over_Time = 1;

    /*  Disable charger*/
    pchr_turn_off_charging();

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
    pchr_turn_off_charging();

    //g_sw_cv_enable = 0;

    return PMU_STATUS_OK;
}

PMU_STATUS BAT_PreChargeModeAction(void)
{
    if (Enable_BATDRV_LOG == 1) {
        printf(  "[BATTERY] Pre-CC mode charge, timer=%d on %d !!\n\r",
        BMT_status.PRE_charging_time, BMT_status.total_charging_time);
    }

    BMT_status.PRE_charging_time += BAT_TASK_PERIOD;
	BMT_status.CC_charging_time = 0;
	BMT_status.TOPOFF_charging_time = 0;
    BMT_status.total_charging_time += BAT_TASK_PERIOD;

    if(g_HW_stop_charging == 0)
    {
        /*  Enable charger */
        pchr_turn_on_charging();
    }
    else
    {
        pchr_turn_off_charging();
    }

    if (BMT_status.bat_vol > V_PRE2CC_THRES)
    {
        BMT_status.bat_charging_state = CHR_CC;
    }

    //g_sw_cv_enable = 0;

	if (g_eco_version == PMIC6329_E1_CID_CODE)
	{
	upmu_chr_hw_cv_en(0); // RG_HWCV_EN
	}

    return PMU_STATUS_OK;
}

PMU_STATUS BAT_ConstantCurrentModeAction(void)
{
	int i=0;

    if (Enable_BATDRV_LOG == 1) {
        printf(  "[BATTERY] CC mode charge, timer=%d on %d !!\n\r",
        BMT_status.CC_charging_time, BMT_status.total_charging_time);
    }

    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time += BAT_TASK_PERIOD;
	BMT_status.TOPOFF_charging_time = 0;
    BMT_status.total_charging_time += BAT_TASK_PERIOD;

    if(g_HW_stop_charging == 0)
    {
        /*  Enable charger */
        pchr_turn_on_charging();
    }
    else
    {
        pchr_turn_off_charging();
    }

	if (g_eco_version == PMIC6329_E1_CID_CODE)
	{
		V_CC2TOPOFF_THRES = 4150;
		BMT_status.bat_vol = g_E1_vbat_sense;
		if (Enable_BATDRV_LOG == 1) {
			printf("[Charger_E1] Update g_E1_vbat_sense=%d, V_CC2TOPOFF_THRES=%d\r\n", g_E1_vbat_sense, V_CC2TOPOFF_THRES);
		}
	}

    if ( BMT_status.bat_vol > V_CC2TOPOFF_THRES )
    {
        BMT_status.bat_charging_state = CHR_TOP_OFF;

		if(g_temp_CC_value == AC_CHARGER_CURRENT)
		{
			for (i=0; i<BATTERY_AVERAGE_SIZE; i++) {
				batteryCurrentBuffer[i] = 650;
        	}
			batteryCurrentSum = 650 * BATTERY_AVERAGE_SIZE;
		}
		else
		{
			for (i=0; i<BATTERY_AVERAGE_SIZE; i++) {
				batteryCurrentBuffer[i] = 450;
        	}
			batteryCurrentSum = 450 * BATTERY_AVERAGE_SIZE;
		}
    }

    //g_sw_cv_enable = 0;

	if (g_eco_version == PMIC6329_E1_CID_CODE)
	{
	upmu_chr_hw_cv_en(0); // RG_HWCV_EN
	}

    return PMU_STATUS_OK;
}

PMU_STATUS BAT_TopOffModeAction(void)
{
    int i = 0;
    int CV_counter = 0;

    if (Enable_BATDRV_LOG == 1) {
        printf(  "[BATTERY] Top Off mode charge, timer=%d on %d !!\n\r",
        BMT_status.TOPOFF_charging_time, BMT_status.total_charging_time);
    }

    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time += BAT_TASK_PERIOD;
    BMT_status.total_charging_time += BAT_TASK_PERIOD;

    /* enable charger */
    pchr_turn_on_charging();

    //g_sw_cv_enable = 1;

	upmu_chr_hw_cv_en(1); // RG_HWCV_EN

    return PMU_STATUS_OK;
}

PMU_STATUS BAT_PostFullModeAction(void)
{
    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY] In BAT_PostFullModeAction() \n");
    }

    //g_sw_cv_enable = 0;

    return PMU_STATUS_OK;
}

int get_pmic_flag=0;

void BAT_thread(void)
{
    int BAT_status = 0;
	kal_uint32 tmp32;

    if (Enable_BATDRV_LOG == 1) {
        printf("[BATTERY] LOG. ---------------------------------------------------------------------\n");
    }

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
    if ( upmu_is_chr_det() == KAL_FALSE )
    {
        BMT_status.charger_type = CHARGER_UNKNOWN;
        BMT_status.bat_full = KAL_FALSE;
        g_bat_full_user_view = KAL_FALSE;
        g_usb_state = USB_UNCONFIGURED;

        g_HW_Charging_Done = 0;
        g_Charging_Over_Time = 0;

        printf("[BATTERY] No Charger, Power OFF !?\n");
        pchr_turn_off_charging();

        printf("[BATTERY] mt6577_power_off !!\n");
        #ifndef NO_POWER_OFF
        mt6577_power_off();
        #endif
        while(1);
    }

    /* Check Battery Status */
    BAT_status = BAT_CheckBatteryStatus();
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
        pchr_turn_off_charging();
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

        if(0)
        {
            if ( (BMT_status.bat_charging_state == CHR_TOP_OFF) &&
                (BMT_status.SOC == 100) &&
                (BMT_status.bat_vol >= Batt_VoltToPercent_Table[10].BattVolt) )
            {
                if (Enable_BATDRV_LOG == 1) {
                    printf("[BATTERY] Battery real full(%d,%d) and disable charging !\n",
                    BMT_status.SOC, Batt_VoltToPercent_Table[10].BattVolt);
                }
                BMT_status.bat_charging_state = CHR_BATFULL;
                BAT_BatteryFullAction();
                return;
            }
        }
        else
        {
            /* charging full condition when charging current < CHARGING_FULL_CURRENT mA on CHR_TOP_OFF mode*/
            if ((BMT_status.bat_charging_state == CHR_TOP_OFF)
                && (BMT_status.TOPOFF_charging_time > 60)
                && (BMT_status.ICharging <= CHARGING_FULL_CURRENT))
            {
                BMT_status.bat_charging_state = CHR_BATFULL;
                BAT_BatteryFullAction();
                printf("[BATTERY] Battery real full and disable charging on %d mA \n", g_BatteryAverageCurrent);
                return;
            }
        }

        /* Charging flow begin */
        switch(BMT_status.bat_charging_state)
        {
            case CHR_PRE :
                BAT_PreChargeModeAction();
                break;

            case CHR_CC :
                BAT_ConstantCurrentModeAction();
                break;

            case CHR_TOP_OFF :
                BAT_TopOffModeAction();
                break;

            case CHR_POST_FULL :
                BAT_PostFullModeAction();
                break;

            case CHR_BATFULL:
                BAT_BatteryFullAction();
                break;

            case CHR_ERROR:
                BAT_BatteryStatusFailAction();
                break;

            default :
                if (Enable_BATDRV_LOG == 1) {
                    printf("BMT_status.bat_charging_state ??\n");
                }
                break;
        }
    }

    g_SW_CHR_OUT_EN = 1;

}

#ifdef MTK_NCP1851_SUPPORT
///////////////////////////////////////////////////////////////////////////////////////////
//// ncp1851
///////////////////////////////////////////////////////////////////////////////////////////
void pmu_init_for_ncp1851(void)
{
	printf("[pmu_init_for_ncp1851] Start\n");

	upmu_chr_chrwdt_int_en(0);				// CHRWDT_INT_EN
	upmu_chr_chrwdt_en(0); 					// CHRWDT_EN
	upmu_chr_chrwdt_flag_wr(0);				// CHRWDT_FLAG
	upmu_chr_csdac_enable(0);				// CSDAC_EN
	upmu_chr_enable(0);						// CHR_EN
	upmu_chr_hw_cv_en(0);					// RG_HWCV_EN

	upmu_chr_vcdt_mode(0x0);			//VCDT_MODE
	upmu_chr_vcdt_hv_enable(0x1); 	//VCDT_HV_EN
	upmu_chr_vcdt_hv_vth(0xD);         // VCDT_HV_VTH 8.5V
	upmu_chr_bc11_rst(0x1);			//BC11_RST

	printf("[pmu_init_for_ncp1851] Done\n");
}

void BAT_thread_ncp1851(void)
{
    int BAT_status = 0;
    kal_uint32 ncp1851_status=0;
    kal_uint32 tmp32;
    int i = 0;
    int chr_err_cnt = 0;

    printf("[BAT_thread_ncp1851] mtk_wdt_restart()\n");
    mtk_wdt_restart();

    if (Enable_BATDRV_LOG == 1) {
        printf("[BAT_thread_ncp1851] LOG. %d,%d----------------------------\n", BATTERY_AVERAGE_SIZE, RECHARGING_VOLTAGE);
    }

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
            printf("[Battery:ncp1851] Get PMIC version > E1\n");
        }

        get_pmic_flag = 1;
    }

    /* If charger does not exist */
    if ( upmu_is_chr_det() == KAL_FALSE )
    {
        BMT_status.charger_type = CHARGER_UNKNOWN;
        BMT_status.bat_full = KAL_FALSE;
        g_bat_full_user_view = KAL_FALSE;
        g_usb_state = USB_UNCONFIGURED;

        g_HW_Charging_Done = 0;
        g_Charging_Over_Time = 0;

        printf("[BATTERY:ncp1851] No Charger, Power OFF !?\n");
        pchr_turn_off_charging_ncp1851();

        printf("[BATTERY:ncp1851] mt6577_power_off !!\n");
	#ifndef NO_POWER_OFF
        mt6577_power_off();
	#endif
        while(1);
    }

    /* Check Battery Status */
    BAT_status = BAT_CheckBatteryStatus_ncp1851();

    if(BMT_status.bat_charging_state == CHR_ERROR)
    {
        chr_err_cnt = 600; //10mins debounce
    }
    else if(chr_err_cnt > 0)
    {
        chr_err_cnt --;
    }
	
    if( BAT_status == PMU_STATUS_FAIL )
        g_Battery_Fail = KAL_TRUE;
    else if( chr_err_cnt > 0 )
        g_Battery_Fail = KAL_TRUE;
    else
        g_Battery_Fail = KAL_FALSE;

    /* No Charger */
    if(BAT_status == PMU_STATUS_FAIL || g_Battery_Fail)
    {
        BMT_status.total_charging_time = 0;
        BMT_status.PRE_charging_time = 0;
        BMT_status.CC_charging_time = 0;
        BMT_status.TOPOFF_charging_time = 0;
        BMT_status.POSTFULL_charging_time = 0;

        pchr_turn_off_charging_ncp1851();
        //Set SPM = 0, for lenovo platform, GPIO83
        mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO83, GPIO_OUT_ZERO);

        if (Enable_BATDRV_LOG == 1)
        {
            printf("[BATTERY:ncp1851] PMU_STATUS_FAIL (chr_err_cnt = %d)\n", chr_err_cnt);
        }
    }
    /* Battery Full */
    /* HW charging done, real stop charging */
    else if (g_HW_Charging_Done == 1)
    {
        if (Enable_BATDRV_LOG == 1)
        {
            printf("[BATTERY:ncp1851] Battery real full. \n");
        }
        BMT_status.bat_full = KAL_TRUE;
        BMT_status.total_charging_time = 0;
        BMT_status.PRE_charging_time = 0;
        BMT_status.CC_charging_time = 0;
        BMT_status.TOPOFF_charging_time = 0;
        BMT_status.POSTFULL_charging_time = 0;
        g_HW_Charging_Done = 1;
#ifdef NCP1851_PWR_PATH
        pchr_turn_on_charging_ncp1851();//do not turn off charging because NCP1851 has a power path management feature
#else
        pchr_turn_off_charging_ncp1851();
#endif
        //Set SPM = 1, for lenovo platform, GPIO83
        mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO83, GPIO_OUT_ONE);
    }

    /* Charging Overtime, can not charging (not for total charging timer) */
    else if (g_Charging_Over_Time == 1)
    {
        if (Enable_BATDRV_LOG == 1)
        {
            printf("[BATTERY:ncp1851] Charging Over Time. \n");
        }
        pchr_turn_off_charging_ncp1851();
        //Set SPM = 0, for lenovo platform, GPIO83
        mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO83, GPIO_OUT_ZERO);
    }

    /* Battery Not Full and Charger exist : Do Charging */
    else
    {
        if (Enable_BATDRV_LOG == 1)
        {
            printf("[BATTERY:ncp1851] state=%ld, \n", BMT_status.bat_charging_state);
        }

        /* Charging OT */
        if(BMT_status.total_charging_time >= MAX_CHARGING_TIME)
        {
            BMT_status.total_charging_time = 0;
            BMT_status.PRE_charging_time = 0;
            BMT_status.CC_charging_time = 0;
            BMT_status.TOPOFF_charging_time = 0;
            BMT_status.POSTFULL_charging_time = 0;
            g_HW_Charging_Done = 1;
	     g_Charging_Over_Time = 1;
            pchr_turn_off_charging_ncp1851();
            //Set SPM = 0, for lenovo platform, GPIO83
            mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
            mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO83, GPIO_OUT_ZERO);
            return;
        }

        ncp1851_status = ncp1851_get_chip_status();

        /* check battery full */
        if((BMT_status.bat_vol > RECHARGING_VOLTAGE) && ((ncp1851_status == 0x6) || (ncp1851_status == 0x7))) //TODO: monitor charge done, check if need to monitor DPP
        {
            BMT_status.bat_charging_state = CHR_BATFULL;
            BMT_status.bat_full = KAL_TRUE;
            BMT_status.total_charging_time = 0;
            BMT_status.PRE_charging_time = 0;
            BMT_status.CC_charging_time = 0;
            BMT_status.TOPOFF_charging_time = 0;
            BMT_status.POSTFULL_charging_time = 0;
            g_HW_Charging_Done = 1;
#ifdef NCP1851_PWR_PATH
            pchr_turn_on_charging_ncp1851(); //do not turn off charging because NCP1851 has a power path management feature
#else
            pchr_turn_off_charging_ncp1851();
#endif
            printf("[BATTERY:ncp1851] Battery real full and enable ncp1851 powerpath management (%ld) \n", ncp1851_status);
            //Set SPM = 1, for lenovo platform, GPIO83
            mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
            mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO83, GPIO_OUT_ONE);
            return;
        }
        else if((ncp1851_status == 0x6) || (ncp1851_status == 0x7)) //false alarm, reset status
        {
            ncp1851_set_chg_en(0);
            mt6577_sleep(1000, KAL_FALSE);

            if (Enable_BATDRV_LOG == 1) {
                printf("[BATTERY:ncp1851] Ncp1851_status is (%ld), but battery voltage not over recharge voltage\n", ncp1851_status);
            }
        }

        /* Charging flow begin */
        BMT_status.total_charging_time += BAT_TASK_PERIOD;
        BMT_status.bat_charging_state = CHR_CC;
        pchr_turn_on_charging_ncp1851();
        //Set SPM = 1, for lenovo platform, GPIO83
        mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO83, GPIO_OUT_ONE);

        if (Enable_BATDRV_LOG == 1)
        {
            printf(  "[BATTERY:ncp1851] Total charging timer=%ld, ncp1851_status=%ld \n\r",
            BMT_status.total_charging_time, ncp1851_status);
        }
    }

    g_SW_CHR_OUT_EN = 1;
    g_HW_stop_charging = 0;

    if (Enable_BATDRV_LOG == 1)
    {
        ncp1851_dump_register();
    }

    if (Enable_BATDRV_LOG == 1)
    {
        printf("[BAT_thread_ncp1851] End ....\n");
    }
}
#endif

void batdrv_init(void)
{
    int i = 0;
#ifdef MTK_NCP1851_SUPPORT
    int ret = 0;
#endif
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

	upmu_chr_baton_tdet_en(0);
#ifndef MTK_NCP1851_SUPPORT
	if ((upmu_is_chr_det() == KAL_TRUE))
	{
		mdelay(500);
		upmu_chr_csdac_mode(1);	//CSDAC_MODE
		pchr_turn_on_charging();
		printf("[BATTERY] pchr_turn_on_charging : first !\n");
	}

	upmu_chr_vcdt_hv_vth(0xB);		//VCDT_HV_VTH, 7V
#else
       /* init i2c0 interface*/
       ret = i2c0_v1_init();
       printf("Init I2C0: %s(%d)\n", ret ? "FAIL" : "OK", ret);

       if ((upmu_is_chr_det() == KAL_TRUE))
       {
           printf("ncp1851 hw init\n");
           ncp1851_dump_register();
           pmu_init_for_ncp1851();
           pchr_turn_on_charging_ncp1851();
       }

       upmu_chr_vcdt_hv_vth(0xD);	   //VCDT_HV_VTH, 8.5V
#endif

    printf("[BATTERY] batdrv_init : Done\n");
}

void check_point_sync_leds(void)
{
    int battery_level = BattVoltToPercent(BMT_status.bat_vol);
    printf("[BATTERY] %s  battery_level = %d \n", __func__, battery_level);
    
    if(battery_level >= 90)   //Full ARGB
    {
        leds_battery_full_charging();
    }
    else  //Low and Medium ARGB
    {
        leds_battery_medium_charging();
    }
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

    sc_mod_init();
    batdrv_init();

    BMT_status.bat_full = FALSE;
#ifdef MTK_NCP1851_SUPPORT
    BMT_status.bat_vol = PMIC_IMM_GetBatChannelValue(5);
    if((BMT_status.bat_vol > RECHARGING_VOLTAGE) && (upmu_is_chr_det() == KAL_TRUE))
    {
        if(Enable_BATDRV_LOG == 1)
            printf("battery voltage during charging (%ld) > %ld\n", BMT_status.bat_vol, RECHARGING_VOLTAGE);
        pchr_turn_off_charging_ncp1851(); //turn off charging at first to prevent initial voltage measure reach 4.2 when pushing reset button and adaptor plugin
        //Set SPM = 0, for lenovo platform, GPIO83
        mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO83, GPIO_OUT_ZERO);
        BMT_status.bat_vol = PMIC_IMM_GetBatChannelValue(5);
    }
#if 1
    else if((BMT_status.bat_vol > 3450) && (upmu_is_chr_det() == KAL_TRUE))
    {
        CHR_Type_num = mt_charger_type_detection();
        BMT_status.charger_type = CHR_Type_num;

        if(BMT_status.charger_type != STANDARD_HOST)
        {
            pchr_turn_off_charging_ncp1851(); //turn off charging at first to prevent initial voltage measure reach 4.2 when pushing reset button and adaptor plugin
            //Set SPM = 0, for lenovo platform, GPIO83
            mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
            mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO83, GPIO_OUT_ZERO);
            printf("initial battery voltage (charging) is %ld\n", BMT_status.bat_vol);
            mdelay(50);
            pchr_turn_on_charging_ncp1851();
        }
    }
#endif
#else
	BAT_GetVoltage_notbat();
	BAT_GetVoltage_notbat();
#endif

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

#ifdef MTK_NCP1851_SUPPORT
        pchr_turn_on_charging_ncp1851(); //turn on charging for powerpath
        //Set SPM = 1, for lenovo platform, GPIO83
        mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO83, GPIO_OUT_ONE);
#endif

        while (1)
        {
            if (rtc_boot_check(true) || meta_mode_check() || (pwrkey_ready == true) 
            	|| mtk_wdt_boot_check()==WDT_BY_PASS_PWK_REBOOT)
            {
#ifdef MTK_NCP1851_SUPPORT
                // Low Battery Safety Booting
                BMT_status.bat_vol = PMIC_IMM_GetBatChannelValue(1); //we do not turn off charging because of power path support, the battery voltage may under UVLO
                printf("check VBAT during charging =%d mV with %d mV\n", BMT_status.bat_vol, BATTERY_LOWVOL_THRESOLD);
#else
                // Low Battery Safety Booting
                pchr_turn_off_charging();
                BMT_status.bat_vol = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
                printf("check VBAT=%d mV with %d mV\n", BMT_status.bat_vol, BATTERY_LOWVOL_THRESOLD);
                pchr_turn_on_charging();
#endif
#if 1
                while ( BMT_status.bat_vol < BATTERY_LOWVOL_THRESOLD )
                {
#ifdef MTK_NCP1851_SUPPORT
                    if(BMT_status.charger_type != STANDARD_HOST)
                        break;
#endif
                    if (low_bat_boot_display == 0)
                    {
                        mt65xx_disp_power(TRUE);
                        mt65xx_backlight_off();
                        printf("Before mt6516_disp_show_low_battery\r\n");
                        mt65xx_disp_show_low_battery();
                        printf("After mt6516_disp_show_low_battery\r\n");
                        mt65xx_disp_wait_idle();
                        printf("After mt6516_disp_wait_idle\r\n");
#ifdef MTK_NCP1851_SUPPORT
                        bl_switch = KAL_FALSE;
                        bl_switch_timer = 0;
#endif
                        low_bat_boot_display = 1;

                        printf("Set low brightness\r\n");
                        mt65xx_leds_brightness_set(6, 20);
                    }

                    rtc_boot_check(false);
#ifdef MTK_NCP1851_SUPPORT
                    BAT_thread_ncp1851();
#else
                    BAT_thread();
#endif
                    printf("-");

                    #ifdef GPT_TIMER
                        if (g_bl_on == KAL_TRUE)
                            mt6577_sleep(1000, KAL_FALSE);
                        else
#ifdef MTK_NCP1851_SUPPORT
                            mt6577_sleep(1000, KAL_FALSE);
#else
                            mt6577_sleep(1000, KAL_TRUE);
#endif
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

#ifdef MTK_NCP1851_SUPPORT
                    BMT_status.bat_vol = PMIC_IMM_GetBatChannelValue(1); //we do not turn off charging because of power path support, the battery voltage may under UVLO
                    if(BMT_status.bat_vol < BATTERY_LOWVOL_THRESOLD)
                        printf("VBAT=%d < %d\n", BMT_status.bat_vol, BATTERY_LOWVOL_THRESOLD);
#else
                    pchr_turn_off_charging();
                    BMT_status.bat_vol = PMIC_IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL,1);
                    printf("VBAT=%d < %d\n", BMT_status.bat_vol, BATTERY_LOWVOL_THRESOLD);
                    pchr_turn_on_charging();
#endif
                }

                if(back_to_charging_animation_flag == false)
                {
                    mt65xx_disp_power(TRUE);
    
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
                    check_point_sync_leds();					
                    mt65xx_backlight_on();
#endif

#ifdef MTK_NCP1851_SUPPORT
                pchr_turn_on_charging_ncp1851(); //turn on charging for powerpath
                //Set SPM = 1, for lenovo platform, GPIO83
                mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
                mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
                mt_set_gpio_out(GPIO83, GPIO_OUT_ONE);
#else
                pchr_turn_off_charging();
#endif
                sc_mod_exit();
                return;
            }
                back_to_charging_animation_flag = false;
                //  pwrkey_ready = false;
                low_bat_boot_display = 0;
                
                //   bl_switch = KAL_FALSE;
                // bl_switch_timer = 0;
                // g_bl_on = KAL_TRUE;				
            }
            else
            {
                //printf("[BATTERY] U-BOOT Charging !! \n\r");
            }

            if (g_thread_count >= 5)
            {
                g_thread_count = 1;
#ifdef MTK_NCP1851_SUPPORT
                BAT_thread_ncp1851();
#else
                BAT_thread();
#endif
                printf(".");
            }
            else
            {
                g_thread_count++;
            }

            if(print_msg==FALSE)
            {
                if((BMT_status.bat_vol < BATTERY_LOWVOL_THRESOLD) && ((BMT_status.total_charging_time < 2) || (BMT_status.charger_type == STANDARD_HOST)) && (g_uboot_anime_on != 1))
                {
                    BAT_UbootAnimationControl(KAL_FALSE);
                }
                else
                {
                    g_uboot_anime_on = 1;
                    BAT_UbootAnimationControl(KAL_TRUE);
                }            
                printf("[BATTERY] Charging !! Press Power Key to Booting !!! \n\r");
                print_msg = TRUE;
            }

            #ifdef GPT_TIMER
                if (g_bl_on == KAL_TRUE)
                    mt6577_sleep(200, KAL_FALSE);
                else
#ifdef MTK_NCP1851_SUPPORT
                    mt6577_sleep(200, KAL_FALSE);
#else
                    mt6577_sleep(200, KAL_TRUE);
#endif
            #else
                tmo = get_timer(0);
                while(get_timer(tmo) <= 200 /* ms */);
            #endif

            if (loop_count++ == 60) loop_count = 0;
        			
            if (mt6577_detect_key(BACKLIGHT_KEY) || (!mt6577_detect_powerkey() && press_pwrkey_count > 0))
            {
                bl_switch = KAL_FALSE;
                bl_switch_timer = 0;
                g_bl_on = 1;
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
            				            				
            if (((loop_count % 5) == 0) && bl_switch == false) // update charging screen every 1s (200ms * 5)
            {
                if (Enable_BATDRV_LOG == 1)
                {
                    printf("[BATTERY] loop_count = %d\n", loop_count);
                }
                if((BMT_status.bat_vol < BATTERY_LOWVOL_THRESOLD) && ((BMT_status.total_charging_time < 2) || (BMT_status.charger_type == STANDARD_HOST)) && (g_uboot_anime_on != 1))
                {
                    BAT_UbootAnimationControl(KAL_FALSE);
                }
                else
                {
                    g_uboot_anime_on = 1;
                    BAT_UbootAnimationControl(KAL_TRUE);
                }            
            }  
        }
    }
    else
    {
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
#ifdef MTK_NCP1851_SUPPORT
            pchr_turn_off_charging_ncp1851();
            //Set SPM = 0, for lenovo platform, GPIO83
            mt_set_gpio_mode(GPIO83, GPIO_MODE_00);
            mt_set_gpio_dir(GPIO83, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO83, GPIO_OUT_ZERO);
#else
            pchr_turn_off_charging();
#endif
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
