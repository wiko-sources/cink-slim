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
//#include <mach/hotplug.h>
#include "mach/mtk_cpu_management.h"

extern void cpufreq_thermal_protect(int limited_freq);
//extern void mtk_hotplug_mechanism_thermal_protect(int limited_cpus);
static int mtk_cpu_management_debug_log = 1;

#define mtk_cpu_management_dprintk(fmt, args...)   \
do {                                    \
    if (mtk_cpu_management_debug_log) {                \
        xlog_printk(ANDROID_LOG_INFO, "Power/cpu_management", fmt, ##args); \
    }                                   \
} while(0)


static struct file *openFile(char *path,int flag,int mode) 
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
static int file_read(struct file* file, loff_t offset, unsigned char* data, unsigned int size) 
{
    int ret;
		mm_segment_t oldfs; 

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

		set_fs(oldfs);
    return ret;
}
static int file_write(struct file* file, loff_t offset, unsigned char* data, unsigned int size) 
{
    int ret;
    mm_segment_t oldfs; 

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}
static int closeFile(struct file *fp) 
{ 
		filp_close(fp,NULL); 
		return 0; 
}

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
				mtk_cpu_management_dprintk("[mtktscpu_full_loading]:open file ");
				memset(buf1,0,sizeof(buf1));
				if ((ret = file_read(fp, 0, buf1, sizeof(buf1)))>0) 
						mtk_cpu_management_dprintk("[CPU_full_loading]:buf1:%s\n",buf1); 
				else 
						mtk_cpu_management_dprintk("read file error %d\n",ret);
				
				if( !strncmp(buf1, ondemand, sizeof(ondemand)) )
				{	
						mtk_cpu_management_dprintk("[CPU_full_loading]:ondemand->performance->ondemand\n"); 
						file_write(fp, 0, performance, sizeof(performance));
						file_write(fp, 0, ondemand, sizeof(ondemand));
				}
				else if( !strncmp(buf1, performance, sizeof(performance)) )
				{
						mtk_cpu_management_dprintk("[CPU_full_loading]:performance->ondemand->performance\n"); 
						file_write(fp, 0, ondemand, sizeof(ondemand));
						file_write(fp, 0, performance, sizeof(performance));
				}
				else if( !strncmp(buf1, powersave, sizeof(powersave)) )
				{
						mtk_cpu_management_dprintk("[CPU_full_loading]:powersave->ondemand->powersave\n"); 
						file_write(fp, 0, ondemand, sizeof(ondemand));
						file_write(fp, 0, powersave, sizeof(powersave));
				}
				else if( !strncmp(buf1, userspace, sizeof(userspace)) )
				{
						mtk_cpu_management_dprintk("[CPU_full_loading]:userspace->ondemand->userspace\n"); 
						file_write(fp, 0, ondemand, sizeof(ondemand));
						file_write(fp, 0, userspace, sizeof(userspace));
				}
 
				closeFile(fp); 
				return 0;
		}
		else{
				mtk_cpu_management_dprintk("[CPU_full_loading]open fail\n");
				return 1;
		}	
}

static DEFINE_MUTEX(management_lock);
static int step_counter[10]={0};
static int event_counter[EVENT_COUNT]={0};		

int cpu_opp_limit(mtk_cpu_management_event event, int limited_freq, int limited_cpu, bool flag)
{
		int step=0, ret=0, i, lowest_step=0;

		return 1;

		if(event > EVENT_COUNT)
				return -1;

		mtk_cpu_management_dprintk("cpu_opp_limit: event=%d, limited_freq=%d, limited_cpu=%d, flag=%d",event, limited_freq, limited_cpu, flag);

		if(limited_freq==DVFS_OPP1 &&  limited_cpu==2)
		{
				step = 0;
		}
		else if(limited_freq==DVFS_OPP2 &&  limited_cpu==2)
		{
				step = 1;
		}
		else if(limited_freq==DVFS_OPP3 &&  limited_cpu==2)
		{
				step = 2;
		}
		else if(limited_freq==DVFS_OPP4 &&  limited_cpu==2)
		{
				step = 3;
		}
		else if(limited_freq==DVFS_OPP1 &&  limited_cpu==1)
		{
				step = 4;
		}
		else if(limited_freq==DVFS_OPP2 &&  limited_cpu==1)
		{
				step = 5;
		}
		else if(limited_freq==DVFS_OPP3 &&  limited_cpu==1)
		{
				step = 6;
		}
		else if(limited_freq==DVFS_OPP4 &&  limited_cpu==1)
		{
				step = 7;
		}
		else if(limited_freq==DVFS_OPP5 &&  limited_cpu==1)
		{
				step = 8;
		}
		else if(limited_freq==DVFS_OPP6 &&  limited_cpu==1)
		{
				step = 9;
		}
		else
		{
				//parameter error	
				return -1;
		}

		if(flag == true)
		{
				event_counter[event]++;
				step_counter[step]++;
				mtk_cpu_management_dprintk("%d, %d",event_counter[event], step_counter[step]);
		}
		else
		{
				if(step_counter[step]==0)
						return -1;
				event_counter[event]--;
				step_counter[step]--;
		}


		mtk_cpu_management_dprintk("Step0=%d,Step1=%d,Step2=%d,Step3=%d,Step4=%d,Step5=%d,Step6=%d,Step7=%d,Step8=%d,Step9=%d\n", 
		step_counter[0],step_counter[1],step_counter[2],step_counter[3],step_counter[4],step_counter[5],step_counter[6],step_counter[7],step_counter[8],step_counter[9]);

		for(i=9; i>-1; i--)
		{
				if(step_counter[i] != 0)
				{	
						lowest_step = i;
						mtk_cpu_management_dprintk("lowest step=%d", lowest_step);
						break;
				}		
		}

		if(lowest_step == 0)
		{
				cpufreq_thermal_protect(DVFS_OPP1);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(2);
		}
		else if(lowest_step == 1)	
		{
				cpufreq_thermal_protect(DVFS_OPP2);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(2);				
		}
		else if(lowest_step == 2)	
		{
				cpufreq_thermal_protect(DVFS_OPP3);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(2);
		}
		else if(lowest_step == 3)	
		{
				cpufreq_thermal_protect(DVFS_OPP4);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(2);
		}
		else if(lowest_step == 4)	
		{
				cpufreq_thermal_protect(DVFS_OPP1);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(1);
		}
		else if(lowest_step == 5)	
		{
				cpufreq_thermal_protect(DVFS_OPP2);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(1);
		}
		else if(lowest_step == 6)	
		{
				cpufreq_thermal_protect(DVFS_OPP3);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(1);
		}
		else if(lowest_step == 7)	
		{
				cpufreq_thermal_protect(DVFS_OPP4);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(1);
		}
		else if(lowest_step == 8)	
		{
				cpufreq_thermal_protect(DVFS_OPP5);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(1);
		}
		else if(lowest_step == 9)
		{
				cpufreq_thermal_protect(DVFS_OPP6);
				ret = CPU_full_loading();
//				mtk_hotplug_mechanism_thermal_protect(1);
		}
		return 0;
}
EXPORT_SYMBOL(cpu_opp_limit);


struct module_info info_ori;
int cpu_opp_mask(char *name, int *mask, bool flag)
{
	struct list_head *p = NULL;
	struct module_info *info = NULL;
	bool new_module = true;
	int Final_Mask[10]={1,1,1,1,1,1,1,1,1,1};
	int i, lowest_step=0;

	return 1;

	mtk_cpu_management_dprintk("cpu_opp_mask: name=%s, flag=%d\n",name, flag);

	mutex_lock(&management_lock);
	list_for_each(p, &info_ori.link)
	{
		info = list_entry(p, struct module_info, link);
		if (!strcmp(info->module_name, name))
		{
			mtk_cpu_management_dprintk("cpu_opp_mask: find old module name=%s\n",name);
			new_module = false;
			break;
		}
	}

	if((new_module==true) && (flag==true))
	{
		info = NULL;
		info = kmalloc(sizeof(struct module_info), GFP_KERNEL);
		if(!info)
		{
			mtk_cpu_management_dprintk("cpu_opp_mask:%s malloc fail\n",name);
			mutex_unlock(&management_lock);
			return 1;
		}
		else
		{
			sprintf(info->module_name, "%s", name);
			list_add_tail(&info->link, &info_ori.link);
			for(i=0; i<10; i++)
				info->mask[i] = mask[i];
			mtk_cpu_management_dprintk("cpu_opp_mask: Add new module--%s\n",info->module_name);
			mtk_cpu_management_dprintk("cpu_opp_mask: new module mask----%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
										info->mask[0],info->mask[1],info->mask[2],info->mask[3],
										info->mask[4],info->mask[5],info->mask[6],info->mask[7],
										info->mask[8],info->mask[9]);
		}
	}
	else if((new_module==false) && (flag==false))
	{
		mtk_cpu_management_dprintk("cpu_opp_mask: Remove old module--%s\n",info->module_name);
		list_del(&info->link);
		kfree(info);
	}
	else if((new_module==true) && (flag==false))
	{
		mtk_cpu_management_dprintk("cpu_opp_mask: This module name not exist and need be created first\n");
		mutex_unlock(&management_lock);
		return 1;
	}
	else if((new_module==false) && (flag==true))
	{
		mtk_cpu_management_dprintk("cpu_opp_mask: Change module mask\n");
		for(i=0; i<10; i++)
				info->mask[i] = mask[i];
	}
	
	list_for_each(p, &info_ori.link)
	{
		info = NULL;
		info = list_entry(p, struct module_info, link);
		mtk_cpu_management_dprintk("cpu_opp_mask: exist module--%s\n",info->module_name);
		mtk_cpu_management_dprintk("cpu_opp_mask: exist module mask--%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
										info->mask[0],info->mask[1],info->mask[2],info->mask[3],
										info->mask[4],info->mask[5],info->mask[6],info->mask[7],
										info->mask[8],info->mask[9]);
		for(i=0; i<10; i++)
			Final_Mask[i] &= info->mask[i];
	}
	mtk_cpu_management_dprintk("cpu_opp_mask: Final mask---------%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
										Final_Mask[0],Final_Mask[1],Final_Mask[2],Final_Mask[3],
										Final_Mask[4],Final_Mask[5],Final_Mask[6],Final_Mask[7],
										Final_Mask[8],Final_Mask[9]);

	for(i=0; i<10; i++)
	{
		if(Final_Mask[i] != 0)
		{
			lowest_step = i;
			mtk_cpu_management_dprintk("cpu_opp_mask: lowest step=%d\n", lowest_step);
			break;
		}
	}

	switch(lowest_step)
	{
		case 0:
		{
			cpufreq_thermal_protect(DVFS_OPP1);
//			mtk_hotplug_mechanism_thermal_protect(2);
			break;
		}	
		case 1:	
		{
			cpufreq_thermal_protect(DVFS_OPP2);
//			mtk_hotplug_mechanism_thermal_protect(2);		
			break;
		}
		case 2:
		{
			cpufreq_thermal_protect(DVFS_OPP3);
//			mtk_hotplug_mechanism_thermal_protect(2);
			break;
		}	
		case 3:
		{
			cpufreq_thermal_protect(DVFS_OPP4);
//			mtk_hotplug_mechanism_thermal_protect(2);
			break;
		}	
		case 4:
		{
			cpufreq_thermal_protect(DVFS_OPP1);
//			mtk_hotplug_mechanism_thermal_protect(1);
			break;
		}	
		case 5:	
		{
			cpufreq_thermal_protect(DVFS_OPP2);
//			mtk_hotplug_mechanism_thermal_protect(1);
			break;
		}	
		case 6:
		{
			cpufreq_thermal_protect(DVFS_OPP3);
//			mtk_hotplug_mechanism_thermal_protect(1);
			break;
		}	
		case 7:	
		{
			cpufreq_thermal_protect(DVFS_OPP4);
//			mtk_hotplug_mechanism_thermal_protect(1);
			break;
		}
		case 8:
		{
			cpufreq_thermal_protect(DVFS_OPP5);
//			mtk_hotplug_mechanism_thermal_protect(1);
			break;
		}
		case 9:	
		{
			cpufreq_thermal_protect(DVFS_OPP6);
//			mtk_hotplug_mechanism_thermal_protect(1);
			break;
		}
		default:
			break;
	}
	mutex_unlock(&management_lock);
	return 0;
}
EXPORT_SYMBOL(cpu_opp_mask);





static int __init mtk_cpu_management_init(void)
{
	INIT_LIST_HEAD(&info_ori.link);	
	return 0;
}

static void __exit mtk_cpu_management_exit(void)
{
		
}

module_init(mtk_cpu_management_init);
module_exit(mtk_cpu_management_exit);
