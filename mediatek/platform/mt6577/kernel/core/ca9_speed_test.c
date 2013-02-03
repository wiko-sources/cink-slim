#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/wakelock.h>
#include <linux/module.h>
#include <asm/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gfp.h>
#include <asm/io.h>
#include <asm/memory.h>
#include <asm/outercache.h>
#include <linux/spinlock.h>

#include <linux/leds-mt65xx.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include "mach/ca9_slt.h"

/*
README:
    Modify 'CONFIG_MAX_DRAM_SIZE_SUPPORT' in 'mediatek/config/mt6575/autoconfig/kconfig/platform' to reserve a region
    mt6575_fixup() in mediatek/platform/mt6575/kernel/core/mt6575_devs.c reference this CONFIG_MAX_DRAM_SIZE_SUPPORT
*/

extern int max_power_loop(void);
extern int speed_indicative_loop(void);

static int g_iCPU0_FinalResult, g_iCPU1_FinalResult;
static int g_iCPU0_PassFail, g_iCPU1_PassFail;

int g_iADDR1_CPU0, g_iADDR2_CPU0, g_iADDR3_CPU0, g_iADDR4_CPU0, g_iADDR5_CPU0, g_iADDR6_CPU0, g_iADDR7_CPU0, g_iADDR8_CPU0;
int g_iADDR1_CPU1, g_iADDR2_CPU1, g_iADDR3_CPU1, g_iADDR4_CPU1, g_iADDR5_CPU1, g_iADDR6_CPU1, g_iADDR7_CPU1, g_iADDR8_CPU1;
int g_iDATA1_CPU0, g_iDATA5_CPU0;
int g_iDATA2_CPU1, g_iDATA6_CPU1;

static DEFINE_MUTEX(cpu0_speed_mutex);
static DEFINE_MUTEX(cpu1_speed_mutex);

int ca9_speed_test(int cpu_id)
{
    int iResult;
    
    if(cpu_id == 0)
    {
        iResult = max_power_loop();
        
        if(iResult == g_iCPU0_FinalResult)
        {
            iResult = speed_indicative_loop();                
            if(iResult == 0)
                printk("cpu0 speed_indicative_loop fail\n");             
        }
        else
        { 
            printk("cpu0 max_power_loop fail (result = 0x%x)\n",iResult);
            iResult = 0;
        }       
    }
    else
    {
        iResult = max_power_loop();
        
        if(iResult == g_iCPU1_FinalResult)
        {    
            iResult = speed_indicative_loop();
                
            if(iResult == 0)
                printk("cpu1 speed_indicative_loop fail\n");             
        }
        else
        { 
            printk("cpu1 max_power_loop fail (result = 0x%x)\n",iResult);
            iResult = 0;
        }           
    }
      
    return iResult;
}

static struct device_driver cpu0_slt_ca9_max_power_drv =
{
    .name = "cpu0_ca9_max_power",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static struct device_driver cpu1_slt_ca9_max_power_drv =
{
    .name = "cpu1_ca9_max_power",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static ssize_t cpu0_slt_ca9_max_power_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "CPU0 CA9_MAX_POWER - %s(loop_count = %d)\n", g_iCPU0_PassFail != SLT_LOOP_CNT ? "FAIL" : "PASS", g_iCPU0_PassFail);
}

static ssize_t cpu1_slt_ca9_max_power_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "CPU1 CA9_MAX_POWER - %s(loop_count = %d)\n", g_iCPU1_PassFail != SLT_LOOP_CNT ? "FAIL" : "PASS", g_iCPU1_PassFail);
}

static ssize_t cpu0_slt_ca9_max_power_store(struct device_driver *driver, const char *buf, size_t count)
{
    unsigned int i, ret;
        
    unsigned long mask;
    int ResultR0, ResultR1;
    unsigned int pTestMem;  
    int retry=0;       

    const int ResultR2 = 0x55555555;
    const int ResultR3 = 0x55555555;
    const int ResultR4 = 0xFAFAFAFB;
    const int ResultR10 = 0x5F5F5F60;

    pTestMem = (unsigned int)vmalloc(17*1024);
    if((void*)pTestMem == NULL)
    {
        printk("allocate memory for cpu0 speed test fail\n");
        return 0;
    }     

    g_iADDR1_CPU0 = pTestMem + 0x0040;  //size 64 byte
    g_iADDR2_CPU0 = pTestMem + 0x0080;  //size 64 byte
    g_iADDR3_CPU0 = pTestMem + 0x00C0;  //size 64 byte
    g_iADDR4_CPU0 = pTestMem + 0x00E0;  //size 64 byte
    g_iADDR5_CPU0 = pTestMem + 0x0100;  //size 64 byte
    g_iADDR6_CPU0 = pTestMem + 0x0120;  //size 64 byte
    g_iADDR7_CPU0 = pTestMem + 0x0140;  //size 64 byte
    g_iADDR8_CPU0 = pTestMem + 0x0160;  //size 64 byte

    ResultR0 = g_iADDR1_CPU0 + 8;
    ResultR1 = g_iADDR2_CPU0 + 8;

    g_iCPU0_FinalResult = ResultR0 + ResultR1 + ResultR2 + ResultR3 + ResultR4 + ResultR10;
    
    g_iDATA5_CPU0 = pTestMem + 0x0180;  //size 64 byte
    g_iDATA1_CPU0 = pTestMem + 0x0400;  //size 16 KB
           
    mutex_lock(&cpu0_speed_mutex);   
            
    //mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, 255);
       
    mask = 1; /* processor 0 */
    while(sched_setaffinity(0, (struct cpumask*) &mask) < 0) 
    {      
        printk("Could not set cpu 0 affinity for current process(%d).\n", retry); 
        retry++;
        if(retry > 100)
        {
            mutex_unlock(&cpu1_speed_mutex);
            vfree((void*)pTestMem);           
            return count;            
        }               
    }
    
    printk("\n>> CPU0 speed test start (cpu id = %d) <<\n\n", raw_smp_processor_id()); 
                   
    for (i = 0, g_iCPU0_PassFail = 0; i < SLT_LOOP_CNT; i++) {    
        //printk("processor id = %d, loop_count=%d\n",raw_smp_processor_id(), i);	    
        ret = ca9_speed_test(0);
        if(ret != 1)
            printk("CPU0 ca9_speed_test fail (loop count=%d)\n", i);
        else     
            g_iCPU0_PassFail++; 
    }

    if (g_iCPU0_PassFail == SLT_LOOP_CNT) {
        printk("\n>> CPU0 speed test pass <<\n\n"); 
    }else {
        printk("\n>> CPU0 speed test fail (loop count = %d)<<\n\n",g_iCPU0_PassFail); 

        //mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, 0);
        //while (1);
    }
    
    mutex_unlock(&cpu0_speed_mutex);
    vfree((void*)pTestMem);
            
    return count;
}

static ssize_t cpu1_slt_ca9_max_power_store(struct device_driver *driver, const char *buf, size_t count)
{
    unsigned int i, ret;
        
    unsigned long mask;
    int ResultR0, ResultR1;
    unsigned int pTestMem;    
    int retry=0;
        
    const int ResultR2 = 0x55555555;
    const int ResultR3 = 0x55555555;
    const int ResultR4 = 0xFAFAFAFB;
    const int ResultR10 = 0x5F5F5F60;

    pTestMem = (unsigned int)vmalloc(17*1024);
    if((void*)pTestMem == NULL)
    {
        printk("allocate memory for cpu1 speed test fail\n");
        return 0;
    }   
    
    g_iADDR1_CPU1 = pTestMem + 0x0040;  //size 64 byte
    g_iADDR2_CPU1 = pTestMem + 0x0080;  //size 64 byte
    g_iADDR3_CPU1 = pTestMem + 0x00C0;  //size 32 byte
    g_iADDR4_CPU1 = pTestMem + 0x00E0;  //size 32 byte
    g_iADDR5_CPU1 = pTestMem + 0x0100;  //size 32 byte
    g_iADDR6_CPU1 = pTestMem + 0x0120;  //size 32 byte
    g_iADDR7_CPU1 = pTestMem + 0x0140;  //size 32 byte
    g_iADDR8_CPU1 = pTestMem + 0x0160;  //size 32 byte

    ResultR0 = g_iADDR1_CPU1 + 8;
    ResultR1 = g_iADDR2_CPU1 + 8;

    g_iCPU1_FinalResult = ResultR0 + ResultR1 + ResultR2 + ResultR3 + ResultR4 + ResultR10;

    g_iDATA6_CPU1 = pTestMem + 0x0180;  //size 64 byte     
    g_iDATA2_CPU1 = pTestMem + 0x0400;  //size 16 KB    
            
    mutex_lock(&cpu1_speed_mutex);   
       
    //mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, 255);

    mask = 2; /* processor 1 */
    while(sched_setaffinity(0, (struct cpumask*)&mask) < 0) 
    {      
        printk("Could not set cpu 1 affinity for current process(%d).\n", retry);    
        retry++;
        if(retry > 100)
        {
            mutex_unlock(&cpu1_speed_mutex);
            vfree((void*)pTestMem);           
            return count;            
        }      
    }

    printk("\n>> CPU1 speed test start (cpu id = %d) <<\n\n", raw_smp_processor_id()); 
           
    for (i = 0, g_iCPU1_PassFail = 0; i < SLT_LOOP_CNT; i++) { 
        //printk("processor id = %d, loop_count=%d\n",raw_smp_processor_id(), i);	           
        ret = ca9_speed_test(1);
        if(ret != 1)
            printk("CPU1 ca9_speed_test fail (loop count=%d)\n", i);
        else     
            g_iCPU1_PassFail++;         
    }

    if (g_iCPU1_PassFail == SLT_LOOP_CNT) {
        printk("\n>> CPU1 speed test pass <<\n\n"); 
    }else {
        printk("\n>> CPU1 speed test fail (loop count = %d)<<\n\n",g_iCPU1_PassFail); 

        //mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, 0);
        //while (1);
    }

    mutex_unlock(&cpu1_speed_mutex);
    vfree((void*)pTestMem);
            
    return count;
}

DRIVER_ATTR(cpu0_slt_ca9_max_power, 0644, cpu0_slt_ca9_max_power_show, cpu0_slt_ca9_max_power_store);
DRIVER_ATTR(cpu1_slt_ca9_max_power, 0644, cpu1_slt_ca9_max_power_show, cpu1_slt_ca9_max_power_store);

int __init cpu0_slt_ca9_max_power_init(void)
{
    int ret;
    
    ret = driver_register(&cpu0_slt_ca9_max_power_drv);
    if (ret) {
        printk("fail to create CA9 max power SLT driver (CPU0)\n");
    }
    else
    {
        printk("success to create CA9 max power SLT driver (CPU0)\n");    
    }
    

    ret = driver_create_file(&cpu0_slt_ca9_max_power_drv, &driver_attr_cpu0_slt_ca9_max_power);
    if (ret) {
        printk("fail to create CA9 max power sysfs files (CPU0)\n");
    }
    else
    {
        printk("success to create CA9 max power sysfs files (CPU0)\n");        
    }

    return 0;
}

int __init cpu1_slt_ca9_max_power_init(void)
{
    int ret;

    ret = driver_register(&cpu1_slt_ca9_max_power_drv);
    if (ret) {
        printk("fail to create CA9 max power SLT driver (CPU1)\n");
    }
    else
    {
        printk("success to create CA9 max power SLT driver (CPU1)\n");    
    }
    

    ret = driver_create_file(&cpu1_slt_ca9_max_power_drv, &driver_attr_cpu1_slt_ca9_max_power);
    if (ret) {
        printk("fail to create CA9 max power sysfs files (CPU1)\n");
    }
    else
    {
        printk("success to create CA9 max power sysfs files (CPU1)\n");        
    }

    return 0;
}
arch_initcall(cpu0_slt_ca9_max_power_init);
arch_initcall(cpu1_slt_ca9_max_power_init);
