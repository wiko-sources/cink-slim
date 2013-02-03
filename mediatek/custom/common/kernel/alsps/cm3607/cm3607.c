/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/* drivers/hwmon/mt6516/amit/al3006.c - AL3006 ALS/PS driver
 * 
 * Author: MingHsien Hsieh <minghsien.hsieh@mediatek.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>

#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>
#include <asm/io.h>
#include <cust_eint.h>
#include <cust_alsps.h>
#include "cm3607.h"
#include <linux/hwmsen_helper.h>

#ifdef MT6516
#include <mach/mt6516_devs.h>
#include <mach/mt6516_typedefs.h>
#include <mach/mt6516_gpio.h>
#include <mach/mt6516_pll.h>
#endif

#ifdef MT6573
#include <mach/mt6573_devs.h>
#include <mach/mt6573_typedefs.h>
#include <mach/mt6573_gpio.h>
#include <mach/mt6573_pll.h>
#endif

#ifdef MT6575
#include <mach/mt6575_devs.h>
#include <mach/mt6575_typedefs.h>
#include <mach/mt6575_gpio.h>
#include <mach/mt6575_pm_ldo.h>
#endif

#ifdef MT6577
#include <mach/mt6577_devs.h>
#include <mach/mt6577_typedefs.h>
#include <mach/mt6577_gpio.h>
#include <mach/mt6577_pm_ldo.h>
#endif

#ifdef MT6573
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);

#endif

#ifdef MT6575
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);

#endif

#ifdef MT6577
	extern void mt65xx_eint_unmask(unsigned int line);
	extern void mt65xx_eint_mask(unsigned int line);
	extern void mt65xx_eint_set_polarity(unsigned int eint_num, unsigned int pol);
	extern void mt65xx_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms);
	extern unsigned int mt65xx_eint_set_sens(unsigned int eint_num, unsigned int sens);
	extern void mt65xx_eint_registration(unsigned int eint_num, unsigned int is_deb_en, unsigned int pol, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask);
#endif
/*-------------------------MT6516&MT6573 define-------------------------------*/
#ifdef MT6516
#define POWER_NONE_MACRO MT6516_POWER_NONE
#endif

#ifdef MT6573
#define POWER_NONE_MACRO MT65XX_POWER_NONE
#endif

#ifdef MT6575
#define POWER_NONE_MACRO MT65XX_POWER_NONE
#endif

#ifdef MT6577
#define POWER_NONE_MACRO MT65XX_POWER_NONE
#endif

/******************************************************************************
 * configuration
*******************************************************************************/
#define CM3607_DEV_NAME     "CM3607"
/*----------------------------------------------------------------------------*/
#define APS_TAG                  "[ALS/PS] "
#define APS_FUN(f)               printk(KERN_INFO APS_TAG"%s\n", __FUNCTION__)
#define APS_ERR(fmt, args...)    printk(KERN_ERR  APS_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define APS_LOG(fmt, args...)    printk(KERN_INFO APS_TAG fmt, ##args)
#define APS_DBG(fmt, args...)    printk(KERN_INFO fmt, ##args)                 
/******************************************************************************
 * extern functions
*******************************************************************************/
#ifdef MT6516
extern void MT6516_EINTIRQUnmask(unsigned int line);
extern void MT6516_EINTIRQMask(unsigned int line);
extern void MT6516_EINT_Set_Polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void MT6516_EINT_Set_HW_Debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 MT6516_EINT_Set_Sensitivity(kal_uint8 eintno, kal_bool sens);
extern void MT6516_EINT_Registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);
#endif
/*----------------------------------------------------------------------------*/

static struct cm3607_priv *g_cm3607_ptr = NULL;

//Ivan
#define ALSPS_POWER_DOWN_ENABLE 1
#define GPIO_PS_EN_PIN	GPIO_ALS_EN_PIN		//GPIO186

#if ALSPS_POWER_DOWN_ENABLE
static int cm3607_power_down( bool enable);
static int cm3607_power_flag = 0;
#endif

/*----------------------------------------------------------------------------*/
typedef enum {
    CMC_TRC_APS_DATA = 0x0002,
    CMC_TRC_EINT    = 0x0004,
    CMC_TRC_IOCTL   = 0x0008,
    CMC_TRC_I2C     = 0x0010,
    CMC_TRC_CVT_ALS = 0x0020,
    CMC_TRC_CVT_PS  = 0x0040,
    CMC_TRC_DEBUG   = 0x8000,
} CMC_TRC;
/*----------------------------------------------------------------------------*/
typedef enum {
    CMC_BIT_ALS    = 1,
    CMC_BIT_PS     = 2,
} CMC_BIT;
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
struct cm3607_priv {
    struct alsps_hw  *hw;
//    struct i2c_client *client;
    struct delayed_work  eint_work;
	//struct timer_list   first_read_ps_timer;
	//struct timer_list   first_read_als_timer;
    
    /*misc*/
    atomic_t    trace;
    atomic_t    i2c_retry;
    atomic_t    als_suspend;
    atomic_t    als_debounce;   /*debounce time after enabling als*/
    atomic_t    als_deb_on;     /*indicates if the debounce is on*/
    atomic_t    als_deb_end;    /*the jiffies representing the end of debounce*/
    atomic_t    ps_mask;        /*mask ps: always return far away*/
    atomic_t    ps_debounce;    /*debounce time after enabling ps*/
    atomic_t    ps_deb_on;      /*indicates if the debounce is on*/
    atomic_t    ps_deb_end;     /*the jiffies representing the end of debounce*/
    atomic_t    ps_suspend;


    /*data*/
    u8         als;
    u8          ps;
    u8          _align;
    u16         als_level_num;
    u16         als_value_num;
    u32         als_level[C_CUST_ALS_LEVEL-1];
    u32         als_value[C_CUST_ALS_LEVEL];

    bool    als_enable;    /*record current als status*/
	unsigned int    als_widow_loss; 
	
    bool    ps_enable;     /*record current ps status*/
    unsigned int    ps_thd_val;     /*the cmd value can't be read, stored in ram*/
    ulong       enable;         /*record HAL enalbe status*/
    ulong       pending_intr;   /*pending interrupt*/
    //ulong        first_read;   // record first read ps and als
    /*early suspend*/
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend    early_drv;
#endif     
};

static struct cm3607_priv *cm3607_obj = NULL;
static struct platform_driver cm3607_alsps_driver;

static int cm3607_get_ps_value(struct cm3607_priv *obj, u8 ps);
//static int cm3607_get_als_value(struct cm3607_priv *obj, u8 als);

/*----------------------------------------------------------------------------*/
int cm3607_get_timing(void)
{
return 200;
/*
	u32 base = I2C2_BASE; 
	return (__raw_readw(mt6516_I2C_HS) << 16) | (__raw_readw(mt6516_I2C_TIMING));
*/
}

/*----------------------------------------------------------------------------*/
int cm3607_read_data( u8 *data)
{
	struct cm3607_priv *obj = cm3607_obj;    
	int ret = 0;
	//u8 aps_data=0;
	//u8 addr = 0x00;


	//hwmsen_read_byte_sr(client,APS_BOTH_DATA,&aps_data);
	*data = mt_get_gpio_in(GPIO_ALS_EINT_PIN);
	APS_DBG("cm3607_read_data:  %x\n", *data);
	
/*	
	if(hwmsen_read_byte_sr(client,APS_BOTH_DATA,data))
	{
		APS_ERR("reads aps data = %d\n", ret);
		return -EFAULT;
	}
*/	
	if(atomic_read(&obj->trace) & CMC_TRC_APS_DATA)
	{
		APS_DBG("APS:  0x%04X\n", (u32)(*data));
	}
	return 0;    
}
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static void cm3607_power(struct alsps_hw *hw, unsigned int on) 
{
	static unsigned int power_on = 0;

	//APS_LOG("power %s\n", on ? "on" : "off");

	if(hw->power_id != POWER_NONE_MACRO)
	{
		if(power_on == on)
		{
			APS_LOG("ignore power control: %d\n", on);
		}
		else if(on)
		{
			if(!hwPowerOn(hw->power_id, hw->power_vol, "CM3607")) 
			{
				APS_ERR("power on fails!!\n");
			}
		}
		else
		{
			if(!hwPowerDown(hw->power_id, "CM3607")) 
			{
				APS_ERR("power off fail!!\n");   
			}
		}
	}
	power_on = on;
}

#ifdef ALSPS_POWER_DOWN_ENABLE
static int cm3607_power_down(bool enable)
{
    u8 regdata=0;
    
    if (cm3607_power_flag != enable)
    {
    
	
	if (enable)
	{
	    mt65xx_eint_mask(CUST_EINT_ALS_NUM);	    
	    APS_LOG("cm3607_power_down on!\n");
	    mt_set_gpio_out(GPIO_PS_EN_PIN, 1);		//Low = operate; High = power down
	}
	else
	{
	    mt_set_gpio_out(GPIO_PS_EN_PIN, 0);		//Low = operate; High = power down
	    APS_LOG("cm3607_power_down off!\n");	
	    msleep(50);			//Ivan++	
	    mt65xx_eint_unmask(CUST_EINT_ALS_NUM);  	    	    	    
	}
    }
    cm3607_power_flag = enable;
    return 0;
    
}    
#endif

/*----------------------------------------------------------------------------*/
static int cm3607_enable_ps(struct cm3607_priv* obj, bool enable)
{
	APS_LOG(" cm3607_enable_ps %d\n",enable); 
//	struct cm3607_priv *obj = i2c_get_clientdata(client);
	int err=0;
	int trc = atomic_read(&obj->trace);
	u8 regdata=0;
	if(enable == obj->ps_enable)
	{
	   return 0;
	}

	if(enable == TRUE)//enable ps
	{
		APS_LOG("first enable ps!\n");
//Ivan		 atomic_set(&obj->ps_deb_on, 1);
		 atomic_set(&obj->ps_deb_end, jiffies+atomic_read(&obj->ps_debounce)/(1000/HZ));
		 set_bit(CMC_BIT_PS,  &obj->pending_intr);
		 schedule_delayed_work(&obj->eint_work,120);
		 APS_LOG("first enalbe ps set pending interrupt %d\n",obj->pending_intr);
	}
	else//disable ps
	{
		 if(false == obj->als_enable)
		 {
		   APS_LOG("PS(0): disable both !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		 }
		 //del_timer_sync(&obj->first_read_ps_timer);
	}
	

	obj->ps_enable = enable;

	if(trc & CMC_TRC_DEBUG)
	{
		APS_LOG("enable ps (%d)\n", enable);
	}

	return err;
}

/*----------------------------------------------------------------------------*/
void cm3607_eint_func(void)
{
	struct cm3607_priv *obj = g_cm3607_ptr;
	APS_LOG("fwq interrupt fuc\n");
	if(!obj)
	{
		return;
	}
	
	schedule_delayed_work(&obj->eint_work,0);
	if(atomic_read(&obj->trace) & CMC_TRC_EINT)
	{
		APS_LOG("eint: als/ps intrs\n");
	}
}
/*----------------------------------------------------------------------------*/
static void cm3607_eint_work(struct work_struct *work)
{
	struct cm3607_priv *obj = (struct cm3607_priv *)container_of(work, struct cm3607_priv, eint_work);
	int err;
	s32 data;
	hwm_sensor_data sensor_data;
	
	memset(&sensor_data, 0, sizeof(sensor_data));

	APS_LOG("cm3607_eint_work\n");
	
#ifdef ALSPS_POWER_DOWN_ENABLE
//Ivan added
	if (obj->ps_enable != 1 && cm3607_power_flag == 1)
	{
	    mt65xx_eint_unmask(CUST_EINT_ALS_NUM);  
	    APS_LOG("cm3607_eint_work All sensor OFF\n");
	    return;
	}
//Ivan added
	if (cm3607_power_flag == 1)
	{
	    err = cm3607_power_down(false);	
	}
#endif

	APS_LOG("cm3607_eint_work &obj->pending_intr =%d\n",obj->pending_intr);
	
	if((1<<CMC_BIT_PS) &  obj->pending_intr)
	{
	  //get raw data
	  APS_LOG("fwq ps INT\n");
	  cm3607_read_data(&obj->ps);
	  sensor_data.values[0] = cm3607_get_ps_value(obj, obj->ps);
	  sensor_data.value_divide = 1;
	  sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
	  //let up layer to know
	  APS_LOG("cm3607 read ps data  = %d \n",sensor_data.values[0]);
	  if(err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data))
	  {
		APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
	  }
	}
	
	data = mt_get_gpio_in(GPIO_ALS_EINT_PIN);
	if (data & 0x01)
	    mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_POLARITY_LOW);
	else	    
	    mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_POLARITY_HIGH);

	
	#ifdef MT6516
	MT6516_EINTIRQUnmask(CUST_EINT_ALS_NUM);      
	#endif     
	#ifdef MT6573
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);  
	#endif
	#ifdef MT6575
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);      
	#endif

	#ifdef MT6577
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);      
	#endif
	
}

/*----------------------------------------------------------------------------*/
int cm3607_setup_eint()
{
//	struct cm3607_priv *obj = i2c_get_clientdata(client);        

	g_cm3607_ptr = cm3607_obj;	//Ivan
	/*configure to GPIO function, external interrupt*/

	
	mt_set_gpio_mode(GPIO_ALS_EINT_PIN, GPIO_ALS_EINT_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_ALS_EINT_PIN, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_ALS_EINT_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_ALS_EINT_PIN, GPIO_PULL_UP);
	
	//mt_set_gpio_dir(GPIO_ALS_EINT_PIN, GPIO_DIR_IN);
	//mt_set_gpio_mode(GPIO_ALS_EINT_PIN, GPIO_ALS_EINT_PIN_M_EINT);
	//mt_set_gpio_pull_enable(GPIO_ALS_EINT_PIN, GPIO_PULL_ENABLE);
	//mt_set_gpio_pull_select(GPIO_ALS_EINT_PIN, GPIO_PULL_UP);

#ifdef MT6516

	MT6516_EINT_Set_Sensitivity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
	MT6516_EINT_Set_Polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
	MT6516_EINT_Set_HW_Debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
	MT6516_EINT_Registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, cm3607_eint_func, 0);
	MT6516_EINTIRQUnmask(CUST_EINT_ALS_NUM);  
#endif
    //
#ifdef MT6573
	
    mt65xx_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
	mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
	mt65xx_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
	mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, cm3607_eint_func, 0);
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);  
#endif  

#ifdef MT6575
	
    mt65xx_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
	mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
	mt65xx_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
	mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, cm3607_eint_func, 0);
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);  
#endif  	
	
#ifdef MT6577

		mt65xx_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
		mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
		mt65xx_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
		mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, cm3607_eint_func, 0);
		mt65xx_eint_unmask(CUST_EINT_ALS_NUM);	

#endif
	
    return 0;
}


/******************************************************************************
 * Sysfs attributes
*******************************************************************************/
static ssize_t cm3607_show_config(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	
	if(!cm3607_obj)
	{
		APS_ERR("cm3607_obj is null!!\n");
		return 0;
	}
	
	res = snprintf(buf, PAGE_SIZE, "(%d %d %d %d %d)\n", 
		atomic_read(&cm3607_obj->i2c_retry), atomic_read(&cm3607_obj->als_debounce), 
		atomic_read(&cm3607_obj->ps_mask), cm3607_obj->ps_thd_val, atomic_read(&cm3607_obj->ps_debounce));     
	return res;    
}
/*----------------------------------------------------------------------------*/
static ssize_t cm3607_store_config(struct device_driver *ddri, char *buf, size_t count)
{
	int retry, als_deb, ps_deb, mask, thres;
	if(!cm3607_obj)
	{
		APS_ERR("cm3607_obj is null!!\n");
		return 0;
	}
	
	if(5 == sscanf(buf, "%d %d %d %d %d", &retry, &als_deb, &mask, &thres, &ps_deb))
	{ 
		atomic_set(&cm3607_obj->i2c_retry, retry);
		atomic_set(&cm3607_obj->als_debounce, als_deb);
		atomic_set(&cm3607_obj->ps_mask, mask);
		cm3607_obj->ps_thd_val= thres;        
		atomic_set(&cm3607_obj->ps_debounce, ps_deb);
	}
	else
	{
		APS_ERR("invalid content: '%s', length = %d\n", buf, count);
	}
	return count;    
}
/*----------------------------------------------------------------------------*/
static ssize_t cm3607_show_trace(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	if(!cm3607_obj)
	{
		APS_ERR("cm3607_obj is null!!\n");
		return 0;
	}

	res = snprintf(buf, PAGE_SIZE, "0x%04X\n", atomic_read(&cm3607_obj->trace));     
	return res;    
}
/*----------------------------------------------------------------------------*/
static ssize_t cm3607_store_trace(struct device_driver *ddri, char *buf, size_t count)
{
    int trace;
    if(!cm3607_obj)
	{
		APS_ERR("cm3607_obj is null!!\n");
		return 0;
	}
	
	if(1 == sscanf(buf, "0x%x", &trace))
	{
		atomic_set(&cm3607_obj->trace, trace);
	}
	else 
	{
		APS_ERR("invalid content: '%s', length = %d\n", buf, count);
	}
	return count;    
}

/*----------------------------------------------------------------------------*/
static ssize_t cm3607_show_ps(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	u8 dat=0;
	if(!cm3607_obj)
	{
		APS_ERR("cm3607_obj is null!!\n");
		return 0;
	}

	dat = mt_get_gpio_in(GPIO_ALS_EINT_PIN);	
/*
	if(res = cm3607_read_data(cm3607_obj->client, &cm3607_obj->ps))
	{
		return snprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
	else
*/
	    dat = dat & 0x01;
	    return snprintf(buf, PAGE_SIZE, "0x%04X\n", dat);     
}

/*----------------------------------------------------------------------------*/
static ssize_t cm3607_show_status(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	
	if(!cm3607_obj)
	{
		APS_ERR("cm3607_obj is null!!\n");
		return 0;
	}
	
	if(cm3607_obj->hw)
	{
	
		len += snprintf(buf+len, PAGE_SIZE-len, "CUST: %d, (%d %d)\n", 
			cm3607_obj->hw->i2c_num, cm3607_obj->hw->power_id, cm3607_obj->hw->power_vol);
		
	}
	else
	{
		len += snprintf(buf+len, PAGE_SIZE-len, "CUST: NULL\n");
	}

	#ifdef MT6516
	len += snprintf(buf+len, PAGE_SIZE-len, "EINT: %d (%d %d %d %d)\n", mt_get_gpio_in(GPIO_ALS_EINT_PIN),
				CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_DEBOUNCE_CN);

	len += snprintf(buf+len, PAGE_SIZE-len, "GPIO: %d (%d %d %d %d)\n",	GPIO_ALS_EINT_PIN, 
				mt_get_gpio_dir(GPIO_ALS_EINT_PIN), mt_get_gpio_mode(GPIO_ALS_EINT_PIN), 
				mt_get_gpio_pull_enable(GPIO_ALS_EINT_PIN), mt_get_gpio_pull_select(GPIO_ALS_EINT_PIN));
	#endif

	len += snprintf(buf+len, PAGE_SIZE-len, "MISC: %d %d\n", atomic_read(&cm3607_obj->als_suspend), atomic_read(&cm3607_obj->ps_suspend));

	return len;
}

/*----------------------------------------------------------------------------*/
#define IS_SPACE(CH) (((CH) == ' ') || ((CH) == '\n'))
/*----------------------------------------------------------------------------*/
static int read_int_from_buf(struct cm3607_priv *obj, const char* buf, size_t count,
                             u32 data[], int len)
{
	int idx = 0;
	char *cur = (char*)buf, *end = (char*)(buf+count);

	while(idx < len)
	{
		while((cur < end) && IS_SPACE(*cur))
		{
			cur++;        
		}

		if(1 != sscanf(cur, "%d", &data[idx]))
		{
			break;
		}

		idx++; 
		while((cur < end) && !IS_SPACE(*cur))
		{
			cur++;
		}
	}
	return idx;
}

/*----------------------------------------------------------------------------*/
//static DRIVER_ATTR(als,     S_IWUSR | S_IRUGO, cm3607_show_als,   NULL);
static DRIVER_ATTR(ps,      S_IWUSR | S_IRUGO, cm3607_show_ps,    NULL);
static DRIVER_ATTR(config,  S_IWUSR | S_IRUGO, cm3607_show_config,cm3607_store_config);
//static DRIVER_ATTR(alslv,   S_IWUSR | S_IRUGO, cm3607_show_alslv, cm3607_store_alslv);
//static DRIVER_ATTR(alsval,  S_IWUSR | S_IRUGO, cm3607_show_alsval,cm3607_store_alsval);
static DRIVER_ATTR(trace,   S_IWUSR | S_IRUGO, cm3607_show_trace, cm3607_store_trace);
static DRIVER_ATTR(status,  S_IWUSR | S_IRUGO, cm3607_show_status,  NULL);
//static DRIVER_ATTR(reg,     S_IWUSR | S_IRUGO, cm3607_show_reg,   NULL);
//static DRIVER_ATTR(i2c,     S_IWUSR | S_IRUGO, cm3607_show_i2c,   cm3607_store_i2c);
/*----------------------------------------------------------------------------*/
static struct device_attribute *cm3607_attr_list[] = {
//    &driver_attr_als,
    &driver_attr_ps,    
    &driver_attr_trace,        /*trace log*/
    &driver_attr_config,
//    &driver_attr_alslv,
//    &driver_attr_alsval,
    &driver_attr_status,
//    &driver_attr_i2c,
//    &driver_attr_reg,
};
/*----------------------------------------------------------------------------*/
static int cm3607_create_attr(struct device_driver *driver) 
{
	int idx, err = 0;
	int num = (int)(sizeof(cm3607_attr_list)/sizeof(cm3607_attr_list[0]));
	if (driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		if(err = driver_create_file(driver, cm3607_attr_list[idx]))
		{            
			APS_ERR("driver_create_file (%s) = %d\n", cm3607_attr_list[idx]->attr.name, err);
			break;
		}
	}    
	return err;
}
/*----------------------------------------------------------------------------*/
	static int cm3607_delete_attr(struct device_driver *driver)
	{
	int idx ,err = 0;
	int num = (int)(sizeof(cm3607_attr_list)/sizeof(cm3607_attr_list[0]));

	if (!driver)
	return -EINVAL;

	for (idx = 0; idx < num; idx++) 
	{
		driver_remove_file(driver, cm3607_attr_list[idx]);
	}
	
	return err;
}
/****************************************************************************** 
 * Function Configuration
******************************************************************************/

/*----------------------------------------------------------------------------*/
static int cm3607_get_ps_value(struct cm3607_priv *obj, u8 ps)
{
  
	int val= -1;
	int invalid = 0;

	if(ps)
	{
		val = 1;  /*far away*/
	}
	else
	{
		val = 0;  /*close*/	    
	}
//Ivan removed
/*
	if(atomic_read(&obj->ps_suspend))
	{
		invalid = 1;
	}
	else 
*/
	if(1 == atomic_read(&obj->ps_deb_on))
	{
		unsigned long endt = atomic_read(&obj->ps_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->ps_deb_on, 0);
			//clear_bit(CMC_BIT_PS, &obj->first_read);
		}
		
		if (1 == atomic_read(&obj->ps_deb_on))
		{
			invalid = 1;
		}
	}

	if(!invalid)
	{
		if(unlikely(atomic_read(&obj->trace) & CMC_TRC_CVT_PS))
		{
		   APS_DBG("PS:  %05d => %05d\n", ps, val);
		}
		return val;
		
	}	
	else
	{
		if(unlikely(atomic_read(&obj->trace) & CMC_TRC_CVT_PS))
		{
			APS_DBG("PS:  %05d => %05d (-1)\n", ps, val);    
		}
		return -1;
	}	
	
}

/****************************************************************************** 
 * Function Configuration
******************************************************************************/
static int cm3607_open(struct inode *inode, struct file *file)
{
	file->private_data = cm3607_obj;

	if (!file->private_data)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}
	
	return nonseekable_open(inode, file);
}
/*----------------------------------------------------------------------------*/
static int cm3607_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*----------------------------------------------------------------------------*/
//static int cm3607_unlocked_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
//       unsigned long arg)
static long cm3607_unlocked_ioctl(struct file *file, unsigned int cmd,unsigned long arg)       

{
//	struct i2c_client *client = (struct i2c_client*)file->private_data;
	struct cm3607_priv *obj = (struct i2c_client*)file->private_data; 
	int err = 0;
	void __user *ptr = (void __user*) arg;
	int dat;
	uint32_t enable;

	switch (cmd)
	{
		case ALSPS_SET_PS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
//Ivan
			APS_LOG("### ALSPS_SET_PS_MODE enable = %d\n",enable);

			if(enable)
			{
				cm3607_power_down(0);			//Ivan			    
				if(err = cm3607_enable_ps(obj, true))
				{
					APS_ERR("enable ps fail: %d\n", err); 
					goto err_out;
				}
				set_bit(CMC_BIT_PS, &obj->enable);
			}
			else
			{	
				if(err = cm3607_enable_ps(obj, false))
				{
					APS_ERR("disable ps fail: %d\n", err); 
					goto err_out;
				}
				
				clear_bit(CMC_BIT_PS, &obj->enable);
				cm3607_power_down(1);			//Ivan				
			}
			break;

		case ALSPS_GET_PS_MODE:
			enable = test_bit(CMC_BIT_PS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
//Ivan
			APS_LOG("### ALSPS_GET_PS_MODE enable = %d\n",enable);

			break;

		case ALSPS_GET_PS_DATA:    
			if(err = cm3607_read_data( &obj->ps))
			{
				goto err_out;
			}
			dat = cm3607_get_ps_value(obj, obj->ps);
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
//Ivan
			APS_LOG("### ALSPS_GET_PS_DATA data = %d\n",dat);

			break;

		case ALSPS_GET_PS_RAW_DATA:    
			if(err = cm3607_read_data( &obj->ps))
			{
				goto err_out;
			}
			
			dat = obj->ps & 0x01;
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
//Ivan
			APS_LOG("### ALSPS_GET_PS_RAW_DATA data = %d\n",dat);

			break;            


		default:
			APS_ERR("%s not supported = 0x%04x", __FUNCTION__, cmd);
			err = -ENOIOCTLCMD;
			break;
	}

	err_out:
	return err;    
}
/*----------------------------------------------------------------------------*/
static struct file_operations cm3607_fops = {
	.owner = THIS_MODULE,
	.open = cm3607_open,
	.release = cm3607_release,
	.unlocked_ioctl = cm3607_unlocked_ioctl,
};
/*----------------------------------------------------------------------------*/
static struct miscdevice cm3607_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "als_ps",
	.fops = &cm3607_fops,
};

/*----------------------------------------------------------------------------*/
static void cm3607_early_suspend(struct early_suspend *h) 
{   /*early_suspend is only applied for ALS*/
	struct cm3607_priv *obj = container_of(h, struct cm3607_priv, early_drv);   
	int err;
	APS_FUN();    

	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}
/*
	atomic_set(&obj->als_suspend, 1);    
	if(err = cm3607_enable_als(obj->client, false))
	{
		APS_ERR("disable als fail: %d\n", err); 
	}
*/
}
/*----------------------------------------------------------------------------*/
static void cm3607_late_resume(struct early_suspend *h)
{   /*early_suspend is only applied for ALS*/
	struct cm3607_priv *obj = container_of(h, struct cm3607_priv, early_drv);         
	int err;
	APS_FUN();

	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}

}

int cm3607_ps_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	hwm_sensor_data* sensor_data;
	struct cm3607_priv *obj = (struct cm3607_priv *)self;
	
	//APS_FUN(f);
	switch (command)
	{
		case SENSOR_DELAY:
		    APS_LOG("##### PS SENSOR_DELAY!!\n");
		    
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{				
				value = *(int *)buff_in;
//Ivan
				APS_LOG("##### PS SENSOR_ENABLE value = %d\n",value);

				if(value)
				{
	    				cm3607_power_down(0);			//Ivan
					if(err = cm3607_enable_ps(obj, true))
					{
						APS_ERR("enable ps fail: %d\n", err); 
						return -1;
					}
					set_bit(CMC_BIT_PS, &obj->enable);
				}
				else
				{
					if(err = cm3607_enable_ps(obj, false))
					{
						APS_ERR("disable ps fail: %d\n", err); 
						return -1;
					}
					clear_bit(CMC_BIT_PS, &obj->enable);
					cm3607_power_down(1);			//Ivan
				}
			}
			break;

		case SENSOR_GET_DATA:
			//APS_LOG("fwq get ps data !!!!!!\n");
			APS_LOG("##### PS SENSOR_GET_DATA data++\n");
			
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
#ifdef ALSPS_POWER_DOWN_ENABLE
//Ivan added
/*
				while(atomic_read(&obj->ps_suspend))
				{
				    msleep(50);
				    APS_LOG("##### PS SENSOR_GET_DATA data Sensor suspend\n");
				}
*/
#endif

				sensor_data = (hwm_sensor_data *)buff_out;
				
				if(err = cm3607_read_data( &obj->ps))
				{
					err = -1;;
					APS_LOG("##### PS SENSOR_GET_DATA cm3607_read_data ERROR\n");
					
				}
				else
				{
				    while(-1 == cm3607_get_ps_value(obj, obj->ps))
				    {
				      cm3607_read_data(&obj->ps);
//				      APS_ERR("get sensor cm3607_get_ps_value error data = %d!\n",obj->ps);
				      msleep(50);
				    }
				   
					sensor_data->values[0] = cm3607_get_ps_value(obj, obj->ps);
					sensor_data->value_divide = 1;
					sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
					//APS_LOG("fwq get ps data =%d\n",sensor_data->values[0]);
				    
					
				}				
//Ivan
			    APS_LOG("##### PS SENSOR_GET_DATA data = %d\n", sensor_data->values[0]);

			}
			break;
		default:
			APS_ERR("proxmy sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}


/*----------------------------------------------------------------------------*/
static int cm3607_probe(struct platform_device *pdev) 
{
	struct alsps_hw *hw = get_cust_alsps_hw();
	//struct cm3607_i2c_addr addr;
	struct cm3607_priv *obj;
	struct hwmsen_object obj_ps, obj_als;
	int err = 0;

	mt_set_gpio_mode(GPIO_PS_EN_PIN, GPIO_MODE_GPIO);
	mt_set_gpio_dir(GPIO_PS_EN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_pull_enable(GPIO_PS_EN_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_PS_EN_PIN, GPIO_PULL_UP);

	cm3607_power(hw, 1);    
	mt_set_gpio_out(GPIO_PS_EN_PIN, 0);		//Low = operate; High = power down
	
//Ivan added

	if(!(obj = kzalloc(sizeof(*obj), GFP_KERNEL)))
	{
		err = -ENOMEM;
		goto exit;
	}
	memset(obj, 0, sizeof(*obj));
	cm3607_obj = obj;
//	g_cm3607_ptr = obj;
//Ivan remove??
	obj->hw = get_cust_alsps_hw();


	INIT_DELAYED_WORK(&obj->eint_work, cm3607_eint_work);
	atomic_set(&obj->als_debounce, 1000);
	atomic_set(&obj->als_deb_on, 0);
	atomic_set(&obj->als_deb_end, 0);
	atomic_set(&obj->ps_debounce, 1000);
	atomic_set(&obj->ps_deb_on, 0);
	atomic_set(&obj->ps_deb_end, 0);
	atomic_set(&obj->ps_mask, 0);
	atomic_set(&obj->trace, 0x00);
	atomic_set(&obj->als_suspend, 0);

	obj->ps_enable = 0;
	obj->als_enable = 0;
	obj->enable = 0;
	obj->pending_intr = 0;
	BUG_ON(sizeof(obj->als_level) != sizeof(obj->hw->als_level));
	BUG_ON(sizeof(obj->als_value) != sizeof(obj->hw->als_value));
	if((err = cm3607_setup_eint()))
	{
		APS_ERR("setup eint: %d\n", err);
		return err;
	}
	
	if(err = misc_register(&cm3607_device))
	{
		APS_ERR("cm3607_device register failed\n");
		goto exit_misc_device_register_failed;
	}

	if(err = cm3607_create_attr(&cm3607_alsps_driver.driver))
	{
		APS_ERR("create attribute err = %d\n", err);
		goto exit_create_attr_failed;
	}
	obj_ps.self = cm3607_obj;
	if(1 == obj->hw->polling_mode)
	{
	  obj_ps.polling = 1;
	}
	else
	{
	  obj_ps.polling = 0;//interrupt mode
	}
	
	obj_ps.polling = 0;
	obj_ps.sensor_operate = cm3607_ps_operate;
	if(err = hwmsen_attach(ID_PROXIMITY, &obj_ps))
	{
		APS_ERR("attach fail = %d\n", err);
		goto exit_create_attr_failed;
	}

#if defined(CONFIG_HAS_EARLYSUSPEND)
	obj->early_drv.level    = EARLY_SUSPEND_LEVEL_DISABLE_FB - 1,
	obj->early_drv.suspend  = cm3607_early_suspend,
	obj->early_drv.resume   = cm3607_late_resume,    
	register_early_suspend(&obj->early_drv);
#endif

	APS_LOG("%s: OK\n", __func__);
	return 0;

	exit_create_attr_failed:
	misc_deregister(&cm3607_device);
	exit_misc_device_register_failed:
	exit_init_failed:
	//i2c_detach_client(client);
	exit_kfree:
	kfree(obj);
	exit:
//	cm3607_i2c_client = NULL;           
	#ifdef MT6516        
	MT6516_EINTIRQMask(CUST_EINT_ALS_NUM);  /*mask interrupt if fail*/
	#endif
	APS_ERR("%s: err = %d\n", __func__, err);
	return err;
	//cm3607_get_addr(hw, &addr);
/*	
	cm3607_force[0] = hw->i2c_num;
	//cm3607_force[1] = addr.init;
	if(i2c_add_driver(&cm3607_i2c_driver))
	{
		APS_ERR("add driver error\n");
		return -1;
	} 
*/	
//	return 0;
}
/*----------------------------------------------------------------------------*/
static int cm3607_remove(struct platform_device *pdev)
{
	struct alsps_hw *hw = get_cust_alsps_hw();
	APS_FUN();    
	cm3607_power(hw, 0);    
//	i2c_del_driver(&cm3607_i2c_driver);
	return 0;
}
/*----------------------------------------------------------------------------*/
static struct platform_driver cm3607_alsps_driver = {
	.probe      = cm3607_probe,
	.remove     = cm3607_remove,    
	.driver     = {
		.name  = "als_ps",
		.owner = THIS_MODULE,
	}
};
/*----------------------------------------------------------------------------*/
static int __init cm3607_init(void)
{
	APS_FUN();
	if(platform_driver_register(&cm3607_alsps_driver))
	{
		APS_ERR("failed to register driver");
		return -ENODEV;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static void __exit cm3607_exit(void)
{
	APS_FUN();
	platform_driver_unregister(&cm3607_alsps_driver);
}
/*----------------------------------------------------------------------------*/
module_init(cm3607_init);
module_exit(cm3607_exit);
/*----------------------------------------------------------------------------*/
MODULE_AUTHOR("MingHsien Hsieh");
MODULE_DESCRIPTION("ADXL345 accelerometer driver");
MODULE_LICENSE("GPL");
