#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/wakelock.h>
#include <linux/module.h>
#include <asm/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

#include <linux/leds-mt65xx.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include "mach/ca9_slt.h"

int g_iNeonADDR1_CPU0, g_iNeonADDR2_CPU0, g_iNeonADDR3_CPU0, g_iNeonADDR4_CPU0, g_iNeonADDR5_CPU0, g_iNeonADDR6_CPU0, g_iNeonADDR7_CPU0, g_iNeonADDR8_CPU0;
int g_iNeonADDR1_CPU1, g_iNeonADDR2_CPU1, g_iNeonADDR3_CPU1, g_iNeonADDR4_CPU1, g_iNeonADDR5_CPU1, g_iNeonADDR6_CPU1, g_iNeonADDR7_CPU1, g_iNeonADDR8_CPU1;
int g_iNeonDATA1_CPU0, g_iNeonDATA6_CPU0;
int g_iNeonDATA2_CPU1, g_iNeonDATA5_CPU1;

static const unsigned int __attribute__ ((aligned (64))) NEON_TEST_PATTERN[] = {
0x46B94BFB,
0x1C667E81,
0xFF6ABDDF,
0x9E2CFB54,
0x487201BF,
0x37CF1CE1,
0x37200F47,
0x2313DE56,
0x359DAA88,
0xC6756687,
0x6561137B,
0x73CF59EA,
0x9D853C54,
0x1D2D85A1,
0xAAEE655B,
0xD5502F37,
0x3988CCE3,
0x3DA10279,
0xA5F8D999,
0xF1AB768E,
0xF368EE43,
0x88F51F3F,
0x3FAFBEA6,
0xF78E4D0D,
0x48E2DC29,
0xDDA0E486,
0xF56BE170,
0xC6D34EFF,
0x4B135805,
0x6206FB8F,
0xDDCB53CD,
0x690DC509,
0x799452D3,
0x3AA73B48,
0xA9F1EAB5,
0x152E9D06,
0xB98949B2,
0x6C2C0613,
0xE3228831,
0x00171EAC,
0xA95E7136,
0x29C75246,
0xA3951D16,
0x38FD57F6,
0x8F5E987C,
0xCFA38874,
0x0BBD6D71,
0xD1765C41,
0x2593C701,
0xC8EC0F70,
0xB3D691C0,
0x91BA2F33,
0x80D6AB33,
0xA1D1A50D,
0x58F73EE6,
0x78D57E5E,
0x53A0B1C3,
0x868E74A6,
0x0642C7DB,
0x493D11A8,
0xCE14F3BF,
0x6CADE060,
0xFA2AE325,
0x60970067
} ;

static int g_iFinalResult_CPU0;
static int g_iFinalResult_CPU1;

extern int ca9_neon(void);
extern int ca9_neon_indicative(void);
extern int max_power_neon_loop(void);

static unsigned int g_iNeonPassFail_CPU0;
static unsigned int g_iNeonPassFail_CPU1;
const unsigned int g_NeonTestMem = (const unsigned int) NEON_TEST_PATTERN;

int init_ca9_neon_data(unsigned int *pAddr)
{
    if (((unsigned int) pAddr) % 64 != 0) {
        printk("[init_ca9_neon_data(): parameter not 64B aligned !]\n");
        return 0;
    }
    else
        return 1;
}

DEFINE_SPINLOCK(cpu0_lock);
DEFINE_SPINLOCK(cpu1_lock);
unsigned long cpu0_flags;
unsigned long cpu1_flags;
    
int ca9_neon_test(int cpu_id)
{
    int iV;

    if(cpu_id == 0)
    {    
        spin_lock_irqsave(&cpu0_lock, cpu0_flags);
        iV = ca9_neon();
       
        if (iV == 1)
        {            
#if 1  
            iV = ca9_neon_indicative(); 
               
            if(iV == 1)
            {    
#endif                             
                iV = max_power_neon_loop();
                
                if(iV == g_iFinalResult_CPU0)
                {           
                    iV = 1;            
                }
                else
                {                  
                    iV = 0;
                    printk("cpu0 max_power_neon_loop fail\n");                   
                }        
#if 1  
            }           
            else
            {  
                printk("cpu0 ca9_neon_indicative fail\n");
            } 
#endif           
        }
        else
        {
            printk("cpu0 ca9_neon fail\n");     
        }
        spin_unlock_irqrestore(&cpu0_lock, cpu0_flags);
    }
    else
    {
        spin_lock_irqsave(&cpu1_lock, cpu1_flags);
        iV = ca9_neon();
       
        if (iV == 1)
        {            
#if 0  
            iV = ca9_neon_indicative(); 
               
            if(iV == 1)
            {    
#endif                             
                iV = max_power_neon_loop();
                
                if(iV == g_iFinalResult_CPU1)
                {           
                    iV = 1;            
                }
                else
                {                  
                    iV = 0;
                    printk("cpu1 max_power_neon_loop fail\n");                   
                }        
#if 0  
            }           
            else
            {  
                printk("cpu1 ca9_neon_indicative fail\n");
            } 
#endif           
        }
        else
        {
            printk("cpu1 ca9_neon fail\n");     
        }
        spin_unlock_irqrestore(&cpu1_lock, cpu1_flags);        
    }

    return iV;
}

static struct device_driver cpu0_slt_ca9_neon_drv =
{
    .name = "cpu0_ca9_neon",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static struct device_driver cpu1_slt_ca9_neon_drv =
{
    .name = "cpu1_ca9_neon",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static ssize_t cpu0_slt_ca9_neon_show(struct device_driver *driver, char *buf)
{
    //return snprintf(buf, PAGE_SIZE, "CPU0 CA9_NEON(SLT_LOOP_LED) - %s\n", g_iNeonPassFail_CPU0 == SLT_LOOP_CNT ? "PASS" : "FAIL");
    return snprintf(buf, PAGE_SIZE, "CPU0 CA9_NEON - %s(loop_count = %d)\n", g_iNeonPassFail_CPU0 != SLT_LOOP_CNT ? "FAIL" : "PASS", g_iNeonPassFail_CPU0);
}

static ssize_t cpu1_slt_ca9_neon_show(struct device_driver *driver, char *buf)
{
    //return snprintf(buf, PAGE_SIZE, "CPU1 CA9_NEON(SLT_LOOP_LED) - %s\n", g_iNeonPassFail_CPU1 == SLT_LOOP_CNT ? "PASS" : "FAIL");
    return snprintf(buf, PAGE_SIZE, "CPU1 CA9_NEON - %s(loop_count = %d)\n", g_iNeonPassFail_CPU1 != SLT_LOOP_CNT ? "FAIL" : "PASS", g_iNeonPassFail_CPU1);    
}

static ssize_t cpu0_slt_ca9_neon_store(struct device_driver *driver, const char *buf, size_t count)
{
    int i;   
    unsigned long mask;                
    int ResultR0, ResultR1;
    unsigned int pTestMem;  
    int retry=0;

    const int ResultR2 = 0x55555555;
    const int ResultR3 = 0x55555555;
    const int ResultR4 = 0xA63CD36A;
    const int ResultR10 = 0x5F5F5F60;
        
    g_iNeonPassFail_CPU0 = 0;

    pTestMem = (unsigned int)vmalloc(1*1024);
    if((void*)pTestMem == NULL)
    {
        printk("allocate memory for cpu0 neon test fail\n");
        return 0;
    }    

    if (init_ca9_neon_data((unsigned int *) g_NeonTestMem) == 0)
        return 0;

    g_iNeonDATA1_CPU0 = pTestMem + 0x0000;  //size 256 byte
    g_iNeonDATA6_CPU0 = pTestMem + 0x0100;  //size 64 byte

    g_iNeonADDR1_CPU0 = pTestMem + 0x0140;  //size 64 byte
    g_iNeonADDR2_CPU0 = pTestMem + 0x0180;  //size 64 byte
    g_iNeonADDR3_CPU0 = pTestMem + 0x01C0;  //size 32 byte
    g_iNeonADDR4_CPU0 = pTestMem + 0x01E0;  //size 32 byte
    g_iNeonADDR5_CPU0 = pTestMem + 0x0200;  //size 32 byte
    g_iNeonADDR6_CPU0 = pTestMem + 0x0220;  //size 32 byte
    g_iNeonADDR7_CPU0 = pTestMem + 0x0240;  //size 32 byte
    g_iNeonADDR8_CPU0 = pTestMem + 0x0260;  //size 32 byte
            
    ResultR0 = g_iNeonADDR1_CPU0 + 8;
    ResultR1 = g_iNeonADDR2_CPU0 + 8;

    g_iFinalResult_CPU0 = ResultR0 + ResultR1 + ResultR2 + ResultR3 + ResultR4 + ResultR10;    
    
    //mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, 255);

    mask = 1; /* processor 0 */
    while(sched_setaffinity(0, (struct cpumask*) &mask) < 0) 
    {      
        printk("Could not set cpu 0 affinity for current process (%d).\n", retry);    
        retry++;
        if(retry > 100)
        {
            vfree((void*)pTestMem);           
            return count;            
        }               
    }
    
    printk("\n>> CPU0 ca9 neon test start (cpu id = %d) <<\n", raw_smp_processor_id());
        
    for (i = 0, g_iNeonPassFail_CPU0 = 0; i < SLT_LOOP_CNT; i++) {
        //printk("processor id = %d, loop_count = %d\n",raw_smp_processor_id(), i);	        
        g_iNeonPassFail_CPU0 += ca9_neon_test(0);  // 1: PASS, 0:Fail         
    }

    if (g_iNeonPassFail_CPU0 == SLT_LOOP_CNT) {
        printk("\n>> CPU0 ca9 neon test pass <<\n");
    }else {
        printk("\n>> CPU0 ca9 neon test fail (loop_count = %d) <<\n", g_iNeonPassFail_CPU0);
        //mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, 0);
        //while (1);
    }
    
    vfree((void*)pTestMem);

    return count;
}

static ssize_t cpu1_slt_ca9_neon_store(struct device_driver *driver, const char *buf, size_t count)
{
    int i;
    
    unsigned long mask;
    int ResultR0, ResultR1;
    unsigned int pTestMem;   
    int retry=0;   

    const int ResultR2 = 0x55555555;
    const int ResultR3 = 0x55555555;
    const int ResultR4 = 0xA63CD36A;
    const int ResultR10 = 0x5F5F5F60;
        
    g_iNeonPassFail_CPU1 = 0;

    pTestMem = (unsigned int)vmalloc(1*1024);
    if((void*)pTestMem == NULL)
    {
        printk("allocate memory for cpu1 neon test fail\n");
        return 0;
    }   

    if (init_ca9_neon_data((unsigned int *) g_NeonTestMem) == 0)
        return 0;


    g_iNeonDATA2_CPU1 = pTestMem + 0x0000;  //size 256 byte
    g_iNeonDATA5_CPU1 = pTestMem + 0x0100;  //size 64 byte

    g_iNeonADDR1_CPU1 = pTestMem + 0x0140;  //size 64 byte
    g_iNeonADDR2_CPU1 = pTestMem + 0x0180;  //size 64 byte
    g_iNeonADDR3_CPU1 = pTestMem + 0x01C0;  //size 64 byte
    g_iNeonADDR4_CPU1 = pTestMem + 0x01E0;  //size 64 byte
    g_iNeonADDR5_CPU1 = pTestMem + 0x0200;  //size 64 byte
    g_iNeonADDR6_CPU1 = pTestMem + 0x0220;  //size 64 byte
    g_iNeonADDR7_CPU1 = pTestMem + 0x0240;  //size 64 byte
    g_iNeonADDR8_CPU1 = pTestMem + 0x0260;  //size 64 byte
          
    ResultR0 = g_iNeonADDR1_CPU1 + 8;
    ResultR1 = g_iNeonADDR2_CPU1 + 8;

    g_iFinalResult_CPU1 = ResultR0 + ResultR1 + ResultR2 + ResultR3 + ResultR4 + ResultR10;    
    
    //mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, 255);
    
    mask = 2; /* processor 1 */
    while(sched_setaffinity(0, (struct cpumask*) &mask) < 0) 
    {      
        printk("Could not set cpu 1 affinity for current process (%d).\n", retry);
        retry++;
        if(retry > 100)
        {
            vfree((void*)pTestMem);           
            return count;            
        }                   
    }

    printk("\n>> CPU1 ca9 neon test start (cpu id = %d) <<\n", raw_smp_processor_id());
        
    for (i = 0, g_iNeonPassFail_CPU1 = 0; i < SLT_LOOP_CNT; i++) {
        //printk("processor id = %d, loop_count = %d\n",raw_smp_processor_id(), i);	        
        g_iNeonPassFail_CPU1 += ca9_neon_test(1);  // 1: PASS, 0:Fail         
    }

    if (g_iNeonPassFail_CPU1 == SLT_LOOP_CNT) {
        printk("\n>> CPU1 ca9 neon test pass <<\n");
    }else {
        printk("\n>> CPU1 ca9 neon test fail (loop_count = %d) <<\n", g_iNeonPassFail_CPU1);
        //mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, 0);
        //while (1);
    }
    
    vfree((void*)pTestMem);
            
    return count;
}

DRIVER_ATTR(cpu0_slt_ca9_neon, 0644, cpu0_slt_ca9_neon_show, cpu0_slt_ca9_neon_store);
DRIVER_ATTR(cpu1_slt_ca9_neon, 0644, cpu1_slt_ca9_neon_show, cpu1_slt_ca9_neon_store);

int __init cpu0_slt_ca9_neon_init(void)
{
    int ret;

    ret = driver_register(&cpu0_slt_ca9_neon_drv);
    if (ret) {
        printk("fail to create CA9 NEON SLT driver (CPU0)\n");
    }
    else
    {
        printk("success to create CA9 NEON SLT driver (CPU0)\n");
    }

    ret = driver_create_file(&cpu0_slt_ca9_neon_drv, &driver_attr_cpu0_slt_ca9_neon);
    if (ret) {
        printk("fail to create CA9 NEON SLT sysfs files (CPU0)\n");
    }
    else
    {
        printk("success to create CA9 NEON SLT sysfs files (CPU0)\n");
    }    

    return 0;
}

int __init cpu1_slt_ca9_neon_init(void)
{
    int ret;

    ret = driver_register(&cpu1_slt_ca9_neon_drv);
    if (ret) {
        printk("fail to create CA9 NEON SLT driver (CPU1)\n");
    }
    else
    {
        printk("success to create CA9 NEON SLT driver (CPU1)\n");
    }

    ret = driver_create_file(&cpu1_slt_ca9_neon_drv, &driver_attr_cpu1_slt_ca9_neon);
    if (ret) {
        printk("fail to create CA9 NEON SLT sysfs files (CPU1)\n");
    }
    else
    {
        printk("success to create CA9 NEON SLT sysfs files (CPU1)\n");
    }    

    return 0;
}
arch_initcall(cpu0_slt_ca9_neon_init);
arch_initcall(cpu1_slt_ca9_neon_init);