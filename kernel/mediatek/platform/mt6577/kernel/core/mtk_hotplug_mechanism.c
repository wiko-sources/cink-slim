/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*********************************
* include
**********************************/
#include <linux/kernel.h> 
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cpu.h>
#include <linux/earlysuspend.h>
#include <linux/proc_fs.h>
#include <mach/hotplug.h>
#include <asm/uaccess.h>

#include "mach/mtk_cpu_management.h"

extern void disable_hotplug_policy(bool disable, int limit_cpu_num);

/*********************************
* macro
**********************************/
#ifdef CONFIG_HAS_EARLYSUSPEND

#define STATE_INIT                  0
#define STATE_ENTER_EARLY_SUSPEND   1
#define STATE_ENTER_LATE_RESUME     2

#endif //#ifdef CONFIG_HAS_EARLYSUSPEND


#ifdef CONFIG_HOTPLUG_PROFILING

#define CPU_UP(X)                                                                                               \
do {                                                                                                            \
    g_tmp_tick = sched_clock();                                                                                 \
    cpu_up(X);                                                                                                  \
    g_tmp_tick = sched_clock() - g_tmp_tick;                                                                    \
    if (g_tmp_tick > g_up_time_max)                                                                             \
       g_up_time_max = g_tmp_tick;                                                                              \
    if (g_tmp_tick < g_up_time_min)                                                                             \
       g_up_time_min = g_tmp_tick;                                                                              \
    g_up_time_array[(g_up_time_index++) % CONFIG_HOTPLUG_PROFILING_COUNT] = g_tmp_tick;                         \
} while (0)

#define CPU_DOWN(X)                                                                                             \
do {                                                                                                            \
    g_tmp_tick = sched_clock();                                                                                 \
    cpu_down(X);                                                                                                \
    g_tmp_tick = sched_clock() - g_tmp_tick;                                                                    \
    if (g_tmp_tick > g_down_time_max)                                                                           \
       g_down_time_max = g_tmp_tick;                                                                            \
    if (g_tmp_tick < g_down_time_min)                                                                           \
       g_down_time_min = g_tmp_tick;                                                                            \
    g_down_time_array[(g_down_time_index++) % CONFIG_HOTPLUG_PROFILING_COUNT] = g_tmp_tick;                     \
} while (0)     

#else //#ifdef CONFIG_HOTPLUG_PROFILING

#define CPU_UP(X)       cpu_up(X)
#define CPU_DOWN(X)     cpu_down(X)

#endif //#ifdef CONFIG_HOTPLUG_PROFILING



/*********************************
* glabal variable
**********************************/
static int g_max_cpus;
static int g_limited_cpus;
static int g_enable = 1;
static DEFINE_MUTEX(g_mtk_hotplug_mechanism_lock);

#ifdef CONFIG_HAS_EARLYSUSPEND
static struct early_suspend mtk_hotplug_mechanism_early_suspend_handler =
{
    .level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 250,
    .suspend = NULL,
    .resume  = NULL,
};
static int g_cur_state = STATE_ENTER_LATE_RESUME;
#endif //#ifdef CONFIG_HAS_EARLYSUSPEND

#ifdef CONFIG_HOTPLUG_PROFILING
static unsigned long long g_tmp_tick;
static unsigned int g_up_time_index = 0;
static unsigned int g_down_time_index = 0;
static unsigned long long g_up_time_max = 0ULL;
static unsigned long long g_down_time_max = 0ULL;
static unsigned long long g_up_time_min = ULLONG_MAX;
static unsigned long long g_down_time_min = ULLONG_MAX;
static unsigned long long g_up_time_array[CONFIG_HOTPLUG_PROFILING_COUNT] = {0};
static unsigned long long g_down_time_array[CONFIG_HOTPLUG_PROFILING_COUNT] = {0};
#endif //#ifdef CONFIG_HOTPLUG_PROFILING



/*********************************
* early suspend callback function
**********************************/
#ifdef CONFIG_HAS_EARLYSUSPEND
static void mtk_hotplug_mechanism_early_suspend(struct early_suspend *h)
{    
    int mask[10] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1};
    
    HOTPLUG_INFO("mtk_hotplug_mechanism_early_suspend");
    
    if (g_enable)
        cpu_opp_mask("SCREEN_OFF", mask, true);
    g_cur_state = STATE_ENTER_EARLY_SUSPEND;
    
    return;
}
#endif //#ifdef CONFIG_HAS_EARLYSUSPEND



/*******************************
* late resume callback function
********************************/
#ifdef CONFIG_HAS_EARLYSUSPEND
static void mtk_hotplug_mechanism_late_resume(struct early_suspend *h)
{    
    HOTPLUG_INFO("mtk_hotplug_mechanism_late_resume");
    
    if (g_enable)
        cpu_opp_mask("SCREEN_OFF", NULL, false);
    g_cur_state = STATE_ENTER_LATE_RESUME;
    
    return;
}
#endif //#ifdef CONFIG_HAS_EARLYSUSPEND



/**************************************************************
* mtk hotplug mechanism control interface for procfs enable
***************************************************************/
static int mtk_hotplug_mechanism_read_enable(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = buf;
    
    p += sprintf(p, "%d\n", g_enable);
    *eof = 1;
    
    return p - buf;
}

static int mtk_hotplug_mechanism_write_enable(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int len = 0, enable = 0;
    char desc[32];
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len))
    {
        return 0;
    }
    desc[len] = '\0';
    
    if (sscanf(desc, "%d", &enable) == 1)
    {
        g_enable = enable;
        return count;
    }
    else
    {
        HOTPLUG_INFO("mtk_hotplug_mechanism_write_enable, bad argument\n");
    }
    
    return -EINVAL;
}



/**************************************************************
* mtk hotplug mechanism control interface for procfs thermal
***************************************************************/
static int mtk_hotplug_mechanism_read_thermal_protect(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = buf;
    
    p += sprintf(p, "%d\n", g_limited_cpus);
    *eof = 1;
    
    return p - buf;
}

static int mtk_hotplug_mechanism_write_thermal_protect(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int len = 0, limited_cpus = 0;
    char desc[32];
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len))
    {
        return 0;
    }
    desc[len] = '\0';
    
    if (sscanf(desc, "%d", &limited_cpus) == 1)
    {
        mtk_hotplug_mechanism_thermal_protect(limited_cpus);
        return count;
    }
    else
    {
        HOTPLUG_INFO("mtk_hotplug_mechanism_write_thermal_protect, bad argument\n");
    }
    
    return -EINVAL;
}



/**************************************************************
* mtk hotplug mechanism control interface for procfs profiling
***************************************************************/
#ifdef CONFIG_HOTPLUG_PROFILING
static int mtk_hotplug_mechanism_read_up_time_average(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = buf;
    unsigned long long g_up_time_average = 0ULL;
    unsigned long long g_up_time_total = 0ULL;
    int i;
    
    if (g_up_time_index)
    {
        if (g_up_time_index < CONFIG_HOTPLUG_PROFILING_COUNT)
        {
            for (i = 0; i < g_up_time_index; ++i)
                g_up_time_total += g_up_time_array[i];
            //g_up_time_average = g_up_time_total / g_up_time_index;    
            while (g_up_time_total >= g_up_time_index)
            {
                g_up_time_total -= g_up_time_index;
                g_up_time_average += 1ULL;
            }
        }
        else
        {
            for (i = 0; i < CONFIG_HOTPLUG_PROFILING_COUNT; ++i)
                g_up_time_total += g_up_time_array[i];
            //g_up_time_average = g_up_time_total / CONFIG_HOTPLUG_PROFILING_COUNT;
            while (g_up_time_total >= CONFIG_HOTPLUG_PROFILING_COUNT)
            {
                g_up_time_total -= CONFIG_HOTPLUG_PROFILING_COUNT;
                g_up_time_average += 1ULL;
            }
        }
    }
    p += sprintf(p, "%llu\n", g_up_time_average);
    *eof = 1;
    
    return p - buf;
}

static int mtk_hotplug_mechanism_read_up_time_max(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = buf;

    if (g_up_time_index)
    {
        p += sprintf(p, "%llu\n", g_up_time_max);
    }
    else
    {
        p += sprintf(p, "%llu\n", 0ULL);
    }
    *eof = 1;
    
    return p - buf;
}

static int mtk_hotplug_mechanism_read_up_time_min(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = buf;

    if (g_up_time_index)
    {
        p += sprintf(p, "%llu\n", g_up_time_min);
    }
    else
    {
        p += sprintf(p, "%llu\n", 0ULL);
    }
    *eof = 1;
    
    return p - buf;
}

static int mtk_hotplug_mechanism_read_down_time_average(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = buf;
    unsigned long long g_down_time_average = 0ULL;
    unsigned long long g_down_time_total = 0ULL;
    int i;
    
    if (g_down_time_index)
    {
        if (g_down_time_index < CONFIG_HOTPLUG_PROFILING_COUNT)
        {
            for (i = 0; i < g_down_time_index; ++i)
                g_down_time_total += g_down_time_array[i];
            //g_down_time_average = g_down_time_total / g_down_time_index;
            while (g_down_time_total >= g_down_time_index)
            {
                g_down_time_total -= g_down_time_index;
                g_down_time_average += 1ULL;
            }
        }
        else
        {
            for (i = 0; i < CONFIG_HOTPLUG_PROFILING_COUNT; ++i)
                g_down_time_total += g_down_time_array[i];
            //g_down_time_average = g_down_time_total / CONFIG_HOTPLUG_PROFILING_COUNT;
            while (g_down_time_total >= CONFIG_HOTPLUG_PROFILING_COUNT)
            {
                g_down_time_total -= CONFIG_HOTPLUG_PROFILING_COUNT;
                g_down_time_average += 1ULL;
            }
        }
    }
    p += sprintf(p, "%llu\n", g_down_time_average);
    *eof = 1;
    
    return p - buf;
}

static int mtk_hotplug_mechanism_read_down_time_max(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = buf;

    if (g_down_time_index)
    {
        p += sprintf(p, "%llu\n", g_down_time_max);
    }
    else
    {
        p += sprintf(p, "%llu\n", 0ULL);
    }
    *eof = 1;
    
    return p - buf;
}

static int mtk_hotplug_mechanism_read_down_time_min(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = buf;

    if (g_down_time_index)
    {
        p += sprintf(p, "%llu\n", g_down_time_min);
    }
    else
    {
        p += sprintf(p, "%llu\n", 0ULL);
    }
    *eof = 1;
    
    return p - buf;
}
#endif //#ifdef CONFIG_HOTPLUG_PROFILING



/*******************************
* kernel module init function
********************************/
static int __init mtk_hotplug_mechanism_init(void)
{
    struct proc_dir_entry *entry = NULL;
    struct proc_dir_entry *mtk_hotplug_dir = NULL;
    
    HOTPLUG_INFO("mtk_hotplug_mechanism_init");
    
    mtk_hotplug_dir = proc_mkdir("mtk_hotplug", NULL);
    if (!mtk_hotplug_dir)
    {
        HOTPLUG_INFO("mkdir /proc/mtk_hotplug failed");
    }
    else
    {
        entry = create_proc_entry("enable", S_IRUGO | S_IWUSR, mtk_hotplug_dir);
        if (entry)
        {
            entry->read_proc = mtk_hotplug_mechanism_read_enable;
            entry->write_proc = mtk_hotplug_mechanism_write_enable;
        }
        entry = create_proc_entry("thermal_protect", S_IRUGO | S_IWUSR, mtk_hotplug_dir);
        if (entry)
        {
            entry->read_proc = mtk_hotplug_mechanism_read_thermal_protect;
            entry->write_proc = mtk_hotplug_mechanism_write_thermal_protect;
        }
#ifdef CONFIG_HOTPLUG_PROFILING
        entry = create_proc_entry("up_time_average", S_IRUGO | S_IWUSR, mtk_hotplug_dir);
        if (entry)
        {
            entry->read_proc = mtk_hotplug_mechanism_read_up_time_average;
            //entry->write_proc = mtk_hotplug_mechanism_write_up_time_average;
        }
        entry = create_proc_entry("up_time_max", S_IRUGO | S_IWUSR, mtk_hotplug_dir);
        if (entry)
        {
            entry->read_proc = mtk_hotplug_mechanism_read_up_time_max;
            //entry->write_proc = mtk_hotplug_mechanism_write_up_time_max;
        }
        entry = create_proc_entry("up_time_min", S_IRUGO | S_IWUSR, mtk_hotplug_dir);
        if (entry)
        {
            entry->read_proc = mtk_hotplug_mechanism_read_up_time_min;
            //entry->write_proc = mtk_hotplug_mechanism_write_up_time_min;
        }
        entry = create_proc_entry("down_time_average", S_IRUGO | S_IWUSR, mtk_hotplug_dir);
        if (entry)
        {
            entry->read_proc = mtk_hotplug_mechanism_read_down_time_average;
            //entry->write_proc = mtk_hotplug_mechanism_write_down_time_average;
        }
        entry = create_proc_entry("down_time_max", S_IRUGO | S_IWUSR, mtk_hotplug_dir);
        if (entry)
        {
            entry->read_proc = mtk_hotplug_mechanism_read_down_time_max;
            //entry->write_proc = mtk_hotplug_mechanism_write_down_time_max;
        }
        entry = create_proc_entry("down_time_min", S_IRUGO | S_IWUSR, mtk_hotplug_dir);
        if (entry)
        {
            entry->read_proc = mtk_hotplug_mechanism_read_down_time_min;
            //entry->write_proc = mtk_hotplug_mechanism_write_down_time_min;
        }
#endif //#ifdef CONFIG_HOTPLUG_PROFILING
    }
    
    g_limited_cpus = g_max_cpus = num_possible_cpus();
#ifdef CONFIG_HAS_EARLYSUSPEND
    mtk_hotplug_mechanism_early_suspend_handler.suspend = mtk_hotplug_mechanism_early_suspend;
    mtk_hotplug_mechanism_early_suspend_handler.resume = mtk_hotplug_mechanism_late_resume;
    register_early_suspend(&mtk_hotplug_mechanism_early_suspend_handler);
#endif //#ifdef CONFIG_HAS_EARLYSUSPEND    
    return 0;
}
module_init(mtk_hotplug_mechanism_init);



/*******************************
* kernel module exit function
********************************/
static void __exit mtk_hotplug_mechanism_exit(void)
{
    HOTPLUG_INFO("mtk_hotplug_mechanism_exit");
}
module_exit(mtk_hotplug_mechanism_exit);



/**************************************************************
* mtk hotplug mechanism control interface for thermal protect
***************************************************************/
void mtk_hotplug_mechanism_thermal_protect(int limited_cpus)
{
    int i;
    
    HOTPLUG_INFO("mtk_hotplug_mechanism_thermal_protect: %d\n", limited_cpus);
    
    if ((limited_cpus < 1 ) || (limited_cpus > g_max_cpus))
        return;
    
    mutex_lock(&g_mtk_hotplug_mechanism_lock);
    
    g_limited_cpus = limited_cpus;
    
    if (g_limited_cpus == 1)
    {
        disable_hotplug_policy(true, g_limited_cpus);
    }
    else
    {
        disable_hotplug_policy(false, g_limited_cpus);
    }
    
    for (i = g_limited_cpus; i < g_max_cpus; ++i)
        cpu_down(i);
    
    mutex_unlock(&g_mtk_hotplug_mechanism_lock);
    
}
EXPORT_SYMBOL(mtk_hotplug_mechanism_thermal_protect);



MODULE_DESCRIPTION("MediaTek CPU Hotplug Mechanism");
MODULE_LICENSE("GPL");
