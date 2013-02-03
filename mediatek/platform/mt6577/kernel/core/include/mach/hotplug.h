#ifndef _HOTPLUG
#define _HOTPLUG

#include <linux/xlog.h>


/* log */
#define HOTPLUG_LOG_NONE                                0
#define HOTPLUG_LOG_WITH_XLOG                           1
#define HOTPLUG_LOG_WITH_PRINTK                         2

#define HOTPLUG_LOG_PRINT                               HOTPLUG_LOG_WITH_PRINTK

#if (HOTPLUG_LOG_PRINT == HOTPLUG_LOG_NONE)
#define HOTPLUG_INFO(fmt, args...)                    
#elif (HOTPLUG_LOG_PRINT == HOTPLUG_LOG_WITH_XLOG)
#define HOTPLUG_INFO(fmt, args...)                      xlog_printk(ANDROID_LOG_INFO, "Power/hotplug", fmt, ##args)
#elif (HOTPLUG_LOG_PRINT == HOTPLUG_LOG_WITH_PRINTK)
#define HOTPLUG_INFO(fmt, args...)                      printk("[Power/hotplug] "fmt, ##args)
#endif


/* profilling */
//#define CONFIG_HOTPLUG_PROFILING                        
#define CONFIG_HOTPLUG_PROFILING_COUNT                  100


/* register address */
#define RST_CTL0                    (0xf0009010)
#define PWR_CTL1                    (0xf0009024)
#define PWR_MON                     (0xf00090a8)
#define BOOTROM_BOOT_ADDR           (0xf0002030)
#define SCU_CPU_PWR_STATUS          (0xf000a008)


/* register read/write */
#define REG_READ(addr)           (*(volatile u32 *)(addr))
#define REG_WRITE(addr, value)   (*(volatile u32 *)(addr) = (u32)(value))


/* power on/off cpu1 */
extern void power_on_cpu1(void);
extern void power_off_cpu1(void);


/* mtk hotplug mechanism control interface for thermal protect */
extern void mtk_hotplug_mechanism_thermal_protect(int limited_cpus);


#endif //enf of #ifndef _HOTPLUG