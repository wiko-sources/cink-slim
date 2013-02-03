#include <linux/init.h>
#include <linux/smp.h>
#include <linux/spinlock.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <asm/smp_scu.h>
#include <asm/localtimer.h>
#include <asm/fiq_glue.h>
#include <mach/mt6577_reg_base.h>
#include <mach/smp.h>
#include <mach/sync_write.h>
#include <mach/hotplug.h>

#define SLAVE_MAGIC_REG 0xF011A008
#define SLAVE_MAGIC_NUM 0x534C4156
#define SLAVE_JUMP_REG 0xF011A000

extern void mt_secondary_startup(void);
extern void irq_raise_softirq(const struct cpumask *mask, unsigned int irq);
extern void mt_gic_secondary_init(void);

#ifdef CONFIG_LOCAL_WDT
enum wk_wdt_type {
	WK_WDT_LOC_TYPE,
	WK_WDT_EXT_TYPE
};
extern void mtk_wdt_restart(enum wk_wdt_type type);
extern void wk_start_kick_cpu_hotplug(int cpu);



#endif
/*
 * control for which core is the next to come out of the secondary
 * boot "holding pen".
 */
volatile int pen_release = -1;

static DEFINE_SPINLOCK(boot_lock);

void __cpuinit platform_secondary_init(unsigned int cpu)
{
    printk(KERN_CRIT "Slave cpu init\n");

    mt_gic_secondary_init();
    
    HOTPLUG_INFO("platform_secondary_init, cpu: %d\n", cpu);
    pen_release = -1;
    smp_wmb();

#ifdef CONFIG_LOCAL_WDT
    printk("[WDK]cpu %d plug on platform_secondary_init++++++\n", cpu);
    wk_start_kick_cpu_hotplug(cpu);
    mtk_wdt_restart(WK_WDT_LOC_TYPE);
    mtk_wdt_restart(WK_WDT_EXT_TYPE);
    printk("[WDK]cpu %d plug on platform_secondary_init------\n", cpu);
#endif	    

    fiq_glue_resume();

    /*
     * Synchronise with the boot thread.
     */
    spin_lock(&boot_lock);
    spin_unlock(&boot_lock);
}

int __cpuinit boot_secondary(unsigned int cpu, struct task_struct *idle)
{
    unsigned long timeout;
    static int is_first_boot = 1;
    
    printk(KERN_CRIT "Boot slave CPU\n");
    
    /*
     * Set synchronisation state between this boot processor
     * and the secondary one
     */
    spin_lock(&boot_lock);
    
    HOTPLUG_INFO("boot_secondary, cpu: %d\n", cpu);
    /*
    * The secondary processor is waiting to be released from
    * the holding pen - release it, then wait for it to flag
    * that it has been released by resetting pen_release.
    *
    * Note that "pen_release" is the hardware CPU ID, whereas
    * "cpu" is Linux's internal ID.
    */
    pen_release = cpu;
    __cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
    outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));
    
    if (is_first_boot)
    {
        mt65xx_reg_sync_writel(SLAVE_MAGIC_NUM, SLAVE_MAGIC_REG);
        is_first_boot = 0;
    }
    else
    {
        mt65xx_reg_sync_writel(virt_to_phys(mt_secondary_startup), BOOTROM_BOOT_ADDR);
    }
    power_on_cpu1();
    
    smp_cross_call(cpumask_of(cpu));
    
    timeout = jiffies + (1 * HZ);
    while (time_before(jiffies, timeout)) {
        smp_rmb();
        if (pen_release == -1)
            break;
        
        udelay(10);
    }

    /*
     * Now the secondary core is starting up let it run its
     * calibrations, then wait for it to finish
     */
    spin_unlock(&boot_lock);

    return pen_release != -1 ? -ENOSYS : 0;
}

void __init smp_init_cpus(void)
{
    unsigned int i, ncores;
    
    /*
     * NoteXXX: CPU 1 may not be reset clearly after power-ON.
     *          Need to apply a S/W workaround to manualy reset it first.
     */
    u32 val;
    val = *(volatile u32 *)0xF0009010;
    mt65xx_reg_sync_writel(val | 0x2, 0xF0009010);
    udelay(10);
    mt65xx_reg_sync_writel(val & ~0x2, 0xF0009010);
    udelay(10);

    ncores = scu_get_core_count((void *)SCU_BASE);

    if (ncores > NR_CPUS) {
        printk(KERN_WARNING
               "SCU core count (%d) > NR_CPUS (%d)\n", ncores, NR_CPUS);
        printk(KERN_WARNING
               "set nr_cores to NR_CPUS (%d)\n", NR_CPUS);
        ncores = NR_CPUS;
    }

    for (i = 0; i < ncores; i++)
        set_cpu_possible(i, true);

    set_smp_cross_call(irq_raise_softirq);
    
}

void __init platform_smp_prepare_cpus(unsigned int max_cpus)
{
    int i;

    for (i = 0; i < max_cpus; i++)
        set_cpu_present(i, true);

    scu_enable((void *)SCU_BASE);

    /* write the address of slave startup into the system-wide flags register */
    mt65xx_reg_sync_writel(virt_to_phys(mt_secondary_startup), SLAVE_JUMP_REG);
}
