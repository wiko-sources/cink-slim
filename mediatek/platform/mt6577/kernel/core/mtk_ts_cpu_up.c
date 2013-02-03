#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/dmi.h>
#include <linux/acpi.h>
#include <linux/thermal.h>
#include <linux/platform_device.h>
#include <linux/aee.h>
#include <linux/xlog.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>

#include "mach/mtk_thermal_monitor.h"
#include <mach/system.h>

#include "mach/mt6577_typedefs.h"
#include "mach/mt6577_thermal.h"
#include <mach/hotplug.h>

#define DVFS_OPP1 (1)
#define DVFS_OPP2 (2)
#define DVFS_OPP3 (3)
#define DVFS_OPP4 (4)
#define DVFS_OPP5 (5)
#define DVFS_OPP6 (6)

extern void cpufreq_thermal_protect(int limited_freq);

static unsigned int interval = 0; /* seconds, 0 : no auto polling */
static unsigned int trip_temp[10] = {120000,110000,100000,90000,80000,70000,65000,60000,55000,50000};
static unsigned int cl_dev_F1x1_state = 0;
static unsigned int cl_dev_F2x1_state = 0;
static unsigned int cl_dev_F3x1_state = 0;
static unsigned int cl_dev_F4x1_state = 0;
static unsigned int cl_dev_F5x1_state = 0;
static unsigned int cl_dev_F6x1_state = 0;
static unsigned int cl_dev_sysrst_state = 0;
static struct thermal_zone_device *thz_dev;

static struct thermal_cooling_device *cl_dev_F1x1;
static struct thermal_cooling_device *cl_dev_F2x1;
static struct thermal_cooling_device *cl_dev_F3x1;
static struct thermal_cooling_device *cl_dev_F4x1;
static struct thermal_cooling_device *cl_dev_F5x1;
static struct thermal_cooling_device *cl_dev_F6x1;
static struct thermal_cooling_device *cl_dev_sysrst;
static int mtktscpu_debug_log = 0;
static int kernelmode = 0;
static int g_THERMAL_TRIP[10] = {0,0,0,0,0,0,0,0,0,0};

static int num_trip=6;
int MA_len=5;
int MA_len_temp=0;
static int proc_write_flag=0;

static char g_bind0[20]="mtktscpu-sysrst";
static char g_bind1[20]="mtktscpu-F6x1";
static char g_bind2[20]="mtktscpu-F5x1";
static char g_bind3[20]="mtktscpu-F4x1";
static char g_bind4[20]="mtktscpu-F3x1";
static char g_bind5[20]="mtktscpu-F2x1";
static char g_bind6[20]={0};
static char g_bind7[20]={0};
static char g_bind8[20]={0};
static char g_bind9[20]={0};

static DEFINE_MUTEX(TS_lock);

#define MTKTSCPU_TEMP_CRIT 120000 /* 120.000 degree Celsius */

#define mtktscpu_dprintk(fmt, args...)   \
do {                                    \
	if (mtktscpu_debug_log) {                \
		xlog_printk(ANDROID_LOG_INFO, "Power/CPU_Thermal", fmt, ##args); \
	}                                   \
} while(0)

extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
extern int IMM_IsAdcInitReady(void);
static int last_cpu_t=0;
int last_abb_t=0;
int	last_pa_t=0;

static kal_int32 g_adc_ge = 0;
static kal_int32 g_adc_oe = 0;
static kal_int32 g_o_vts = 0;
static kal_int32 g_o_vbg = 0;
static kal_int32 g_degc_cali = 0;
static kal_int32 g_adc_cali_en = 0;
static kal_int32 g_o_vts_abb = 0;
static kal_int32 g_o_slope = 0;
static kal_int32 g_o_slope_sign = 0;
static kal_int32 g_id = 0;

static kal_int32 g_ge = 0;
static kal_int32 g_oe = 0;
static kal_int32 y_vts_plus = 0;
static kal_int32 g_gain = 0;
static kal_int32 g_x_roomt = 0;
static kal_int32 y_vts_abb_plus = 0;
static kal_int32 g_x_roomt_abb = 0;

static int y_curr_repeat_times = 1;


struct file *openFile(char *path,int flag,int mode) 
{ 
	struct file *fp=NULL; 
	int err = 0;
	mm_segment_t oldfs; 
	
	oldfs = get_fs();
	set_fs(get_ds());
	fp = filp_open(path, flag, mode);
	set_fs(oldfs);
	if(IS_ERR(fp)) 
	{
		err = PTR_ERR(fp);
		return NULL;
	}
	return fp;
} 
int file_read(struct file* file, loff_t offset, unsigned char* data, unsigned int size) 
{
	int ret;
	mm_segment_t oldfs; 

	oldfs = get_fs();
	set_fs(get_ds());

	ret = vfs_read(file, data, size, &offset);

	set_fs(oldfs);
	return ret;
}
int file_write(struct file* file, loff_t offset, unsigned char* data, unsigned int size) 
{
	int ret;
	mm_segment_t oldfs; 

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	ret = vfs_write(file, data, size, &offset);

	set_fs(oldfs);
	return ret;
}
int closeFile(struct file *fp) 
{ 
	filp_close(fp,NULL); 
	return 0; 
}

static void thermal_cal_prepare(void)
{
	kal_uint32 temp = 0;
	
	temp = DRV_Reg32(0xF1019048);
	g_adc_ge = (temp & 0x000000FF);
	mtktscpu_dprintk("temp = 0x%x, g_adc_ge = 0x%x\n", temp, g_adc_ge);
	
	temp = DRV_Reg32(0xF1019044);
	g_adc_oe = (temp & 0x000000FF);
	mtktscpu_dprintk("temp = 0x%x, g_adc_oe = 0x%x\n", temp, g_adc_oe);
	
	temp = DRV_Reg32(0xF1019040);
	//g_o_vts_abb   = ((temp & 0xFC000000) >> 26);
	g_o_vts       = ((temp & 0x03FE0000) >> 17);
	g_o_vbg       = ((temp & 0x0001FF00) >> 8);
	//g_degc_cali   = ((temp & 0x000000FE) >> 1);
	g_degc_cali   = ((temp & 0x0000007E) >> 1);
	g_adc_cali_en = ((temp & 0x00000001) >> 0);

	g_o_slope     = ((temp & 0xFC000000) >> 26);
	g_o_slope_sign= ((temp & 0x00000080) >> 7);    

	temp = DRV_Reg32(0xF1019100);
	g_id = ((temp & 0x80000000) >> 31);

	if(g_id==0)
	{
		g_o_slope = 0;
	}
	
	if(g_adc_cali_en == 1)
	{
		//thermal_enable = true;        
	}
	else
	{
		g_adc_ge = 128;
		g_adc_oe = 128;
		g_o_vts = 292;
		g_o_vbg = 167;
		g_degc_cali = 40;
		g_o_slope = 0;
		g_o_slope_sign = 0;
	}
	
	mtktscpu_dprintk("temp = 0x%x, g_o_vts = 0x%x, g_o_vbg = 0x%x, g_degc_cali = 0x%x, g_adc_cali_en = 0x%x, g_o_vts_abb = 0x%x, g_o_slope = 0x%x, g_o_slope_sign = 0x%x, g_id = 0x%x\n", 
		temp, g_o_vts, g_o_vbg, g_degc_cali, g_adc_cali_en, g_o_vts_abb, g_o_slope, g_o_slope_sign, g_id);

}

static void thermal_cal_prepare_2(kal_uint32 ret)
{
	kal_int32 y_vbg_plus = ret;
	kal_int32 format_1 = 0;
	kal_int32 format_2 = 0;
	kal_int32 format_3 = 0;
	kal_int32 format_3_abb = 0;
	kal_int32 format_4 = 0;
	kal_int32 format_4_abb = 0;
	
	g_ge = ((g_adc_ge - 128) * 10000 ) / 4096; // ge * 10000
	g_oe = (g_adc_oe - 128);
	
	format_1 = (y_vbg_plus - g_oe);
	format_2 = ((g_o_vbg + 1800) - g_oe);
	format_3 = ((g_o_vts + 2700) - g_oe);
	//format_3_abb = ((g_o_vts + 2700 + g_o_vts_abb - 20) - g_oe);
	format_3_abb = ((g_o_vts + 2700) - g_oe);
	
	y_vts_plus = ((format_1 * format_3) / format_2) + g_oe;
	y_vts_abb_plus = ((format_1 * format_3_abb) / format_2) + g_oe;
	g_gain = (10000 + g_ge) * format_1 / format_2; // gain * 10000
	
	format_4 = (y_vts_plus - g_oe);
	format_4_abb = (y_vts_abb_plus - g_oe);
	
	g_x_roomt = (((format_4 * 10000) / 4096) * 10000) / g_gain; // x_roomt * 10000
	g_x_roomt_abb = (((format_4_abb * 10000) / 4096) * 10000) / g_gain; // x_roomt * 10000
	
	mtktscpu_dprintk("g_ge = %d, g_oe = %d, y_vts_prime = %d, g_gain = %d, g_x_roomt = %d, y_vts_abb_prime = %d, g_x_roomt_abb = %d\n", 
		g_ge, g_oe, y_vts_plus, g_gain, g_x_roomt, y_vts_abb_plus, g_x_roomt_abb);
}

static kal_int32 thermal_cal_exec(kal_uint32 ret)
{
	kal_int32 t_current = 0;
	kal_int32 y_curr = ret;
	kal_int32 format_1 = 0;
	kal_int32 format_2 = 0;
	kal_int32 format_3 = 0;
	kal_int32 format_4 = 0;

	if(ret==0)
	{
		return 0;
	}
	
	format_1 = (g_degc_cali / 2);
	format_2 = (y_curr - g_oe);
	format_3 = (((((format_2) * 10000) / 4096) * 10000) / g_gain) - g_x_roomt;

	//format_4 = ((format_3 * 100) / 139); // uint = 0.1 deg
	if(g_o_slope_sign==0)
	{
		//format_4 = ((format_3 * 100) / (139+g_o_slope)); // uint = 0.1 deg
		format_4 = ((format_3 * 100) / (165+g_o_slope)); // uint = 0.1 deg
	}
	else
	{
		//format_4 = ((format_3 * 100) / (139-g_o_slope)); // uint = 0.1 deg
		format_4 = ((format_3 * 100) / (165-g_o_slope)); // uint = 0.1 deg
	} 
	format_4 = format_4 - (2 * format_4);
	
	t_current = (format_1 * 10) + format_4; // uint = 0.1 deg
	
	return t_current;
}

static kal_int32 thermal_cal_exec_abb(kal_uint32 ret)
{
	kal_int32 t_current = 0;
	kal_int32 y_curr = ret;
	kal_int32 format_1 = 0;
	kal_int32 format_2 = 0;
	kal_int32 format_3 = 0;
	kal_int32 format_4 = 0;

	if(ret==0)
	{
		return 0;
	}
	
	format_1 = (g_degc_cali / 2);
	format_2 = (y_curr - g_oe);
	format_3 = (((((format_2) * 10000) / 4096) * 10000) / g_gain) - g_x_roomt_abb;

	//format_4 = ((format_3 * 100) / 139); // uint = 0.1 deg 
	if(g_o_slope_sign==0)
	{
		//format_4 = ((format_3 * 100) / (139+g_o_slope)); // uint = 0.1 deg
		format_4 = ((format_3 * 100) / (165+g_o_slope)); // uint = 0.1 deg
	}
	else
	{
		//format_4 = ((format_3 * 100) / (139-g_o_slope)); // uint = 0.1 deg
		format_4 = ((format_3 * 100) / (165-g_o_slope)); // uint = 0.1 deg
	}    
	format_4 = format_4 - (2 * format_4);
	
	t_current = (format_1 * 10) + format_4; // uint = 0.1 deg
	
	return t_current;
}

static int thermal_auxadc_get_data(int times, int Channel)
{
	int ret = 0, data[4], i, ret_value = 0, ret_temp = 0;

	if( IMM_IsAdcInitReady() == 0 )
	{
        mtktscpu_dprintk("[thermal_auxadc_get_data]: AUXADC is not ready\n");
		return 0;
	}
	
	i = times;
	while (i--)
	{
		ret_value = IMM_GetOneChannelValue(Channel, data, &ret_temp);
		ret += ret_temp;
        mtktscpu_dprintk("[thermal_auxadc_get_data(ADCIN5)]: ret_temp=%d\n",ret_temp);        
//		msleep(10);
	}
	
	ret = ret / times;
	
	return ret;
}

typedef struct{
    int PATemp;
    int ADCValue;
} PA_TEMPERATURE;


int pa_adc_to_temp(int Res)
{
    int i = 0;
    int RES1 = 0, RES2 = 0;
    int Tpa_Value = -200, TMP1 = 0, TMP2 = 0;

    PA_TEMPERATURE PA_Temperature_Table[] = {
        {-10,3674},
        {  5,3326},
        { 20,2855},
        { 35,2343},
        { 45,1954},
        { 55,1593},
        { 60,1456},
        { 65,1308}    
    };

    if(Res >= PA_Temperature_Table[0].ADCValue)
    {
        Tpa_Value = -10;
    }
    else if(Res <= PA_Temperature_Table[7].ADCValue)
    {
        Tpa_Value = 65;
    }
    else
    {
        RES1 = PA_Temperature_Table[0].ADCValue;
        TMP1 = PA_Temperature_Table[0].PATemp;
        
        for (i = 0; i <= 7; i++)
        {
            if(Res >= PA_Temperature_Table[i].ADCValue)
            {
                RES2 = PA_Temperature_Table[i].ADCValue;
                TMP2 = PA_Temperature_Table[i].PATemp;
                break;
            }
            else
            {
                RES1 = PA_Temperature_Table[i].ADCValue;
                TMP1 = PA_Temperature_Table[i].PATemp;
            }
        }
        
        Tpa_Value = (((Res - RES2) * TMP1) + ((RES1 - Res) * TMP2)) / (RES1-RES2);
    }
        
    return Tpa_Value;    
}

static void thermal_calibration(void)
{
	kal_uint32 ret = 0;
	
	//thermal_cal_prepare();
	
	//get VBG - workaround
	if(g_adc_cali_en == 1)
	{
		ret = g_o_vbg + 1800;
		mtktscpu_dprintk("[thermal_calibration]: VBG = g_o_vbg(%d) + 1800 = %d \n", g_o_vbg, ret);
	}
	else
	{
		ret = 1967; // y_vbg_plus default value
		mtktscpu_dprintk("[thermal_calibration]: VBG = y_vbg_plus = %d \n", ret);            
	}
	
	if (g_adc_cali_en == 1)
	{
		thermal_cal_prepare_2(ret); // will save DEGC_cali, OE, Gain and X_roomt in memory for future use
	}
	else
	{
		thermal_cal_prepare_2(ret); // will save DEGC_cali, OE, Gain and X_roomt in memory for future use
	}
	
	mtktscpu_dprintk("[thermal_calibration]: Done\n");
}

static int MA_counter=0, MA_first_time=0;
static int mtktscpu_get_hw_temp(void)
{
	int ret=0, len=0;
	int t_ret0 = 0, t_ret1=0, t_ret2 = 0;
	static int cpu[60]={0};
	static int abb[60]={0};
	static int pa[60]={0};
	int	i=0;

	//get HW CPU temp (TSMCU)
mutex_lock(&TS_lock);
	
	if(proc_write_flag==1)
	{
		MA_counter=0, MA_first_time=0;
		MA_len=MA_len_temp;
		proc_write_flag=0;
		mtktscpu_dprintk("[mtktscpu_get_hw_temp]:MA_len=%d",MA_len);
	}
	DRV_WriteReg32(TS_CON1, 0x0002);
	DRV_WriteReg32(TS_CON2, 0x0000);
	msleep(1);
	ret = thermal_auxadc_get_data(y_curr_repeat_times, 5);
	mtktscpu_dprintk("[mtktscpu_get_hw_temp]: TSMCU average %d times channel 5 (0x%x,0x%x,0x%x,0x%x) = %d\n",
			y_curr_repeat_times, DRV_Reg16(TS_CON0), DRV_Reg16(TS_CON1), DRV_Reg16(TS_CON2), DRV_Reg16(TS_CON3), ret);

	t_ret0 = thermal_cal_exec(ret);
	t_ret0 = t_ret0 * 100;
	cpu[MA_counter] = t_ret0;
	mtktscpu_dprintk("[mtktscpu_get_hw_temp] T_MCU, %d\n", t_ret0);


	//get HW Abb temp(TS_abb)
	DRV_WriteReg32(TS_CON1, 0x0002);
	DRV_WriteReg32(TS_CON2, 0x0100);
	msleep(1);
	ret = thermal_auxadc_get_data(y_curr_repeat_times, 5);
	mtktscpu_dprintk("[mtktsabb_get_hw_temp]: TSABB average %d times channel 5 (0x%x,0x%x,0x%x,0x%x) = %d\n",
			y_curr_repeat_times, DRV_Reg16(TS_CON0), DRV_Reg16(TS_CON1), DRV_Reg16(TS_CON2), DRV_Reg16(TS_CON3), ret);

	t_ret1 = thermal_cal_exec_abb(ret);
	t_ret1 = t_ret1 * 100;
	abb[MA_counter] = t_ret1;
	mtktscpu_dprintk("[mtktscpu_get_hw_temp] T_ABB, %d\n", t_ret1);

	//get HW PA temp
	ret = thermal_auxadc_get_data(y_curr_repeat_times, 3);
	t_ret2 = pa_adc_to_temp(ret);
    t_ret2 = t_ret2 * 1000;
	pa[MA_counter] = t_ret2;
	mtktscpu_dprintk("[mtktspa_get_hw_temp] T_PA, %d\n", t_ret2);
	
	if(MA_counter==0 && MA_first_time==0 && MA_len!=1)
	{
		MA_counter++;
		DRV_WriteReg32(TS_CON1, 0x0002);
		DRV_WriteReg32(TS_CON2, 0x0000);
		msleep(1);
		ret = thermal_auxadc_get_data(y_curr_repeat_times, 5);
		mtktscpu_dprintk("[mtktscpu_get_hw_temp]: TSMCU average %d times channel 5 (0x%x,0x%x,0x%x,0x%x) = %d\n",
				y_curr_repeat_times, DRV_Reg16(TS_CON0), DRV_Reg16(TS_CON1), DRV_Reg16(TS_CON2), DRV_Reg16(TS_CON3), ret);

		t_ret0 = thermal_cal_exec(ret);
		t_ret0 = t_ret0 * 100;
		cpu[MA_counter] = t_ret0;
		mtktscpu_dprintk("[mtktscpu_get_hw_temp] T_MCU, %d\n", t_ret0);


		//get HW Abb temp(TS_abb)
		DRV_WriteReg32(TS_CON1, 0x0002);
		DRV_WriteReg32(TS_CON2, 0x0100);
		msleep(1);
		ret = thermal_auxadc_get_data(y_curr_repeat_times, 5);
		mtktscpu_dprintk("[mtktsabb_get_hw_temp]: TSABB average %d times channel 5 (0x%x,0x%x,0x%x,0x%x) = %d\n",
				y_curr_repeat_times, DRV_Reg16(TS_CON0), DRV_Reg16(TS_CON1), DRV_Reg16(TS_CON2), DRV_Reg16(TS_CON3), ret);
	
		t_ret1 = thermal_cal_exec_abb(ret);
		t_ret1 = t_ret1 * 100;
		abb[MA_counter] = t_ret1;
		mtktscpu_dprintk("[mtktscpu_get_hw_temp] T_ABB, %d\n", t_ret1);


		//get HW PA temp
		ret = thermal_auxadc_get_data(y_curr_repeat_times, 3);
		t_ret2 = pa_adc_to_temp(ret);
		t_ret2 = t_ret2 * 1000;
		pa[MA_counter] = t_ret2;
		mtktscpu_dprintk("[mtktspa_get_hw_temp] T_PA, %d\n", t_ret2);
	}	
	DRV_WriteReg32(TS_CON2, 0x0300);


	MA_counter++;
	if(MA_first_time==0)
		len = MA_counter;
	else
		len = MA_len;

	t_ret0 = t_ret1 = t_ret2 = 0;
	for(i=0; i<len; i++)
	{
		t_ret0 += cpu[i];
		t_ret1 += abb[i];
		t_ret2 += pa[i];
	}
	last_cpu_t = t_ret0 / len;
	last_abb_t = t_ret1 / len;
	last_pa_t = t_ret2 / len;

	mtktscpu_dprintk("[mtktscpu_get_hw_temp] MA_MCU=%d, MA_ABB=%d, MA_PA=%d\n", last_cpu_t, last_abb_t, last_pa_t);
	mtktscpu_dprintk("[mtktscpu_get_hw_temp] MA_counter=%d, MA_first_time=%d, MA_len=%d\n", MA_counter, MA_first_time, MA_len);

	
	if(MA_counter==MA_len )
	{
		MA_counter=0;
		MA_first_time=1;
	}

mutex_unlock(&TS_lock);

	return last_cpu_t;
}

//static int last_temp = 0;	
static int mtktscpu_get_temp(struct thermal_zone_device *thermal,
			 unsigned long *t)
{
	  int curr_temp;
//	mtktscpu_dprintk("[mtktscpu_get_temp] start \n");
	
	curr_temp = mtktscpu_get_hw_temp();
	*t = (unsigned long) curr_temp;
/*   	if (0 == last_temp)
    {
        // TODO: Over 100 degree C, treat as abnormal
        if (curr_temp > 100000)
        {
            *t = 0;
        }
        else
        {
            *t = (unsigned long) curr_temp;
            last_temp = curr_temp;
        }
    }
    else
    {
        // TODO: If the gap is over 25 degree C, treat it as abnormal
        if ((curr_temp - last_temp) > 25000)
        {
            *t = (unsigned long) last_temp;
        }
        else
        {
            *t = (unsigned long) curr_temp;
            last_temp = curr_temp;
        }
    }
*/
//	  mtktscpu_dprintk("[mtktscpu_get_temp] stop \n");
	return 0;
}

static int mtktscpu_bind(struct thermal_zone_device *thermal,
		struct thermal_cooling_device *cdev)
{
	int table_val=0;

	if(!strcmp(cdev->type, g_bind0))
	{
		table_val = 0;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind1))
	{
		table_val = 1;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind2))
	{
		table_val = 2;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind3))
	{
		table_val = 3;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind4))
	{
		table_val = 4;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind5))
	{
		table_val = 5;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind6))
	{
		table_val = 6;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind7))
	{
		table_val = 7;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind8))
	{
		table_val = 8;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind9))
	{
		table_val = 9;
		mtktscpu_dprintk("[mtktscpu_bind] %s\n", cdev->type);
	}
	else
	{	
		return 0;
	}

	if (mtk_thermal_zone_bind_cooling_device(thermal, table_val, cdev)) {
		mtktscpu_dprintk("[mtktscpu_bind] error binding cooling dev\n");
	return -EINVAL;
	} else {
		mtktscpu_dprintk("[mtktscpu_bind] binding OK, %d\n", table_val);
	}

	return 0;
}

static int mtktscpu_unbind(struct thermal_zone_device *thermal,
		struct thermal_cooling_device *cdev)
{
	int table_val=0;

	if(!strcmp(cdev->type, g_bind0))
	{
		table_val = 0;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind1))
	{
		table_val = 1;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind2))
	{
		table_val = 2;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind3))
	{
		table_val = 3;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind4))
	{
		table_val = 4;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind5))
	{
		table_val = 5;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind6))
	{
		table_val = 6;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind7))
	{
		table_val = 7;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind8))
	{
		table_val = 8;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else if(!strcmp(cdev->type, g_bind9))
	{
		table_val = 9;
		mtktscpu_dprintk("[mtktscpu_unbind] %s\n", cdev->type);
	}
	else
		return 0;
	
	
	
	if (thermal_zone_unbind_cooling_device(thermal, table_val, cdev)) {
		mtktscpu_dprintk("[mtktscpu_unbind] error unbinding cooling dev\n");
	return -EINVAL;
	} else {
		mtktscpu_dprintk("[mtktscpu_unbind] unbinding OK\n");
	}

	return 0;
}

static int mtktscpu_get_mode(struct thermal_zone_device *thermal,
			enum thermal_device_mode *mode)
{
//	mtktscpu_dprintk("[mtktscpu_get_mode] %d\n", kernelmode);

	*mode = (kernelmode) ? THERMAL_DEVICE_ENABLED
			 : THERMAL_DEVICE_DISABLED;

	return 0;
}

static int mtktscpu_set_mode(struct thermal_zone_device *thermal,
			enum thermal_device_mode mode)
{
//	mtktscpu_dprintk("[mtktscpu_set_mode] %d\n", mode);

	kernelmode = mode;
	
	return 0;
}

static int mtktscpu_get_trip_type(struct thermal_zone_device *thermal, int trip,
		 enum thermal_trip_type *type)
{
//	mtktscpu_dprintk("[mtktscpu_get_trip_type] %d\n", trip);

	*type = g_THERMAL_TRIP[trip];
	return 0;
}

static int mtktscpu_get_trip_temp(struct thermal_zone_device *thermal, int trip,
		 unsigned long *temp)
{
//	mtktscpu_dprintk("[mtktscpu_get_trip_temp] %d\n", trip);

	*temp = trip_temp[trip];
	return 0;
}

static int mtktscpu_get_crit_temp(struct thermal_zone_device *thermal,
		 unsigned long *temperature)
{
//	mtktscpu_dprintk("[mtktscpu_get_crit_temp] \n");
	
	*temperature = MTKTSCPU_TEMP_CRIT;
	
	return 0;
}

/* bind callback functions to thermalzone */
static struct thermal_zone_device_ops mtktscpu_dev_ops = {
	.bind = mtktscpu_bind,
	.unbind = mtktscpu_unbind,
	.get_temp = mtktscpu_get_temp,
	.get_mode = mtktscpu_get_mode,
	.set_mode = mtktscpu_set_mode,
	.get_trip_type = mtktscpu_get_trip_type,
	.get_trip_temp = mtktscpu_get_trip_temp,
	.get_crit_temp = mtktscpu_get_crit_temp,
};


static int CPU_full_loading(void)
{
	char buf1[12];
	char performance[11]="performance";
	char ondemand[8]="ondemand";
	char powersave[9]="powersave";
	char userspace[9]="userspace";
	struct file *fp=NULL; 
	int ret;

	fp=openFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor",O_RDWR,0);
	if (fp!=NULL) 
	{
		mtktscpu_dprintk("[mtktscpu_full_loading]:open file ");
		memset(buf1,0,sizeof(buf1));
		if ((ret = file_read(fp, 0, buf1, sizeof(buf1)))>0) 
			mtktscpu_dprintk("[mtktscpu_full_loading]:buf1:%s\n",buf1); 
		else 
			mtktscpu_dprintk("read file error %d\n",ret);
		
		if( !strncmp(buf1, ondemand, sizeof(ondemand)) )
		{	
			mtktscpu_dprintk("[mtktscpu_full_loading]:ondemand->performance->ondemand\n"); 
			file_write(fp, 0, performance, sizeof(performance));
			file_write(fp, 0, ondemand, sizeof(ondemand));
		}
		else if( !strncmp(buf1, performance, sizeof(performance)) )
		{
			mtktscpu_dprintk("[mtktscpu_full_loading]:performance->ondemand->performance\n"); 
			file_write(fp, 0, ondemand, sizeof(ondemand));
			file_write(fp, 0, performance, sizeof(performance));
		}
		else if( !strncmp(buf1, powersave, sizeof(powersave)) )
		{
			mtktscpu_dprintk("[mtktscpu_full_loading]:powersave->ondemand->powersave\n"); 
			file_write(fp, 0, ondemand, sizeof(ondemand));
			file_write(fp, 0, powersave, sizeof(powersave));
		}
		else if( !strncmp(buf1, userspace, sizeof(userspace)) )
		{
			mtktscpu_dprintk("[mtktscpu_full_loading]:userspace->ondemand->userspace\n"); 
			file_write(fp, 0, ondemand, sizeof(ondemand));
			file_write(fp, 0, userspace, sizeof(userspace));
		}
		 
		closeFile(fp); 
		return 0;
	}
	else{
		mtktscpu_dprintk("[mtktscpu_full_loading]open fail\n");
		return 1;
	}	
}

static int	now_freq=DVFS_OPP1;

static int _mtktscpu_set_power_consumption_state(void)
{
	int ret;

    if (1 == cl_dev_F6x1_state)
    {
		if(now_freq != DVFS_OPP6)
        {
        	cpufreq_thermal_protect(DVFS_OPP6);
        	ret = CPU_full_loading();
        	if(ret)
        	return 1;
        	now_freq = DVFS_OPP6;
        }
    }
    else if (1 == cl_dev_F5x1_state)
    {
		if(now_freq != DVFS_OPP5)
        {
			cpufreq_thermal_protect(DVFS_OPP5);
        	ret = CPU_full_loading();
        	if(ret)
        	return 1;
        	now_freq = DVFS_OPP5;
        }
    }
    else if (1 == cl_dev_F4x1_state)
    {
		if(now_freq != DVFS_OPP4)
        {
        	cpufreq_thermal_protect(DVFS_OPP4);
        	ret = CPU_full_loading();
        	if(ret)
        	return 1;
			now_freq = DVFS_OPP4;
        }
    }
    else if (1 == cl_dev_F3x1_state)
    {
		if(now_freq != DVFS_OPP3)
		{
			cpufreq_thermal_protect(DVFS_OPP3);
			ret = CPU_full_loading();
        	if(ret)
        		return 1;
			now_freq = DVFS_OPP3;
        }
    }
    else if (1 == cl_dev_F2x1_state)
    {
        if(now_freq != DVFS_OPP2)
        {
        	cpufreq_thermal_protect(DVFS_OPP2);
        	ret = CPU_full_loading();
        	if(ret)
        		return 1;
			now_freq = DVFS_OPP2;
        }
    }
    else
    {
    	if(now_freq != DVFS_OPP1)
    	{
        	cpufreq_thermal_protect(DVFS_OPP1);
        	ret = CPU_full_loading();
        	if(ret)
        		return 1;
        	now_freq = DVFS_OPP1;
        }
    }
    return 0;

}

static int cpufreq_F1x1_get_max_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = 1;    
	return 0;
}
static int cpufreq_F1x1_get_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = cl_dev_F1x1_state;
	return 0;
}
static int cpufreq_F1x1_set_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long state)
{
    cl_dev_F1x1_state = state;
    _mtktscpu_set_power_consumption_state();
    return 0;
}


static int cpufreq_F2x1_get_max_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = 1;    
	return 0;
}
static int cpufreq_F2x1_get_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = cl_dev_F2x1_state;
	return 0;
}
static int cpufreq_F2x1_set_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long state)
{
    cl_dev_F2x1_state = state;
    _mtktscpu_set_power_consumption_state();
    return 0;
}



static int cpufreq_F3x1_get_max_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = 1;    
	return 0;
}
static int cpufreq_F3x1_get_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = cl_dev_F3x1_state;
	return 0;
}
static int cpufreq_F3x1_set_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long state)
{
    cl_dev_F3x1_state = state;
    _mtktscpu_set_power_consumption_state();

    return 0;
}


static int cpufreq_F4x1_get_max_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = 1;    
	return 0;
}
static int cpufreq_F4x1_get_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = cl_dev_F4x1_state;
	return 0;
}
static int cpufreq_F4x1_set_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long state)
{
    cl_dev_F4x1_state = state;
    _mtktscpu_set_power_consumption_state();
    return 0;
}


static int cpufreq_F5x1_get_max_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = 1;    
	return 0;
}
static int cpufreq_F5x1_get_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = cl_dev_F5x1_state;
	return 0;
}
static int cpufreq_F5x1_set_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long state)
{
	cl_dev_F5x1_state = state;
	_mtktscpu_set_power_consumption_state();
	return 0;
}


static int cpufreq_F6x1_get_max_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = 1;    
	return 0;
}
static int cpufreq_F6x1_get_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{
	*state = cl_dev_F6x1_state;
	return 0;
}
static int cpufreq_F6x1_set_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long state)
{

	cl_dev_F6x1_state = state;
	_mtktscpu_set_power_consumption_state();
	return 0;
}

/*
 * cooling device callback functions (mtktscpu_cooling_sysrst_ops)
 * 1 : ON and 0 : OFF
 */
static int sysrst_get_max_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = 1;
	return 0;
}
static int sysrst_get_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long *state)
{        
	*state = cl_dev_sysrst_state;
	return 0;
}
static int sysrst_set_cur_state(struct thermal_cooling_device *cdev,
		 unsigned long state)
{
	cl_dev_sysrst_state = state;
	if(cl_dev_sysrst_state == 1)
	{	
		printk("Power/CPU_Thermal: reset, reset, reset!!!");
		printk("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
		printk("*****************************************");
		printk("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");

		BUG();
		//arch_reset(0,NULL);   
	}	
	return 0;
}

/* bind fan callbacks to fan device */
static struct thermal_cooling_device_ops mtktscpu_cooling_F1x1_ops = {
	.get_max_state = cpufreq_F1x1_get_max_state,
	.get_cur_state = cpufreq_F1x1_get_cur_state,
	.set_cur_state = cpufreq_F1x1_set_cur_state,
};
static struct thermal_cooling_device_ops mtktscpu_cooling_F2x1_ops = {
	.get_max_state = cpufreq_F2x1_get_max_state,
	.get_cur_state = cpufreq_F2x1_get_cur_state,
	.set_cur_state = cpufreq_F2x1_set_cur_state,
};
static struct thermal_cooling_device_ops mtktscpu_cooling_F3x1_ops = {
	.get_max_state = cpufreq_F3x1_get_max_state,
	.get_cur_state = cpufreq_F3x1_get_cur_state,
	.set_cur_state = cpufreq_F3x1_set_cur_state,
};
static struct thermal_cooling_device_ops mtktscpu_cooling_F4x1_ops = {
	.get_max_state = cpufreq_F4x1_get_max_state,
	.get_cur_state = cpufreq_F4x1_get_cur_state,
	.set_cur_state = cpufreq_F4x1_set_cur_state,
};
static struct thermal_cooling_device_ops mtktscpu_cooling_F5x1_ops = {
	.get_max_state = cpufreq_F5x1_get_max_state,
	.get_cur_state = cpufreq_F5x1_get_cur_state,
	.set_cur_state = cpufreq_F5x1_set_cur_state,
};
static struct thermal_cooling_device_ops mtktscpu_cooling_F6x1_ops = {
	.get_max_state = cpufreq_F6x1_get_max_state,
	.get_cur_state = cpufreq_F6x1_get_cur_state,
	.set_cur_state = cpufreq_F6x1_set_cur_state,
};
static struct thermal_cooling_device_ops mtktscpu_cooling_sysrst_ops = {
	.get_max_state = sysrst_get_max_state,
	.get_cur_state = sysrst_get_cur_state,
	.set_cur_state = sysrst_set_cur_state,
};

static int mtktscpu_read_opp(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
        int len = 0;
        char *p = buf;

        if (1 == cl_dev_F6x1_state)
                p += sprintf(p, "%s", "F6x1\n");
        else if (1 == cl_dev_F5x1_state)
                p += sprintf(p, "%s", "F5x1\n");
        else if (1 == cl_dev_F4x1_state)
                p += sprintf(p, "%s", "F4x1\n");
        else if (1 == cl_dev_F3x1_state)
                p += sprintf(p, "%s", "F3x1\n");
        else if (1 == cl_dev_F2x1_state)
                p += sprintf(p, "%s", "F2x1\n");
        else
                p += sprintf(p, "%s", "F1x1\n");

        *start = buf + off;

        len = p - buf;
        if (len > off)
                len -= off;
        else
                len = 0;
        
        return len < count ? len  : count;
}

static int mtktscpu_read_log(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	char *p = buf;
	
	p += sprintf(p, "[ mtktscpu_read_log] log = %d\n",mtktscpu_debug_log);

	*start = buf + off;

	len = p - buf;
	if (len > off)
		len -= off;
	else
		len = 0;
	
	return len < count ? len  : count;
}

static int mtktscpu_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	char *p = buf;
	
		p += sprintf(p, "[ mtktscpu_read] trip_0_temp=%d,trip_1_temp=%d,trip_2_temp=%d,trip_3_temp=%d,trip_4_temp=%d,\n\
trip_5_temp=%d,trip_6_temp=%d,trip_7_temp=%d,trip_8_temp=%d,trip_9_temp=%d,\n\
g_THERMAL_TRIP_0=%d,g_THERMAL_TRIP_1=%d,g_THERMAL_TRIP_2=%d,g_THERMAL_TRIP_3=%d,g_THERMAL_TRIP_4=%d,\n\
g_THERMAL_TRIP_5=%d,g_THERMAL_TRIP_6=%d,g_THERMAL_TRIP_7=%d,g_THERMAL_TRIP_8=%d,g_THERMAL_TRIP_9=%d,\n\
cooldev0=%s,cooldev1=%s,cooldev2=%s,cooldev3=%s,cooldev4=%s,\n\
cooldev5=%s,cooldev6=%s,cooldev7=%s,cooldev8=%s,cooldev9=%s,time_ms=%d\n",
				trip_temp[0],trip_temp[1],trip_temp[2],trip_temp[3],trip_temp[4],
				trip_temp[5],trip_temp[6],trip_temp[7],trip_temp[8],trip_temp[9],
				g_THERMAL_TRIP[0],g_THERMAL_TRIP[1],g_THERMAL_TRIP[2],g_THERMAL_TRIP[3],g_THERMAL_TRIP[4],
				g_THERMAL_TRIP[5],g_THERMAL_TRIP[6],g_THERMAL_TRIP[7],g_THERMAL_TRIP[8],g_THERMAL_TRIP[9],
				g_bind0,g_bind1,g_bind2,g_bind3,g_bind4,g_bind5,g_bind6,g_bind7,g_bind8,g_bind9,
				interval*1000);


	*start = buf + off;

	len = p - buf;
	if (len > off)
		len -= off;
	else
		len = 0;
	
	return len < count ? len  : count;
}

static ssize_t mtktscpu_write_log(struct file *file, const char *buffer, unsigned long count, void *data)
{
	
	char	desc[32];
	int	log_switch;
	int	len = 0;
	
	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
	{
		return 0;
	}
	desc[len] = '\0';
	
	if (sscanf(desc, "%d", &log_switch) == 1)
	{
		mtktscpu_debug_log = log_switch;
		mtktscpu_dprintk("[mtktscpu_write_log] mtktscpu_debug_log=%d\n", mtktscpu_debug_log);
		return count;
	}
	else
	{
		mtktscpu_dprintk("[mtktscpu_write_log] bad argument\n");
	}
	return -EINVAL;
}


int mtktscpu_register_thermal(void);
void mtktscpu_unregister_thermal(void);

static ssize_t mtktscpu_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int len=0,time_msec=0;
	int trip[10]={0};
	int t_type[10]={0};
	int i;
	char bind0[20],bind1[20],bind2[20],bind3[20],bind4[20];
	char bind5[20],bind6[20],bind7[20],bind8[20],bind9[20];
	char desc[512];


	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
	{
		return 0;
	}
	desc[len] = '\0';

	if (sscanf(desc, "%d %d %d %s %d %d %s %d %d %s %d %d %s %d %d %s %d %d %s %d %d %s %d %d %s %d %d %s %d %d %s %d %d",
				&num_trip, &trip[0],&t_type[0],bind0, &trip[1],&t_type[1],bind1,
						&trip[2],&t_type[2],bind2, &trip[3],&t_type[3],bind3,
						&trip[4],&t_type[4],bind4, &trip[5],&t_type[5],bind5,
						&trip[6],&t_type[6],bind6, &trip[7],&t_type[7],bind7,
						&trip[8],&t_type[8],bind8, &trip[9],&t_type[9],bind9,
						&time_msec, &MA_len_temp) == 33)
	{
		cpufreq_thermal_protect(DVFS_OPP1);
		now_freq = DVFS_OPP1;
		
		mtktscpu_dprintk("[mtktscpu_write] mtktscpu_unregister_thermal\n");
		mtktscpu_unregister_thermal();
	
		for(i=0; i<num_trip; i++)
			g_THERMAL_TRIP[i] = t_type[i];	

		g_bind0[0]=g_bind1[0]=g_bind2[0]=g_bind3[0]=g_bind4[0]=g_bind5[0]=g_bind6[0]=g_bind7[0]=g_bind8[0]=g_bind9[0]='\0';
		
		for(i=0; i<20; i++)
		{
			g_bind0[i]=bind0[i]; 
			g_bind1[i]=bind1[i]; 
			g_bind2[i]=bind2[i]; 
			g_bind3[i]=bind3[i]; 
			g_bind4[i]=bind4[i];
			g_bind5[i]=bind5[i]; 
			g_bind6[i]=bind6[i]; 
			g_bind7[i]=bind7[i]; 
			g_bind8[i]=bind8[i]; 
			g_bind9[i]=bind9[i];
		}

		mtktscpu_dprintk("[mtktscpu_write] g_THERMAL_TRIP_0=%d,g_THERMAL_TRIP_1=%d,g_THERMAL_TRIP_2=%d,g_THERMAL_TRIP_3=%d,g_THERMAL_TRIP_4=%d,\
g_THERMAL_TRIP_5=%d,g_THERMAL_TRIP_6=%d,g_THERMAL_TRIP_7=%d,g_THERMAL_TRIP_8=%d,g_THERMAL_TRIP_9=%d,\n",
							g_THERMAL_TRIP[0],g_THERMAL_TRIP[1],g_THERMAL_TRIP[2],g_THERMAL_TRIP[3],g_THERMAL_TRIP[4],
							g_THERMAL_TRIP[5],g_THERMAL_TRIP[6],g_THERMAL_TRIP[7],g_THERMAL_TRIP[8],g_THERMAL_TRIP[9]);
		mtktscpu_dprintk("[mtktscpu_write] cooldev0=%s,cooldev1=%s,cooldev2=%s,cooldev3=%s,cooldev4=%s,\
cooldev5=%s,cooldev6=%s,cooldev7=%s,cooldev8=%s,cooldev9=%s\n",
							g_bind0,g_bind1,g_bind2,g_bind3,g_bind4,g_bind5,g_bind6,g_bind7,g_bind8,g_bind9);

		for(i=0; i<num_trip; i++)
		{
			trip_temp[i]=trip[i];
		}

		interval=time_msec / 1000;

		mtktscpu_dprintk("[mtktscpu_write] trip_0_temp=%d,trip_1_temp=%d,trip_2_temp=%d,trip_3_temp=%d,trip_4_temp=%d,\
trip_5_temp=%d,trip_6_temp=%d,trip_7_temp=%d,trip_8_temp=%d,trip_9_temp=%d,time_ms=%d\n", 
			trip_temp[0],trip_temp[1],trip_temp[2],trip_temp[3],trip_temp[4],
			trip_temp[5],trip_temp[6],trip_temp[7],trip_temp[8],trip_temp[9],interval*1000);
		

					
		mtktscpu_dprintk("[mtktscpu_write] mtktscpu_register_thermal\n");
		mtktscpu_register_thermal();

		
		proc_write_flag=1;
		return count;
	}
	else
	{
		mtktscpu_dprintk("[mtktscpu_write] bad argument\n");
	}
	
	return -EINVAL;					
}

int mtktscpu_register_DVFS_cooler(void)
{
	cl_dev_F1x1 = mtk_thermal_cooling_device_register("mtktscpu-F1x1", NULL,
			 &mtktscpu_cooling_F1x1_ops);
	cl_dev_F2x1 = mtk_thermal_cooling_device_register("mtktscpu-F2x1", NULL,
			 &mtktscpu_cooling_F2x1_ops);
	cl_dev_F3x1 = mtk_thermal_cooling_device_register("mtktscpu-F3x1", NULL,
			 &mtktscpu_cooling_F3x1_ops);
	cl_dev_F4x1 = mtk_thermal_cooling_device_register("mtktscpu-F4x1", NULL,
			 &mtktscpu_cooling_F4x1_ops);
	cl_dev_F5x1 = mtk_thermal_cooling_device_register("mtktscpu-F5x1", NULL,
			 &mtktscpu_cooling_F5x1_ops);
	cl_dev_F6x1 = mtk_thermal_cooling_device_register("mtktscpu-F6x1", NULL,
			 &mtktscpu_cooling_F6x1_ops);
	cl_dev_sysrst = mtk_thermal_cooling_device_register("mtktscpu-sysrst", NULL,
			 &mtktscpu_cooling_sysrst_ops);
	return 0;
}
int mtktscpu_register_thermal(void)
{
	mtktscpu_dprintk("[mtktscpu_register_thermal] \n");

	/* trips : trip 0~3 */
	thz_dev = mtk_thermal_zone_device_register("mtktscpu", num_trip, NULL,
				&mtktscpu_dev_ops, 0, 0, 0, interval*1000);

	return 0;
}
void mtktscpu_unregister_DVFS_cooler(void)
{
	if(cl_dev_F1x1) {
		mtk_thermal_cooling_device_unregister(cl_dev_F1x1);
		cl_dev_F1x1 = NULL;
	}
	if(cl_dev_F2x1) {
		mtk_thermal_cooling_device_unregister(cl_dev_F2x1);
		cl_dev_F2x1 = NULL;
	}
	if(cl_dev_F3x1) {
		mtk_thermal_cooling_device_unregister(cl_dev_F3x1);
		cl_dev_F3x1 = NULL;
	}
	if(cl_dev_F4x1) {
		mtk_thermal_cooling_device_unregister(cl_dev_F4x1);
		cl_dev_F4x1 = NULL;
	}
	if(cl_dev_F5x1) {
		mtk_thermal_cooling_device_unregister(cl_dev_F5x1);
		cl_dev_F5x1 = NULL;
	}
	if(cl_dev_F6x1) {
		mtk_thermal_cooling_device_unregister(cl_dev_F6x1);
		cl_dev_F6x1 = NULL;
	}
	if(cl_dev_sysrst) {
		mtk_thermal_cooling_device_unregister(cl_dev_sysrst);
		cl_dev_sysrst = NULL;
	}
}
void mtktscpu_unregister_thermal(void)
{
	mtktscpu_dprintk("[mtktscpu_unregister_thermal] \n");
	
	if(thz_dev) {
		mtk_thermal_zone_device_unregister(thz_dev);
		thz_dev = NULL;
	}
}

static int __init mtktscpu_init(void)
{
	int err = 0;
	struct proc_dir_entry *entry = NULL;
	struct proc_dir_entry *mtktscpu_dir = NULL;

	mtktscpu_dprintk("[mtktscpu_init] \n");


	err = mtktscpu_register_DVFS_cooler();
	if(err)
		return err;

	err = mtktscpu_register_thermal();
	if (err)
		goto err_unreg;

	DRV_WriteReg16(TS_CON3, DRV_Reg16(TS_CON3) | 0x0100); // release turbo mode and enable LPF mode

	thermal_cal_prepare();
	thermal_calibration();

	mtktscpu_dir = proc_mkdir("mtktscpu", NULL);
	if (!mtktscpu_dir)
	{
		mtktscpu_dprintk("[mtktscpu_init]: mkdir /proc/mtktscpu failed\n");
	}
	else
	{
		entry = create_proc_entry("mtktscpu", S_IRUGO | S_IWUSR, mtktscpu_dir);
		if (entry)
		{
			entry->read_proc = mtktscpu_read;
			entry->write_proc = mtktscpu_write;
		}
		
		entry = create_proc_entry("mtktscpu_log", S_IRUGO | S_IWUSR, mtktscpu_dir);
		if (entry)
		{
			entry->read_proc = mtktscpu_read_log;
			entry->write_proc = mtktscpu_write_log;
		}

		entry = create_proc_entry("mtktscpu_opp", S_IRUGO, mtktscpu_dir);
		if (entry)
		{
			entry->read_proc = mtktscpu_read_opp;
			entry->write_proc = NULL;
		}
	}

	return 0;

err_unreg:
	mtktscpu_unregister_DVFS_cooler();
	return err;
}

static void __exit mtktscpu_exit(void)
{
	mtktscpu_dprintk("[mtktscpu_exit] \n");
	mtktscpu_unregister_thermal();
	mtktscpu_unregister_DVFS_cooler();
}

module_init(mtktscpu_init);
module_exit(mtktscpu_exit);
