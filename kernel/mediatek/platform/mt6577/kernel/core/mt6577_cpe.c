#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/earlysuspend.h>
#include <linux/xlog.h>

#include "mach/irqs.h"
#include "mach/mt6577_cpe.h"
#include "mach/mt6577_clock_manager.h"

#define CPE_NAME        "mt6577-cpe"

#define cpe_dprintk(fmt, args...)                                   \
do {                                                                \
    if (cpe_debug_mode) {                                           \
        xlog_printk(ANDROID_LOG_INFO, "Power/CPE", fmt, ##args);    \
    }                                                               \
} while(0)

static struct hrtimer cpe_timer;

struct task_struct *cpe_thread = NULL;

static DECLARE_WAIT_QUEUE_HEAD(cpe_timer_waiter);

static int cpe_period = 30;
static int cpe_timer_flag = 0;

static bool cpe_debug_mode = false;
static bool cpe_period_mode = false;

static kal_uint32 g_path1 = 0;
static kal_uint32 g_path2 = 0;
static kal_uint32 g_path3 = 0;

static kal_uint16 cpe_mux_output = 0;

enum hrtimer_restart cpe_timer_func(struct hrtimer *timer)
{
    cpe_dprintk("[%s]: enter timer function\n", __FUNCTION__);
    
    cpe_timer_flag = 1; wake_up_interruptible(&cpe_timer_waiter);
    
    return HRTIMER_NORESTART;
}

kal_uint16 mtk_cpe_one_shot_handler(kal_uint16 path1, kal_uint16 path2, kal_uint16 path3)
{
    kal_uint16 count, try_count, ret;
    
    DRV_WriteReg16(CA9_ROSC_CON0, 0x8);  // reset the overflow bit of CA9 frequency meter
    DRV_WriteReg16(CA9_ROSC_CON0, 0x4);  // reset the circuit of CA9 frequency meter
    
    DRV_WriteReg16(CA9_ROSC_MON_CTRL_0, 0x0);   // switch back to CA9 control
    
    DRV_WriteReg16(CA9_ROSC_CON3, path1);  // CA9 ROSC delay path selection [15:0]
    DRV_WriteReg16(CA9_ROSC_CON4, path2);  // CA9 ROSC delay path selection [31:16]
    DRV_WriteReg16(CA9_ROSC_CON5, path3);  // CA9 ROSC delay path selection [47:32]
    
    ret = DRV_Reg16(CA9_ROSC_CON6); // CA9 ROSC toggle count during window is high
    cpe_dprintk("[before toggling] the CA9 frequency meter value is %d\n", ret);
    
    DRV_WriteReg16(CA9_ROSC_CON0, 0x5);  // enable ROSC to start
    
    //DRV_WriteReg16(CA9_ROSC_CON1, 0xD48); // frequency meter window duration
    //DRV_WriteReg16(CA9_ROSC_CON2, 0x3);  // frequency meter clock divider
    DRV_WriteReg16(CA9_ROSC_CON1, 0x550); // frequency meter window duration
    DRV_WriteReg16(CA9_ROSC_CON2, 0x1);  // frequency meter clock divider
    
    DRV_WriteReg16(CA9_ROSC_CON0, 0x7);  // reload window duration, enable ROSC to start and start to count
    
    try_count = 5;
    ret = DRV_Reg16(CA9_ROSC_CON6); // CA9 ROSC toggle count during window is high
    while (ret == 0 && try_count--)
    {
        ret = DRV_Reg16(CA9_ROSC_CON6); // CA9 ROSC toggle count during window is high
        udelay(10);
    }
    
    try_count = 5;
    while (try_count--)
    {
        while (1)
        {
            ret = DRV_Reg16(CA9_ROSC_CON6); // CA9 ROSC toggle count during window is high
            udelay(10);
            
            if (ret == DRV_Reg16(CA9_ROSC_CON6)) break;
        }
        
        ret = DRV_Reg16(CA9_ROSC_CON_STATUS);
        if ((ret & CA9_FQMTR_BUSY) == 0)
        {
            break;
        }
        else
        {
            udelay(10);
        }
    }
    
    ret = DRV_Reg16(CA9_ROSC_CON_STATUS);
    if ((ret & CA9_FQMTR_OVERFLOW) == CA9_FQMTR_OVERFLOW)
        cpe_dprintk("overflow condition occurred\n");
    
    count = DRV_Reg16(CA9_ROSC_CON6); // CA9 ROSC toggle count during window is high
    cpe_dprintk("[after  toggling] the CA9 frequency meter value is %d\n", count);
    
    DRV_WriteReg16(CA9_ROSC_MON_CTRL_0, 0x0);   // switch back to CA9 control
    
    DRV_WriteReg16(CA9_ROSC_CON0, 0x8);  // disable ROSC and reset the overflow bit of CA9 frequency meter
    DRV_WriteReg16(CA9_ROSC_CON0, 0x4);  // reset the circuit of CA9 frequency meter
    
    cpe_mux_output = count;
    
    return count;
}
EXPORT_SYMBOL(mtk_cpe_one_shot_handler);

int cpe_thread_handler(void *unused)
{
    kal_uint32 ret = 0;
    
    do
    {
        ktime_t ktime = ktime_set(cpe_period, 0);
        
        wait_event_interruptible(cpe_timer_waiter, cpe_timer_flag != 0);
        cpe_timer_flag = 0;
        
        ret = mtk_cpe_one_shot_handler((kal_uint16)g_path1, (kal_uint16)g_path2, (kal_uint16)g_path3);
        xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "got critical path rosc counter = %d\n", ret);
        
        hrtimer_start(&cpe_timer, ktime, HRTIMER_MODE_REL);
    } while (!kthread_should_stop());
    
    return 0;
}

static int cpe_delay_path_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *p = buf;
    
    p += sprintf(p, "g_path1 = %d, g_path2 = %d, g_path3 = %d", g_path1, g_path2, g_path3);
    
    *start = buf + off;
    
    len = p - buf;
    if (len > off)
        len -= off;
    else
        len = 0;
    
    return len < count ? len  : count;
}

static ssize_t cpe_delay_path_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int len = 0;
    char desc[32];
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len))
    {
        return 0;
    }
    desc[len] = '\0';
    
    if (sscanf(desc, "%u %u %u", &g_path1, &g_path2, &g_path3) == 3)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "got g_path1 = %d, g_path2 = %d, g_path3 = %d\n", g_path1, g_path2, g_path3);
    }
    else
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "bad argument!!\n");
    }
    
    return -EINVAL;
}

static int cpe_period_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *p = buf;
    
    p += sprintf(p, "%d", cpe_period);
    
    *start = buf + off;
    
    len = p - buf;
    if (len > off)
        len -= off;
    else
        len = 0;
    
    return len < count ? len  : count;
}

static ssize_t cpe_period_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int len = 0, time = 0;
    char desc[32];
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len))
    {
        return 0;
    }
    desc[len] = '\0';
    
    if (sscanf(desc, "%d", &time) == 1)
    {
        cpe_period = time;
        xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "config critical path emulator period = %d (s)\n", time);
        return count;
    }
    else
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "bad argument!!\n");
    }
    
    return -EINVAL;
}

static int cpe_period_mode_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *p = buf;
    
    if (cpe_period_mode)
        p += sprintf(p, "enable");
    else
        p += sprintf(p, "disable");
    
    *start = buf + off;
    
    len = p - buf;
    if (len > off)
        len -= off;
    else
        len = 0;
    
    return len < count ? len  : count;
}

static ssize_t cpe_period_mode_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
   int len = 0;
    char mode[20], desc[32];
    ktime_t ktime = ktime_set(cpe_period, 0);
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len))
    {
        return 0;
    }
    desc[len] = '\0';
    
    if (sscanf(desc, "%s", mode) == 1)
    {
        if (!strcmp(mode, "enable"))
        {
            cpe_period_mode = true;
            cpe_thread = kthread_run(cpe_thread_handler, 0, "critical path emulator");
            if (IS_ERR(cpe_thread))
            {
                xlog_printk(ANDROID_LOG_ERROR, "Power/CPE", "failed to create critical path emulator thread\n");
            }
            
            hrtimer_start(&cpe_timer, ktime, HRTIMER_MODE_REL);
            
            xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "enable critical path emulator period mode\n");
            return count;
        }
        else if (!strcmp(mode, "disable"))
        {
            cpe_period_mode = false;
            kthread_stop(cpe_thread);
            hrtimer_cancel(&cpe_timer);
            
            xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "disable critical path emulator period mode\n");
            return count;
        }
        else
        {
            xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "bad argument!! argument should be \"enable\" or \"disable\"\n");
        }
    }
    else
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "bad argument!! argument should be \"enable\" or \"disable\"\n");
    }
    
    return -EINVAL;
}

static int cpe_debug_mode_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *p = buf;
    
    if (cpe_debug_mode)
        p += sprintf(p, "enable");
    else
        p += sprintf(p, "disable");
    
    *start = buf + off;
    
    len = p - buf;
    if (len > off)
        len -= off;
    else
        len = 0;
    
    return len < count ? len  : count;
}

static ssize_t cpe_debug_mode_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int len = 0;
    char mode[20], desc[32];
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len))
    {
        return 0;
    }
    desc[len] = '\0';
    
    if (sscanf(desc, "%s", mode) == 1)
    {
        if (!strcmp(mode, "enable"))
        {
            cpe_debug_mode = true;
            xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "enable critical path emulator debug mode\n");
            return count;
        }
        else if (!strcmp(mode, "disable"))
        {
            cpe_debug_mode = false;
            xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "disable critical path emulator debug mode\n");
            return count;
        }
        else
        {
            xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "bad argument!! argument should be \"enable\" or \"disable\"\n");
        }
    }
    else
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "bad argument!! argument should be \"enable\" or \"disable\"\n");
    }
    
    return -EINVAL;
}

static ssize_t cpe_one_shot_mode_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int len = 0;
    char mode[20], desc[32];
    kal_uint32 ret = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) 
    {
        return 0;
    }
    desc[len] = '\0';
        
    printk("\n=============== Critical Path Emulator One Shot Mode ================\n");
    if (sscanf(desc, "%s", mode) == 1)
    {
        if (!strcmp(mode, "enable"))
        {
            printk("  enable critical path emulator one shot mode\n");
            
            ret = mtk_cpe_one_shot_handler(g_path1, g_path2, g_path3);
            printk("  got critical path rosc counter = %d\n", ret);
            printk("==========================================\n");
            return count;
        }
        else
        {
            printk("  bad argument!! argument should be \"enable\" or \"disable\"\n");
            printk(  "==========================================\n");
        }
    }
    else
    {
        printk("  bad argument!!\n");
        printk(  "==========================================\n");
    }
    
    return -EINVAL;
}

static int cpe_mux_output_read(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
    int len = 0;
    char *p = buf;
    
    p += sprintf(p, "%d", cpe_mux_output);

    *start = buf + off;
    
    len = p - buf;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;
}

static int __init mtk_cpe_init(void)
{
    struct proc_dir_entry *entry = NULL;
    struct proc_dir_entry *cpe_dir = NULL;
    
    xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "critical path emulator initialized\n");
    
    hrtimer_init(&cpe_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    cpe_timer.function = cpe_timer_func;
    
    cpe_dir = proc_mkdir("cpe", NULL);
    if (!cpe_dir)
    {
        xlog_printk(ANDROID_LOG_ERROR, "Power/CPE", "mkdir /proc/cpe failed\n");
    }
    else
    {
        entry = create_proc_entry("cpe_debug_mode", S_IRUGO | S_IWUSR, cpe_dir);
        if (entry)
        {
            entry->read_proc = cpe_debug_mode_read;
            entry->write_proc = cpe_debug_mode_write;
        }
        
        entry = create_proc_entry("cpe_period_mode", S_IRUGO | S_IWUSR, cpe_dir);
        if (entry)
        {
            entry->read_proc = cpe_period_mode_read;
            entry->write_proc = cpe_period_mode_write;
        }
        
        entry = create_proc_entry("cpe_period", S_IRUGO | S_IWUSR, cpe_dir);
        if (entry)
        {
            entry->read_proc = cpe_period_read;
            entry->write_proc = cpe_period_write;
        }
        
        entry = create_proc_entry("cpe_delay_path", S_IRUGO | S_IWUSR, cpe_dir);
        if (entry)
        {
            entry->read_proc = cpe_delay_path_read;
            entry->write_proc = cpe_delay_path_write;
        }
        
        entry = create_proc_entry("cpe_mux_output", S_IRUGO | S_IWUSR, cpe_dir);
        if (entry) 
        {
            entry->read_proc = cpe_mux_output_read;
        }
        
        entry = create_proc_entry("cpe_one_shot_mode", S_IRUGO | S_IWUSR, cpe_dir);
        if (entry) 
        {
            entry->write_proc = cpe_one_shot_mode_write;
        }
    }
    
    return 0;
}

static void __exit mtk_cpe_exit(void)
{
    xlog_printk(ANDROID_LOG_INFO, "Power/CPE", "critical path emulator de-initialized\n");
}

module_init(mtk_cpe_init);
module_exit(mtk_cpe_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MediaTek Critical Path Emulator Driver");
MODULE_AUTHOR("Chun-Wei Chen <chun-wei.chen@mediatek.com>");