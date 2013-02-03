/*****************************************************************************
 *
 * Filename:
 * ---------
 *    mt6577_auxadc.c
 *
 * Project:
 * --------
 *   Android_Software
 *
 * Description:
 * ------------
 *   This Module defines functions of mt6577 AUXADC
 *
 * Author:
 * -------
 * James Lo
 *
 ****************************************************************************/
 
#include <linux/init.h>        /* For init/exit macros */
#include <linux/module.h>      /* For MODULE_ marcros  */
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <mach/mt6577_auxadc_sw.h>
#include <mach/mt6577_auxadc_hw.h>

#include <mach/hardware.h>
#include <mach/mt6577_gpt.h>
#include "mach/mt6577_clock_manager.h"

#include <cust_adc.h> // generate by DCT Tool

//#define AUXADC_BATTERY_VOLTAGE_CHANNEL     0
//#define AUXADC_REF_CURRENT_CHANNEL     1
//#define AUXADC_CHARGER_VOLTAGE_CHANNEL     2
//#define AUXADC_TEMPERATURE_CHANNEL     3


//#include <cust_battery.h>

//#include "pmu6577_sw.h"
//#include "upmu_sw.h"


// Define
//typedef unsigned short  kal_uint16;

//#define DRV_Reg(addr)               (*(volatile kal_uint16 *)(addr))
//#define DRV_WriteReg(addr,data)     ((*(volatile kal_uint16 *)(addr)) = (kal_uint16)(data))

#define DRV_ClearBits(addr,data)     {\
   kal_uint16 temp;\
   temp = DRV_Reg(addr);\
   temp &=~(data);\
   DRV_WriteReg(addr,temp);\
}

#define DRV_SetBits(addr,data)     {\
   kal_uint16 temp;\
   temp = DRV_Reg(addr);\
   temp |= (data);\
   DRV_WriteReg(addr,temp);\
}

#define DRV_SetData(addr, bitmask, value)     {\
   kal_uint16 temp;\
   temp = (~(bitmask)) & DRV_Reg(addr);\
   temp |= (value);\
   DRV_WriteReg(addr,temp);\
}

#define AUXADC_DRV_ClearBits16(addr, data)           DRV_ClearBits(addr,data)
#define AUXADC_DRV_SetBits16(addr, data)             DRV_SetBits(addr,data)
#define AUXADC_DRV_WriteReg16(addr, data)            DRV_WriteReg(addr, data)
#define AUXADC_DRV_ReadReg16(addr)                   DRV_Reg(addr)
#define AUXADC_DRV_SetData16(addr, bitmask, value)   DRV_SetData(addr, bitmask, value)

#define AUXADC_DVT_DELAYMACRO(u4Num)                                     \
{                                                                        \
    unsigned int u4Count = 0 ;                                           \
    for (u4Count = 0; u4Count < u4Num; u4Count++ );                      \
}

#define AUXADC_SET_BITS(BS,REG)       ((*(volatile u32*)(REG)) |= (u32)(BS))
#define AUXADC_CLR_BITS(BS,REG)       ((*(volatile u32*)(REG)) &= ~((u32)(BS)))

#define VOLTAGE_FULL_RANGE  2500 // VA voltage
#define AUXADC_PRECISE      4096 // 12 bits

/*****************************************************************************
 * Integrate with NVRAM 
****************************************************************************/
#define AUXADC_CALI_DEVNAME    "mtk-adc-cali"
#define AUXADC_CALI_MAJOR      0

#define TEST_ADC_CALI_PRINT _IO('k', 0)
#define SET_ADC_CALI_Slop   _IOW('k', 1, int)
#define SET_ADC_CALI_Offset _IOW('k', 2, int)
#define SET_ADC_CALI_Cal    _IOW('k', 3, int)
#define ADC_CHANNEL_READ    _IOW('k', 4, int)

#define R_BAT_SENSE 2		// times of voltage
#define R_I_SENSE 2			// times of voltage
#define R_CHARGER_1 330
#define R_CHARGER_2 39

static DEFINE_MUTEX(auxadc_mutex);

int auxadc_cali_slop[14]   = {1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000};
int auxadc_cali_offset[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int auxadc_cali_cal[1]     = {0};

int auxadc_in_data[2]  = {1,1};
int auxadc_out_data[2] = {1,1};

static int adc_auto_set =0;
kal_bool g_AUXADC_Cali = KAL_FALSE;

static dev_t auxadc_cali_devno;
static int auxadc_cali_major = 0;
static struct cdev *auxadc_cali_cdev;
static struct class *auxadc_cali_class = NULL;

#if 0
extern void upmu_adc_measure_vbat_enable(upmu_chr_list_enum chr, kal_bool enable);
extern void upmu_adc_measure_vsen_enable(upmu_chr_list_enum chr, kal_bool enable);
extern void upmu_adc_measure_vchr_enable(upmu_chr_list_enum chr, kal_bool enable);
#endif

int auxadc_debug = 0;
module_param(auxadc_debug, int, 00664);

///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
//// Internal API
inline static void mt6577_ADC_2G_power_up(void)
{
    //2010/07/27: mt6577, the ADC 2G power on is controlled by APMCU_CG_CLR0
    #define PDN_CLR0 (0xF7026308)  
    unsigned int poweron = 1 << 4;
    AUXADC_SET_BITS(poweron, PDN_CLR0);
}

inline static void mt6577_ADC_2G_power_down(void)
{
    //2010/07/27: mt6577, the ADC 2G power on is controlled by APMCU_CG_SET0
    #define PDN_SET0 (0xF7026304)  
    unsigned int poweroff = 1 << 4;
    AUXADC_SET_BITS(poweroff, PDN_SET0);
}

inline static void mt6577_ADC_3G_power_up(void)
{
    //2010/07/27: mt6577, the ADC 3G power on is controlled by APMCU_CG_CLR0
    #define PDN_CLR0 (0xF7026308)  
    unsigned int poweron = 1 << 13;
    AUXADC_SET_BITS(poweron, PDN_CLR0);
}

inline static void mt6577_ADC_3G_power_down(void)
{
    //2010/07/27: mt6577, the ADC 3G power on is controlled by APMCU_CG_SET0
    #define PDN_SET0 (0xF7026304)  
    unsigned int poweroff = 1 << 13;
    AUXADC_SET_BITS(poweroff, PDN_SET0);
}
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
//// Common API
static int g_adc_init_flag =0;

#define ADC_CHANNEL_MAX 14
//#define  CUST_ADC_MD_CHANNEL 1
typedef struct adc_info
{
   char channel_name[64];
   int channel_number;
   int reserve1;
   int reserve2;
   int reserve3;
}ADC_INFO;

ADC_INFO g_adc_info[ADC_CHANNEL_MAX];



int IMM_get_adc_channel_num(char *channel_name, int len)
{
  unsigned int i;

  printk("[ADC] name = %s\n", channel_name);
  printk("[ADC] name_len = %d\n", len);
  for(i=0; i<ADC_CHANNEL_MAX; i++)
  {
    if (!strncmp(channel_name, g_adc_info[i].channel_name, len))
    {
      return g_adc_info[i].channel_number;
    }
  }
  printk("[ADC] find channel number failed\n");
  return -1;
}


int adc_channel_info_init(void)
{
   // unsigned int ap_domain = 0;
	unsigned int used_channel_counter = 0;
	used_channel_counter = 0;
	#ifdef AUXADC_TEMPERATURE_CHANNEL
    //ap_domain &= ~(1<<CUST_ADC_MD_CHANNEL);
    sprintf(g_adc_info[used_channel_counter].channel_name, "ADC_RFTMP");
    g_adc_info[used_channel_counter].channel_number = AUXADC_TEMPERATURE_CHANNEL;
	printk("[ADC] channel_name = %s channel num=%d\n", g_adc_info[used_channel_counter].channel_name
		,g_adc_info[used_channel_counter].channel_number);
    used_channel_counter++;
	#endif

	#ifdef AUXADC_ADC_FDD_RF_PARAMS_DYNAMIC_CUSTOM_CH_CHANNEL
	sprintf(g_adc_info[used_channel_counter].channel_name, "ADC_FDD_Rf_Params_Dynamic_Custom");
    g_adc_info[used_channel_counter].channel_number = AUXADC_ADC_FDD_RF_PARAMS_DYNAMIC_CUSTOM_CH_CHANNEL;
	printk("[ADC] channel_name = %s channel num=%d\n", g_adc_info[used_channel_counter].channel_name
		,g_adc_info[used_channel_counter].channel_number);
    used_channel_counter++;
	#endif
	
	return 0;

}

int IMM_IsAdcInitReady(void)
{
  return g_adc_init_flag;
}

int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata)
{
   unsigned int channel[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   int idle_count =0;
   int data_ready_count=0;
 //  unsigned int i = 0, data = 0;
 //  unsigned int poweron, poweroff;

   // Polling until bit STA = 0
   while ((*(volatile u16 *)AUXADC_CON3) & 0x01) 
   {
       printk("[adc_api]: wait for module idle\n");
       msleep(100);
	   idle_count++;
	   if(idle_count>30)
	   {
	      //wait for idle time out
	      printk("[adc_api]: wait for aux/adc idle time out\n");
	      return -1;
	   }
   } 

   
   while ((*(volatile u16 *)(AUXADC_DAT0 + dwChannel * 0x04)) & (1<<12)) 
   {
       printk("[adc_api]: wait for channel[%d] data ready\n",dwChannel);
       msleep(100);
	   data_ready_count++;
	   if(data_ready_count>30)
	   {
	      //wait for idle time out
	      printk("[adc_api]: wait for channel[%d] data ready time out\n",dwChannel);
	      return -2;
	   }
   }
       
   //read data
  
   if(0==adc_auto_set)
   {
      //clear bit
	  *(volatile u16 *)AUXADC_CON1 = *(volatile u16 *)AUXADC_CON1 & (~(1 << dwChannel));
	  msleep(20);
	  //set bit
	  *(volatile u16 *)AUXADC_CON1 = *(volatile u16 *)AUXADC_CON1 | (1 << dwChannel);
   }
   msleep(20);
   //read data
   
   channel[dwChannel] = (*(volatile u16 *)(AUXADC_DAT0 + dwChannel * 0x04)) & 0x0FFF;
   if(NULL != rawdata)
   {
      *rawdata = channel[dwChannel];
   }
   //printk("[adc_api: imm mode raw data => channel[%d] = %d\n",dwChannel, channel[dwChannel]);
   //printk("[adc_api]: imm mode => channel[%d] = %d.%d\n", dwChannel, (channel[dwChannel] * 250 / 4096 / 100), ((channel[dwChannel] * 250 / 4096) % 100));
   data[0] = (channel[dwChannel] * 250 / 4096 / 100);
   data[1] = ((channel[dwChannel] * 250 / 4096) % 100);
   
   msleep(20);
   if(0 == adc_auto_set)
   {
	   //clear bit
	   *(volatile u16 *)AUXADC_CON1 = *(volatile u16 *)AUXADC_CON1 & (~(1 << dwChannel));
   }
	 
   return 0;
   
}

#if 0
int IMM_GetOneChannelValue(int dwChannel, int deCount)
{
    unsigned int u4Sample_times = 0;
    unsigned int dat = 0;
    unsigned int u4channel[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    unsigned int adc_result = 0;
    unsigned int adc_temp_result = 0;
	unsigned int reg_val = 0;
	unsigned int reg_val_check = 0;
    
    /* Enable ADC power bit */    
    mt6577_ADC_2G_power_up();
    mt6577_ADC_3G_power_up();
    
    /* Initialize ADC control register */
    AUXADC_DRV_WriteReg16(AUXADC_CON0, 0);
    AUXADC_DRV_WriteReg16(AUXADC_CON1, 0);    
    AUXADC_DRV_WriteReg16(AUXADC_CON2, 0);    
    AUXADC_DRV_WriteReg16(AUXADC_CON3, 0);   
    
    do
    {
        //pmic_adc_vbat_enable(KAL_TRUE);		// move to the whom driver
        //pmic_adc_isense_enable(KAL_TRUE); 	// move to the whom driver

		//printk("Read : REG[0xF702FFD4]=%x\r\n", INREG16(0xF702FFD4));
		//printk("Read : REG[0xF702FE00]=%x\r\n", INREG16(0xF702FE00));

		//printk("Before Set : REG[0xF702FA20]=%x\r\n", INREG16(0xF702FA20));
		//DRV_WriteReg16(0xF702FA20, 0x7000);
		*((volatile unsigned int *) 0xF702FA20) = 0x7000 | (*((volatile unsigned int *) 0xF702FA20));
		//printk("After Set : REG[0xF702FA20]=%x\r\n", INREG16(0xF702FA20));
		
        AUXADC_DRV_WriteReg16(AUXADC_CON1, 0);        
        AUXADC_DRV_WriteReg16(AUXADC_CON1, 0x1FFF);
        
        AUXADC_DVT_DELAYMACRO(1000);

		//printk("Before BUSY : REG[0xF702FA20]=%x\r\n", INREG16(0xF702FA20));
		//printk("Before BUSY : REG[0xF702FFE0]=%x\r\n", INREG16(0xF702FFE0));
		
        /* Polling until bit STA = 0 */
        while (0 != (0x01 & AUXADC_DRV_ReadReg16(AUXADC_CON3)));    

		//printk("After BUSY : REG[0xF702FA20]=%x\r\n", INREG16(0xF702FA20));
		//printk("After BUSY : REG[0xF702FFE0]=%x\r\n", INREG16(0xF702FFE0));

		reg_val = INREG16(0xF702FA20);
		reg_val_check = (reg_val >> 12);

		if(reg_val_check != 0x0)		
		{
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT0);        
	        u4channel[0]  += (dat & 0x0FFF);
			//printk("[%d] AUXADC_DAT0=%d, ", u4Sample_times, dat);
			//printk("[%d] AUXADC_DAT0=%d\r\n", u4Sample_times, dat);
			
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT1);        
	        u4channel[1]  += (dat & 0x0FFF);   
			//printk("[%d] AUXADC_DAT1=%d, ", u4Sample_times, dat);
			
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT2);        
	        u4channel[2]  += (dat & 0x0FFF);
			//printk("[%d] AUXADC_DAT2=%d, ", u4Sample_times, dat);
			
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT3);        
	        u4channel[3]  += (dat & 0x0FFF);
			//printk("[%d] AUXADC_DAT3=%d\n", u4Sample_times, dat);
			
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT4);
	        u4channel[4]  += (dat & 0x0FFF);
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT5);
	        u4channel[5]  += (dat & 0x0FFF);
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT6);
	        u4channel[6]  += (dat & 0x0FFF);  
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT7);
	        u4channel[7]  += (dat & 0x0FFF);
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT8);
	        u4channel[8]  += (dat & 0x0FFF);    
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT9);
	        u4channel[9]  += (dat & 0x0FFF);
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT10);
	        u4channel[10] += (dat & 0x0FFF);
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT11);
	        u4channel[11] += (dat & 0x0FFF);
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT12);
	        u4channel[12] += (dat & 0x0FFF);		
	        dat = AUXADC_DRV_ReadReg16(AUXADC_DAT13);
	        u4channel[13] += (dat & 0x0FFF);
	        
	        u4Sample_times++;
		}
		else
		{
			printk("Oops! REG[0xF702FA20]=%d, ignore the value, try again\n", reg_val);			
		}
    } while (u4Sample_times < deCount);
    
    /* Disable ADC power bit */    
    mt6577_ADC_2G_power_down();
    mt6577_ADC_3G_power_down();
    
    #if 0
    printk("BAT_GetVoltage : channel_0  = %d / %d \n", u4channel[0], u4Sample_times);
    printk("BAT_GetVoltage : channel_1  = %d / %d \n", u4channel[1], u4Sample_times);
    printk("BAT_GetVoltage : channel_2  = %d / %d \n", u4channel[2], u4Sample_times);
    printk("BAT_GetVoltage : channel_3  = %d / %d \n", u4channel[3], u4Sample_times);
    printk("BAT_GetVoltage : channel_4  = %d / %d \n", u4channel[4], u4Sample_times);
    printk("BAT_GetVoltage : channel_5  = %d / %d \n", u4channel[5], u4Sample_times);
    printk("BAT_GetVoltage : channel_6  = %d / %d \n", u4channel[6], u4Sample_times);
    printk("BAT_GetVoltage : channel_7  = %d / %d \n", u4channel[7], u4Sample_times);
    printk("BAT_GetVoltage : channel_8  = %d / %d \n", u4channel[8], u4Sample_times);
    printk("BAT_GetVoltage : channel_9  = %d / %d \n", u4channel[9], u4Sample_times);
    printk("BAT_GetVoltage : channel_10 = %d / %d \n", u4channel[10], u4Sample_times);
    printk("BAT_GetVoltage : channel_11 = %d / %d \n", u4channel[11], u4Sample_times);
    printk("BAT_GetVoltage : channel_12 = %d / %d \n", u4channel[12], u4Sample_times);
    printk("BAT_GetVoltage : channel_13 = %d / %d \n", u4channel[13], u4Sample_times);	
    #endif
    
    /* Value averaging  */ 
    u4channel[0]  = u4channel[0]  / deCount;
    u4channel[1]  = u4channel[1]  / deCount;
    u4channel[2]  = u4channel[2]  / deCount;
    u4channel[3]  = u4channel[3]  / deCount;
    u4channel[4]  = u4channel[4]  / deCount;
    u4channel[5]  = u4channel[5]  / deCount;
    u4channel[6]  = u4channel[6]  / deCount;
    u4channel[7]  = u4channel[7]  / deCount;
    u4channel[8]  = u4channel[8]  / deCount;
    u4channel[9]  = u4channel[9]  / deCount;
    u4channel[10] = u4channel[10] / deCount;
    u4channel[11] = u4channel[11] / deCount;
    u4channel[12] = u4channel[12] / deCount;
    u4channel[13] = u4channel[13] / deCount;
    
    adc_temp_result = ((u4channel[dwChannel] * VOLTAGE_FULL_RANGE) / AUXADC_PRECISE);
    
    if (g_AUXADC_Cali) {
        adc_result = ((adc_temp_result * (*(auxadc_cali_slop + dwChannel))) + (*(auxadc_cali_offset + dwChannel))) / 1000;
    } else {
        adc_result = adc_temp_result;
    }
    
    if (auxadc_debug) {
        printk("mt6577_auxadc: auxadc_cali_slop[%d] = %d, auxadc_cali_offset[%d] = %d\n", dwChannel, *(auxadc_cali_slop + dwChannel), dwChannel, *(auxadc_cali_offset + dwChannel));
        printk("mt6577_auxadc: adc_temp_result = %d, adc_result = %d\n", adc_temp_result, adc_result);  
    }
        
    return adc_result;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////
//// fop API 
///////////////////////////////////////////////////////////////////////////////////////////
static long auxadc_cali_unlocked_ioctl(struct file *file, unsigned int cmd,unsigned long arg)
{
    int i = 0, ret = 0;
    int *user_data_addr;
    int *nvram_data_addr;
    
    mutex_lock(&auxadc_mutex);

    switch(cmd)
    {
        case TEST_ADC_CALI_PRINT :
            g_AUXADC_Cali = KAL_FALSE;
            break;
        
        case SET_ADC_CALI_Slop:            
            nvram_data_addr = (int *)arg;
            ret = copy_from_user(auxadc_cali_slop, nvram_data_addr, 36);
            g_AUXADC_Cali = KAL_FALSE;           
            /* Protection */
            for (i = 0; i < 14; i++) 
            { 
                if ((*(auxadc_cali_slop + i) == 0) || (*(auxadc_cali_slop + i) == 1)) {
                    *(auxadc_cali_slop + i) = 1000;
                }
            }
            for (i = 0; i < 14; i++) printk("auxadc_cali_slop[%d] = %d\n", i, *(auxadc_cali_slop+i));
            printk("**** MT6573 auxadc_cali ioctl : SET_ADC_CALI_Slop Done!\n");
            break;    
            
        case SET_ADC_CALI_Offset:            
            nvram_data_addr = (int *)arg;
            ret = copy_from_user(auxadc_cali_offset, nvram_data_addr, 36);
            g_AUXADC_Cali = KAL_FALSE;
            for (i = 0; i < 14; i++) printk("auxadc_cali_offset[%d] = %d\n", i, *(auxadc_cali_offset+i));
            printk("**** MT6573 auxadc_cali ioctl : SET_ADC_CALI_Offset Done!\n");            
            break;
            
        case SET_ADC_CALI_Cal :            
            nvram_data_addr = (int *)arg;
            ret = copy_from_user(auxadc_cali_cal, nvram_data_addr, 4);
            g_AUXADC_Cali = KAL_TRUE; /* enable calibration after setting AUXADC_CALI_Cal */
            if (auxadc_cali_cal[0] == 1) {
                g_AUXADC_Cali = KAL_TRUE;
            } else {
                g_AUXADC_Cali = KAL_FALSE;
            }            
            for (i = 0; i < 1; i++) printk("auxadc_cali_cal[%d] = %d\n", i, *(auxadc_cali_cal + i));
            printk("**** MT6573 auxadc_cali ioctl : SET_ADC_CALI_Cal Done!\n");            
            break;    

        case ADC_CHANNEL_READ:       
            g_AUXADC_Cali = KAL_FALSE; /* 20100508 Infinity */
            user_data_addr = (int *)arg;
            ret = copy_from_user(auxadc_in_data, user_data_addr, 8); /* 2*int = 2*4 */
            
            /*ChannelNUm, Counts*/
            //auxadc_out_data[0] = GetOneChannelValue(auxadc_in_data[0], auxadc_in_data[1]);  
            #if 0          
            upmu_adc_measure_vbat_enable(CHR, KAL_TRUE);
            upmu_adc_measure_vsen_enable(CHR, KAL_TRUE);
            upmu_adc_measure_vchr_enable(CHR, KAL_TRUE);
            #endif
			#if 0
            if(auxadc_in_data[0] == 0) // I_SENSE
            {
                auxadc_out_data[0] = IMM_GetOneChannelValue(AUXADC_REF_CURRENT_CHANNEL, auxadc_in_data[1]) * R_BAT_SENSE * auxadc_in_data[1];
            }
            else if( auxadc_in_data[0] == 1 ) // BAT_SENSE
            {
                auxadc_out_data[0] = IMM_GetOneChannelValue(AUXADC_BATTERY_VOLTAGE_CHANNEL, auxadc_in_data[1]) * R_I_SENSE * auxadc_in_data[1];
            }
            else if( auxadc_in_data[0] == 3 ) // V_Charger
            {
                auxadc_out_data[0] = IMM_GetOneChannelValue(AUXADC_CHARGER_VOLTAGE_CHANNEL, auxadc_in_data[1]) * (((R_CHARGER_1 + R_CHARGER_2) * 100) / R_CHARGER_2) * auxadc_in_data[1];			
                auxadc_out_data[0] = auxadc_out_data[0] / 100;
            }	
            else
            {
                auxadc_out_data[0] = IMM_GetOneChannelValue(auxadc_in_data[0], auxadc_in_data[1]) * auxadc_in_data[1];
            }

            if (auxadc_out_data[0] < 0)
                auxadc_out_data[1] = 1; /* failed */
            else
                auxadc_out_data[1] = 0; /* success */

			#endif
			// this ioctl is removed
			printk("this api is removed !! \n");
            ret = copy_to_user(user_data_addr, auxadc_out_data, 8);
            printk("**** ioctl : AUXADC Channel %d * %d times = %d\n", auxadc_in_data[0], auxadc_in_data[1], auxadc_out_data[0]);            
            break;
	
        default:
            g_AUXADC_Cali = KAL_FALSE;
            break;
    }

    mutex_unlock(&auxadc_mutex);
    
    return 0;
}

static int auxadc_cali_open(struct inode *inode, struct file *file)
{ 
    return 0;
}

static int auxadc_cali_release(struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations auxadc_cali_fops = {
    .owner      = THIS_MODULE,
    .unlocked_ioctl  = auxadc_cali_unlocked_ioctl,
    .open       = auxadc_cali_open,
    .release    = auxadc_cali_release,    
};

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_0_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_0_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 0));
    printk("[EM] AUXADC_Channel_0_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_0_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_0_Slope, 0664, show_AUXADC_Channel_0_Slope, store_AUXADC_Channel_0_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_1_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_1_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 1));
    printk("[EM] AUXADC_Channel_1_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_1_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_1_Slope, 0664, show_AUXADC_Channel_1_Slope, store_AUXADC_Channel_1_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_2_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_2_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 2));
    printk("[EM] AUXADC_Channel_2_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_2_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_2_Slope, 0664, show_AUXADC_Channel_2_Slope, store_AUXADC_Channel_2_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_3_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_3_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 3));
    printk("[EM] AUXADC_Channel_3_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_3_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_3_Slope, 0664, show_AUXADC_Channel_3_Slope, store_AUXADC_Channel_3_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_4_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_4_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 4));
    printk("[EM] AUXADC_Channel_4_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_4_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_4_Slope, 0664, show_AUXADC_Channel_4_Slope, store_AUXADC_Channel_4_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_5_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_5_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 5));
    printk("[EM] AUXADC_Channel_5_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_5_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_5_Slope, 0664, show_AUXADC_Channel_5_Slope, store_AUXADC_Channel_5_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_6_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_6_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 6));
    printk("[EM] AUXADC_Channel_6_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_6_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_6_Slope, 0664, show_AUXADC_Channel_6_Slope, store_AUXADC_Channel_6_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_7_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_7_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 7));
    printk("[EM] AUXADC_Channel_7_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_7_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_7_Slope, 0664, show_AUXADC_Channel_7_Slope, store_AUXADC_Channel_7_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_8_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_8_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 8));
    printk("[EM] AUXADC_Channel_8_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_8_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_8_Slope, 0664, show_AUXADC_Channel_8_Slope, store_AUXADC_Channel_8_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_9_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_9_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 9));
    printk("[EM] AUXADC_Channel_9_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_9_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_9_Slope, 0664, show_AUXADC_Channel_9_Slope, store_AUXADC_Channel_9_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_10_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_10_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 10));
    printk("[EM] AUXADC_Channel_10_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_10_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_10_Slope, 0664, show_AUXADC_Channel_10_Slope, store_AUXADC_Channel_10_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_11_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_11_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 11));
    printk("[EM] AUXADC_Channel_11_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_11_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_11_Slope, 0664, show_AUXADC_Channel_11_Slope, store_AUXADC_Channel_11_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_12_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_12_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 12));
    printk("[EM] AUXADC_Channel_12_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_12_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_12_Slope, 0664, show_AUXADC_Channel_12_Slope, store_AUXADC_Channel_12_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_13_Slope
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_13_Slope(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_slop + 13));
    printk("[EM] AUXADC_Channel_13_Slope : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_13_Slope(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_13_Slope, 0664, show_AUXADC_Channel_13_Slope, store_AUXADC_Channel_13_Slope);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_0_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_0_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 0));
    printk("[EM] AUXADC_Channel_0_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_0_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_0_Offset, 0664, show_AUXADC_Channel_0_Offset, store_AUXADC_Channel_0_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_1_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_1_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 1));
    printk("[EM] AUXADC_Channel_1_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_1_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_1_Offset, 0664, show_AUXADC_Channel_1_Offset, store_AUXADC_Channel_1_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_2_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_2_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 2));
    printk("[EM] AUXADC_Channel_2_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_2_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_2_Offset, 0664, show_AUXADC_Channel_2_Offset, store_AUXADC_Channel_2_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_3_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_3_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 3));
    printk("[EM] AUXADC_Channel_3_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_3_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_3_Offset, 0664, show_AUXADC_Channel_3_Offset, store_AUXADC_Channel_3_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_4_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_4_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 4));
    printk("[EM] AUXADC_Channel_4_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_4_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_4_Offset, 0664, show_AUXADC_Channel_4_Offset, store_AUXADC_Channel_4_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_5_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_5_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 5));
    printk("[EM] AUXADC_Channel_5_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_5_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_5_Offset, 0664, show_AUXADC_Channel_5_Offset, store_AUXADC_Channel_5_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_6_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_6_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 6));
    printk("[EM] AUXADC_Channel_6_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_6_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_6_Offset, 0664, show_AUXADC_Channel_6_Offset, store_AUXADC_Channel_6_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_7_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_7_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 7));
    printk("[EM] AUXADC_Channel_7_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_7_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_7_Offset, 0664, show_AUXADC_Channel_7_Offset, store_AUXADC_Channel_7_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_8_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_8_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 8));
    printk("[EM] AUXADC_Channel_8_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_8_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_8_Offset, 0664, show_AUXADC_Channel_8_Offset, store_AUXADC_Channel_8_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_9_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_9_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 9));
    printk("[EM] AUXADC_Channel_9_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_9_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_9_Offset, 0664, show_AUXADC_Channel_9_Offset, store_AUXADC_Channel_9_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_10_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_10_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 10));
    printk("[EM] AUXADC_Channel_10_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_10_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	printk("[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(AUXADC_Channel_10_Offset, 0664, show_AUXADC_Channel_10_Offset, store_AUXADC_Channel_10_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_11_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_11_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 11));
    printk("[EM] AUXADC_Channel_11_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_11_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_11_Offset, 0664, show_AUXADC_Channel_11_Offset, store_AUXADC_Channel_11_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_12_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_12_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 12));
    printk("[EM] AUXADC_Channel_12_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_12_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_12_Offset, 0664, show_AUXADC_Channel_12_Offset, store_AUXADC_Channel_12_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_13_Offset
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_13_Offset(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 1;
    ret_value = (*(auxadc_cali_offset + 13));
    printk("[EM] AUXADC_Channel_13_Offset : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_13_Offset(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_13_Offset, 0664, show_AUXADC_Channel_13_Offset, store_AUXADC_Channel_13_Offset);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : AUXADC_Channel_Is_Calibration
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_AUXADC_Channel_Is_Calibration(struct device *dev,struct device_attribute *attr, char *buf)
{
    int ret_value = 2;
    ret_value = g_AUXADC_Cali;
    printk("[EM] AUXADC_Channel_Is_Calibration : %d\n", ret_value);
    return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_AUXADC_Channel_Is_Calibration(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support Write Function\n");	
    return size;
}
static DEVICE_ATTR(AUXADC_Channel_Is_Calibration, 0664, show_AUXADC_Channel_Is_Calibration, store_AUXADC_Channel_Is_Calibration);

static ssize_t show_AUXADC_register(struct device *dev,struct device_attribute *attr, char *buf)
{
    
	printk("[adc_udvt]: AUXADC_CON0=%x\n",*(volatile u16 *)AUXADC_CON0);
    printk("[adc_udvt]: AUXADC_CON1=%x\n",*(volatile u16 *)AUXADC_CON1);
    printk("[adc_udvt]: AUXADC_CON2=%x\n",*(volatile u16 *)AUXADC_CON2);
    printk("[adc_udvt]: AUXADC_CON3=%x\n",*(volatile u16 *)AUXADC_CON3);

	if(enable_clock(MT65XX_PDN_PERI_AUXADC,"AUXADC"))
        printk("hwEnableClock AUXADC failed.");

    return sprintf(buf, "AUXADC_CON0:%x\n AUXADC_CON1:%x\n AUXADC_CON2:%x\n AUXADC_CON3:%x\n"
		, *(volatile u16 *)AUXADC_CON0,*(volatile u16 *)AUXADC_CON1,*(volatile u16 *)AUXADC_CON2
		,*(volatile u16 *)AUXADC_CON3);
}

static ssize_t store_AUXADC_register(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    printk("[EM] Not Support store_AUXADC_register\n");	
    return size;
}

static DEVICE_ATTR(AUXADC_register, 0664, show_AUXADC_register, store_AUXADC_register);


static ssize_t show_AUXADC_chanel(struct device *dev,struct device_attribute *attr, char *buf)
{
    //read data
    int i = 0, data[4] = {0,0,0,0};
	char buf_temp[960];
	int res =0;
    for (i = 0; i < 5; i++) 
    {
        //printk("[adc_driver]: i=%d\n",i);
		res = IMM_GetOneChannelValue(i,data,NULL);
		if(res < 0)
		{ 
			   printk("[adc_driver]: get data error\n");
			   break;
			   
		}
		else
		{
		       printk("[adc_driver]: channel[%d]=%d.%d \n",i,data[0],data[1]);
			   sprintf(buf_temp,"channel[%d]=%d.%d \n",i,data[0],data[1]);
			   strcat(buf,buf_temp);
			  // sprintf(buf,"channel[%d]=%d.%d \n",i,data[0],data[1]);
		}
			
    } 
	printk("[adc_driver]: AUXADC_CON0=%x\n",*(volatile u16 *)AUXADC_CON0);
    printk("[adc_driver]: AUXADC_CON1=%x\n",*(volatile u16 *)AUXADC_CON1);
    printk("[adc_driver]: AUXADC_CON2=%x\n",*(volatile u16 *)AUXADC_CON2);
    printk("[adc_driver]: AUXADC_CON3=%x\n",*(volatile u16 *)AUXADC_CON3);

    sprintf(buf_temp, "AUXADC_CON0:%x\n AUXADC_CON1:%x\n AUXADC_CON2:%x\n AUXADC_CON3:%x\n"
		, *(volatile u16 *)AUXADC_CON0,*(volatile u16 *)AUXADC_CON1,*(volatile u16 *)AUXADC_CON2
		,*(volatile u16 *)AUXADC_CON3);
	strcat(buf,buf_temp);
	
    return strlen(buf);
}

static struct task_struct *thread = NULL;
static int g_start_debug_thread =0;


static int dbug_thread(void *unused) 
{
   int i = 0, data[4] = {0,0,0,0};
   int res =0;
   int rawdata=0;
   
   while(g_start_debug_thread)
   {
      for (i = 0; i < 14; i++) 
      {
        //printk("[adc_driver]: i=%d\n",i);
       
		res = IMM_GetOneChannelValue(i,data,&rawdata);
		if(res < 0)
		{ 
			   printk("[adc_driver]: get data error\n");
			   break;
			   
		}
		else
		{
		       printk("[adc_driver]: channel[%d]raw =%d\n",i,rawdata);
		       printk("[adc_driver]: channel[%d]=%d.%.02d \n",i,data[0],data[1]);
			  
		}
			
      } 
	  msleep(500);

   }
   return 0;
}


static ssize_t store_AUXADC_channel(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
    //printk("[EM] Not Support store_AUXADC_channel\n");	
	
	unsigned int start_flag;
	int error;
	
		if (sscanf(buf, "%u", &start_flag) != 1) {
			printk("[adc_driver]: Invalid values\n");
			return -EINVAL;
		}
	
		printk("[adc_driver] start flag =%d \n",start_flag);
	    g_start_debug_thread = start_flag;
		if(1 == start_flag)
		{
		   thread = kthread_run(dbug_thread, 0, "AUXADC");
		   
		   if (IS_ERR(thread)) 
		   { 
			  error = PTR_ERR(thread);
			  printk( "[adc_driver] failed to create kernel thread: %d\n", error);
		   }
		}
	
    return size;
}

static DEVICE_ATTR(AUXADC_read_channel, 0664, show_AUXADC_chanel, store_AUXADC_channel);


// platform_driver API 
static int mt6577_auxadc_probe(struct platform_device *dev)	
{    
    int ret = 0;
    struct class_device *class_dev = NULL;
    
    printk("******** MT6577 AUXADC driver probe!! ********\n");
	adc_channel_info_init();

	if(enable_clock(MT65XX_PDN_PERI_AUXADC,"AUXADC"))
        printk("hwEnableClock AUXADC failed.");
    
    /* Integrate with NVRAM */
    ret = alloc_chrdev_region(&auxadc_cali_devno, 0, 1, AUXADC_CALI_DEVNAME);
    if (ret) 
        printk("Error: Can't Get Major number for auxadc_cali\n");
        
    auxadc_cali_cdev = cdev_alloc();
    auxadc_cali_cdev->owner = THIS_MODULE;
    auxadc_cali_cdev->ops = &auxadc_cali_fops;
    ret = cdev_add(auxadc_cali_cdev, auxadc_cali_devno, 1);
    if(ret)
        printk("auxadc_cali Error: cdev_add\n");
        
    auxadc_cali_major = MAJOR(auxadc_cali_devno);
    auxadc_cali_class = class_create(THIS_MODULE, AUXADC_CALI_DEVNAME);
    class_dev = (struct class_device *)device_create(auxadc_cali_class, 
                NULL, auxadc_cali_devno, NULL, AUXADC_CALI_DEVNAME);
    printk("[MT6577 AUXADC_probe] NVRAM prepare : done !!\n");
    
    /* For EM */
	if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_register)) != 0) goto exit;
	if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_read_channel)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_0_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_1_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_2_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_3_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_4_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_5_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_6_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_7_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_8_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_9_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_10_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_11_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_12_Slope)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_13_Slope)) != 0) goto exit;
    
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_0_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_1_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_2_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_3_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_4_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_5_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_6_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_7_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_8_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_9_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_10_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_11_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_12_Offset)) != 0) goto exit;
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_13_Offset)) != 0) goto exit;
    
    if ((ret = device_create_file(&(dev->dev), &dev_attr_AUXADC_Channel_Is_Calibration)) != 0) goto exit;

	g_adc_init_flag =1;
	
    
	
exit:
    return ret;
}

static int mt6577_auxadc_remove(struct platform_device *dev)	
{
    printk("******** MT6577 auxadc driver remove!! ********\n" );    
    return 0;
}

static void mt6577_auxadc_shutdown(struct platform_device *dev)	
{
    printk("******** MT6577 auxadc driver shutdown!! ********\n" );
}

static int mt6577_auxadc_suspend(struct platform_device *dev, pm_message_t state)	
{
    printk("******** MT6577 auxadc driver suspend!! ********\n" );
	if(disable_clock(MT65XX_PDN_PERI_AUXADC,"AUXADC"))
        printk("hwEnableClock AUXADC failed.");
	
    return 0;
}

static int mt6577_auxadc_resume(struct platform_device *dev)
{
    printk("******** MT6577 auxadc driver resume!! ********\n" );
	if(enable_clock(MT65XX_PDN_PERI_AUXADC,"AUXADC"))
	{
	    printk("hwEnableClock AUXADC again!!!.");
	    if(enable_clock(MT65XX_PDN_PERI_AUXADC,"AUXADC"))
	    {printk("hwEnableClock AUXADC failed.");}
        
	}
    return 0;
}
/*
struct platform_device MT6573_auxadc_device = {
    .name   = "mt6577-auxadc",
    .id     = -1,
};
*/
static struct platform_driver mt6577_auxadc_driver = {
    .probe      = mt6577_auxadc_probe,
    .remove     = mt6577_auxadc_remove,
    .shutdown   = mt6577_auxadc_shutdown,
    #ifdef CONFIG_PM
        .suspend = mt6577_auxadc_suspend,
        .resume	 = mt6577_auxadc_resume,
    #endif
    .driver     = {
    .name       = "mt6577-auxadc",
    },
};
 
static int __init mt6577_auxadc_init(void)
{
    int ret;
    /*
    ret = platform_device_register(&MT6573_auxadc_device);
    if (ret) {
        printk("****[mt6577_auxadc_driver] Unable to device register(%d)\n", ret);
        return ret;
    }
    */
    ret = platform_driver_register(&mt6577_auxadc_driver);
    if (ret) {
        printk("****[mt6577_auxadc_driver] Unable to register driver (%d)\n", ret);
        return ret;
    }
    
    printk("****[mt6577_auxadc_driver] Initialization : DONE \n");
    return 0;
}

static void __exit mt6577_auxadc_exit (void)
{
}

module_init(mt6577_auxadc_init);
module_exit(mt6577_auxadc_exit);

MODULE_AUTHOR("MTK");
MODULE_DESCRIPTION("MT6577 AUXADC Device Driver");
MODULE_LICENSE("GPL");

