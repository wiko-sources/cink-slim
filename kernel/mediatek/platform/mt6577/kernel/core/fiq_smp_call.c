#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cpu.h>
#include <linux/percpu.h>
#include <linux/smp.h>
#include <mach/irqs.h>

#if defined(CONFIG_FIQ_GLUE)

enum {
    CSD_FLAG_LOCK = 0x01,
};

struct call_function_data
{
    struct call_single_data csd;
    atomic_t refs;
    cpumask_var_t cpumask;
};

static DEFINE_PER_CPU_SHARED_ALIGNED(struct call_function_data, fiq_cfd_data);
static struct call_function_data *current_cfd_data;
static spinlock_t fiq_smp_call_lock;

extern void irq_raise_softirq(const struct cpumask *mask, unsigned int irq);

static int __csd_lock_wait(struct call_single_data *data)
{       
    int cpu, nr_online_cpus = 0;
    
    while (data->flags & CSD_FLAG_LOCK) {
        for_each_cpu(cpu, data->cpumask) {
            if (cpu_online(cpu)) {
                nr_online_cpus++;
            }
        }
        if (!nr_online_cpus)
            return -ENXIO;
        cpu_relax();
    }
    
    return 0;
}       

static void __csd_lock(struct call_single_data *data)
{
    __csd_lock_wait(data);
    data->flags = CSD_FLAG_LOCK;

    /*
     * prevent CPU from reordering the above assignment
     * to ->flags with any subsequent assignments to other
     * fields of the specified call_single_data structure:
     */
    smp_mb();
}

static void __csd_unlock(struct call_single_data *data)
{
    WARN_ON(!(data->flags & CSD_FLAG_LOCK));

    /*
     * ensure we're all done before releasing data:
     */
    smp_mb();

    data->flags &= ~CSD_FLAG_LOCK;
}

/*
 * fiq_smp_call_function: FIQ version of smp_call_function.
 * @func:
 * @info:
 * @wait:
 * Return 0 for success and error code for failure. 
 *
 * This function is designed for the debugger only.
 * Other kernel code or drivers should NOT use this function.
 * This function cannot be invoked with the interrupt disabled or 
 * in the interrupt context. 
 * The caller has better to disable CPU hotplug before invoking the function. 
 */
int fiq_smp_call_function(smp_call_func_t func, void *info, int wait)
{
    struct cpumask *mask = (struct cpumask *)cpu_online_mask;
    struct call_function_data *data;
    int refs, install_csd, this_cpu;

    preempt_disable();

    this_cpu = smp_processor_id();

    WARN_ON_ONCE(cpu_online(this_cpu) && irqs_disabled()
                 && !oops_in_progress && !early_boot_irqs_disabled);

    data = &__get_cpu_var(fiq_cfd_data);
    __csd_lock(&data->csd);

    atomic_set(&data->refs, 0);

    data->csd.func = func;
    data->csd.info = info;

    smp_wmb();

    cpumask_and(data->cpumask, mask, cpu_online_mask);
    cpumask_clear_cpu(this_cpu, data->cpumask);
    refs = cpumask_weight(data->cpumask);
    cpumask_and(data->csd.cpumask, data->cpumask, data->cpumask);

    if (unlikely(!refs)) {
        __csd_unlock(&data->csd);
        goto fiq_smp_call_function_exit;
    }

    /* poll to install data on current_cfd_data */
    install_csd = 0;
    do {
        spin_lock(&fiq_smp_call_lock);

        if (!current_cfd_data) {
            atomic_set(&data->refs, refs);
            current_cfd_data = data;
            install_csd = 1;
        }

        spin_unlock(&fiq_smp_call_lock);
    } while (!install_csd);

    smp_mb();

    /* send a message to all CPUs in the map */
    irq_raise_softirq(data->cpumask, FIQ_SMP_CALL_SGI);

    if (wait)
        __csd_lock_wait(&data->csd);

fiq_smp_call_function_exit:
    preempt_enable();

    return 0;
}

static void fiq_smp_call_handler(void *arg, void *regs, void *svc_sp)
{
    struct call_function_data *data;
    int cpu = 0, refs;
    smp_call_func_t func;

    /* get the current cpu id */
    asm volatile(
        "MRC p15, 0, %0, c0, c0, 5\n"
        "AND %0, %0, #0xf\n"
        : "+r"(cpu)
        :
        : "cc"
    );

    data = current_cfd_data;
    if (data) {
        func = data->csd.func;
        func(data->csd.info);

        cpumask_clear_cpu(cpu, data->csd.cpumask);
        refs = atomic_dec_return(&data->refs);

        if (refs == 0) {
            __csd_unlock(&data->csd);
            current_cfd_data = NULL;
        }
    }
}

static void __fiq_smp_call_init(void *info)
{
    int err;

    err = request_fiq(FIQ_SMP_CALL_SGI, fiq_smp_call_handler, 0, NULL);
    if (err) {
        printk(KERN_ERR "fail to request FIQ for FIQ_SMP_CALL_SGI\n");
    } else {
        printk(KERN_INFO "Request FIQ for FIQ_SMP_CALL_SGI\n");
    }
}

static int __init fiq_smp_call_init(void)
{
    spin_lock_init(&fiq_smp_call_lock);

    __fiq_smp_call_init(NULL);
    smp_call_function(__fiq_smp_call_init, NULL, 1);

    return 0;
}

arch_initcall(fiq_smp_call_init);

#endif  /* CONFIG_FIQ_GLUE */
