/* 
 *
 * (C) Copyright 2009 
 * MediaTek <www.mediatek.com>
 * James Lo <james.lo@mediatek.com>
 *
 * mt6329 Bank1 I2C Device Driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/i2c.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/wakelock.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/aee.h>
#include <linux/xlog.h>
#include <linux/proc_fs.h>

#include <mach/mt6577_typedefs.h>
#include <mach/eint.h>
#include <mach/mt_gpio.h>
#include <mach/mt6577_clock_manager.h>

#include <asm/uaccess.h>

#include <mach/pmic_mt6329_hw_bank1.h> 
#include <mach/pmic_mt6329_sw_bank1.h> 
#include <mach/pmic_mt6329_hw.h>
#include <mach/pmic_mt6329_sw.h>
#include <mach/upmu_common_sw.h>
#include <mach/upmu_hw.h>

#include <mtk_kpd.h>

#include <mach/mtk_rtc.h>

#include "mt6577_battery.h"
#include "linux/leds-mt65xx.h"

#include <mach/mt6577_pm_ldo.h>
#include <mach/mt6577_clock_manager.h>
#include <mach/mt6577_pmic_feature_api.h>

#include <cust_eint.h>

#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/writeback.h>
#include <asm/uaccess.h>

#define PMIC_E2_DVT 0

//////////////////////////////////////////////////////////////////////////////////////////
// Extern 
//////////////////////////////////////////////////////////////////////////////////////////
extern int g_chr_event;
extern struct wake_lock battery_suspend_lock;
extern void wake_up_bat (void);
extern int bat_volt_check_point;

/**********************************************************
  *
  *   [Chip specific XXX list] 
  *
  *********************************************************/

//////////////////////////////////////////////////////////////////////////////////////////
// EINT Setting. If cust_eint.h and cust_gpio.h ready, please use it.
//////////////////////////////////////////////////////////////////////////////////////////
#if 0
#define CUST_EINT_POLARITY_LOW              0
#define CUST_EINT_POLARITY_HIGH             1
#define CUST_EINT_DEBOUNCE_DISABLE          0
#define CUST_EINT_DEBOUNCE_ENABLE           1
#define CUST_EINT_EDGE_SENSITIVE            0
#define CUST_EINT_LEVEL_SENSITIVE           1

//#define CUST_EINT_MT6329_PMIC_NUM              0 // FPGA

#if defined(CONFIG_POWER_EXT) 
#define CUST_EINT_MT6329_PMIC_NUM              8
#else
#define CUST_EINT_MT6329_PMIC_NUM              9
#endif
#define CUST_EINT_MT6329_PMIC_DEBOUNCE_CN      1
#define CUST_EINT_MT6329_PMIC_POLARITY         CUST_EINT_POLARITY_HIGH
#define CUST_EINT_MT6329_PMIC_SENSITIVE        CUST_EINT_EDGE_SENSITIVE
#define CUST_EINT_MT6329_PMIC_DEBOUNCE_EN      CUST_EINT_DEBOUNCE_ENABLE
#endif

#if 0
#define GPIO_PMIC_EINT_PIN         GPIO59
#define GPIO_PMIC_EINT_PIN_M_GPIO  GPIO_MODE_00
#define GPIO_PMIC_EINT_PIN_M_EINT  GPIO_MODE_01
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// ECO CODE
//////////////////////////////////////////////////////////////////////////////////////////
#define PMIC6329_E1_CID_CODE    0x0029
#define PMIC6329_E2_CID_CODE    0x1029

/**********************************************************
  *
  *   [I2C Slave Setting] 
  *
  *********************************************************/
#if 0
#define mt6329_BANK0_SLAVE_ADDR_WRITE	0xC0
#define mt6329_BANK0_SLAVE_ADDR_Read	0xC1
#define mt6329_BANK1_SLAVE_ADDR_WRITE	0xC2
#define mt6329_BANK1_SLAVE_ADDR_Read	0xC3

/* Addresses to scan */
//static unsigned short normal_i2c[] = { mt6329_BANK1_SLAVE_ADDR_WRITE,  I2C_CLIENT_END }; // correct
//static unsigned short normal_i2c[] = { mt6329_BANK0_SLAVE_ADDR_WRITE,  I2C_CLIENT_END }; // test

//static unsigned short ignore = I2C_CLIENT_END;

static struct i2c_client *new_client = NULL;

static const struct i2c_device_id mt6329_bank1_i2c_id[] = {{"mt6329_pmic_bank1",0},{}};

//static unsigned short force[] = {2, 0xC0, I2C_CLIENT_END, I2C_CLIENT_END}; // id=2 for using I2C dual
static unsigned short force[] = {2, mt6329_BANK1_SLAVE_ADDR_WRITE, I2C_CLIENT_END, I2C_CLIENT_END}; // id=2 for using I2C dual // correct
//static unsigned short force[] = {2, mt6329_BANK0_SLAVE_ADDR_WRITE, I2C_CLIENT_END, I2C_CLIENT_END}; // id=2 for using I2C dual // test

static const unsigned short * const forces[] = { force, NULL };              
static struct i2c_client_address_data addr_data = { .forces = forces,}; 
#endif
//static int mt6329_bank1_driver_detect(struct i2c_client *client, int kind, struct i2c_board_info *info);
static int mt6329_bank1_driver_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int mt6329_bank1_driver_remove(struct i2c_client *client);

static struct i2c_client *new_client = NULL;

static const struct i2c_device_id mt6329_bank1_i2c_id[] = 
{
	{"mt6329_pmic_bank1",0},
	{}
};

struct i2c_driver mt6329_bank1_driver = {                       
    .probe = mt6329_bank1_driver_probe,                                   
    .remove = mt6329_bank1_driver_remove, 
    .driver = {
		.name = "mt6329_pmic_bank1",
	},               
    .id_table = mt6329_bank1_i2c_id,                             
                       
};

/**********************************************************
  *
  *   [I2C Function For Read/Write PMIC] 
  *
  *********************************************************/

static DEFINE_MUTEX(pmic_i2c_access_mutex_1);

ssize_t mt6329_bank1_read_byte(u8 cmd, u8 *returnData)
{
    char     cmd_buf[1]={0x00};
    char     readData = 0;
    int     ret=0;

	mutex_lock(&pmic_i2c_access_mutex_1);
	
#if 0
    cmd_buf[0] = cmd;
    ret = i2c_master_send(new_client, &cmd_buf[0], 1);
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_bank1_read_byte] PMIC sends command error!! \n");	
        return 0;
    }
    ret = i2c_master_recv(new_client, &readData, 1);
    if (ret < 0) 
	{	
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_bank1_read_byte] PMIC reads data error!! \n");	
        return 0;
    } 
    //xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[mt6329_bank1_read_byte] Reg[%x]=0x%x \n", cmd, readData);
    *returnData = readData;
#else
	new_client->addr = ((new_client->addr) & I2C_MASK_FLAG) | I2C_WR_FLAG;	

	cmd_buf[0] = cmd;
    ret = i2c_master_send(new_client, &cmd_buf[0], (1<<8 | 1));
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_bank1_read_byte:write&read] PMIC sends command error!! \n");	
		new_client->addr = new_client->addr & I2C_MASK_FLAG;
		mutex_unlock(&pmic_i2c_access_mutex_1);
        return 0;
    }
	readData = cmd_buf[0];
    //xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[mt6329_bank1_read_byte:write&read] Reg[%x]=0x%x \n", cmd, readData);
    *returnData = readData;

	new_client->addr = new_client->addr & I2C_MASK_FLAG;
#endif

	mutex_unlock(&pmic_i2c_access_mutex_1);
    return 1;
}

int mt6329_bank1_multi_read_byte(u8 cmd, u8 *returnData, int len)
{
    char cmd_buf[1]={0x00};
    char readData[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	int index=0;
    int ret=0;

    cmd_buf[0] = cmd;
    ret = i2c_master_send(new_client, &cmd_buf[0], 1);
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_bank1_multi_read_byte] PMIC sends command error!! \n");	
        return 0;
    } 
	ret = i2c_master_recv(new_client, readData, len);
    if (ret < 0) {
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_bank1_multi_read_byte] PMIC reads data error!! \n");
        return 0;
    }
	
    for(index=0 ; index<len ; index++)
    {
    	returnData[index] = readData[index];
    }
    
    return 1;
}

ssize_t mt6329_bank1_write_byte(u8 cmd, u8 writeData)
{
    char    write_data[2] = {0};
    int    ret=0;
    
	mutex_lock(&pmic_i2c_access_mutex_1);
	
    write_data[0] = cmd;
    write_data[1] = writeData;
    
    ret = i2c_master_send(new_client, write_data, 2);
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_bank1_write_byte] sends command error!! \n");
		mutex_unlock(&pmic_i2c_access_mutex_1);
        return 0;
    }
    
	mutex_unlock(&pmic_i2c_access_mutex_1);
    return 1;
}

int mt6329_bank1_multi_write_byte(u8 *cmd, u8 *writeData, int len)
{
    char write_data[8] = {0};
	int count=0;
	int index=0;
    int ret=0;

	for(count=0 ; count<len ; count++)
	{
    	write_data[index] = cmd[count];
		index++;
    	write_data[index] = writeData[count];
		index++;
	}

	//check
	for(index=0 ; index<8 ; index++)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[mt6329_bank1_multi_write_byte] %x\n", write_data[index]);
	}
	
    ret = i2c_master_send(new_client, write_data, len*2);
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_bank1_multi_write_byte] sends command error!! \n");
        return 0;
    }
    
    return 1;
}

/**********************************************************
  *
  *   [PMIC Variable] 
  *
  *********************************************************/
kal_uint8 pmic6329_bank1_reg[PMIC_REG_NUM] = {0};

/**********************************************************
  *
  *   [PMIC Function] 
  *
  *********************************************************/
extern int mt_i2c_polling_read(int port, unsigned char addr, unsigned char *buffer, int len);
extern int mt_i2c_polling_write(int port, unsigned char addr, unsigned char *buffer, int len);
extern int mt_i2c_polling_writeread(int port, unsigned char addr, unsigned char *buffer, int write_len, int read_len);

extern int g_switch_to_i2c_polling_mode;

kal_uint32 pmic_bank1_read_interface (kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	//kal_uint32 ret_code = PMIC_BANK1_TEST_PASS;
	kal_uint8 pmic6329_reg = 0;

	kal_uint8 chip_slave_address = 0xC2;
    kal_uint8 buffer = 0x0;    
    int result_tmp;

if(g_switch_to_i2c_polling_mode == 0)
{
	mt6329_bank1_read_byte(RegNum, &pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_read_interface] Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
	
	pmic6329_reg &= (MASK << SHIFT);
	*val = (pmic6329_reg >> SHIFT);
	
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_read_interface] val=0x%x\n", *val);
}
else
{
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "@r");
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_read_interface] g_switch_to_i2c_polling_mode = 1\r\n");
	
	buffer = RegNum;	    
	result_tmp = mt_i2c_polling_writeread(2,chip_slave_address,&buffer,1,1);

    buffer &= (MASK << SHIFT);
    *val = (buffer >> SHIFT);

	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_read_interface] Reg[0x%x]=0x%x, val=0x%x\n", RegNum, buffer, *val);
}

	return PMIC_BANK1_TEST_PASS;
}

kal_uint32 pmic_bank1_config_interface (kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	//kal_uint32 ret_code = PMIC_BANK1_TEST_PASS;
	kal_uint8 pmic6329_reg = 0;

	kal_uint8 chip_slave_address = 0xC2;
    kal_uint8 buffer = 0x0;    
    int result_tmp;
	kal_uint8 write_data[2];

if(g_switch_to_i2c_polling_mode == 0)
{
	mt6329_bank1_read_byte(RegNum, &pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_config_interface] Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
	
	pmic6329_reg &= ~(MASK << SHIFT);
	pmic6329_reg |= (val << SHIFT);

	mt6329_bank1_write_byte(RegNum, pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_config_interface] write Reg[%x]=0x%x\n", RegNum, pmic6329_reg);

	// Check
	//mt6329_bank1_read_byte(RegNum, &pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_config_interface] Check Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
}
else
{
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "@c");
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_config_interface] g_switch_to_i2c_polling_mode = 1\r\n");

	buffer = RegNum;
	result_tmp = mt_i2c_polling_writeread(2,chip_slave_address,&buffer,1,1);
	
	buffer &= ~(MASK << SHIFT);
	buffer |= (val << SHIFT);
	pmic6329_reg = buffer;

	write_data[0] = RegNum;
    write_data[1] = pmic6329_reg;
	
	result_tmp = mt_i2c_polling_write(2,chip_slave_address,write_data,2);

	//Check
	//buffer = RegNum;
	//result_tmp = mt_i2c_polling_writeread(2,chip_slave_address,&buffer,1,1);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_config_interface] Reg[0x%x]=0x%x, val=0x%x\n", RegNum, buffer, pmic6329_reg);
}

	return PMIC_BANK1_TEST_PASS;
}

kal_uint32 pmic_bank1_int_en_interface (kal_uint8 RegNum, kal_uint8 sel, kal_bool enable)
{
	//kal_uint32 ret_code = PMIC_BANK1_TEST_PASS;
	kal_uint8 pmic6329_reg = 0;

	mt6329_bank1_read_byte(RegNum, &pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_int_en_interface] Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
	
	if (enable){
		pmic6329_reg |= (U8)sel;
	}else{
		pmic6329_reg &= ~((U8)sel);
	}
	
	mt6329_bank1_write_byte(RegNum, pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_int_en_interface] write Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
	
	// Check
	mt6329_bank1_read_byte(RegNum, &pmic6329_reg);
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_int_en_interface] Check Reg[%x]=0x%x\n", RegNum, pmic6329_reg);

	return PMIC_BANK1_TEST_PASS;
}

// only for test
void pmic_get_chip_version_bank1_test(void)
{
    kal_uint16 eco_version = 0;
    kal_uint8 tmp8;
    kal_bool result_tmp;
    
    /* Low part of CID */
	result_tmp = mt6329_bank1_read_byte(0x0, &tmp8);	
    eco_version |= tmp8;

    /* High part of CID */
    result_tmp = mt6329_bank1_read_byte(0x1, &tmp8);	
    eco_version |= (tmp8 << 8);

#if 1	
    if (eco_version == PMIC6326_E1_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_bank1_test] PMIC6326_ECO_1_VERSION !!\n ");
    }
    else if (eco_version == PMIC6326_E2_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_bank1_test] PMIC6326_ECO_2_VERSION !!\n ");
    }
    else if (eco_version == PMIC6326_E3_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_bank1_test] PMIC6326_ECO_3_VERSION !!\n ");
    }
    else if (eco_version == PMIC6326_E4_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_bank1_test] PMIC6326_ECO_4_VERSION !!\n ");
    }
    else
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_bank1_test] UNKNOWN VERSION !!\n ");
    }
#endif	
	
}

void pmic_dump_register_bank1(void)
{
	int i=0;
    for (i=0;i<PMIC_REG_NUM;i++)
	{
		mt6329_bank1_read_byte(i, &pmic6329_bank1_reg[i]);
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_dump_register_bank1] Reg[0x%X]=0x%X\n", i, pmic6329_bank1_reg[i]);        
    }
}

int pmic_interface_test_bank1(void)
{	
	kal_uint32 ret=0;
	kal_uint8 temp_val;

	//For MT6326
#if 1	
	ret=pmic_bank1_read_interface(0x0, &temp_val, 0xFF, 0x0);
	if(temp_val == 0xb)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test_bank1] pmic_read_interface : PASS\n");		
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test_bank1] pmic_read_interface : FAIL(%d)\n", temp_val);
		return 0;
	}
	
	ret=pmic_bank1_config_interface(0x81, 0x3, 0x7, 0x4);
	ret=pmic_bank1_read_interface(0x81, &temp_val, 0x7, 0x4);
	if(temp_val == 0x3)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test_bank1] pmic_config_interface : PASS\n");
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test_bank1] pmic_config_interface : FAIL(%d)\n", temp_val);
		return 0;
	}

	ret=pmic_bank1_int_en_interface(PMIC_INIT_CTRL_1, INT1_EN_ALL, KAL_FALSE);
	ret=pmic_bank1_read_interface(PMIC_INIT_CTRL_1, &temp_val, 0xFF, 0x0);
	if(temp_val == 0x0)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test_bank1] pmic_int_en_interface : PASS\n");
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test_bank1] pmic_int_en_interface : FAIL(%d)\n", temp_val);
		return 0;
	}

	ret=pmic_bank1_int_en_interface(PMIC_INIT_CTRL_1, INT1_EN_ALL, KAL_TRUE);
	ret=pmic_bank1_read_interface(PMIC_INIT_CTRL_1, &temp_val, 0xFF, 0x0);
	if(temp_val == 0xFF)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test_bank1] pmic_int_en_interface : PASS\n");
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test_bank1] pmic_int_en_interface : FAIL(%d)\n", temp_val);
		return 0;
	}
#endif

	return 0;
}

void pmic_bank1_test(void)
{
	int val=0;

	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_test]\n");

	val=upmu_get_reg_value_bank1(0x00); 
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_test] Reg[0x00]=%x\n", val);

	val=upmu_get_reg_value_bank1(0x01); 
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_test] Reg[0x01]=%x\n", val);	
}

void pmic_bank1_testmode_test(void)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_testmode_test]\n");
	
	upmu_testmode_pchr_man_rst(0x1);	
}

void pmic_bank1_top2_test(void)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_top2_test]\n");

	upmu_top2_oc_gear_vbaton_undet(0x1);
	upmu_top2_polarity_vbaton_undet(0x1);
	upmu_top2_vrf18_oc_thd(0x3);
}

void pmic_bank1_driver_test(void)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_driver_test]\n");

	upmu_flash_sfstr_time(0x3);
	upmu_kpled_mode_select(0x1);
	upmu_isinks_ch0_step(0x5);
	upmu_boost_cs(0x2);
}

void pmic_bank1_spk_test(void)
{
	int ret=0;

	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_spk_test]\n");

	upmu_spk_offset_l_sw(0x3);
	ret=upmu_spk_trim_done_r();
	upmu_asw_ana_sw_sel(0x1);
	
}

void pmic_bank1_fgpll_test(void)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_fgpll_test]\n");

	upmu_fgpll_bp(0x2);
}

void pmic_bank1_fgadc_test(void)
{
	int ret=0;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_fgadc_test]\n");
	
	upmu_fgadc_fgadc_en_sw(0x1);
	ret=upmu_fgadc_l_int_sts();
}

void pmic_bank1_otpc_test(void)
{
	int ret=0;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_otpc_test]\n");

	upmu_otpc_otp_w_lock_key(0x7);
	ret=upmu_otpc_otp_w_lock();
}

void pmic_bank1_i2c_test(void)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_bank1_i2c_test]\n");

	upmu_i2c_scl_deglitch(0x5);
	upmu_i2c_sdin_deglitch(0x6);
}

void PMIC_COMMON_SETTING(void)
{
	kal_uint32 ret=0;
	
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] Digital clock related initial setting\n");		
	ret=pmic_bank1_config_interface(BANK1_TOP2_CON1, 0x1, BANK_1_RG_FGADC_CK_PDN_MASK, BANK_1_RG_FGADC_CK_PDN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_TOP2_CON1, 0x1, BANK_1_RG_BST_DRV_CK_PDN_MASK, BANK_1_RG_BST_DRV_CK_PDN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_TOP2_CON1, 0x1, BANK_1_RG_SPK_CK_PDN_MASK, BANK_1_RG_SPK_CK_PDN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_TOP2_CON1, 0x1, BANK_1_RG_PWMOC_CK_PDN_MASK, BANK_1_RG_PWMOC_CK_PDN_SHIFT);

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] Charger watchdog timer related initial setting\n");
	ret=pmic_config_interface(BANK0_CHR_COND, 0x3, BANK_0_CHRWDT_TD_MASK, BANK_0_CHRWDT_TD_SHIFT);
	ret=pmic_config_interface(BANK0_CHR_COND, 0x1, BANK_0_CHRWDT_EN_MASK, BANK_0_CHRWDT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON0, 0x1, BANK_0_RG_WATCHDOG_INT_EN_MASK, BANK_0_RG_WATCHDOG_INT_EN_SHIFT);

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] RG_VPROC_CTRL\n");
	ret=pmic_config_interface(BANK0_VPROC_CONC, 0x0, BANK_0_RG_VPROC_CTRL_MASK, BANK_0_RG_VPROC_CTRL_SHIFT);
	ret=pmic_config_interface(BANK0_VPROC_CON5, 0x14, BANK_0_RG_VPROC_VOSEL_MASK, BANK_0_RG_VPROC_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_VPROC_CON6, 0x14, BANK_0_VPROC_VOSEL_SRCLKEN0_MASK, BANK_0_VPROC_VOSEL_SRCLKEN0_SHIFT);
	ret=pmic_config_interface(BANK0_VPROC_CON7, 0x14, BANK_0_VPROC_VOSEL_SRCLKEN1_MASK, BANK_0_VPROC_VOSEL_SRCLKEN1_SHIFT);
	ret=pmic_config_interface(BANK0_VPROC_CON8, 0x14, BANK_0_VPROC_VOSEL_DVS00_MASK, BANK_0_VPROC_VOSEL_DVS00_SHIFT);
	ret=pmic_config_interface(BANK0_VPROC_CON9, 0x14, BANK_0_VPROC_VOSEL_DVS01_MASK, BANK_0_VPROC_VOSEL_DVS01_SHIFT);
	ret=pmic_config_interface(BANK0_VPROC_CONA, 0x14, BANK_0_VPROC_VOSEL_DVS10_MASK, BANK_0_VPROC_VOSEL_DVS10_SHIFT);
	ret=pmic_config_interface(BANK0_VPROC_CONB, 0x14, BANK_0_VPROC_VOSEL_DVS11_MASK, BANK_0_VPROC_VOSEL_DVS11_SHIFT);
	ret=pmic_config_interface(BANK0_VPROC_CONC, 0x0, BANK_0_RG_VPROC_VOSEL_SFCHG_EN_MASK, BANK_0_RG_VPROC_VOSEL_SFCHG_EN_SHIFT);
	ret=pmic_config_interface(BANK0_VPROC_CON4, 0x1, BANK_0_RG_VPROC_NDIS_EN_MASK, BANK_0_RG_VPROC_NDIS_EN_SHIFT);
	
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] Soft Star interval\n");
	ret=pmic_config_interface(BANK0_VRF18_CON4, 0x1, BANK_0_RG_VRF18_STBTD_MASK, BANK_0_RG_VRF18_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON2, 0x1, BANK_0_RG_VM12_1_STBTD_MASK, BANK_0_RG_VM12_1_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON6, 0x1, BANK_0_RG_VM12_2_STBTD_MASK, BANK_0_RG_VM12_2_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CONC, 0x1, BANK_0_RG_VM12_INT_STBTD_MASK, BANK_0_RG_VM12_INT_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON11, 0x1, BANK_0_RG_VIO28_STBTD_MASK, BANK_0_RG_VIO28_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON14, 0x1, BANK_0_RG_VSIM_STBTD_MASK, BANK_0_RG_VSIM_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON17, 0x1, BANK_0_RG_VSIM2_STBTD_MASK, BANK_0_RG_VSIM2_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON1A, 0x1, BANK_0_RG_VUSB_STBTD_MASK, BANK_0_RG_VUSB_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON1D, 0x1, BANK_0_RG_VCAMD_STBTD_MASK, BANK_0_RG_VCAMD_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON20, 0x1, BANK_0_RG_VCAM_IO_STBTD_MASK, BANK_0_RG_VCAM_IO_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON23, 0x1, BANK_0_RG_VCAM_AF_STBTD_MASK, BANK_0_RG_VCAM_AF_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON26, 0x1, BANK_0_RG_VMC_STBTD_MASK, BANK_0_RG_VMC_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON2A, 0x1, BANK_0_RG_VMCH_STBTD_MASK, BANK_0_RG_VMCH_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON2E, 0x1, BANK_0_RG_VGP_STBTD_MASK, BANK_0_RG_VGP_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON31, 0x1, BANK_0_RG_VGP2_STBTD_MASK, BANK_0_RG_VGP2_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON34, 0x1, BANK_0_RG_VIBR_STBTD_MASK, BANK_0_RG_VIBR_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_ANALDO_CON2, 0x1, BANK_0_RG_VRF_STBTD_MASK, BANK_0_RG_VRF_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_ANALDO_CON5, 0x1, BANK_0_RG_VTCXO_STBTD_MASK, BANK_0_RG_VTCXO_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_ANALDO_CON8, 0x1, BANK_0_RG_VA1_STBTD_MASK, BANK_0_RG_VA1_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_ANALDO_CONC, 0x1, BANK_0_RG_VA2_STBTD_MASK, BANK_0_RG_VA2_STBTD_SHIFT);
	ret=pmic_config_interface(BANK0_ANALDO_CONF, 0x1, BANK_0_RG_VCAMA_STBTD_MASK, BANK_0_RG_VCAMA_STBTD_SHIFT);

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] Over current Gear\n");
	ret=pmic_bank1_config_interface(BANK1_CC_CTL1, 0x1, BANK_1_OC_GEAR_LDO_MASK, BANK_1_OC_GEAR_LDO_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_OC_PWMCTL1, 0x0, BANK_1_VPA_OC_WND_MASK, BANK_1_VPA_OC_WND_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_OC_PWMCTL1, 0x0, BANK_1_VPA_OC_THD_MASK, BANK_1_VPA_OC_THD_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_OC_PWMCTL1, 0x0, BANK_1_VRF18_OC_WND_MASK, BANK_1_VRF18_OC_WND_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_OC_PWMCTL1, 0x0, BANK_1_VRF18_OC_THD_MASK, BANK_1_VRF18_OC_THD_SHIFT);

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] SPK auto trim function\n");
	ret=pmic_bank1_config_interface(BANK1_TOP2_CON1, 0x0, BANK_1_RG_SPK_CK_PDN_MASK, BANK_1_RG_SPK_CK_PDN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON0, 0x1, BANK_1_SPK_TRIM_EN_L_MASK, BANK_1_SPK_TRIM_EN_L_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON0, 0x1, BANK_1_SPKMODE_L_MASK, BANK_1_SPKMODE_L_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON6, 0x1, BANK_1_SPK_TRIM_EN_R_MASK, BANK_1_SPK_TRIM_EN_R_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON6, 0x1, BANK_1_SPKMODE_R_MASK, BANK_1_SPKMODE_R_SHIFT);
	//later
	ret=pmic_bank1_config_interface(BANK1_SPK_CON0, 0x1, BANK_1_SPK_EN_L_MASK, BANK_1_SPK_EN_L_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON6, 0x1, BANK_1_SPK_EN_R_MASK, BANK_1_SPK_EN_R_SHIFT);

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] Interrupt\n");
	ret=pmic_config_interface(BANK0_INT_CON0, 0x1, BANK_0_RG_OV_INT_EN_MASK, BANK_0_RG_OV_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON0, 0x1, BANK_0_RG_CHRDET_INT_EN_MASK, BANK_0_RG_CHRDET_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON0, 0x1, BANK_0_RG_BVALID_DET_INT_EN_MASK, BANK_0_RG_BVALID_DET_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON0, 0x1, BANK_0_RG_VBATON_UNDET_INT_EN_MASK, BANK_0_RG_VBATON_UNDET_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON0, 0x0, BANK_0_RG_THR_H_INT_EN_MASK, BANK_0_RG_THR_H_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON0, 0x0, BANK_0_RG_THR_L_INT_EN_MASK, BANK_0_RG_THR_L_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON0, 0x1, BANK_0_RG_PWRKEY_INT_EN_MASK, BANK_0_RG_PWRKEY_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON0, 0x1, BANK_0_RG_WATCHDOG_INT_EN_MASK, BANK_0_RG_WATCHDOG_INT_EN_SHIFT);
	
	ret=pmic_config_interface(BANK0_INT_CON1, 0x0, BANK_0_RG_FG_BAT_H_INT_EN_MASK, BANK_0_RG_FG_BAT_H_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON1, 0x0, BANK_0_RG_FG_BAT_L_INT_EN_MASK, BANK_0_RG_FG_BAT_L_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON1, 0x0, BANK_0_RG_HIGH_BAT_INT_EN_MASK, BANK_0_RG_HIGH_BAT_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON1, 0x0, BANK_0_RG_LOW_BAT_INT_EN_MASK, BANK_0_RG_LOW_BAT_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON1, 0x0, BANK_0_RG_SPKR_D_OC_INT_EN_MASK, BANK_0_RG_SPKR_D_OC_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON1, 0x0, BANK_0_RG_SPKL_D_OC_INT_EN_MASK, BANK_0_RG_SPKL_D_OC_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON1, 0x0, BANK_0_RG_SPKR_AB_OC_INT_EN_MASK, BANK_0_RG_SPKR_AB_OC_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON1, 0x0, BANK_0_RG_SPKL_AB_OC_INT_EN_MASK, BANK_0_RG_SPKL_AB_OC_INT_EN_SHIFT);

	ret=pmic_config_interface(BANK0_INT_CON2, 0x0, BANK_0_RG_HOMEKEY_INT_EN_MASK, BANK_0_RG_HOMEKEY_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON2, 0x1, BANK_0_RG_LDO_OC_INT_EN_MASK, BANK_0_RG_LDO_OC_INT_EN_SHIFT);

	ret=pmic_bank1_config_interface(BANK1_TOP2_CON1, 0x1, BANK_1_RG_PWMOC_CK_PDN_MASK, BANK_1_RG_PWMOC_CK_PDN_SHIFT);	
	ret=pmic_config_interface(BANK0_INT_CON2, 0x1, BANK_0_RG_VRF18_OC_INT_EN_MASK, BANK_0_RG_VRF18_OC_INT_EN_SHIFT);
	ret=pmic_config_interface(BANK0_INT_CON2, 0x1, BANK_0_RG_VPA_OC_INT_EN_MASK, BANK_0_RG_VPA_OC_INT_EN_SHIFT);	
	
}

void PMIC_CUSTOM_SETTING(void)
{
	kal_uint32 ret=0;
	
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] Charger and OTG control\n");	
	ret=pmic_config_interface(0x81, 0x2, 0x7, 0x4);
	ret=pmic_config_interface(0x81, 0x0, 0x7, 0x0);

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] Voltage and Current select\n");
	ret=pmic_config_interface(BANK0_ANALDO_CONE, 0x3, BANK_0_RG_VCAMA_VOSEL_MASK, BANK_0_RG_VCAMA_VOSEL_SHIFT);	
	ret=pmic_config_interface(BANK0_DIGLDO_CON1C, 0x1, BANK_0_RG_VCAMD_VOSEL_MASK, BANK_0_RG_VCAMD_VOSEL_SHIFT);	
	ret=pmic_config_interface(BANK0_DIGLDO_CON30, 0x4, BANK_0_RG_VGP2_VOSEL_MASK, BANK_0_RG_VGP2_VOSEL_SHIFT);	
	ret=pmic_config_interface(BANK0_DIGLDO_CON2D, 0x4, BANK_0_RG_VGP_VOSEL_MASK, BANK_0_RG_VGP_VOSEL_SHIFT);	

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] Analog switch select\n");
	ret=pmic_bank1_config_interface(BANK1_ASW_CON0, 0x0, BANK_1_RG_ANA_SW_SEL_MASK, BANK_1_RG_ANA_SW_SEL_SHIFT);

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] Black light\n");
	ret=pmic_bank1_config_interface(BANK1_TOP2_CON1, 0x1, BANK_1_RG_BST_DRV_CK_PDN_MASK, BANK_1_RG_BST_DRV_CK_PDN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_BOOST_CON0, 0x0, BANK_1_BOOST_ISINK_HW_SEL_MASK, BANK_1_BOOST_ISINK_HW_SEL_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_BOOST_CON0, 0x0, BANK_1_BOOST_MODE_MASK, BANK_1_BOOST_MODE_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_BOOST_CON0, 0x0, BANK_1_BOOST_CABC_EN_MASK, BANK_1_BOOST_CABC_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_BOOST_CON0, 0x0, BANK_1_BOOST_EN_MASK, BANK_1_BOOST_EN_SHIFT);
	
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON0, 0x0, BANK_1_ISINK_DIM0_DUTY_MASK, BANK_1_ISINK_DIM0_DUTY_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON1, 0xB, BANK_1_ISINK_DIM0_FSEL_MASK, BANK_1_ISINK_DIM0_FSEL_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON2, 0x0, BANK_1_ISINK_DIM1_DUTY_MASK, BANK_1_ISINK_DIM1_DUTY_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON3, 0x8, BANK_1_ISINK_DIM1_FSEL_MASK, BANK_1_ISINK_DIM1_FSEL_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON4, 0x0, BANK_1_ISINK_DIM2_DUTY_MASK, BANK_1_ISINK_DIM2_DUTY_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON5, 0x8, BANK_1_ISINK_DIM2_FSEL_MASK, BANK_1_ISINK_DIM2_FSEL_SHIFT);
	
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON6, 0x0, BANK_1_ISINKS_CH5_EN_MASK, BANK_1_ISINKS_CH5_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON6, 0x0, BANK_1_ISINKS_CH4_EN_MASK, BANK_1_ISINKS_CH4_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON6, 0x0, BANK_1_ISINKS_CH3_EN_MASK, BANK_1_ISINKS_CH3_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON6, 0x0, BANK_1_ISINKS_CH2_EN_MASK, BANK_1_ISINKS_CH2_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON6, 0x0, BANK_1_ISINKS_CH1_EN_MASK, BANK_1_ISINKS_CH1_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON6, 0x0, BANK_1_ISINKS_CH0_EN_MASK, BANK_1_ISINKS_CH0_EN_SHIFT);

	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON7, 0x0, BANK_1_ISINKS_CH5_CABC_EN_MASK, BANK_1_ISINKS_CH5_CABC_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON7, 0x0, BANK_1_ISINKS_CH4_CABC_EN_MASK, BANK_1_ISINKS_CH4_CABC_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON7, 0x0, BANK_1_ISINKS_CH3_CABC_EN_MASK, BANK_1_ISINKS_CH3_CABC_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON7, 0x0, BANK_1_ISINKS_CH2_CABC_EN_MASK, BANK_1_ISINKS_CH2_CABC_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON7, 0x0, BANK_1_ISINKS_CH1_CABC_EN_MASK, BANK_1_ISINKS_CH1_CABC_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON7, 0x0, BANK_1_ISINKS_CH0_CABC_EN_MASK, BANK_1_ISINKS_CH0_CABC_EN_SHIFT);

	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON8, 0x0, BANK_1_ISINKS_CH0_MODE_MASK, BANK_1_ISINKS_CH0_MODE_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON9, 0x0, BANK_1_ISINKS_CH1_MODE_MASK, BANK_1_ISINKS_CH1_MODE_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON10, 0x0, BANK_1_ISINKS_CH2_MODE_MASK, BANK_1_ISINKS_CH2_MODE_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON11, 0x0, BANK_1_ISINKS_CH3_MODE_MASK, BANK_1_ISINKS_CH3_MODE_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON12, 0x0, BANK_1_ISINKS_CH4_MODE_MASK, BANK_1_ISINKS_CH4_MODE_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_ISINKS_CON13, 0x0, BANK_1_ISINKS_CH5_MODE_MASK, BANK_1_ISINKS_CH5_MODE_SHIFT);

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_COMMON_SETTING] SPK select\n");
	ret=pmic_bank1_config_interface(BANK1_SPK_CON0, 0x0, BANK_1_SPK_THER_SHDN_L_EN_MASK, BANK_1_SPK_THER_SHDN_L_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON6, 0x0, BANK_1_SPK_THER_SHDN_R_EN_MASK, BANK_1_SPK_THER_SHDN_R_EN_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON1, 0x0, BANK_1_SPK_OC_SHDN_DL_MASK, BANK_1_SPK_OC_SHDN_DL_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON7, 0x0, BANK_1_SPK_OC_SHDN_DR_MASK, BANK_1_SPK_OC_SHDN_DR_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON1, 0x3, BANK_1_SPK_VOL_L_MASK, BANK_1_SPK_VOL_L_SHIFT);
	ret=pmic_bank1_config_interface(BANK1_SPK_CON7, 0x3, BANK_1_SPK_VOL_R_MASK, BANK_1_SPK_VOL_R_SHIFT);	
	
}

void PMIC_INIT_SETTING_V1(void)
{
	kal_uint32 eco_version = 0;
    kal_uint32 tmp32;
	kal_uint32 ret=0;	
    
    /* Low part of CID */
    tmp32=upmu_get_cid0();
    eco_version |= tmp32;
    /* High part of CID */
    tmp32=upmu_get_cid1();
    eco_version |= (tmp32 << 8);

	if (eco_version == PMIC6329_E1_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_INIT_SETTING_V1] PMIC6329_E1_CID_CODE !!\n");
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_INIT_SETTING_V1] Version : 2011/05/13 from CL\n");

		//<ECO_E1>
		//
		//bank0 0x3F[7:6] = 0x1
		ret=pmic_config_interface(0x3F,0x1,0x3,0x6);
		//
		//bank0 0x40[7:6] = 0x2
		ret=pmic_config_interface(0x40,0x2,0x3,0x6);
		//
		//bank0 0x43[5:4] = 0x3
		ret=pmic_config_interface(0x43,0x3,0x3,0x4);
		//
		//bank0 0x43[0:0] = 0x0
		ret=pmic_config_interface(0x43,0x0,0x1,0x0);
		//
		//bank0 0x52[5:4] = 0x3
		ret=pmic_config_interface(0x52,0x3,0x3,0x4);
		//
		//bank0 0x56[5:4] = 0x3
		ret=pmic_config_interface(0x56,0x3,0x3,0x4);
		//
		//bank0 0x56[0:0] = 0x0
		ret=pmic_config_interface(0x56,0x0,0x1,0x0);
		//
		//bank0 0x59[0:0] = 0x1
		ret=pmic_config_interface(0x59,0x1,0x1,0x0);
		//
		//bank0 0x6B[5:4] = 0x3
		ret=pmic_config_interface(0x6B,0x3,0x3,0x4);
		//
		//bank0 0x6B[0:0] = 0x0
		ret=pmic_config_interface(0x6B,0x0,0x1,0x0);
		//
		//bank0 0x70[5:4] = 0x2
		ret=pmic_config_interface(0x70,0x2,0x3,0x4);
		//
		//bank0 0x71[7:6] = 0x2
		ret=pmic_config_interface(0x71,0x2,0x3,0x6);
		//
		//bank0 0x74[5:4] = 0x3
		ret=pmic_config_interface(0x74,0x3,0x3,0x4);
		//Turn on GPIO hysteresis enable
		//bank1 0x4[5:5] = 0x1
		ret=pmic_bank1_config_interface(0x4,0x1,0x1,0x5);
		//</ECO_E1> 
		
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_INIT_SETTING_V1] 20110823-Juinn-Ting update !!\n");
		//20110823-Juinn-Ting update
		ret=pmic_config_interface(0x44,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x45,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x46,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x47,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x48,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x49,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x4A,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x8A,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x8B,0x16,0xFF,0x0);
		
    }
	else if (eco_version == PMIC6329_E2_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_INIT_SETTING_V1] PMIC6329_E2_CID_CODE !!\n");
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_INIT_SETTING_V1] Version : 2011/11/17 from CL\n");

		//<ECO_E2>
		//20111109		
		//bank0 0xE[5:5] = 0x1
		ret=pmic_config_interface(0xE,0x1,0x1,0x5);
		//20111117
		//bank0 0x10[6:6] = 0x1
		ret=pmic_config_interface(0x10,0x1,0x1,0x6);
		//VPROC_soft change step time=4uS
		//bank0 0x15[6:4] = 0x4
		ret=pmic_config_interface(0x15,0x4,0x7,0x4);
		//20111109
		//bank0 0x17[7:7] = 0x1
		ret=pmic_config_interface(0x17,0x1,0x1,0x7);
		//20111109
		//bank0 0x17[6:6] = 0x1
		ret=pmic_config_interface(0x17,0x1,0x1,0x6);
		//
		//bank0 0x3F[7:6] = 0x1
		ret=pmic_config_interface(0x3F,0x1,0x3,0x6);
		//
		//bank0 0x40[7:6] = 0x2
		ret=pmic_config_interface(0x40,0x2,0x3,0x6);
		//
		//bank0 0x43[5:4] = 0x3
		ret=pmic_config_interface(0x43,0x3,0x3,0x4);
#if 0        
		//20111108 update
		//bank0 0x44[4:0] = 0x16
		ret=pmic_config_interface(0x44,0x16,0x1F,0x0);
		//VPROC_VOSEL_SRCLKEN=0.9V
		//bank0 0x45[4:0] = 0x8
		ret=pmic_config_interface(0x45,0x8,0x1F,0x0);
		//VPROC_DVS00=0.9V
		//bank0 0x47[4:0] = 0x8
		ret=pmic_config_interface(0x47,0x8,0x1F,0x0);
		//VPROC_DVS01=1.0V
		//bank0 0x48[4:0] = 0xC
		ret=pmic_config_interface(0x48,0xC,0x1F,0x0);
		//VPROC_DVS10=1.1V
		//bank0 0x49[4:0] = 0x10
		ret=pmic_config_interface(0x49,0x10,0x1F,0x0);
		//VPROC_DVS11=1.25V
		//bank0 0x4A[4:0] = 0x16
		ret=pmic_config_interface(0x4A,0x16,0x1F,0x0);
#endif        
		//VPROC_soft change enable, control by DVS1/0@SRCLKEN=1
		//bank0 0x4B[2:2] = 0x1
		ret=pmic_config_interface(0x4B,0x1,0x1,0x2);        
		//VPROC_soft change enable, control by DVS1/0@SRCLKEN=1
		//bank0 0x4B[1:0] = 0x2
		ret=pmic_config_interface(0x4B,0x2,0x3,0x0);
		//low_power_20111108
		//bank0 0x4F[7:7] = 0x1
		ret=pmic_config_interface(0x4F,0x1,0x1,0x7);
		//low_power_20111108
		//bank0 0x4F[6:6] = 0x1
		ret=pmic_config_interface(0x4F,0x1,0x1,0x6);
		//low_power_20111108
		//bank0 0x4F[5:5] = 0x1
		ret=pmic_config_interface(0x4F,0x1,0x1,0x5);
		//
		//bank0 0x4F[4:4] = 0x0
		ret=pmic_config_interface(0x4F,0x0,0x1,0x4);
		//low_power_20111108
		//bank0 0x4F[0:0] = 0x1
		ret=pmic_config_interface(0x4F,0x1,0x1,0x0);
		//
		//bank0 0x52[5:4] = 0x3
		ret=pmic_config_interface(0x52,0x3,0x3,0x4);
		//VCORE slow discharge
		//bank0 0x53[5:4] = 0x2
		ret=pmic_config_interface(0x53,0x2,0x3,0x4);
		//
		//bank0 0x56[5:4] = 0x3
		ret=pmic_config_interface(0x56,0x3,0x3,0x4);
		//VCORE_soft change, control by SRCLKEN  (XXXX 0x59->0x57->0x58)
		//bank0 0x59[2:2] = 0x1
		ret=pmic_config_interface(0x59,0x1,0x1,0x2);
		//
		//bank0 0x59[0:0] = 0x1
		ret=pmic_config_interface(0x59,0x1,0x1,0x0);
		//VPROC_DVS00=0.9V
		//bank0 0x57[4:0] = 0x8
		ret=pmic_config_interface(0x57,0x8,0x1F,0x0);
        #if 0
        //VCORE_VOSEL_CON1=1.1V
		//bank0 0x58[4:0] = 0x10
		ret=pmic_config_interface(0x58,0x10,0x1F,0x0);
        #else
        //VCORE_VOSEL_CON1=1.15V
		//bank0 0x58[4:0] = 0x12
		ret=pmic_config_interface(0x58,0x12,0x1F,0x0);
        #endif
		//20111110 force PWM mode
		//bank0 0x5F[0:0] = 0x1
		ret=pmic_config_interface(0x5F,0x1,0x1,0x0);
		//20111108 updated
		//bank0 0x67[5:4] = 0x2
		ret=pmic_config_interface(0x67,0x2,0x3,0x4);
		//20110608 updated
		//bank0 0x67[1:0] = 0x1
		ret=pmic_config_interface(0x67,0x1,0x3,0x0);
		//
		//bank0 0x6B[5:4] = 0x3
		ret=pmic_config_interface(0x6B,0x3,0x3,0x4);
		//20111109
		//bank0 0x71[7:6] = 0x3
		ret=pmic_config_interface(0x71,0x3,0x3,0x6);
		//20111109
		//bank0 0x76[4:0] = 0x6
		ret=pmic_config_interface(0x76,0x6,0x1F,0x0);
		//20111109
		//bank0 0x77[4:0] = 0x9
		ret=pmic_config_interface(0x77,0x9,0x1F,0x0);
		//20111109
		//bank0 0x78[4:0] = 0xB
		ret=pmic_config_interface(0x78,0xB,0x1F,0x0);
		//20111109
		//bank0 0x79[4:0] = 0xD
		ret=pmic_config_interface(0x79,0xD,0x1F,0x0);
		//20111109
		//bank0 0x7A[4:0] = 0xF
		ret=pmic_config_interface(0x7A,0xF,0x1F,0x0);
		//20111109
		//bank0 0x7B[4:0] = 0x11
		ret=pmic_config_interface(0x7B,0x11,0x1F,0x0);
		//20111109
		//bank0 0x7C[4:0] = 0x15
		ret=pmic_config_interface(0x7C,0x15,0x1F,0x0);
		//20111109
		//bank0 0x7D[4:0] = 0x19
		ret=pmic_config_interface(0x7D,0x19,0x1F,0x0);
		//20110525 VPA control setting
		//bank0 0x7E[0:0] = 0x1
		ret=pmic_config_interface(0x7E,0x1,0x1,0x0);
		//20111109
		//bank0 0xB6[3:3] = 0x1
		ret=pmic_config_interface(0xB6,0x1,0x1,0x3);
		//Turn on GPIO hysteresis enable
		//bank1 0x4[5:5] = 0x1
		ret=pmic_bank1_config_interface(0x4,0x1,0x1,0x5);
		//20111108_debeb
		//bank1 0x17[7:7] = 0x1
		ret=pmic_bank1_config_interface(0x17,0x1,0x1,0x7);
		//20111108_debeb
		//bank1 0x17[6:6] = 0x1
		ret=pmic_bank1_config_interface(0x17,0x1,0x1,0x6);
		//20111108_debeb
		//bank1 0x17[5:5] = 0x1
		ret=pmic_bank1_config_interface(0x17,0x1,0x1,0x5);
		//20111109
		//bank1 0x23[1:1] = 0x1
		ret=pmic_bank1_config_interface(0x23,0x1,0x1,0x1);
		//20111109
		//bank1 0x28[1:1] = 0x1
		ret=pmic_bank1_config_interface(0x28,0x1,0x1,0x1);
		//20111109
		//bank1 0x46[6:6] = 0x1
		ret=pmic_bank1_config_interface(0x46,0x1,0x1,0x6);
		//20111109
		//bank1 0x4C[6:6] = 0x1
		ret=pmic_bank1_config_interface(0x4C,0x1,0x1,0x6);
		//</ECO_E2>

		#if 0
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_INIT_SETTING_V1] 20110823-Juinn-Ting update !!\n");
		//20110823-Juinn-Ting update
		ret=pmic_config_interface(0x44,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x45,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x46,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x47,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x48,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x49,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x4A,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x8A,0x16,0xFF,0x0);
		ret=pmic_config_interface(0x8B,0x16,0xFF,0x0);
		#endif
		
    }
    else
    {
    	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_INIT_SETTING_V1] Unknown PMIC6329 CODE !!\n");
    }
}

#if 0
//test
extern void dct_pmic_VCAMD_enable(kal_bool dctEnable);
extern void dct_pmic_VCAM_IO_enable(kal_bool dctEnable);
extern void dct_pmic_VCAM_AF_enable(kal_bool dctEnable);
extern void dct_pmic_VMCH_enable(kal_bool dctEnable);
extern void dct_pmic_VGP_enable(kal_bool dctEnable);
extern void dct_pmic_VGP2_enable(kal_bool dctEnable);
extern void dct_pmic_VIBR_enable(kal_bool dctEnable);
#endif

void PMIC_CUSTOM_SETTING_V1(void)
{
	kal_uint32 ret=0;
	
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_CUSTOM_SETTING_V1] ...............\n");
	ret=pmic_config_interface(BANK0_DIGLDO_CON1C, 0x7, BANK_0_RG_VCAMD_VOSEL_MASK, BANK_0_RG_VCAMD_VOSEL_SHIFT);		
	ret=pmic_config_interface(BANK0_DIGLDO_CON1F, 0x2, BANK_0_RG_VCAM_IO_VOSEL_MASK, BANK_0_RG_VCAM_IO_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON22, 0x4, BANK_0_RG_VCAM_AF_VOSEL_MASK, BANK_0_RG_VCAM_AF_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON29, 0x7, BANK_0_RG_VMCH_VOSEL_MASK, BANK_0_RG_VMCH_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON2D, 0x2, BANK_0_RG_VGP_VOSEL_MASK, BANK_0_RG_VGP_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON30, 0x4, BANK_0_RG_VGP2_VOSEL_MASK, BANK_0_RG_VGP2_VOSEL_SHIFT);
	ret=pmic_config_interface(BANK0_DIGLDO_CON33, 0x4, BANK_0_RG_VIBR_VOSEL_MASK, BANK_0_RG_VIBR_VOSEL_SHIFT);
	#if 0
	//test
	dct_pmic_VCAMD_enable(KAL_TRUE);
	dct_pmic_VCAM_IO_enable(KAL_TRUE);
	dct_pmic_VCAM_AF_enable(KAL_TRUE);
	dct_pmic_VMCH_enable(KAL_TRUE);
	dct_pmic_VGP_enable(KAL_TRUE);
	dct_pmic_VGP2_enable(KAL_TRUE);
	dct_pmic_VIBR_enable(KAL_TRUE);	
	#endif

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[Disable All Power Good Detection] ...............\n");
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VCORE_PG_ENB_MASK, BANK_0_RG_VCORE_PG_ENB_SHIFT);	
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VPROC_PG_ENB_MASK, BANK_0_RG_VPROC_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VIO18_PG_ENB_MASK, BANK_0_RG_VIO18_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VIO28_PG_ENB_MASK, BANK_0_RG_VIO28_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VA1_PG_ENB_MASK, BANK_0_RG_VA1_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VM12_1_PG_ENB_MASK, BANK_0_RG_VM12_1_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VM12_2_PG_ENB_MASK, BANK_0_RG_VM12_2_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN1, 0x1, BANK_0_RG_VM12_INT_PG_ENB_MASK, BANK_0_RG_VM12_INT_PG_ENB_SHIFT);
	ret=pmic_config_interface(BANK0_STRUP_VPG_EN2, 0x1, BANK_0_RG_VTCXO_PG_ENB_MASK, BANK_0_RG_VTCXO_PG_ENB_SHIFT);

}


void dump_oc_status(void)
{
	kal_uint32 ret=0;
	kal_uint8 ret_val=0;
	
	ret=pmic_read_interface(BANK0_OCSTATUS0, &ret_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[LDO OC] BANK0_OCSTATUS0=%x\n", ret_val);
	ret=pmic_read_interface(BANK0_OCSTATUS1, &ret_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[LDO OC] BANK1_OCSTATUS1=%x\n", ret_val);
	ret=pmic_read_interface(BANK0_OCSTATUS2, &ret_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[LDO OC] BANK2_OCSTATUS2=%x\n", ret_val);
	ret=pmic_read_interface(BANK0_OCSTATUS3, &ret_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[LDO OC] BANK3_OCSTATUS3=%x\n", ret_val);
}

//////////////////////////////////////////////////////////////////////////////////////////
// DVT Task For EINT 
//////////////////////////////////////////////////////////////////////////////////////////
int gDVT_INTERRUPT_FLAG_1=0;
int gDVT_INTERRUPT_FLAG_2=0;
int gDVT_INTERRUPT_FLAG_3=0;
int gDVT_INTERRUPT_FLAG_4=0;
int gDVT_INTERRUPT_FLAG_5=0;

void Set_gDVT_INTERRUPT_FLAG_1(int val){
	gDVT_INTERRUPT_FLAG_1=val;
}
EXPORT_SYMBOL(Set_gDVT_INTERRUPT_FLAG_1);
void Set_gDVT_INTERRUPT_FLAG_2(int val){
	gDVT_INTERRUPT_FLAG_2=val;
}
EXPORT_SYMBOL(Set_gDVT_INTERRUPT_FLAG_2);
void Set_gDVT_INTERRUPT_FLAG_3(int val){
	gDVT_INTERRUPT_FLAG_3=val;
}
EXPORT_SYMBOL(Set_gDVT_INTERRUPT_FLAG_3);
void Set_gDVT_INTERRUPT_FLAG_4(int val){
	gDVT_INTERRUPT_FLAG_4=val;
}
EXPORT_SYMBOL(Set_gDVT_INTERRUPT_FLAG_4);
void Set_gDVT_INTERRUPT_FLAG_5(int val){
	gDVT_INTERRUPT_FLAG_5=val;
}
EXPORT_SYMBOL(Set_gDVT_INTERRUPT_FLAG_5);

void do_DVT_task(void)
{
	kal_uint32 ret=0;
	kal_uint32 ret_val=0;

	if(gDVT_INTERRUPT_FLAG_1==1)
	{
		//1. LOW BAT interrupt
		//1.1 Read starus
		ret_val=upmu_interrupt_get_low_bat_int_status();
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[LOW BAT] RG_LOW_BAT_INT_STATUS=%x\n", ret_val);		
		//1.2 W1C
		ret=pmic_config_interface(BANK0_INT_STATUS6,1,BANK_0_RG_LOW_BAT_INT_STATUS_MASK,BANK_0_RG_LOW_BAT_INT_STATUS_SHIFT);
		//ret=pmic_config_interface(BANK0_INT_STATUS6,1,BANK_0_RG_LOW_BAT_INT_STATUS_MASK,BANK_0_RG_LOW_BAT_INT_STATUS_SHIFT);
		//ret=pmic_config_interface(BANK0_INT_STATUS6,1,BANK_0_RG_LOW_BAT_INT_STATUS_MASK,BANK_0_RG_LOW_BAT_INT_STATUS_SHIFT);
		upmu_interrupt_low_bat_int_en(0);
	}

	if(gDVT_INTERRUPT_FLAG_2==1)
	{
		//2. HIGH BAT interrupt
		//2.1 Read starus
		ret_val=upmu_interrupt_get_high_bat_int_status();
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[LOW BAT] RG_HIGH_BAT_INT_STATUS=%x\n", ret_val);
		//2.2 W1C
		ret=pmic_config_interface(BANK0_INT_STATUS6,1,BANK_0_RG_HIGH_BAT_INT_STATUS_MASK,BANK_0_RG_HIGH_BAT_INT_STATUS_SHIFT);
		//ret=pmic_config_interface(BANK0_INT_STATUS6,1,BANK_0_RG_HIGH_BAT_INT_STATUS_MASK,BANK_0_RG_HIGH_BAT_INT_STATUS_SHIFT);
		//ret=pmic_config_interface(BANK0_INT_STATUS6,1,BANK_0_RG_HIGH_BAT_INT_STATUS_MASK,BANK_0_RG_HIGH_BAT_INT_STATUS_SHIFT);
		upmu_interrupt_high_bat_int_en(0);
	}
	
	if(gDVT_INTERRUPT_FLAG_3==1)
	{
		//3. LDO OC interrupt	
		//3.1 Read starus	
		ret_val=upmu_interrupt_get_ldo_oc_int_status();
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[LDO OC] RG_LDO_OC_INT_STATUS=%x\n", ret_val);	
		dump_oc_status();	
		//3.2 W1C
		ret=pmic_config_interface(BANK0_INT_STATUS7,1,BANK_0_RG_LDO_OC_INT_STATUS_MASK,BANK_0_RG_LDO_OC_INT_STATUS_SHIFT);
		
		upmu_interrupt_ldo_oc_int_en(0);
	}

	if(gDVT_INTERRUPT_FLAG_4==1)
	{
		//4. VPA OC interrupt
		//4.1 Read starus	
		ret_val=upmu_interrupt_get_vpa_oc_int_status();
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[BUCK OC] RG_VPA_OC_INT_STATUS=%x\n", ret_val);	
		dump_oc_status();
		//4.2 W1C
		ret=pmic_config_interface(BANK0_INT_STATUS7,1,BANK_0_RG_VPA_OC_INT_STATUS_MASK,BANK_0_RG_VPA_OC_INT_STATUS_SHIFT);

		upmu_interrupt_vpa_oc_int_en(0);
		upmu_interrupt_vpa_oc_int_en(1);
	}

	if(gDVT_INTERRUPT_FLAG_5==1)
	{
		//5. VRF18 OC interrupt
		//5.1 Read starus	
		ret_val=upmu_interrupt_get_vrf18_oc_int_status();
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[BUCK OC] RG_VRF18_OC_INT_STATUS=%x\n", ret_val);	
		dump_oc_status();	
		//5.2 W1C
		ret=pmic_config_interface(BANK0_INT_STATUS7,1,BANK_0_RG_VRF18_OC_INT_STATUS_MASK,BANK_0_RG_VRF18_OC_INT_STATUS_SHIFT);

		upmu_interrupt_vrf18_oc_int_en(0);
		upmu_interrupt_vrf18_oc_int_en(1);
	}

	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[BUCK OC] Reg[0x75]=0x%x, Reg[0x62]=0x%x\r\n", 
			upmu_get_reg_value(0x75), upmu_get_reg_value(0x62));

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[do_DVT_task] Done\n");
	
}

//////////////////////////////////////////////////////////////////////////////////////////
// PMIC-Charger Type Detection
//////////////////////////////////////////////////////////////////////////////////////////
CHARGER_TYPE g_ret = CHARGER_UNKNOWN;
int g_charger_in_flag = 0;
int g_first_check=0;

CHARGER_TYPE hw_charger_type_detection(void)
{
	CHARGER_TYPE ret 				= CHARGER_UNKNOWN;
	unsigned int USB_U2PHYACR3_2 	= 0xF126081C;
	unsigned int USBPHYRegs			= 0xF1260800; //U2B20_Base+0x800
	kal_uint8 wChargerAvail	= 0;
	unsigned short bLineState_B		= 0;
	kal_uint8 bLineState_C 	= 0;
	kal_uint32 ret_val				= 0;
	//unsigned short testReadValue	= 0;
	kal_uint8 reg_val				= 0;

	//msleep(400);
	//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : start!\r\n");

/********* Step 0.0 : enable USB memory and clock *********/
	enable_pll(MT65XX_UPLL,"USB_PLL");
  	hwPowerOn(MT65XX_POWER_LDO_VUSB,VOL_3300,"VUSB_LDO");
  	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[hw_charger_type_detection] enable VUSB and UPLL before connect\n");

/********* Step 1.0 : PMU_BC11_Detect_Init ***************/		
	SETREG8(USB_U2PHYACR3_2,0x80); //USB_U2PHYACR3[7]=1		
	
	//BC11_RST=1
	ret_val=pmic_config_interface(0x33,0x1,BANK_0_BC11_RST_MASK,BANK_0_BC11_RST_SHIFT); 
	//BC11_RST=0	
	//ret_val=pmic_config_interface(0x33,0x0,BANK_0_BC11_RST_MASK,BANK_0_BC11_RST_SHIFT); 
	//BC11_BB_CTRL=1
	ret_val=pmic_config_interface(0x33,0x1,BANK_0_BC11_BB_CTRL_MASK,BANK_0_BC11_BB_CTRL_SHIFT);
	
	//RG_BC11_BIAS_EN=1	
	ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_BIAS_EN_MASK,BANK_0_BC11_BIAS_EN_SHIFT); 
	//RG_BC11_VSRC_EN[1:0]=00
	ret_val=pmic_config_interface(0x33,0x0,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT); 
	//RG_BC11_VREF_VTH = 0
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_VREF_VTH_MASK,BANK_0_BC11_VREF_VTH_SHIFT); 
	//RG_BC11_CMP_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
	//RG_BC11_IPU_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);
	//RG_BC11_IPD_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPD_EN_MASK,BANK_0_BC11_IPD_EN_SHIFT);

	//ret_val=pmic_read_interface(0x33,&testReadValue,0xFF,0);		
	//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x33]=%x, ", testReadValue);
	//ret_val=pmic_read_interface(0x34,&testReadValue,0xFF,0);		
	//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "Reg[0x34]=%x \r\n", testReadValue);

/********* Step A *************************************/
	//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step A\r\n");
	//RG_BC11_IPU_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);
	
	SETREG8(USBPHYRegs+0x1C,0x1000);//RG_PUPD_BIST_EN = 1	
	CLRREG8(USBPHYRegs+0x1C,0x0400);//RG_EN_PD_DM=0
	
	//RG_BC11_VSRC_EN[1.0] = 10 
	ret_val=pmic_config_interface(0x33,0x2,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT); 
	//RG_BC11_IPD_EN[1:0] = 01
	ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_IPD_EN_MASK,BANK_0_BC11_IPD_EN_SHIFT);
	//RG_BC11_VREF_VTH = 0
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_VREF_VTH_MASK,BANK_0_BC11_VREF_VTH_SHIFT);
  	//RG_BC11_CMP_EN[1.0] = 01
  	ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
	//mdelay(400);
	mdelay(100);
		
	ret_val=pmic_read_interface(0x33,&wChargerAvail,BANK_0_BC11_CMP_OUT_MASK,BANK_0_BC11_CMP_OUT_SHIFT); 
	//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step A : wChargerAvail=%x\r\n", wChargerAvail);
	
	//RG_BC11_VSRC_EN[1:0]=00
	ret_val=pmic_config_interface(0x33,0x0,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT); 
	//RG_BC11_IPD_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPD_EN_MASK,BANK_0_BC11_IPD_EN_SHIFT);
	//RG_BC11_CMP_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
	mdelay(50);
	
	if(wChargerAvail==1)
	{
/********* Step B *************************************/
		//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step B\r\n");

		//RG_BC11_IPU_EN[1:0]=10
		ret_val=pmic_config_interface(0x34,0x2,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);		
		mdelay(80);
		
		bLineState_B = INREG8(USBPHYRegs+0x76);
		//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step B : bLineState_B=%x\r\n", bLineState_B);
		if(bLineState_B & 0x80)
		{
			ret = STANDARD_CHARGER;
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step B : STANDARD CHARGER!\r\n");
		}
		else
		{
			ret = CHARGING_HOST;
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step B : Charging Host!\r\n");
		}
	}
	else
	{
/********* Step C *************************************/
		//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step C\r\n");

		//RG_BC11_IPU_EN[1:0]=01
		ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);
		//RG_BC11_CMP_EN[1.0] = 01
		ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
		//ret_val=pmic_read_interface(0x34,&testReadValue,0xFF,0);		
		//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step C : Reg[0x34]=%x\r\n", testReadValue);		
		mdelay(80);
				
		ret_val=pmic_read_interface(0x33,&bLineState_C,0xFF,0);
		//xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step C : bLineState_C=%x\r\n", bLineState_C);
		if(bLineState_C & 0x80)
		{
			ret = NONSTANDARD_CHARGER;
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step C : UNSTANDARD CHARGER!!!\r\n");
			//RG_BC11_IPU_EN[1:0]=10
			ret_val=pmic_config_interface(0x34,0x2,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);
			mdelay(80);
		}
		else
		{
			ret = STANDARD_HOST;
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : step C : Standard USB Host!!\r\n");
		}
	}
/********* Finally setting *******************************/
	//RG_BC11_VSRC_EN[1:0]=00
	ret_val=pmic_config_interface(0x33,0x0,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT); 
	//RG_BC11_VREF_VTH = 0
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_VREF_VTH_MASK,BANK_0_BC11_VREF_VTH_SHIFT);
	//RG_BC11_CMP_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_CMP_EN_MASK,BANK_0_BC11_CMP_EN_SHIFT);
	//RG_BC11_IPU_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPU_EN_MASK,BANK_0_BC11_IPU_EN_SHIFT);
	//RG_BC11_IPD_EN[1.0] = 00
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_IPD_EN_MASK,BANK_0_BC11_IPD_EN_SHIFT);
	//RG_BC11_BIAS_EN=0
	ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_BIAS_EN_MASK,BANK_0_BC11_BIAS_EN_SHIFT); 
	
	CLRREG8(USB_U2PHYACR3_2,0x80); //USB_U2PHYACR3[7]=0

	hwPowerDown(MT65XX_POWER_LDO_VUSB,"VUSB_LDO");
	disable_pll(MT65XX_UPLL,"USB_PLL");
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[hw_charger_type_detection] disable VUSB and UPLL before disconnect\n");

	if( (ret==STANDARD_HOST) || (ret==CHARGING_HOST) )
	{
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "mt_charger_type_detection : SW workaround for USB\r\n");
		//RG_BC11_BB_CTRL=1
		ret_val=pmic_config_interface(0x33,0x1,BANK_0_BC11_BB_CTRL_MASK,BANK_0_BC11_BB_CTRL_SHIFT);
		//RG_BC11_BIAS_EN=1
		ret_val=pmic_config_interface(0x34,0x1,BANK_0_BC11_BIAS_EN_MASK,BANK_0_BC11_BIAS_EN_SHIFT); 
		//RG_BC11_VSRC_EN[1.0] = 11		
		ret_val=pmic_config_interface(0x33,0x3,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT);
		//check
		ret_val=pmic_read_interface(0x33,&reg_val,0xFF,0);
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[Bank0] Reg[0x33]=0x%x\n", reg_val);
		ret_val=pmic_read_interface(0x34,&reg_val,0xFF,0);
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[Bank0] Reg[0x34]=0x%x\n", reg_val);
	}
	
	//step4:done, ret the type	
	return ret;
	
}

CHARGER_TYPE mt_charger_type_detection(void)
{
    if( g_first_check == 0 )
    {
		g_first_check = 1;
		g_ret = hw_charger_type_detection();
    }
    else
    {
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt_charger_type_detection] Got data !!, %d, %d\r\n", g_charger_in_flag, g_first_check);
    }

    return g_ret;
}

//////////////////////////////////////////////////////////////////////////////////////////
// For EINT handler
//////////////////////////////////////////////////////////////////////////////////////////
int pmic_thread_timeout=0;
static DEFINE_MUTEX(pmic_mutex);
static DECLARE_WAIT_QUEUE_HEAD(pmic_thread_wq);
static kal_bool led_switch = true;

void wake_up_pmic(void)
{
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[wake_up_pmic]\r\n");

	pmic_thread_timeout = 1;		
    wake_up(&pmic_thread_wq);
}
EXPORT_SYMBOL(wake_up_pmic);

#define WAKE_LOCK_INITIALIZED            (1U << 8)

static kal_bool is_first = true;
static int pmic_thread_kthread(void *x)
{
	kal_uint32 ret=0;
	kal_uint8 val_int_status_5=0;
	kal_uint8 val_int_status_6=0;
	kal_uint8 val_int_status_7=0;
	//kal_uint32 val_CHR_0=0;
	kal_uint32 ret_val=0;
	kal_uint8 reg_val=0;

	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thread_kthread] enter\r\n");

    /* Run on a process content */  
    while (1) {           
        mutex_lock(&pmic_mutex);        

		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thread_kthread] running\r\n");

		//#if defined(CONFIG_POWER_EXT)
		//do_DVT_task();
		//#endif
		
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[BATTERY] Reg[0x21]=0x%x, Reg[0x22]=0x%x, Reg[0x38]=0x%x\r\n", 
			upmu_get_reg_value(0x21), upmu_get_reg_value(0x22), upmu_get_reg_value(0x38) );
		
		/*If we unpluuged USB/Charger before pmic thread runs, we need turn off led */
	    if(!upmu_get_PCHR_CHRDET() && is_first)
		{
			mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, LED_OFF);
			mt65xx_leds_brightness_set(MT65XX_LED_TYPE_GREEN, LED_OFF);
			mt65xx_leds_brightness_set(MT65XX_LED_TYPE_BLUE, LED_OFF);
			is_first = false;
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread] Power Off Nled First Time \r\n");
		}

		ret=upmu_interrupt_get_chrdet_int_status();		
		if(ret==1)
		{
			ret=upmu_status_chr_pchr_chrdet();
			if(ret==1)
			{
				if(led_switch)
				{
					xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread] Power On NLed Battery Level = %d\r\n", bat_volt_check_point);
					if(bat_volt_check_point >= 100)    //Full ARGB	//Ivan
					{

						mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, LED_OFF);
						mt65xx_leds_brightness_set(MT65XX_LED_TYPE_GREEN, LED_FULL);
						mt65xx_leds_brightness_set(MT65XX_LED_TYPE_BLUE, LED_OFF);
					}
					else  //Low and Medium ARGB
					{
						mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, LED_FULL);
						mt65xx_leds_brightness_set(MT65XX_LED_TYPE_GREEN, LED_OFF);
						mt65xx_leds_brightness_set(MT65XX_LED_TYPE_BLUE, LED_OFF);
					}
				}
				xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread] charger exist!\r\n");
				g_charger_in_flag = 1;
#ifdef MTK_NCP1851_SUPPORT
                            mdelay(400);
#endif
			}
			else
			{
				if(led_switch)
				{
					xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread] Power Off NLed\r\n");
					mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, LED_OFF);
					mt65xx_leds_brightness_set(MT65XX_LED_TYPE_GREEN, LED_OFF);
					mt65xx_leds_brightness_set(MT65XX_LED_TYPE_BLUE, LED_OFF);
				}
				xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread] charger NOT exist!\r\n");
				g_charger_in_flag = 0;
				g_first_check = 0;
				
				//RG_BC11_BB_CTRL=1
				ret_val=pmic_config_interface(0x33,0x1,BANK_0_BC11_BB_CTRL_MASK,BANK_0_BC11_BB_CTRL_SHIFT);
				//RG_BC11_BIAS_EN=0
				ret_val=pmic_config_interface(0x34,0x0,BANK_0_BC11_BIAS_EN_MASK,BANK_0_BC11_BIAS_EN_SHIFT);
				//RG_BC11_VSRC_EN[1:0]=00
				ret_val=pmic_config_interface(0x33,0x0,BANK_0_RG_BC11_VSRC_EN_MASK,BANK_0_RG_BC11_VSRC_EN_SHIFT);
				//check
				ret_val=pmic_read_interface(0x33,&reg_val,0xFF,0);
		        xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[Bank0] Reg[0x33]=0x%x\n", reg_val);
				ret_val=pmic_read_interface(0x34,&reg_val,0xFF,0);
		        xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[Bank0] Reg[0x34]=0x%x\n", reg_val);
				
			}
			
			#if defined(CONFIG_POWER_EXT)				
				xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[mt6329_pmic_eint_irq] Environment=FPGA/EVB\n");
			#else                                
				if(battery_suspend_lock.flags & WAKE_LOCK_INITIALIZED)
				{
					wake_lock(&battery_suspend_lock);
					xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[mt6329_pmic_eint_irq] battery_suspend_lock(%d) inited \r\n", battery_suspend_lock.flags);
				}
				else
				{
					xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[mt6329_pmic_eint_irq] battery_suspend_lock(%d) not init \r\n", battery_suspend_lock.flags);
				}                             
				g_chr_event = 1;
			    wake_up_bat();	
			#endif
			
		}	

#if (PMIC_E2_DVT == 1)				

		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thread_kthread_E2] Reg[0x19]=0x%x, Reg[0x1B]=0x%x\r\n", 
			upmu_get_reg_value(0x19), upmu_get_reg_value(0x1B));		

		ret=upmu_interrupt_get_pwrkey_int_status();		
		if(ret==1)
		{			
			xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thread_kthread_E2] Get PWRKEY EINT\r\n");					
			
			ret=upmu_status_chr_pwrkey_deb();
			if(ret==1)
			{
				xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thread_kthread_E2] Release PWRKEY\r\n");
			}
			else
			{
				xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thread_kthread_E2] Press PWRKEY\r\n");
			}
		}

		ret=upmu_interrupt_get_homekey_int_status();
		if(ret==1)
		{			
			xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thread_kthread_E2] Get HomeKey EINT\r\n");

			ret=upmu_status_oc_homekey_deb();
			if(ret==1)
			{
				xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thread_kthread_E2] Release HomeKey\r\n");
			}
			else
			{
				xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thread_kthread_E2] Press HomeKey\r\n");
			}
		}
		
		do_DVT_task(); // VPA and VRF18 OC interrupt
		
		//clear int status, read all and write all		
		ret=pmic_config_interface(0x19,0xFF,0xFF,0x0);
		ret=pmic_config_interface(0x1A,0xFF,0xFF,0x0);
		ret=pmic_config_interface(0x1B,0xFF,0xFF,0x0);		
#else

		if( get_pmic6329_chip_version() == PMIC6329_E1_CODE )
		{
		ret=upmu_interrupt_get_pwrkey_int_status();		
		if(ret==1)
		{			
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread] Get PWRKEY EINT\r\n");		
			kpd_pwrkey_pmic_handler(0);
		}
		}
		else // E2
		{
			ret=upmu_interrupt_get_pwrkey_int_status();		
			if(ret==1)
			{			
				xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread_E2] Get PWRKEY EINT\r\n");					
				
				ret=upmu_status_chr_pwrkey_deb();
				if(ret==1)
				{
					xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread_E2] Release PWRKEY\r\n");
					kpd_pwrkey_pmic_handler(0x0);
				}
				else
				{
					xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread_E2] Press PWRKEY\r\n");
					kpd_pwrkey_pmic_handler(0x1);
				}
			}

			ret=upmu_interrupt_get_homekey_int_status();
			if(ret==1)
			{			
				xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread_E2] Get HomeKey EINT\r\n");

				ret=upmu_status_oc_homekey_deb();
				if(ret==1)
				{
					xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread_E2] Release HomeKey\r\n");
					kpd_pmic_rstkey_handler(0x0);
				}
				else
				{
					xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread_E2] Press HomeKey\r\n");
					kpd_pmic_rstkey_handler(0x1);
				}
			}
		}

		//clear int status, read all and write all		
		ret=pmic_config_interface(0x19,0xFF,0xFF,0x0);
		ret=pmic_config_interface(0x1A,0xFF,0xFF,0x0);
		ret=pmic_config_interface(0x1B,0xFF,0xFF,0x0);
		ret=pmic_read_interface(0x19,(&val_int_status_5),0xFF,0x0);
		ret=pmic_read_interface(0x1A,(&val_int_status_6),0xFF,0x0);
		ret=pmic_read_interface(0x1B,(&val_int_status_7),0xFF,0x0);		
		//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Reg[0x19]=0x%x\n", val_int_status_5);		
		//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Reg[0x1A]=0x%x\n", val_int_status_6);
		//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Reg[0x1B]=0x%x\n", val_int_status_7);
		
		//INT_EN
		ret=pmic_config_interface(0x16,0x0,0xFF,0x0);
		ret=pmic_config_interface(0x17,0x0,0xFF,0x0);
		ret=pmic_config_interface(0x18,0x0,0xFF,0x0);
		upmu_interrupt_chrdet_int_en(1);
		upmu_interrupt_pwrkey_int_en(1);
		if( get_pmic6329_chip_version() == PMIC6329_E1_CODE )
		{
			upmu_interrupt_homekey_int_en(0);
		}
		else
		{
			upmu_interrupt_homekey_int_en(1);
		}
		//---------------------------------------------------------
#endif		

		mt65xx_eint_unmask(CUST_EINT_MT6329_PMIC_NUM);

		mutex_unlock(&pmic_mutex);

		wait_event(pmic_thread_wq, pmic_thread_timeout);

        pmic_thread_timeout=0;
    }

    return 0;
}

void mt6329_pmic_eint_irq(void)
{
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_pmic_eint_irq] receive interrupt\n");

	//pmic internal
	wake_up_pmic();	
	
    return ;
}

void PMIC_EINT_SETTING(void)
{
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_EINT_SETTING] start: CUST_EINT_MT6329_PMIC_NUM=%d\n",CUST_EINT_MT6329_PMIC_NUM);	
	
	//CON 0
	upmu_interrupt_ov_en(0);
	upmu_interrupt_chrdet_int_en(1);
	upmu_interrupt_bvalid_det_int_en(0);
	upmu_interrupt_vbaton_undet_int_en(0);
	upmu_interrupt_thr_h_int_en(0);
	upmu_interrupt_thr_l_int_en(0);
	upmu_interrupt_pwrkey_int_en(1);
	upmu_interrupt_watchdog_int_en(0);
	//CON 1
	upmu_interrupt_fg_bat_h_int_en(0);
	upmu_interrupt_fg_bat_l_int_en(0);
	upmu_interrupt_high_bat_int_en(0);
	upmu_interrupt_low_bat_int_en(0);
	upmu_interrupt_spkr_d_oc_int_en(0);
	upmu_interrupt_spkl_d_oc_int_en(0);
	upmu_interrupt_spkr_ab_oc_int_en(0);
	upmu_interrupt_spkl_ab_oc_int_en(0);
	//CON 2
	if( get_pmic6329_chip_version() == PMIC6329_E1_CODE )
	{
	upmu_interrupt_homekey_int_en(0);
	}
	else
	{
		upmu_interrupt_homekey_int_en(1);
		upmu_RG_PWRKEY_INT_SEL(1); //bank1, 0x17
		upmu_RG_HOMEKEY_INT_SEL(1); //bank1, 0x17
	}
	upmu_interrupt_vrf18_oc_int_en(0);
	upmu_interrupt_vpa_oc_int_en(0);
	upmu_interrupt_ldo_oc_int_en(0);

	mt65xx_eint_set_sens(		CUST_EINT_MT6329_PMIC_NUM,
			            		CUST_EINT_MT6329_PMIC_SENSITIVE);
	mt65xx_eint_set_polarity(	CUST_EINT_MT6329_PMIC_NUM,
			         			CUST_EINT_MT6329_PMIC_POLARITY);// set positive polarity
	mt65xx_eint_set_hw_debounce(CUST_EINT_MT6329_PMIC_NUM,
								CUST_EINT_MT6329_PMIC_DEBOUNCE_CN); 	// set debounce time

#if 0
	mt_set_gpio_mode(			GPIO_PMIC_EINT_PIN,	0x0);
	mt_set_gpio_pull_enable(	GPIO_PMIC_EINT_PIN,	true);
	mt_set_gpio_pull_select(	GPIO_PMIC_EINT_PIN,	false);
	mt_set_gpio_mode(			GPIO_PMIC_EINT_PIN,	0x01);	
#endif

	mt65xx_eint_registration(	CUST_EINT_MT6329_PMIC_NUM,
			         			CUST_EINT_MT6329_PMIC_DEBOUNCE_EN,
				 				CUST_EINT_MT6329_PMIC_POLARITY,
				 				mt6329_pmic_eint_irq,
				 				0);

	mt65xx_eint_unmask(CUST_EINT_MT6329_PMIC_NUM);

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[CUST_EINT] CUST_EINT_MT6329_PMIC_NUM=%d\n", CUST_EINT_MT6329_PMIC_NUM);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[CUST_EINT] CUST_EINT_MT6329_PMIC_DEBOUNCE_CN=%d\n", CUST_EINT_MT6329_PMIC_DEBOUNCE_CN);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[CUST_EINT] CUST_EINT_MT6329_PMIC_POLARITY=%d\n", CUST_EINT_MT6329_PMIC_POLARITY);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[CUST_EINT] CUST_EINT_MT6329_PMIC_SENSITIVE=%d\n", CUST_EINT_MT6329_PMIC_SENSITIVE);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[CUST_EINT] CUST_EINT_MT6329_PMIC_DEBOUNCE_EN=%d\n", CUST_EINT_MT6329_PMIC_DEBOUNCE_EN);
	
}

void PMIC_DUMP_ALL_Register(void)
{
	kal_uint32 i=0;
	kal_uint32 ret=0;
	kal_uint8 reg_val=0;
	
	for (i=0;i<0xFF;i++)
	{
		ret=pmic_read_interface(i,&reg_val,0xFF,0);
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[Bank0] Reg[0x%x]=0x%x\n", i, reg_val);        
    }
	
    for (i=0;i<0xFF;i++)
	{
		ret=pmic_bank1_read_interface(i,&reg_val,0xFF,0);
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[Bank1] Reg[0x%x]=0x%x\n", i, reg_val);        
    }
}

void PMIC_check_trim_warning(void)
{
	kal_uint32 data_60_56=0;
	kal_uint32 data_55_48=0;

	data_60_56 = upmu_otpc_otp_pdo_63_56() & 0x1F;
	data_55_48 = upmu_otpc_otp_pdo_55_48();
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_check_trim_warning] data_60_56=0x%x, data_55_48=0x%x\n", data_60_56, data_55_48);
	
	if( (data_60_56==0) && (data_55_48==0) )
	{
		//PMIC-AUXADC not trim
		aee_kernel_reminding("PMIC-AUXADC not trim.","WARNING : PMIC-AUXADC not trim, will impact FG performance. You can use \"aee -c dal\" to clean screen.");
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_check_trim_warning] Not Trim\n");
	}
	else
	{
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_check_trim_warning] Trim\n");
	}
	
}

//////////////////////////////////////////////////////////////////////////////////////////
// pmic_VERIFY_DEFAULT_VALUE
//////////////////////////////////////////////////////////////////////////////////////////
#if (PMIC_E2_DVT == 1)
/*Define value test forBANK0*/
static kal_uint32 aPMURegBeg_bank0[255][2]= {  /* Register*//*reset val*/																
									{0x0 ,0x29}, 
									{0x1 ,0x10}, 
									{0x2 ,0xe3}, 
									{0x3 ,0x72}, 
									{0x4 ,0x32}, 
									{0x5 ,0x31}, 
									{0x6 ,0xff}, 
									{0x7 ,0x0 }, 
									{0x8 ,0x0 }, 
									{0x9 ,0x0 }, 
									{0x0a,0x0 }, 
									{0x0b,0x0 }, 
									{0x0c,0x40}, 
									{0x0d,0x0 }, 
									{0x0e,0x0 }, 
									{0x0f,0x40}, 
									{0x10,0x1 }, 
									{0x11,0x0 }, 
									{0x12,0x0 }, 
									{0x13,0x0 }, 
									{0x14,0x0 }, 
									{0x15,0x0 }, 
									{0x16,0xf3}, 
									{0x17,0x0 }, 
									{0x18,0x4 }, 
									{0x19,0x0 }, 
									{0x1a,0x0 }, 
									{0x1b,0x0 }, 
									{0x1c,0x0 }, 
									{0x1d,0x0 }, 
									{0x1e,0x0 }, 
									{0x1f,0x0 }, 
									{0x20,0x0 }, 
									{0x21,0x63}, 
									{0x22,0xf2}, 
									{0x23,0x84}, 
									{0x24,0x80}, 
									{0x25,0x0f}, 
									{0x26,0x0 }, 
									{0x27,0x3 }, 
									{0x28,0x2 }, 
									{0x29,0x0 }, 
									{0x2a,0x0 }, 
									{0x2b,0x20}, 
									{0x2c,0x0 }, 
									{0x2d,0x0 }, 
									{0x2e,0x10}, 
									{0x2f,0x0 }, 
									{0x30,0x0 }, 
									{0x31,0x0 }, 
									{0x32,0x0 }, 
									{0x33,0x0 }, 
									{0x34,0x0 }, 
									{0x35,0x22}, 
									{0x36,0x24}, 
									{0x37,0x44}, 
									{0x38,0x10}, 
									{0x39,0x0 }, 
									{0x3a,0x0 }, 
									{0x3b,0x0 }, 
									{0x3c,0x0 }, 
									{0x3d,0x0 }, 
									{0x3e,0x55}, 
									{0x3f,0x0 }, 
									{0x40,0x0 }, 
									{0x41,0x0 }, 
									{0x42,0x81}, 
									{0x43,0x1 }, 
									{0x44,0x14}, 
									{0x45,0x14}, 
									{0x46,0x14}, 
									{0x47,0x14}, 
									{0x48,0x14}, 
									{0x49,0x14}, 
									{0x4a,0x14}, 
									{0x4b,0x0 }, 
									{0x4c,0x0 }, 
									{0x4d,0x0 }, 
									{0x4e,0x14}, 
									{0x4f,0x0 }, 
									{0x50,0x0 }, 
									{0x51,0x0 }, 
									{0x52,0x0 }, 
									{0x53,0x0 }, 
									{0x54,0x0 }, 
									{0x55,0x81}, 
									{0x56,0x1 }, 
									{0x57,0x10}, 
									{0x58,0x10}, 
									{0x59,0x0 }, 
									{0x5a,0x10}, 
									{0x5b,0x0 }, 
									{0x5c,0x0 }, 
									{0x5d,0x0 }, 
									{0x5e,0x0 }, 
									{0x5f,0x0 }, 
									{0x60,0x0 }, 
									{0x61,0x41}, 
									{0x62,0x0f}, 
									{0x63,0x0 }, 
									{0x64,0x0 }, 
									{0x65,0x0 }, 
									{0x66,0x0 }, 
									{0x67,0x0 }, 
									{0x68,0x0 }, 
									{0x69,0x0 }, 
									{0x6a,0x81}, 
									{0x6b,0x1 }, 
									{0x6c,0x0f}, 
									{0x6d,0x0 }, 
									{0x6e,0x0 }, 
									{0x6f,0x0 }, 
									{0x70,0x0 }, 
									{0x71,0x0 }, 
									{0x72,0x0 }, 
									{0x73,0x0 }, 
									{0x74,0x1 }, 
									{0x75,0x0 }, 
									{0x76,0x0 }, 
									{0x77,0x0 }, 
									{0x78,0x0 }, 
									{0x79,0x0 }, 
									{0x7a,0x0 }, 
									{0x7b,0x0 }, 
									{0x7c,0x0 }, 
									{0x7d,0x0 }, 
									{0x7e,0x0 }, 
									{0x7f,0x0 }, 
									{0x80,0x0 }, 
									{0x81,0x0 }, 
									{0x82,0x0 }, 
									{0x83,0x81}, 
									{0x84,0x13}, 
									{0x85,0x0 }, 
									{0x86,0x0 }, 
									{0x87,0x81}, 
									{0x88,0x13}, 
									{0x89,0x0 }, 
									{0x8a,0x14}, 
									{0x8b,0x14}, 
									{0x8c,0x10}, 
									{0x8d,0x81}, 
									{0x8e,0x13}, 
									{0x8f,0x0 }, 
									{0x90,0x0 }, 
									{0x91,0x0 }, 
									{0x92,0x81}, 
									{0x93,0x13}, 
									{0x94,0x0 }, 
									{0x95,0x10}, 
									{0x96,0x13}, 
									{0x97,0x0 }, 
									{0x98,0x50}, 
									{0x99,0x13}, 
									{0x9a,0x0 }, 
									{0x9b,0x81}, 
									{0x9c,0x13}, 
									{0x9d,0x0 }, 
									{0x9e,0x10}, 
									{0x9f,0x13}, 
									{0xa0,0x0 }, 
									{0xa1,0x20}, 
									{0xa2,0x13}, 
									{0xa3,0x0 }, 
									{0xa4,0x40}, 
									{0xa5,0x13}, 
									{0xa6,0x0 }, 
									{0xa7,0xf1}, 
									{0xa8,0x13}, 
									{0xa9,0x0 }, 
									{0xaa,0x0 }, 
									{0xab,0xf1}, 
									{0xac,0x13}, 
									{0xad,0x0 }, 
									{0xae,0x0 }, 
									{0xaf,0x40}, 
									{0xb0,0x13}, 
									{0xb1,0x0 }, 
									{0xb2,0x40}, 
									{0xb3,0x13}, 
									{0xb4,0x0 }, 
									{0xb5,0x60}, 
									{0xb6,0x11}, 
									{0xb7,0x0 }, 
									{0xb8,0x0 }, 
									{0xb9,0x14}, 
									{0xba,0x0 }, 
									{0xbb,0x0 }, 
									{0xbc,0x0 }, 
									{0xbd,0x0 }, 
									{0xbe,0x0 }, 
									{0xbf,0x13}, 
									{0xc0,0x0 }, 
									{0xc1,0x83}, 
									{0xc2,0x13}, 
									{0xc3,0x0 }, 
									{0xc4,0x81}, 
									{0xc5,0x13}, 
									{0xc6,0x0 }, 
									{0xc7,0x0 }, 
									{0xc8,0x81}, 
									{0xc9,0x13}, 
									{0xca,0x0 }, 
									{0xcb,0x30}, 
									{0xcc,0x13}, 
									{0xcd,0x0 }, 
									{0xce,0x0 }, 
									{0xcf,0x81}, 
									{0xd0,0x0 }, 
									{0xd1,0x0 }, 
									{0xd2,0x0 }, 
									{0xd3,0x0 }, 
									{0xd4,0x0 }, 
									{0xd5,0x0 }, 
									{0xd6,0x0 }, 
									{0xd7,0x0 }, 
									{0xd8,0x0 }, 
									{0xd9,0x0 }, 
									{0xda,0x0 }, 
									{0xdb,0x0 }, 
									{0xdc,0x0 }, 
									{0xdd,0x0 }, 
									{0xde,0x0 }, 
									{0xdf,0x0 }, 
									{0xe0,0x0 }, 
									{0xe1,0x0 }, 
									{0xe2,0xff}, 
									{0xe3,0x83}, 
									{0xe4,0x0 }, 
									{0xe5,0x0 }, 
									{0xe6,0x0 }, 
									{0xe7,0x0 }, 
									{0xe8,0x2 }, 
									{0xe9,0x18}, 
									{0xea,0x0 }, 
									{0xeb,0x0 }, 
									{0xec,0x0 }, 
									{0xed,0x0 }, 
									{0xee,0x0 }, 
									{0xef,0x0 }, 
									{0xf0,0x0 }, 
									{0xf1,0x0 }, 
									{0xf2,0x0 }, 
									{0xf3,0xc0}, 
									{0xf4,0x0 }, 
									{0xf5,0x2 }, 
									{0xf6,0x6 }, 
									{0xf7,0x11}, 
									{0xf8,0x0 }, 
									{0xf9,0x0 }, 
									{0xfa,0x0 }, 
									{0xfb,0x0 }, 
									{0xfc,0x0 }, 
									{0xfd,0x0 }, 
									{0xfe,0x0 }, 
									{0xff,0x0 }          																
};

/*Define value test forBANK1*/
static kal_uint32 aPMURegBeg_bank1[255][2]= {  /* Register*//*reset val*/																
									{0x0 ,0x0f},
									{0x1 ,0x0 },
									{0x2 ,0xff},
									{0x3 ,0xff},
									{0x4 ,0x2 },
									{0x5 ,0x20},
									{0x6 ,0x16},
									{0x7 ,0x5 },
									{0x8 ,0x0 },
									{0x9 ,0x0 },
									{0x0a,0x0 },
									{0x0b,0x6 },
									{0x0c,0x0 },
									{0x0d,0x80},
									{0x0e,0x0 },
									{0x0f,0x0 },
									{0x10,0x0 },
									{0x11,0x0 },
									{0x12,0x0 },
									{0x13,0x0 },
									{0x14,0x1 },
									{0x15,0x17},
									{0x16,0x1 },
									{0x17,0x0 },
									{0x18,0x0 },
									{0x19,0x0 },
									{0x1a,0x0 },
									{0x1b,0x0 },
									{0x1c,0x0 },
									{0x1d,0x0 },
									{0x1e,0x0 },
									{0x1f,0x0 },
									{0x20,0x0 },
									{0x21,0x0 },
									{0x22,0x0 },
									{0x23,0x0 },
									{0x24,0x0 },
									{0x25,0x0 },
									{0x26,0x0 },
									{0x27,0x0 },
									{0x28,0x0 },
									{0x29,0x0 },
									{0x2a,0x0 },
									{0x2b,0x0 },
									{0x2c,0x0 },
									{0x2d,0x0b},
									{0x2e,0x0 },
									{0x2f,0x8 },
									{0x30,0x0 },
									{0x31,0x8 },
									{0x32,0x0 },
									{0x33,0x0 },
									{0x34,0x0 },
									{0x35,0x0 },
									{0x36,0x0 },
									{0x37,0x0 },
									{0x38,0x0 },
									{0x39,0x0 },
									{0x3a,0x0 },
									{0x3b,0x0 },
									{0x3c,0x0 },
									{0x3d,0x0 },
									{0x3e,0x0 },
									{0x3f,0x0 },
									{0x40,0x0 },
									{0x41,0x0 },
									{0x42,0x0 },
									{0x43,0x0 },
									{0x44,0x0 },
									{0x45,0x0 },
									{0x46,0x0 },
									{0x47,0x3 },
									{0x48,0x0 },
									{0x49,0x0 },
									{0x4a,0x0 },
									{0x4b,0x14},
									{0x4c,0x0 },
									{0x4d,0x3 },
									{0x4e,0x0 },
									{0x4f,0x0 },
									{0x50,0x0 },
									{0x51,0x14},
									{0x52,0x4 },
									{0x53,0x0 },
									{0x54,0x11},
									{0x55,0x8 },
									{0x56,0x0 },
									{0x57,0x0 },
									{0x58,0x0 },
									{0x59,0x11},
									{0x5a,0x0 },
									{0x5b,0x0 },
									{0x5c,0x0 },
									{0x5d,0x0 },
									{0x5e,0x0 },
									{0x5f,0x0 },
									{0x60,0x80},
									{0x61,0x1d},
									{0x62,0x3c},
									{0x63,0x70},
									{0x64,0x40},
									{0x65,0x0 },
									{0x66,0x0 },
									{0x67,0x0 },
									{0x68,0x0 },
									{0x69,0x20},
									{0x6a,0x0 },
									{0x6b,0x0 },
									{0x6c,0x0 },
									{0x6d,0x0 },
									{0x6e,0x0 },
									{0x6f,0x0 },
									{0x70,0x0 },
									{0x71,0x0 },
									{0x72,0x0 },
									{0x73,0x0 },
									{0x74,0x0 },
									{0x75,0x0 },
									{0x76,0x0 },
									{0x77,0x0 },
									{0x78,0x0 },
									{0x79,0x0 },
									{0x7a,0x0 },
									{0x7b,0x0 },
									{0x7c,0x0 },
									{0x7d,0x0 },
									{0x7e,0x0 },
									{0x7f,0x0 },
									{0x80,0x3 },
									{0x81,0x2 },
									{0x82,0x0 },
									{0x83,0x0 },
									{0x84,0x0 },
									{0x85,0x0 },
									{0x86,0x0 },
									{0x87,0x0 },
									{0x88,0x0 },
									{0x89,0x0 },
									{0x8a,0x0 },
									{0x8b,0x0 },
									{0x8c,0x0 },
									{0x8d,0x0 },
									{0x8e,0x0 },
									{0x8f,0x0 },
									{0x90,0x0 },
									{0x91,0x0 },
									{0x92,0x0 },
									{0x93,0x0 },
									{0x94,0x0 },
									{0x95,0x0 },
									{0x96,0x0 },
									{0x97,0x0 },
									{0x98,0x0 },
									{0x99,0x0 },
									{0x9a,0x0 },
									{0x9b,0x0 },
									{0x9c,0x0 },
									{0x9d,0x0 },
									{0x9e,0x0 },
									{0x9f,0x0 },
									{0xa0,0x40},
									{0xa1,0x0 },
									{0xa2,0x0 },
									{0xa3,0x0 },
									{0xa4,0x1 },
									{0xa5,0x1 },
									{0xa6,0x0 },
									{0xa7,0x0 },
									{0xa8,0x0 },
									{0xa9,0x0 },
									{0xaa,0x0 },
									{0xab,0x0 },
									{0xac,0x0 },
									{0xad,0x0 },
									{0xae,0x0 },
									{0xaf,0x0 },
									{0xb0,0x0 },
									{0xb1,0x0 },
									{0xb2,0x0 },
									{0xb3,0x0 },
									{0xb4,0x0 },
									{0xb5,0x0 },
									{0xb6,0x0 },
									{0xb7,0x0 },
									{0xb8,0x0 },
									{0xb9,0x0 },
									{0xba,0x0 },
									{0xbb,0x0 },
									{0xbc,0x0 },
									{0xbd,0x0 },
									{0xbe,0x0 },
									{0xbf,0x0 },
									{0xc0,0x0 },
									{0xc1,0x0 },
									{0xc2,0x0 },
									{0xc3,0x0 },
									{0xc4,0x0 },
									{0xc5,0x0 },
									{0xc6,0x0 },
									{0xc7,0x0 },
									{0xc8,0x0 },
									{0xc9,0x0 },
									{0xca,0x0 },
									{0xcb,0x0 },
									{0xcc,0x0 },
									{0xcd,0x0 },
									{0xce,0x0 },
									{0xcf,0x0 },
									{0xd0,0x0 },
									{0xd1,0x0 },
									{0xd2,0x0 },
									{0xd3,0x0 },
									{0xd4,0x0 },
									{0xd5,0x0 },
									{0xd6,0x0 },
									{0xd7,0x0 },
									{0xd8,0x0 },
									{0xd9,0x0 },
									{0xda,0x0 },
									{0xdb,0x0 },
									{0xdc,0x0 },
									{0xdd,0x0 },
									{0xde,0x0 },
									{0xdf,0x0 },
									{0xe0,0x0 },
									{0xe1,0x0 },
									{0xe2,0x0 },
									{0xe3,0x0 },
									{0xe4,0x0 },
									{0xe5,0x0 },
									{0xe6,0x0 },
									{0xe7,0x0 },
									{0xe8,0x0 },
									{0xe9,0x0 },
									{0xea,0x0 },
									{0xeb,0x0 },
									{0xec,0x0 },
									{0xed,0x0 },
									{0xee,0x0 },
									{0xef,0x0 },
									{0xf0,0x0 },
									{0xf1,0x0 },
									{0xf2,0x0 },
									{0xf3,0x0 },
									{0xf4,0x0 },
									{0xf5,0x0 },
									{0xf6,0x0 },
									{0xf7,0x0 },
									{0xf8,0x0 },
									{0xf9,0x0 },
									{0xfa,0x0 },
									{0xfb,0x0 },
									{0xfc,0x0 },
									{0xfd,0x0 },
									{0xfe,0x0 },
									{0xff,0x0 }           																
};

void pmic_VERIFY_DEFAULT_VALUE_ldvt(void)
{
	kal_uint32 tmp32=0;
	kal_uint32 u2PMUReg = 0;
    kal_uint32 u2Cnt = 0;

	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "PMIC-----------------------------------\r\n");
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "PMIC BANK 0 - diff with Designer report\r\n");
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "RegNum,Result,DefaultValue\r\n");

	for(u2Cnt = 0; u2Cnt < (sizeof(aPMURegBeg_bank0)/sizeof(*aPMURegBeg_bank0)); ++u2Cnt)
	{
	   u2PMUReg = upmu_get_reg_value(    (aPMURegBeg_bank0[u2Cnt][0])  );

	   //xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "%x,%x,%x\r\n",(aPMURegBeg_bank0[u2Cnt][0]), u2PMUReg,(aPMURegBeg_bank0[u2Cnt][1]));

	   if(u2PMUReg != (aPMURegBeg_bank0[u2Cnt][1]))
	   {
	   	   xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "%x,%x,%x\r\n",(aPMURegBeg_bank0[u2Cnt][0]), u2PMUReg,(aPMURegBeg_bank0[u2Cnt][1]));
	   }
	}

	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "PMIC-----------------------------------\r\n");
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "PMIC BANK 1 - diff with Designer report\r\n");
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "RegNum,Result,DefaultValue\r\n");
	
	for(u2Cnt = 0; u2Cnt < (sizeof(aPMURegBeg_bank1)/sizeof(*aPMURegBeg_bank1)); ++u2Cnt)
	{
	   // Need to use the I2C access method of bank 1 
	   u2PMUReg = upmu_get_reg_value_bank1(    (aPMURegBeg_bank1[u2Cnt][0])  );

	   //xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "%x,%x,%x\r\n",(aPMURegBeg_bank1[u2Cnt][0]), u2PMUReg,(aPMURegBeg_bank1[u2Cnt][1]));

	   if(u2PMUReg != (aPMURegBeg_bank1[u2Cnt][1]))
	   {
	   	   xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "%x,%x,%x\r\n",(aPMURegBeg_bank1[u2Cnt][0]), u2PMUReg,(aPMURegBeg_bank1[u2Cnt][1]));
	   }
	}
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "PMIC-----------------------------------\r\n");
}

void pmic_e2_i2c_speed_test_setting(void)
{
	kal_uint32 ret=0;
	kal_uint32 u2PMUReg = 0;
		
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0007120]=0x%x\r\n", DRV_Reg32(0xF0007120));
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0001014]=0x%x\r\n", DRV_Reg32(0xF0001014));
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0001004]=0x%x\r\n", DRV_Reg32(0xF0001004));
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0001000]=0x%x\r\n", DRV_Reg32(0xF0001000));
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] start\r\n");

	//1. Disable MT6329 i2c deglitch function
	ret=pmic_bank1_config_interface(0xA4, 0x0, 0x0, 0x0);
	ret=pmic_bank1_config_interface(0xA5, 0x0, 0x0, 0x0);

	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] BANK1[A4]=0x%x, BANK1[A5]=0x%x\r\n", 
		upmu_get_reg_value_bank1(0xA4), upmu_get_reg_value_bank1(0xA5) );

#if 0
	//2. Enable PLL	
	DRV_WriteReg32(0xF0007120, 0x2B60);
	udelay(30);
	DRV_WriteReg32(0xF0001014, 0x0100);
	DRV_WriteReg32(0xF0001004, 0x000A);
	DRV_WriteReg32(0xF0001000, 0x0002);	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0007120]=0x%x\r\n", DRV_Reg32(0xF0007120));
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0001014]=0x%x\r\n", DRV_Reg32(0xF0001014));
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0001004]=0x%x\r\n", DRV_Reg32(0xF0001004));
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0001000]=0x%x\r\n", DRV_Reg32(0xF0001000));

	//3. Check bus frequency
	DRV_WriteReg32(0xF0007404, 0x003B);
	DRV_WriteReg32(0xF0007400, 0xC800);
	DRV_WriteReg32(0xF0007400, 0x8800);
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0007404]=0x%x\r\n", DRV_Reg32(0xF0007404));
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0007400]=0x%x\r\n", DRV_Reg32(0xF0007400));
	
	ret = DRV_Reg32(0xF0007404);
	ret = ret & 0x00008000;
	while( ret == 0 )
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] ret=%d\r\n", ret);
		ret = DRV_Reg32(0xF0007404);
		ret = ret & 0x00008000;
	}
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF0007410]=0x%x\r\n", DRV_Reg32(0xF0007410));
#endif

	//4. Set i2c dual parameters
	ret = DRV_Reg32(0xF1014020); //0xC1014020[10:8]=0x0 
    ret &= 0xF8FF;
    DRV_WriteReg32(0xF1014020, ret);
	ret = DRV_Reg32(0xF1014020); //0xC1014020[5:0]=0x2 (E1: 0x5) 
	ret &= 0xFFC0;
	ret |= 0x0002;
    DRV_WriteReg32(0xF1014020, ret);
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF1014020]=0x%x\r\n", DRV_Reg32(0xF1014020));

	ret = DRV_Reg32(0xF10140A0); //0xC10140A0[10:8]=0x0 
    ret &= 0xF8FF;
    DRV_WriteReg32(0xF10140A0, ret);
	ret = DRV_Reg32(0xF10140A0); //0xC10140A0[5:0]=0x2 (E1: 0x5)
    ret &= 0xFFC0;
	ret |= 0x0002;
    DRV_WriteReg32(0xF10140A0, ret);
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] Reg[0xF10140A0]=0x%x\r\n", DRV_Reg32(0xF10140A0));
	
	//5. Meausure i2c clock = 1144/4/32/4 = 2.234MHz

	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_e2_i2c_speed_test_setting] end\r\n");
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// driver init
//////////////////////////////////////////////////////////////////////////////////////////
#if 0
static int mt6329_bank1_driver_detect(struct i2c_client *client, int kind, struct i2c_board_info *info) 
{         
    strcpy(info->type, "mt6329_pmic_bank1");                                                         

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_bank1_driver_detect] \n");
	
    return 0;                                                                                       
}
#endif


///////////////////////////////////////////////////////////////////////////////////////////
//// PMIC Thermal Throttling 
///////////////////////////////////////////////////////////////////////////////////////////
#define thermal_dprintk(fmt, args...)   \
do {                                    \
    xlog_printk(ANDROID_LOG_INFO, "Power/Thermal", fmt, ##args); \
} while(0)
//CPU thermal_throttling----------------------------
#define PMIC_NORMAL_MODE 1
#define PMIC_EARLY_HOT_MODE 2
#define PMIC_HOT_MODE 3
#define PMIC_DANGER_MODE 4

static int g_pmic_tt_mode = PMIC_NORMAL_MODE;
static int g_pmic_tt_mode_pre = PMIC_NORMAL_MODE;
static int pmic_thermal_throttling_enable = 0;
static int pmic_tt_test_mode = 0;
static int pmic_tt_test_mode_value = 0;

#define DVFS_F1 (1001000)   // KHz
#define DVFS_F2 ( 500500)   // KHz
#define DVFS_F3 ( 250250)   // KHz
#define DVFS_F4 ( 166833)   // KHz

extern void cpufreq_thermal_protect(int limited_freq);
//void cpufreq_thermal_protect(int limited_freq)
//{
//    // wait DVFS driver ready
//    thermal_dprintk("[cpufreq_thermal_protect]: limited_freq = %d.\n", limited_freq);
//}
//-----------------------------------------------

/*
 * kernel fopen/fclose
 */
static mm_segment_t oldfs;

void my_close(int fd)
{
	set_fs(oldfs);
	sys_close(fd);
}

int my_open(char *fname, int flag)
{
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	return sys_open(fname, flag, 0);
}

#define BRIGHTNESS_ACT_0 0 //0% OFF
#define BRIGHTNESS_ACT_1 1 //50% OFF
#define BRIGHTNESS_ACT_2 2 //25% OFF
#define BRIGHTNESS_ACT_3 3 //75% OFF

void adjust_backlight_brightness(int algo_select)
{
    int fd;
	char buf[64];
	char *pmtdbufp = NULL;
    ssize_t pmtdsize;

    char *pvalue = NULL;
    int got_value=0;
    int new_value=0;
    char out_buf[64];
    const char *out_buf_const;
    int ret=0;
    int len=0;

    //open file and read current value
    fd = my_open("/sys/class/leds/lcd-backlight/brightness", O_RDWR);
	if (fd < 0)
    {   
        thermal_dprintk("[adjust_backlight_brightness]: open file fail");
	    return;
    }
    //thermal_dprintk("[adjust_backlight_brightness]: open file ok");
	buf[sizeof(buf) - 1] = '\0';
	pmtdsize = sys_read(fd, buf, sizeof(buf) - 1);
	pmtdbufp = buf;   
    got_value = simple_strtol(pmtdbufp,&pvalue,10);

    // adjust brightness 
    switch( algo_select ){        
        case BRIGHTNESS_ACT_0:			
 	        // 0% OFF
 	        new_value = got_value;
 	        break;
        case BRIGHTNESS_ACT_1:			
 	        // 50% OFF
 	        new_value = (got_value*50)/100;
 	        break;
        case BRIGHTNESS_ACT_2:
		    // 25% OFF
		    new_value = (got_value*25)/100;
 	        break;
        case BRIGHTNESS_ACT_3:			
 	        // 75% OFF
 	        new_value = (got_value*75)/100;
 	        break;        	
        default:
 	        thermal_dprintk("[adjust_backlight_brightness]: argument invalid!!. algo_select=%d\n", algo_select);
 	        break;
 	}   

    // write the new value back
    ret = sprintf(out_buf, "%d", new_value);
    out_buf_const = out_buf;
    len = strlen(out_buf_const);    
    ret = sys_write(fd, out_buf, len);

    // close file
    my_close(fd);

    // debug
    thermal_dprintk("[adjust_backlight_brightness]: algo_select=%d,read value=%s,got_value=%d,new_value=%d,out_buf=%s,len=%d\n", 
        algo_select,pmtdbufp,got_value,new_value,out_buf,len);
    
}

#define BAT_MS_TO_NS(x) (x * 1000 * 1000)
static struct hrtimer pmic_thermal_detect_timer;
static struct task_struct *pmic_thermal_detect_thread = NULL;
static int pmic_thermal_detect_flag = 0;
static DECLARE_WAIT_QUEUE_HEAD(pmic_thermal_detect_waiter);

void do_pmic_normal_mode(int pmic_mode_value)
{
    kal_uint32 ret_val=0;
    kal_uint8 thermal_status=0;
    ret_val=pmic_read_interface(0x13,&thermal_status,0x7,4);

    //Do task
    cpufreq_thermal_protect(DVFS_F1);
    
    // Check next mode
    if(pmic_tt_test_mode == 1)
    {
        thermal_status = pmic_tt_test_mode_value;
        thermal_dprintk("[do_pmic_normal_mode]: In test mode, thermal_status=%d\n", thermal_status);
    }
    if(thermal_status == 0x1)
    {
        g_pmic_tt_mode = PMIC_EARLY_HOT_MODE;        
    }

    g_pmic_tt_mode_pre = PMIC_NORMAL_MODE;

    thermal_dprintk("[do_pmic_normal_mode]: thermal_status=%d\n", thermal_status);
    thermal_dprintk("[do_pmic_normal_mode]: next mode=%d\n", g_pmic_tt_mode);
}

void do_pmic_early_hot_mode(int pmic_mode_value)
{
    kal_uint32 ret_val=0;
    kal_uint8 thermal_status=0;
    ret_val=pmic_read_interface(0x13,&thermal_status,0x7,4);

    //Do task
    cpufreq_thermal_protect(DVFS_F2);
    
    // Check next mode
    if(pmic_tt_test_mode == 1)
    {
        thermal_status = pmic_tt_test_mode_value;
        thermal_dprintk("[do_pmic_early_hot_mode]: In test mode, thermal_status=%d\n", thermal_status);
    }
    if(thermal_status == 0x3)
    {
        g_pmic_tt_mode = PMIC_HOT_MODE;        
    }

    g_pmic_tt_mode_pre = PMIC_EARLY_HOT_MODE;
    
    thermal_dprintk("[do_pmic_early_hot_mode]: thermal_status=%d\n", thermal_status);
    thermal_dprintk("[do_pmic_early_hot_mode]: next mode=%d\n", g_pmic_tt_mode);
}

void do_pmic_hot_mode(int pmic_mode_value)
{
    kal_uint32 ret_val=0;
    kal_uint8 thermal_status=0;
    ret_val=pmic_read_interface(0x13,&thermal_status,0x7,4);

    //Do task
    cpufreq_thermal_protect(DVFS_F3);

    if(g_pmic_tt_mode_pre != PMIC_HOT_MODE)
    {
        adjust_backlight_brightness(BRIGHTNESS_ACT_1);
    }
        
    // Check next mode
    if(pmic_tt_test_mode == 1)
    {
        thermal_status = pmic_tt_test_mode_value;
        thermal_dprintk("[do_pmic_hot_mode]: In test mode, thermal_status=%d\n", thermal_status);
    }
    if(thermal_status == 0x0)
    {
        g_pmic_tt_mode = PMIC_NORMAL_MODE;
    }
    else if(thermal_status == 0x7) 
    {
        g_pmic_tt_mode = PMIC_DANGER_MODE;
    }
    else
    {
    }

    g_pmic_tt_mode_pre = PMIC_HOT_MODE;
    
    thermal_dprintk("[do_pmic_hot_mode]: thermal_status=%d\n", thermal_status);
    thermal_dprintk("[do_pmic_hot_mode]: next mode=%d\n", g_pmic_tt_mode);
}

void do_pmic_danger_mode(int pmic_mode_value)
{
    kal_uint32 ret_val=0;
    kal_uint8 thermal_status=0;
    ret_val=pmic_read_interface(0x13,&thermal_status,0x7,4);

    // HW will auto PMU power down

    g_pmic_tt_mode_pre = PMIC_DANGER_MODE;
    
    thermal_dprintk("[do_pmic_danger_mode]: thermal_status=%d\n", thermal_status);
    thermal_dprintk("[do_pmic_danger_mode]: next mode=%d\n", g_pmic_tt_mode);
}

void pmic_thermal_throttling_task(void)
{
    switch( g_pmic_tt_mode ){        
        case PMIC_NORMAL_MODE:			
 	        do_pmic_normal_mode(g_pmic_tt_mode);
 	        break;
        case PMIC_EARLY_HOT_MODE:
		    do_pmic_early_hot_mode(g_pmic_tt_mode);     	        
 	        break;
        case PMIC_HOT_MODE:			
 	        do_pmic_hot_mode(g_pmic_tt_mode);
 	        break;
        case PMIC_DANGER_MODE:			
 	        do_pmic_danger_mode(g_pmic_tt_mode);
 	        break;	
        default:
 	        thermal_dprintk("[pmic_thermal_throttling_task]: argument invalid!!. g_pmic_tt_mode=%d\n", g_pmic_tt_mode);
 	        break;
 	}

    //xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thermal_throttling_task] Done\n");
}

int pmic_thermal_detect_sw_thread_handler(void *unused)
{
    ktime_t ktime;

    do
    {
    	ktime = ktime_set(5, BAT_MS_TO_NS(0)); //5s       
    
    	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thermal_detect_sw_thread_handler] \n");
            
        wait_event_interruptible(pmic_thermal_detect_waiter, pmic_thermal_detect_flag != 0);
    
    	pmic_thermal_detect_flag = 0;

        if(pmic_thermal_throttling_enable == 1)
        {
            pmic_thermal_throttling_task();
        }
              
        hrtimer_start(&pmic_thermal_detect_timer, ktime, HRTIMER_MODE_REL);    
		
	} while (!kthread_should_stop());
    
    return 0;
}

enum hrtimer_restart pmic_thermal_throttling_detect(struct hrtimer *timer)
{
	pmic_thermal_detect_flag = 1; 
	wake_up_interruptible(&pmic_thermal_detect_waiter);

    //xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_thermal_throttling_detect] \n");
	
    return HRTIMER_NORESTART;
}

void pmic_thermal_throttling_init(void)
{
    ktime_t ktime;

	ktime = ktime_set(5, BAT_MS_TO_NS(0)); //5s
	hrtimer_init(&pmic_thermal_detect_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    pmic_thermal_detect_timer.function = pmic_thermal_throttling_detect;    
    hrtimer_start(&pmic_thermal_detect_timer, ktime, HRTIMER_MODE_REL);

    pmic_thermal_detect_thread = kthread_run(pmic_thermal_detect_sw_thread_handler, 0, "mtk pmic thermal throttling");
    if (IS_ERR(pmic_thermal_detect_thread))
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[%s]: failed to create pmic_thermal_detect_thread\n", __FUNCTION__);
    }

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "pmic_thermal_throttling_init : done\n" );
}

static int pmic_cmd_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *p = buf;
    
    p += sprintf(p, " pmic_thermal_throttling_enable=%d,\n pmic_tt_test_mode=%d,\n pmic_tt_test_mode_value=%d\n", 
		pmic_thermal_throttling_enable, pmic_tt_test_mode, pmic_tt_test_mode_value);
    
    *start = buf + off;
    
    len = p - buf;
    if (len > off)
        len -= off;
    else
        len = 0;
    
    return len < count ? len  : count;
}

static ssize_t pmic_cmd_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int len = 0, pmic_tt_enable=0, pmic_thr_test_mode=0, pmic_thr_test_value=0;
    char desc[32];
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len))
    {
        return 0;
    }
    desc[len] = '\0';
    
    if (sscanf(desc, "%d %d %d", &pmic_tt_enable, &pmic_thr_test_mode, &pmic_thr_test_value) == 3)
    {
        pmic_thermal_throttling_enable = pmic_tt_enable;
        pmic_tt_test_mode = pmic_thr_test_mode;
		pmic_tt_test_mode_value = pmic_thr_test_value;
		
        xlog_printk(ANDROID_LOG_DEBUG, "Power/Thermal", "pmic_tt_enable=%d, pmic_thr_test_mode=%d, pmic_thr_test_value=%d\n", 
			pmic_thermal_throttling_enable, pmic_tt_test_mode, pmic_tt_test_mode_value);
        
        return count;
    }
    else
    {
        xlog_printk(ANDROID_LOG_DEBUG, "Power/Thermal", "  bad argument, echo [pmic_tt_enable] [pmic_thr_test_mode] [pmic_thr_test_value] > pmic_cmd\n");
    }
    
    return -EINVAL;
}

void pmic_test_cmd_init(void)
{
    struct proc_dir_entry *entry = NULL;
	struct proc_dir_entry *pmic_dir = NULL;

    pmic_dir = proc_mkdir("mtk_pmic_cmd", NULL);
    if (!pmic_dir)
    {
        pr_err("[%s]: mkdir /proc/mtk_pmic_cmd failed\n", __FUNCTION__);
    }
    else
    {
        entry = create_proc_entry("pmic_cmd", S_IRUGO | S_IWUSR, pmic_dir);
        if (entry)
        {
            entry->read_proc = pmic_cmd_read;
            entry->write_proc = pmic_cmd_write;
        }
    }

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "******** mtk_pmic_cmd!! ********\n" );
}

extern int bat_thread_kthread(void *x);
extern void charger_hv_detect_sw_workaround_init(void);

static int mt6329_bank1_driver_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{             
	//struct class_device *class_dev = NULL;
    //int err=0;
	//kal_uint32 eco_version = 0;
	kal_uint32 ret=0;

    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_bank1_driver_probe] !\n");
#if 0
	if (!(new_client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL))) {
        err = -ENOMEM;
        goto exit;
    }	
    memset(new_client, 0, sizeof(struct i2c_client));
#endif
	new_client = client;

#if (PMIC_E2_DVT == 1)
	//Default setting check
	//For E2 DVT, need turn off pmic_init at preloader and uboot
	PMIC_DUMP_ALL_Register();
	pmic_VERIFY_DEFAULT_VALUE_ldvt();

	//I2C speed test
	//pmic_e2_i2c_speed_test_setting();
#endif
	
	if( get_pmic6329_chip_version() == PMIC6329_E1_CODE )
	{
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_bank1_driver_probe] PMIC6329_E1_CODE\n");		
	}
	else if( get_pmic6329_chip_version() == PMIC6329_E2_CODE )
	{
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_bank1_driver_probe] PMIC6329_E2_CODE\n");
	}
	else
	{
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_bank1_driver_probe] Unknown Chip Version\n");
	}
	
#if 1
	/* run init */
	rtc_gpio_enable_32k(RTC_GPIO_USER_PMIC);	
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_INIT_SETTING_V1] rtc_gpio_enable_32k. Reg[0xF1003078]=0x%x\n", INREG16(0xF1003078));
	PMIC_INIT_SETTING_V1();
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_INIT_SETTING_V1] Done\n");	
	PMIC_CUSTOM_SETTING_V1();
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_CUSTOM_SETTING_V1] Done\n");

	kthread_run(pmic_thread_kthread, NULL, "pmic_thread_kthread");
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_thread_kthread] Done\n");
	PMIC_EINT_SETTING();
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_EINT_SETTING] Done\n");

	#if (PMIC_E2_DVT == 1)
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[mt6329_bank1_driver_probe] Init for E2 DVT\n");
	
	//VPA and VRF18 OC interrupt check
	//Receive in do_DVT_task()
	upmu_interrupt_vpa_oc_int_en(1); //0x18
	upmu_interrupt_vrf18_oc_int_en(1); //0x18
	ret=pmic_bank1_config_interface(0x15,0x12,0xFF,0x0);
	gDVT_INTERRUPT_FLAG_4=1;
	gDVT_INTERRUPT_FLAG_5=1;

	//PWRKEY,HOMEKEY press and release interrupt
	upmu_interrupt_pwrkey_int_en(1); //0x16
	upmu_interrupt_homekey_int_en(1); //0x18 
	upmu_RG_PWRKEY_INT_SEL(1); //bank1, 0x17
	upmu_RG_HOMEKEY_INT_SEL(1); //bank1, 0x17
	#endif

	#if 0
	//PMIC reset with SYSRSTB
	ret=pmic_bank1_config_interface(0x0D,0x1,0x1,0x0);
	ret=pmic_bank1_config_interface(0x0D,0x1,0x1,0x2);
	ret=pmic_bank1_config_interface(0x0D,0x1,0x1,0x7);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC_RESET_SETTING] Enable PMIC all reset with SYSRSTB. bank1[0x0D]=0x%x\n", 
		upmu_get_reg_value_bank1(0x0D));
	#endif

    #if 1
    ret=pmic_config_interface(0x38, 0x01, 0x01, 0x5); //BATON_TDET_EN=1					
	ret=pmic_config_interface(0xE9, 0x01, 0x01, 0x1); //RG_BUF_PWD_B=1
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_bank1_driver_probe] BATON_TDET_EN=1 & RG_BUF_PWD_B=1\n");
    #endif

	PMIC_DUMP_ALL_Register();

	/* battery kernel thread for 10s check and charger in/out event */
    kthread_run(bat_thread_kthread, NULL, "bat_thread_kthread"); 
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[bat_thread_kthread] Done\n");

    charger_hv_detect_sw_workaround_init();
#endif

	/* Check PMIC trim */
	PMIC_check_trim_warning();

    /* PMIC thermal throttling */ 
    pmic_thermal_throttling_init();    
    pmic_test_cmd_init();

    return 0;                                                                                       

//exit_kfree:
//    kfree(new_client);
//exit:
//    return err;

}

static int mt6329_bank1_driver_remove(struct i2c_client *client)
{
	kfree(new_client);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
//// For PM check
///////////////////////////////////////////////////////////////////////////////////////////
void pmic_buck_ldo_status(void)
{
	kal_uint32 ret=0;	
	kal_uint8 reg_val=0;
	
	//BUCK
	ret = pmic_read_interface(0x42, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] BUCK_VAPROC_STATUS[0x42] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x55, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] BUCK_VCORE_STATUS[0x55] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x60, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] BUCK_VRF18_STATUS[0x60] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x6A, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] BUCK_VIO18_STATUS[0x6A] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x73, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] BUCK_VPA_STATUS[0x73] : 0x%x\r\n", reg_val);
	
	//LDO
	ret = pmic_read_interface(0x83, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VM12_1_STATUS[0x83] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x87, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VM12_2_STATUS[0x87] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x8D, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VM12_INT_STATUS[0x8D] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x92, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VIO28_STATUS[0x92] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x95, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VSIM_STATUS[0x95] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x98, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VSIM2_STATUS[0x98] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x9B, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VUSB_STATUS[0x9B] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0x9E, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VCAMD_STATUS[0x9E] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xA1, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VCAM_IO_STATUS[0xA1] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xA4, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VCAM_AF_STATUS[0xA4] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xA7, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VMC_STATUS[0xA7] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xAB, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VMCH_STATUS[0xAB] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xAF, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VGP_STATUS[0xAF] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xB2, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VGP2_STATUS[0xB2] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xB5, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VIBR_STATUS[0xB5] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xBE, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VRF_STATUS[0xBE] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xC1, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VTCXO_STATUS[0xC1] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xC4, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VA1_STATUS[0xC4] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xC8, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VA2_STATUS[0xC8] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xCB, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VCAMA_STATUS[0xCB] : 0x%x\r\n", reg_val);
	
	ret = pmic_read_interface(0xCF, &reg_val, 0xFF, 0x0);
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[PMIC] LDO_VRTC_STATUS[0xCF] : 0x%x\r\n", reg_val);
}

void pmic_status_for_sleep(void)
{
	pmic_buck_ldo_status();
}
EXPORT_SYMBOL(pmic_status_for_sleep);

///////////////////////////////////////////////////////////////////////////////////////////
//// platform_driver API 
///////////////////////////////////////////////////////////////////////////////////////////
kal_uint8 g_reg_value=0;
static ssize_t show_pmic_access_bank0(struct device *dev,struct device_attribute *attr, char *buf)
{
	//int ret=0;
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[show_pmic_access_bank0] 0x%x\n", g_reg_value);
	return sprintf(buf, "%u\n", g_reg_value);
}
static ssize_t store_pmic_access_bank0(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	int ret=0;
	char *pvalue = NULL;
	kal_uint8 reg_value = 0;
	kal_uint8 reg_address = 0;
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[store_pmic_access_bank0] \n");
	if(buf != NULL && size != 0)
	{
		//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[store_pmic_access_bank0] buf is %s and size is %d \n",buf,size);
		reg_address = simple_strtoul(buf,&pvalue,16);
		
		if(size > 3)
		{		
			reg_value = simple_strtoul((pvalue+1),NULL,16);		
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[store_pmic_access_bank0] write PMU reg 0x%x with value 0x%x !\n",reg_address,reg_value);
			ret=pmic_config_interface(reg_address, reg_value, 0xFF, 0x0);
		}
		else
		{	
			ret=pmic_read_interface(reg_address, &g_reg_value, 0xFF, 0x0);
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[store_pmic_access_bank0] read PMU reg 0x%x with value 0x%x !\n",reg_address,g_reg_value);
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[store_pmic_access_bank0] Please use \"cat pmic_access_bank0\" to get value\r\n");
		}		
	}	
	return size;
}
static DEVICE_ATTR(pmic_access_bank0, 0664, show_pmic_access_bank0, store_pmic_access_bank0); //664

kal_uint8 g_reg_value_bank1=0;
static ssize_t show_pmic_access_bank1(struct device *dev,struct device_attribute *attr, char *buf)
{
	//int ret=0;
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[show_pmic_access_bank1] 0x%x\n", g_reg_value_bank1);
	return sprintf(buf, "%u\n", g_reg_value_bank1);
}
static ssize_t store_pmic_access_bank1(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	int ret=0;
	char *pvalue = NULL;
	kal_uint8 reg_value = 0;
	kal_uint8 reg_address = 0;
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[store_pmic_access_bank1] \n");
	if(buf != NULL && size != 0)
	{
		//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[store_pmic_access_bank1] buf is %s and size is %d \n",buf,size);
		reg_address = simple_strtoul(buf,&pvalue,16);
		
		if(size > 3)
		{		
			reg_value = simple_strtoul((pvalue+1),NULL,16);		
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[store_pmic_access_bank1] write PMU reg 0x%x with value 0x%x !\n",reg_address,reg_value);
			ret=pmic_bank1_config_interface(reg_address, reg_value, 0xFF, 0x0);
		}
		else
		{	
			ret=pmic_bank1_read_interface(reg_address, &g_reg_value_bank1, 0xFF, 0x0);
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[store_pmic_access_bank1] read PMU reg 0x%x with value 0x%x !\n",reg_address,g_reg_value_bank1);
			xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[store_pmic_access_bank1] Please use \"cat pmic_access_bank1\" to get value\r\n");
		}		
	}	
	return size;
}
static DEVICE_ATTR(pmic_access_bank1, 0664, show_pmic_access_bank1, store_pmic_access_bank1); //664

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For LED SWITCH : pmic_led_switch
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_pmic_led_switch(struct device *dev,struct device_attribute *attr, char *buf)
{
	led_switch = false;
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[show_pmic_led_switch] led_switch = %d\n", led_switch);
	return sprintf(buf, "%d\n", led_switch);
}
/*
static ssize_t store_pmic_led_switch(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	led_switch = false;
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[store_pmic_led_switch] led_switch = %d\n", led_switch);	
	return size;
}
*/
static DEVICE_ATTR(pmic_led_switch, 0664, show_pmic_led_switch, NULL); //664  for cts test, 666->664
///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VAPROC_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VAPROC_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x42;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VAPROC_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VAPROC_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VAPROC_STATUS, 0664, show_BUCK_VAPROC_STATUS, store_BUCK_VAPROC_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VAPROC_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VAPROC_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2;
	int voltage_init=700;
	int voltage_step=25;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x44;
	kal_uint8 reg_val=0;

	ret = pmic_read_interface(reg_address, &reg_val, 0x1F, 0x0);
	ret_value = reg_val;
	ret_value = voltage_init + (ret_value * voltage_step);
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VAPROC_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VAPROC_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VAPROC_VOLTAGE, 0664, show_BUCK_VAPROC_VOLTAGE, store_BUCK_VAPROC_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VCORE_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VCORE_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x55;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VCORE_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VCORE_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VCORE_STATUS, 0664, show_BUCK_VCORE_STATUS, store_BUCK_VCORE_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VCORE_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VCORE_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2;
	int voltage_init=700;
	int voltage_step=25;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x57;
	kal_uint8 reg_val=0;

	ret = pmic_read_interface(reg_address, &reg_val, 0x1F, 0x0);
	ret_value = reg_val;
	ret_value = voltage_init + (ret_value * voltage_step);
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VCORE_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VCORE_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VCORE_VOLTAGE, 0664, show_BUCK_VCORE_VOLTAGE, store_BUCK_VCORE_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VRF18_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VRF18_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x60;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VRF18_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VRF18_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VRF18_STATUS, 0664, show_BUCK_VRF18_STATUS, store_BUCK_VRF18_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VRF18_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VRF18_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2;
	int voltage_init=1500;
	int voltage_step=20;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x62;
	kal_uint8 reg_val=0;

	ret = pmic_read_interface(reg_address, &reg_val, 0x1F, 0x0);
	ret_value = reg_val;
	ret_value = voltage_init + (ret_value * voltage_step);
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VRF18_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VRF18_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VRF18_VOLTAGE, 0664, show_BUCK_VRF18_VOLTAGE, store_BUCK_VRF18_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VIO18_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VIO18_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x6A;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VIO18_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VIO18_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VIO18_STATUS, 0664, show_BUCK_VIO18_STATUS, store_BUCK_VIO18_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VIO18_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VIO18_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2;
	int voltage_init=1500;
	int voltage_step=20;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x6C;
	kal_uint8 reg_val=0;

	ret = pmic_read_interface(reg_address, &reg_val, 0x1F, 0x0);
	ret_value = reg_val;
	ret_value = voltage_init + (ret_value * voltage_step);
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VIO18_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VIO18_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VIO18_VOLTAGE, 0664, show_BUCK_VIO18_VOLTAGE, store_BUCK_VIO18_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VPA_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VPA_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x73;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VPA_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VPA_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VPA_STATUS, 0664, show_BUCK_VPA_STATUS, store_BUCK_VPA_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : BUCK_VPA_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_BUCK_VPA_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2;
	int voltage_init=900;
	int voltage_step=100;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x75;
	kal_uint8 reg_val=0;

	ret = pmic_read_interface(reg_address, &reg_val, 0x1F, 0x0);
	ret_value = reg_val;
	ret_value = voltage_init + (ret_value * voltage_step);
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] BUCK_VPA_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_BUCK_VPA_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(BUCK_VPA_VOLTAGE, 0664, show_BUCK_VPA_VOLTAGE, store_BUCK_VPA_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VM12_1_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VM12_1_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x83;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VM12_1_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VM12_1_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VM12_1_STATUS, 0664, show_LDO_VM12_1_STATUS, store_LDO_VM12_1_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VM12_1_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VM12_1_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=1200;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VM12_1_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VM12_1_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VM12_1_VOLTAGE, 0664, show_LDO_VM12_1_VOLTAGE, store_LDO_VM12_1_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VM12_2_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VM12_2_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x87;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VM12_2_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VM12_2_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VM12_2_STATUS, 0664, show_LDO_VM12_2_STATUS, store_LDO_VM12_2_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VM12_2_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VM12_2_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=1200;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VM12_2_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VM12_2_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VM12_2_VOLTAGE, 0664, show_LDO_VM12_2_VOLTAGE, store_LDO_VM12_2_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VM12_INT_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VM12_INT_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x8D;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VM12_INT_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VM12_INT_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VM12_INT_STATUS, 0664, show_LDO_VM12_INT_STATUS, store_LDO_VM12_INT_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VM12_INT_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VM12_INT_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2;
	int voltage_init=700;
	int voltage_step=25;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x8A;
	kal_uint8 reg_val=0;

	ret = pmic_read_interface(reg_address, &reg_val, 0x1F, 0x0);
	ret_value = reg_val;
	ret_value = voltage_init + (ret_value * voltage_step);
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VM12_INT_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VM12_INT_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VM12_INT_VOLTAGE, 0664, show_LDO_VM12_INT_VOLTAGE, store_LDO_VM12_INT_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VIO28_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VIO28_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x92;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VIO28_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VIO28_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VIO28_STATUS, 0664, show_LDO_VIO28_STATUS, store_LDO_VIO28_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VIO28_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VIO28_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2800;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VIO28_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VIO28_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VIO28_VOLTAGE, 0664, show_LDO_VIO28_VOLTAGE, store_LDO_VIO28_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VSIM_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VSIM_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x95;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VSIM_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VSIM_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VSIM_STATUS, 0664, show_LDO_VSIM_STATUS, store_LDO_VSIM_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VSIM_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VSIM_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x95;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1800;
	}
	else if(reg_val == 1)
	{
		ret_value=3000;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VSIM_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VSIM_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VSIM_VOLTAGE, 0664, show_LDO_VSIM_VOLTAGE, store_LDO_VSIM_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VSIM2_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VSIM2_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x98;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VSIM2_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VSIM2_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VSIM2_STATUS, 0664, show_LDO_VSIM2_STATUS, store_LDO_VSIM2_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VSIM2_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VSIM2_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x98;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x07, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1300;
	}
	else if(reg_val == 1)
	{
		ret_value=1500;
	}
	else if(reg_val == 2)
	{
		ret_value=1800;
	}
	else if(reg_val == 3)
	{
		ret_value=2500;
	}
	else if(reg_val == 4)
	{
		ret_value=2800;
	}
	else if(reg_val == 5)
	{
		ret_value=3000;
	}
	else if(reg_val == 6)
	{
		ret_value=3300;
	}
	else if(reg_val == 7)
	{
		ret_value=3300;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VSIM2_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VSIM2_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VSIM2_VOLTAGE, 0664, show_LDO_VSIM2_VOLTAGE, store_LDO_VSIM2_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VUSB_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VUSB_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x9B;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VUSB_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VUSB_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VUSB_STATUS, 0664, show_LDO_VUSB_STATUS, store_LDO_VUSB_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VUSB_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VUSB_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=3300;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VUSB_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VUSB_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VUSB_VOLTAGE, 0664, show_LDO_VUSB_VOLTAGE, store_LDO_VUSB_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VCAMD_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VCAMD_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x9E;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VCAMD_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VCAMD_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VCAMD_STATUS, 0664, show_LDO_VCAMD_STATUS, store_LDO_VCAMD_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VCAMD_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VCAMD_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0x9E;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x7, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1300;
	}
	else if(reg_val == 1)
	{
		ret_value=1500;
	}
	else if(reg_val == 2)
	{
		ret_value=1800;
	}
	else if(reg_val == 3)
	{
		ret_value=2500;
	}
	else if(reg_val == 4)
	{
		ret_value=2800;
	}
	else if(reg_val == 5)
	{
		ret_value=3000;
	}
	else if(reg_val == 6)
	{
		ret_value=3300;
	}
	else if(reg_val == 7)
	{
		ret_value=3300;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VCAMD_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VCAMD_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VCAMD_VOLTAGE, 0664, show_LDO_VCAMD_VOLTAGE, store_LDO_VCAMD_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VCAM_IO_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VCAM_IO_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xA1;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VCAM_IO_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VCAM_IO_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VCAM_IO_STATUS, 0664, show_LDO_VCAM_IO_STATUS, store_LDO_VCAM_IO_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VCAM_IO_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VCAM_IO_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xA1;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x7, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1300;
	}
	else if(reg_val == 1)
	{
		ret_value=1500;
	}
	else if(reg_val == 2)
	{
		ret_value=1800;
	}
	else if(reg_val == 3)
	{
		ret_value=2500;
	}
	else if(reg_val == 4)
	{
		ret_value=2800;
	}
	else if(reg_val == 5)
	{
		ret_value=3000;
	}
	else if(reg_val == 6)
	{
		ret_value=3300;
	}
	else if(reg_val == 7)
	{
		ret_value=3300;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VCAM_IO_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VCAM_IO_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VCAM_IO_VOLTAGE, 0664, show_LDO_VCAM_IO_VOLTAGE, store_LDO_VCAM_IO_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VCAM_AF_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VCAM_AF_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xA4;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VCAM_AF_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VCAM_AF_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VCAM_AF_STATUS, 0664, show_LDO_VCAM_AF_STATUS, store_LDO_VCAM_AF_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VCAM_AF_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VCAM_AF_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xA4;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x7, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1300;
	}
	else if(reg_val == 1)
	{
		ret_value=1500;
	}
	else if(reg_val == 2)
	{
		ret_value=1800;
	}
	else if(reg_val == 3)
	{
		ret_value=2500;
	}
	else if(reg_val == 4)
	{
		ret_value=2800;
	}
	else if(reg_val == 5)
	{
		ret_value=3000;
	}
	else if(reg_val == 6)
	{
		ret_value=3300;
	}
	else if(reg_val == 7)
	{
		ret_value=3300;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VCAM_AF_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VCAM_AF_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VCAM_AF_VOLTAGE, 0664, show_LDO_VCAM_AF_VOLTAGE, store_LDO_VCAM_AF_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VMC_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VMC_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xA7;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VMC_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VMC_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VMC_STATUS, 0664, show_LDO_VMC_STATUS, store_LDO_VMC_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VMC_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VMC_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xA7;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x7, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1300;
	}
	else if(reg_val == 1)
	{
		ret_value=1500;
	}
	else if(reg_val == 2)
	{
		ret_value=1800;
	}
	else if(reg_val == 3)
	{
		ret_value=2500;
	}
	else if(reg_val == 4)
	{
		ret_value=2800;
	}
	else if(reg_val == 5)
	{
		ret_value=3000;
	}
	else if(reg_val == 6)
	{
		ret_value=3300;
	}
	else if(reg_val == 7)
	{
		ret_value=3300;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VMC_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VMC_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VMC_VOLTAGE, 0664, show_LDO_VMC_VOLTAGE, store_LDO_VMC_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VMCH_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VMCH_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xAB;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VMCH_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VMCH_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VMCH_STATUS, 0664, show_LDO_VMCH_STATUS, store_LDO_VMCH_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VMCH_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VMCH_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xAB;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x7, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1300;
	}
	else if(reg_val == 1)
	{
		ret_value=1500;
	}
	else if(reg_val == 2)
	{
		ret_value=1800;
	}
	else if(reg_val == 3)
	{
		ret_value=2500;
	}
	else if(reg_val == 4)
	{
		ret_value=2800;
	}
	else if(reg_val == 5)
	{
		ret_value=3000;
	}
	else if(reg_val == 6)
	{
		ret_value=3300;
	}
	else if(reg_val == 7)
	{
		ret_value=3300;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VMCH_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VMCH_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VMCH_VOLTAGE, 0664, show_LDO_VMCH_VOLTAGE, store_LDO_VMCH_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VGP_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VGP_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xAF;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VGP_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VGP_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VGP_STATUS, 0664, show_LDO_VGP_STATUS, store_LDO_VGP_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VGP_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VGP_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xAF;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x7, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1300;
	}
	else if(reg_val == 1)
	{
		ret_value=1500;
	}
	else if(reg_val == 2)
	{
		ret_value=1800;
	}
	else if(reg_val == 3)
	{
		ret_value=2500;
	}
	else if(reg_val == 4)
	{
		ret_value=2800;
	}
	else if(reg_val == 5)
	{
		ret_value=3000;
	}
	else if(reg_val == 6)
	{
		ret_value=3300;
	}
	else if(reg_val == 7)
	{
		ret_value=3300;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VGP_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VGP_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VGP_VOLTAGE, 0664, show_LDO_VGP_VOLTAGE, store_LDO_VGP_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VGP2_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VGP2_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xB2;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VGP2_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VGP2_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VGP2_STATUS, 0664, show_LDO_VGP2_STATUS, store_LDO_VGP2_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VGP2_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VGP2_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xB2;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x7, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1300;
	}
	else if(reg_val == 1)
	{
		ret_value=1500;
	}
	else if(reg_val == 2)
	{
		ret_value=1800;
	}
	else if(reg_val == 3)
	{
		ret_value=2500;
	}
	else if(reg_val == 4)
	{
		ret_value=2800;
	}
	else if(reg_val == 5)
	{
		ret_value=3000;
	}
	else if(reg_val == 6)
	{
		ret_value=3300;
	}
	else if(reg_val == 7)
	{
		ret_value=3300;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VGP2_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VGP2_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VGP2_VOLTAGE, 0664, show_LDO_VGP2_VOLTAGE, store_LDO_VGP2_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VIBR_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VIBR_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xB5;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VIBR_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VIBR_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VIBR_STATUS, 0664, show_LDO_VIBR_STATUS, store_LDO_VIBR_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VIBR_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VIBR_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xB5;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x7, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1300;
	}
	else if(reg_val == 1)
	{
		ret_value=1500;
	}
	else if(reg_val == 2)
	{
		ret_value=1800;
	}
	else if(reg_val == 3)
	{
		ret_value=2500;
	}
	else if(reg_val == 4)
	{
		ret_value=2800;
	}
	else if(reg_val == 5)
	{
		ret_value=3000;
	}
	else if(reg_val == 6)
	{
		ret_value=3300;
	}
	else if(reg_val == 7)
	{
		ret_value=3300;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VIBR_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VIBR_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VIBR_VOLTAGE, 0664, show_LDO_VIBR_VOLTAGE, store_LDO_VIBR_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VRF_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VRF_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xBE;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VRF_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VRF_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VRF_STATUS, 0664, show_LDO_VRF_STATUS, store_LDO_VRF_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VRF_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VRF_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2800;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VRF_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VRF_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VRF_VOLTAGE, 0664, show_LDO_VRF_VOLTAGE, store_LDO_VRF_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VTCXO_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VTCXO_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xC1;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VTCXO_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VTCXO_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VTCXO_STATUS, 0664, show_LDO_VTCXO_STATUS, store_LDO_VTCXO_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VTCXO_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VTCXO_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2800;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VTCXO_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VTCXO_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VTCXO_VOLTAGE, 0664, show_LDO_VTCXO_VOLTAGE, store_LDO_VTCXO_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VA1_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VA1_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xC4;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VA1_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VA1_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VA1_STATUS, 0664, show_LDO_VA1_STATUS, store_LDO_VA1_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VA1_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VA1_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xC4;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x3, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=2500;
	}
	else if(reg_val == 1)
	{
		ret_value=2100;
	}
	else if(reg_val == 2)
	{
		ret_value=2000;
	}
	else if(reg_val == 3)
	{
		ret_value=1800;
	}	
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VA1_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VA1_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VA1_VOLTAGE, 0664, show_LDO_VA1_VOLTAGE, store_LDO_VA1_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VA2_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VA2_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xC8;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VA2_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VA2_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VA2_STATUS, 0664, show_LDO_VA2_STATUS, store_LDO_VA2_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VA2_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VA2_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xC8;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x1, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=2500;
	}
	else if(reg_val == 1)
	{
		ret_value=2800;
	}	
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VA2_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VA2_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VA2_VOLTAGE, 0664, show_LDO_VA2_VOLTAGE, store_LDO_VA2_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VCAMA_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VCAMA_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xCB;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VCAMA_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VCAMA_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VCAMA_STATUS, 0664, show_LDO_VCAMA_STATUS, store_LDO_VCAMA_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VCAMA_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VCAMA_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xCB;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x3, 0x4);
	
	if(reg_val == 0)
	{
		ret_value=1500;
	}
	else if(reg_val == 1)
	{
		ret_value=1800;
	}
	else if(reg_val == 2)
	{
		ret_value=2500;
	}
	else if(reg_val == 3)
	{
		ret_value=2800;
	}
	else
	{
		ret_value=0;
	}	
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VCAMA_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VCAMA_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VCAMA_VOLTAGE, 0664, show_LDO_VCAMA_VOLTAGE, store_LDO_VCAMA_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VRTC_STATUS
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VRTC_STATUS(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=0;
	
	kal_uint32 ret=0;
	kal_uint8 reg_address=0xCF;
	kal_uint8 reg_val=0;
	
	ret = pmic_read_interface(reg_address, &reg_val, 0x01, 0x0);
	ret_value = reg_val;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VRTC_STATUS : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VRTC_STATUS(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VRTC_STATUS, 0664, show_LDO_VRTC_STATUS, store_LDO_VRTC_STATUS);

///////////////////////////////////////////////////////////////////////////////////////////
//// Create File For EM : LDO_VRTC_VOLTAGE
///////////////////////////////////////////////////////////////////////////////////////////
static ssize_t show_LDO_VRTC_VOLTAGE(struct device *dev,struct device_attribute *attr, char *buf)
{
	int ret_value=2800;
	
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] LDO_VRTC_VOLTAGE : %d\n", ret_value);
	return sprintf(buf, "%u\n", ret_value);
}
static ssize_t store_LDO_VRTC_VOLTAGE(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[EM] Not Support Write Function\n");	
	return size;
}
static DEVICE_ATTR(LDO_VRTC_VOLTAGE, 0664, show_LDO_VRTC_VOLTAGE, store_LDO_VRTC_VOLTAGE);

///////////////////////////////////////////////////////////////////////////////////////////
//// platform_driver API 
///////////////////////////////////////////////////////////////////////////////////////////
static int mt_pmic_probe(struct platform_device *dev)	
{	
	int ret_device_file = 0;

    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "******** mt_pmic_probe!! ********\n" );
	
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_pmic_access_bank0);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_pmic_access_bank1);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_pmic_led_switch);
	
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VAPROC_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VCORE_STATUS);	
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VRF18_STATUS);	
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VIO18_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VPA_STATUS);
	
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VAPROC_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VCORE_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VRF18_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VIO18_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_BUCK_VPA_VOLTAGE);
	
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VM12_1_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VM12_2_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VM12_INT_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VIO28_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VSIM_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VSIM2_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VUSB_STATUS);	
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VCAMD_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VCAM_IO_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VCAM_AF_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VMC_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VMCH_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VGP_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VGP2_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VIBR_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VRF_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VTCXO_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VA1_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VA2_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VCAMA_STATUS);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VRTC_STATUS);	

	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VM12_1_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VM12_2_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VM12_INT_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VIO28_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VSIM_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VSIM2_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VUSB_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VCAMD_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VCAM_IO_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VCAM_AF_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VMC_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VMCH_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VGP_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VGP2_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VIBR_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VRF_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VTCXO_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VA1_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VA2_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VCAMA_VOLTAGE);
	ret_device_file = device_create_file(&(dev->dev), &dev_attr_LDO_VRTC_VOLTAGE);
	
    return 0;
}

struct platform_device mt_pmic_device = {
    .name   = "mt-pmic",
    .id	    = -1,
};

static struct platform_driver mt_pmic_driver = {
    .probe		= mt_pmic_probe,
    .driver     = {
        .name = "mt-pmic",
    },
};

#define PMIC_I2C_BUSNUM 2
static struct i2c_board_info __initdata pmic_bank1_dev = { I2C_BOARD_INFO("mt6329_pmic_bank1", 0x61) };

static int __init mt6329_bank1_init(void)
{	
	int ret;
	
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_bank1_init] init start\n");

	i2c_register_board_info(PMIC_I2C_BUSNUM, &pmic_bank1_dev, 1);

	if(i2c_add_driver(&mt6329_bank1_driver)!=0)
	{
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_bank1_init] failed to register pmic bank1 i2c driver.\n");
	}
	else
	{
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_bank1_init] Success to register pmic bank1 i2c driver.\n");
	}

	// PMIC user space access interface
	ret = platform_device_register(&mt_pmic_device);
    if (ret) {
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "****[mt6329_bank1_init] Unable to device register(%d)\n", ret);
		return ret;
    }	
    ret = platform_driver_register(&mt_pmic_driver);
    if (ret) {
		xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "****[mt6329_bank1_init] Unable to register driver (%d)\n", ret);
		return ret;
    }
	
	return 0;		
}

static void __exit mt6329_bank1_exit(void)
{
	i2c_del_driver(&mt6329_bank1_driver);
}

module_init(mt6329_bank1_init);
module_exit(mt6329_bank1_exit);
   
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MediaTek I2C mt6329 Bank1 Driver");
MODULE_AUTHOR("James Lo<james.lo@mediatek.com>");

