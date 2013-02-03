#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/smp.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <asm/cacheflush.h>
#include <mach/hotplug.h>

volatile u32 cpu1_killed = 0;

extern volatile int pen_release;
#ifdef CONFIG_LOCAL_WDT	
extern void mpcore_wk_wdt_stop(void);
extern void wk_stop_kick_cpu(int cpu);
#endif

static void hwe_init_dummy(int cpunum)
{
    return;    
}

static void (*hwe_init)(int) = &hwe_init_dummy;
EXPORT_SYMBOL(hwe_init);
static DEFINE_SPINLOCK(cpu1_pwr_ctr_lock);


static inline void cpu_enter_lowpower(void)
{
    HOTPLUG_INFO("cpu_enter_lowpower\n");

    /* Just flush the cache. */
    flush_cache_all();
}

static inline void cpu_leave_lowpower(void)
{
    HOTPLUG_INFO("cpu_leave_lowpower\n");
}

static inline void platform_do_lowpower(unsigned int cpu, int *spurious)
{
    /* Just enter wfi for now. TODO: Properly shut off the cpu. */
    for (;;) {
        /* 6.3 Set CPU1 power status register in SCU to dormant mode or shut-down mode (shutdown mode now) */
        REG_WRITE(SCU_CPU_PWR_STATUS, REG_READ(SCU_CPU_PWR_STATUS) | 0x00000300);
    
        /*
         * here's the WFI
         */
        dsb();
        __asm__ __volatile__("wfi");
        
        if (pen_release == cpu) {
            /*
             * OK, proper wakeup, we're done
             */
            HOTPLUG_INFO("platform_do_lowpower, pen_release: %d, cpu: %d\n", pen_release, cpu);
            break;
        }

        /*
         * Getting here, means that we have come out of WFI without
         * having been woken up - this shouldn't happen
         *
         * Just note it happening - when we're woken, we can report
         * its occurrence.
         */
        (*spurious)++;
    }
}


void power_on_cpu1(void)
{
    unsigned long flags;
    spin_lock_irqsave(&cpu1_pwr_ctr_lock, flags);
    
    cpu1_killed = 0;
    mb();
    
    (*hwe_init)(2);
    
    /* 1. Set RST_CTL0[1] = 1 */
    REG_WRITE(RST_CTL0, REG_READ(RST_CTL0) | 0x00000002);
    
    /* 2. Set PWR_CTL1[15] = 1 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x00008000);
    mb();
    udelay(32); //delay 31.25us(1T of 32K) for power-on settle time
    
    /* 3. Set PWR_CTL1[4] = 1 for dormant mode, or set PWR_CTL1[3:0]=4・b0000 for shutdown mode (shutdown mode now) */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xfffffff0);
    mb();
    mdelay(2); //delay 2ms
    
    /* 4. Set PWR_CTL1[13] = 0 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xffffdfff);
    
    /* 5. Set PWR_CTL1[12] = 0 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xffffefff);
    
    /* 6. Set PWR_CTL1[14] = 0 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xffffbfff);
    
    /* 7. turn on neon1 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x00000800);
    mb();
    udelay(32); //delay 31.25us(1T of 32K)
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xfffffdff);
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xfffffeff);
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xfffffbff);
    
    /* 8.3 Set RST_CTL0[1] = 0 */
    REG_WRITE(RST_CTL0, REG_READ(RST_CTL0) & 0xfffffffd);
    mb();
    udelay(10); //delay 10us for soft reset workaround
    
    /* 8.2 Set CPU1・s power status register in SCU to normal mode */
    REG_WRITE(SCU_CPU_PWR_STATUS, REG_READ(SCU_CPU_PWR_STATUS) & 0xfffffcff);
    /* 8.1 Set PWR_CTL1[25:24] = 2・b00 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xfcffffff);
    mb();
    
    spin_unlock_irqrestore(&cpu1_pwr_ctr_lock, flags);
}


void power_off_cpu1(void)
{
    unsigned long flags;
    spin_lock_irqsave(&cpu1_pwr_ctr_lock, flags);
    
    /* 6.2 Set PWR_CTL1[25:24] = 2・b10 for dormant mode or set 2・b11 for shutdown mode (shutdown mode now) */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x03000000);
    
    /* 1.1 Polling PWR_MON[13] = 1 to make sure CPU1 is in WFI */
    while (!(REG_READ(PWR_MON) & (1U << 13)));
    
    /* 1.2 turn off neon1 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x00000400);
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x00000100);
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x00000200);
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xfffff7ff);
    
    /* 2. Set PWR_CTL1[14] = 1 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x00004000);
    
    /* 3. Set PWR_CTL1[12] = 1 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x00001000);
    
    /* 4. Set PWR_CTL1[13] = 1 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x00002000);
    
    /* 5. Set PWR_CTL1[4] = 0 for dormant mode, or set PWR_CTL1[3:0]=4・b1111 for shutdown mode (shutdown mode now) */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) | 0x0000000f);
    mb();
    mdelay(2); //delay 2ms
    
    /* 6.1 Set PWR_CTL1[15] = 0 */
    REG_WRITE(PWR_CTL1, REG_READ(PWR_CTL1) & 0xffff7fff);
    
    
    mb();
    
    (*hwe_init)(1);
    
    cpu1_killed = 1;
    mb();
    
    spin_unlock_irqrestore(&cpu1_pwr_ctr_lock, flags);
}


/*
 * platform_cpu_kill:
 * @cpu:
 * Return TBD.
 */
int platform_cpu_kill(unsigned int cpu)
{
     HOTPLUG_INFO("platform_cpu_kill, cpu: %d\n", cpu);
     
     power_off_cpu1();
     
     return 1;
}

/*
 * platform_cpu_die: shutdown a CPU
 * @cpu:
 */
void platform_cpu_die(unsigned int cpu)
{
    int spurious = 0;
    
    HOTPLUG_INFO("platform_cpu_die, cpu: %d\n", cpu);

#ifdef CONFIG_LOCAL_WDT
    /* disable local watchdog */
    mpcore_wk_wdt_stop();
    wk_stop_kick_cpu(cpu);
#endif

    /*
     * we're ready for shutdown now, so do it
     */
    cpu_enter_lowpower();
    platform_do_lowpower(cpu, &spurious);

    /*
     * bring this CPU back into the world of cache
     * coherency, and then restore interrupts
     */
    cpu_leave_lowpower();
    
    if (spurious)
        HOTPLUG_INFO("platform_do_lowpower, spurious wakeup call, cpu: %d, spurious: %d\n", cpu, spurious);
}

/*
 * platform_cpu_disable:
 * @cpu:
 * Return error code.
 */
int platform_cpu_disable(unsigned int cpu)
{
    /*
    * we don't allow CPU 0 to be shutdown (it is still too special
    * e.g. clock tick interrupts)
    */
    HOTPLUG_INFO("platform_cpu_disable, cpu: %d\n", cpu);
    return cpu == 0 ? -EPERM : 0;
}
