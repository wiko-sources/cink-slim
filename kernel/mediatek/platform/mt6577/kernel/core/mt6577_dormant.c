/*********************************
* include
**********************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <asm/system.h>
#include <mach/mt6577_dormant.h>
#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_boot.h>
#include <mach/mt6577_sc.h>
#include <mach/mt6577_irq.h>
#include <mach/mt6577_boot.h>



/*********************************
* macro
**********************************/
#define BOOTROM_PWR_CTRL        (BOOTROM_BASE + 0x20)
#define BOOTROM_BOOT_ADDR       (BOOTROM_BASE + 0x30)

#define reg_read(addr)          (*(volatile u32 *)(addr))
#define reg_write(addr, val)    (*(volatile u32 *)(addr) = (u32)(val))

#define A9_SMP_BIT              (1 << 6)

#define C_BIT                   0x1

/*
 * Maximum size of each item of context, in bytes
 * We round these up to 32 bytes to preserve cache-line alignment
 */
#define PMU_DATA_SIZE               128
#define TIMER_DATA_SIZE             128
#define VFP_DATA_SIZE               288
#define GIC_INTERFACE_DATA_SIZE      64
#define GIC_DIST_PRIVATE_DATA_SIZE   96
#define BANKED_REGISTERS_SIZE       192
#define CP15_DATA_SIZE               64
#define DEBUG_DATA_SIZE            1024
#define MMU_DATA_SIZE                64
#define OTHER_DATA_SIZE              32

#define GIC_DIST_SHARED_DATA_SIZE  2592
#define SCU_DATA_SIZE                32
#define L2_DATA_SIZE                256
#define GLOBAL_TIMER_DATA_SIZE      128

/*
 * Flags to record which items of CPU context have been saved
 */
#define SAVED_VFP              (1 << 0)
#define SAVED_PMU              (1 << 1)
#define SAVED_DEBUG            (1 << 2)
#define SAVED_TIMERS           (1 << 3)
#define SAVED_L2               (1 << 16)
#define SAVED_GLOBAL_TIMER     (1 << 17)

#define CPU_A9MPCORE           (0xca9)

#define CPU_SAVE_PMU           (1 << 0)
#define CPU_SAVE_TIMERS        (1 << 1)
#define CPU_SAVE_VFP           (1 << 2)
#define CPU_SAVE_DEBUG         (1 << 3)
#define CPU_SAVE_L2            (1 << 4)



/*********************************
* macro for log
**********************************/
#define CPU_DORMANT_LOG_NONE                                0
#define CPU_DORMANT_LOG_WITH_XLOG                           1
#define CPU_DORMANT_LOG_WITH_PRINTK                         2

#define CPU_DORMANT_LOG_PRINT                               CPU_DORMANT_LOG_WITH_PRINTK

#if (CPU_DORMANT_LOG_PRINT == CPU_DORMANT_LOG_NONE)
#define CPU_DORMANT_INFO(fmt, args...)                    
#elif (CPU_DORMANT_LOG_PRINT == CPU_DORMANT_LOG_WITH_XLOG)
#define CPU_DORMANT_INFO(fmt, args...)                      xlog_printk(ANDROID_LOG_INFO, "Power/cpu_dormant", fmt, ##args)
#elif (CPU_DORMANT_LOG_PRINT == CPU_DORMANT_LOG_WITH_PRINTK)
#define CPU_DORMANT_INFO(fmt, args...)                      printk("[Power/cpu_dormant] "fmt, ##args)
#endif



/*********************************
* typedef
**********************************/
typedef signed int i32;



/*********************************
* struct
**********************************/
struct cpu_cluster_context
{
    u32 saved_items;
    u32 global_timer_data[GLOBAL_TIMER_DATA_SIZE / sizeof(u32)];
    u32 gic_dist_shared_data[GIC_DIST_SHARED_DATA_SIZE / sizeof(u32)];
    u32 l2_data[L2_DATA_SIZE / sizeof(u32)];
    u32 scu_data[SCU_DATA_SIZE / sizeof(u32)];
};

struct cpu_context
{
    /*
     * Maintain the correlation between this structure and the offsets
     */
    u32 endianness;
    u32 actlr;
    u32 sctlr;
    u32 cpacr;
    u32 flags;
    u32 saved_items;
    u32 pmu_data[PMU_DATA_SIZE / sizeof(u32)];
    u32 timer_data[TIMER_DATA_SIZE / sizeof(u32)];
    u32 vfp_data[VFP_DATA_SIZE / sizeof(u32)];
    u32 gic_interface_data[GIC_INTERFACE_DATA_SIZE / sizeof(u32)];
    u32 gic_dist_private_data[GIC_DIST_PRIVATE_DATA_SIZE / sizeof(u32)];
    u32 banked_registers[BANKED_REGISTERS_SIZE / sizeof(u32)];
    u32 cp15_data[CP15_DATA_SIZE / sizeof(u32)];
#if 0 //wschen 2011-08-21
    u32 debug_data[DEBUG_DATA_SIZE / sizeof(u32)];
#endif
    u32 mmu_data[MMU_DATA_SIZE / sizeof(u32)];
    u32 other_data[OTHER_DATA_SIZE / sizeof(u32)];
};

struct cpu
{
    u32 ic_address;              /*  0 => no Interrupt Controller            */
    struct cpu_context context;
    volatile u32 power_state;
};

/*
 * A cluster is a container for CPUs, typically either a single CPU or a
 * coherent cluster.
 * We assume the CPUs in the cluster can be switched off independently.
 */
struct cpu_cluster
{
    u32 cpu_type;                /* A9mpcore, 11mpcore, etc                  */
    u32 cpu_version;
    i32 num_cpus;
    volatile i32 active_cpus;    /* Initialized to number of cpus present    */
    u32 scu_address;             /*  0 => no SCU                             */
    u32 ic_address;              /*  0 => no Interrupt Controller            */
    u32 l2_address;              /*  0 => no L2CC                            */
    struct cpu_cluster_context context;
    struct cpu *cpu_table;
    volatile i32 power_state;
};



/*********************************
* extern
**********************************/
extern unsigned *copy_words(volatile unsigned *destination, volatile unsigned *source, unsigned num_words);

#if 0 //wschen 2011-07-28
extern unsigned write_actlr(unsigned value);
extern unsigned read_actlr(void);
#endif

extern void save_control_registers(struct cpu_context *context);
extern void restore_control_registers(struct cpu_context *context);

extern void save_performance_monitors(u32 *pointer);
extern void restore_performance_monitors(u32 *pointer);

extern void save_banked_registers(u32 *pointer);
extern void restore_banked_registers(u32 *pointer);

extern void save_cp15(u32 *pointer);
extern void restore_cp15(u32 *pointer);

extern void save_vfp(u32 *pointer);
extern void restore_vfp(u32 *pointer);


extern void save_mmu(u32 *pointer);
extern void restore_mmu(u32 *pointer);


extern void save_a9_global_timer(u32 *pointer, unsigned scu_address);
extern void restore_a9_global_timer(u32 *pointer, unsigned scu_address);

#if 0 //wschen 2011-07-28
extern void save_a9_debug(u32 *pointer);
extern void restore_a9_debug(u32 *pointer);
#endif

extern void save_a9_other(u32 *pointer);
extern void restore_a9_other(u32 *pointer);

extern void invalidate_dcache_v7(void);
extern void invalidate_icache_v7(void);
extern void clean_dcache_v7_l1(void);
extern void enable_cache_v7_l1(void);
extern void disable_cache_v7_l1(void);

extern void inv_cache_range_v7_l1(unsigned start, unsigned end);
extern void get_current_stack(void *addr);

extern void cpu_wake_up(void);



/*********************************
* extern for armV7_perf_mon.S
**********************************
* please add armV7_perf_mon.o into Makefile first if you would like to use the following extern function
**********************************/
extern int armV7_perf_mon_is_overflow(unsigned int n);
extern unsigned int armV7_perf_mon_get_cyc_cnt(void);
extern void armV7_perf_mon_reset(void);
extern void armV7_perf_mon_enable(unsigned int n);
    
    

/*********************************
* glabal variable
**********************************/
volatile static struct cpu cpu = {
    .ic_address = GIC_CPU_BASE,   /*  ic_address, flat-mapped GIC CPU I/F address */
    .power_state = STATUS_RUN     /*  power_state, NORMAL(0), STANDBY(1), DORMANT(2), SHUTDOWN(3) */
};

volatile static struct cpu_cluster cpu_cluster = {
    .cpu_type = CPU_A9MPCORE,        /*  cpu_type    */
    .cpu_version = 0x0200,           /*  cpu_version */
    .num_cpus = MAX_CPUS,            /*  num_cpus    */
    .active_cpus = MAX_CPUS,         /*  active_cpus */
    .scu_address = SCU_BASE,         /*  scu_address */
    .ic_address = GIC_DIST_BASE,     /*  ic_address, flat-mapped GIC distributer address */
    .l2_address = PL310_BASE,        /*  l2_address, flat-mapped L2C address */
    .cpu_table = (struct cpu *)&cpu, /*  cpu_table   */
    .power_state = STATUS_RUN        /*  power_state */
};

volatile static int dormant_ret_flag;



/*********************************
* function
**********************************/
typedef struct
{
    volatile unsigned timer_load;                       /* 0x00 */
    volatile unsigned timer_counter;                    /* 0x04 */
    volatile unsigned timer_control;                    /* 0x08 */
    volatile unsigned timer_interrupt_status;           /* 0x0c */
    char padding1[0x10];
    volatile unsigned watchdog_load;                    /* 0x20 */
    volatile unsigned watchdog_counter;                 /* 0x24 */
    volatile unsigned watchdog_control;                 /* 0x28 */
    volatile unsigned watchdog_interrupt_status;        /* 0x2c */
    volatile unsigned watchdog_reset_status;            /* 0x30 */
    volatile unsigned watchdog_disable;                 /* 0x34 */
} a9_timer_registers;

typedef struct
{
    unsigned timer_load;
    unsigned timer_counter;
    unsigned timer_control;
    unsigned timer_interrupt_status;
    unsigned watchdog_load;
    unsigned watchdog_counter;
    unsigned watchdog_control;
    unsigned watchdog_interrupt_status;
} a9_timer_context;

#define A9_TIMERS_OFFSET 0x600

static void save_a9_timers(u32 *pointer, unsigned scu_address)
{
    a9_timer_context *context = (a9_timer_context *)pointer;
    a9_timer_registers *timers = (a9_timer_registers *)(scu_address + A9_TIMERS_OFFSET);

    /*
     * First, stop the timers
     */
    context->timer_control = timers->timer_control;
    timers->timer_control = 0;
    context->watchdog_control = timers->watchdog_control;
    timers->watchdog_control = 0;

    context->timer_load = timers->timer_load;
    context->timer_counter = timers->timer_counter;
    context->timer_interrupt_status = timers->timer_interrupt_status;
    context->watchdog_load = timers->watchdog_load;
    context->watchdog_counter = timers->watchdog_counter;
    context->watchdog_interrupt_status = timers->watchdog_interrupt_status;
    /*
     * We ignore watchdog_reset_status, since it can only clear the status bit.
     * If the watchdog has reset the system, the OS will want to know about it.
     * Similarly, we have no use for watchdog_disable - this is only used for
     * returning to timer mode, which is the default mode after reset.
     */
}

static void restore_a9_timers(u32 *pointer, unsigned scu_address)
{
    a9_timer_context *context = (a9_timer_context *)pointer;
    a9_timer_registers *timers = (a9_timer_registers *)(scu_address + A9_TIMERS_OFFSET);

    timers->timer_control = 0;
    timers->watchdog_control = 0;

    /*
     * We restore the load register first, because it also sets the counter register.
     */
    timers->timer_load = context->timer_load;
    timers->watchdog_load = context->watchdog_load;

    /*
     * If a timer has reached zero (presumably during the context save) and triggered
     * an interrupt, then we set it to the shortest possible expiry time, to make it
     * trigger again real soon.
     * We could fake this up properly, but we would have to wait around until the timer
     * ticked, which could be some time if PERIPHCLK is slow. This approach should be
     * good enough in most cases.
     */
    if (context->timer_interrupt_status) {
        timers->timer_counter = 1;
    } else {
        timers->timer_counter = context->timer_counter;
    }

    if (context->watchdog_interrupt_status) {
        timers->watchdog_counter = 1;
    } else {
        timers->watchdog_counter = context->watchdog_counter;
    }

    timers->timer_control = context->timer_control;
    timers->watchdog_control = context->watchdog_control;
}


struct set_and_clear_regs
{
    volatile unsigned int set[32], clear[32];
};

typedef struct
{
    volatile unsigned int control;                      /* 0x000 */
    const unsigned int controller_type;
    const unsigned int implementer;
    const char padding1[116];
    volatile unsigned int security[32];                 /* 0x080 */
    struct set_and_clear_regs enable;                   /* 0x100 */
    struct set_and_clear_regs pending;                  /* 0x200 */
    volatile const unsigned int active[32];             /* 0x300 */
    const char padding2[128];
    volatile unsigned int priority[256];                /* 0x400 */
    volatile unsigned int target[256];                  /* 0x800 */
    volatile unsigned int configuration[64];            /* 0xC00 */
    const char padding3[512];                           /* 0xD00 */
    volatile unsigned int software_interrupt;           /* 0xF00 */
    const char padding4[220];
    unsigned const int peripheral_id[4];                /* 0xFE0 */
    unsigned const int primecell_id[4];                 /* 0xFF0 */
} interrupt_distributor;

typedef struct
{
    volatile unsigned int control;                      /* 0x00 */
    volatile unsigned int priority_mask;                /* 0x04 */
    volatile unsigned int binary_point;                 /* 0x08 */
    volatile unsigned const int interrupt_ack;          /* 0x0c */
    volatile unsigned int end_of_interrupt;             /* 0x10 */
    volatile unsigned const int running_priority;       /* 0x14 */
    volatile unsigned const int highest_pending;        /* 0x18 */
#if 1 //wschen 2011-12-30
    volatile unsigned int aliased_binary_point;         /* 0x1c */
#endif
} cpu_interface;


/*
 * Saves the GIC CPU interface context
 * Requires 3 words of memory
 */
static void save_gic_interface(u32 *pointer, unsigned gic_interface_address)
{
    cpu_interface *ci = (cpu_interface *)gic_interface_address;

    pointer[0] = ci->control;
    pointer[1] = ci->priority_mask;
    pointer[2] = ci->binary_point;
#if 1 //wschen 2011-12-30
    pointer[3] = ci->aliased_binary_point;
#endif

    /* TODO: add nonsecure stuff */
}

/*
 * Saves this CPU's banked parts of the distributor
 * Returns non-zero if an SGI/PPI interrupt is pending (after saving all required context)
 * Requires 19 words of memory
 */
static void save_gic_distributor_private(u32 *pointer, unsigned gic_distributor_address)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;

    *pointer = id->enable.set[0];
    ++pointer;
    pointer = copy_words(pointer, id->priority, 8);
    pointer = copy_words(pointer, id->target, 8);
#if 1 //wschen 2011-12-30
    *pointer = id->security[0];
    ++pointer;
#endif
    /* Save just the PPI configurations (SGIs are not configurable) */
    *pointer = id->configuration[1];
    ++pointer;
    *pointer = id->pending.set[0];
}

/*
 * Saves the shared parts of the distributor
 * Requires 1 word of memory, plus 20 words for each block of 32 SPIs (max 641 words)
 * Returns non-zero if an SPI interrupt is pending (after saving all required context)
 */
static void save_gic_distributor_shared(u32 *pointer, unsigned gic_distributor_address)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
    unsigned num_spis, *saved_pending;

    /* Calculate how many SPIs the GIC supports */
    num_spis = 32 * (id->controller_type & 0x1f);

    /* TODO: add nonsecure stuff */

    /* Save rest of GIC configuration */
    if (num_spis) {
        pointer = copy_words(pointer, id->enable.set + 1, num_spis / 32);
        pointer = copy_words(pointer, id->priority + 8, num_spis / 4);
        pointer = copy_words(pointer, id->target + 8, num_spis / 4);
        pointer = copy_words(pointer, id->configuration + 2, num_spis / 16);
#if 1 //wschen 2011-12-30
        pointer = copy_words(pointer, id->security + 1, num_spis / 32);
#endif
        saved_pending = pointer;
        pointer = copy_words(pointer, id->pending.set + 1, num_spis / 32);
    }

    /* Save control register */
    *pointer = id->control;
}

static void restore_gic_interface(u32 *pointer, unsigned gic_interface_address)
{
    cpu_interface *ci = (cpu_interface *)gic_interface_address;

    /* TODO: add nonsecure stuff */

    ci->priority_mask = pointer[1];
    ci->binary_point = pointer[2];
#if 1 //wschen 2011-12-30
    ci->aliased_binary_point = pointer[3];
#endif

    /* Restore control register last */
    ci->control = pointer[0];
}

static void restore_gic_distributor_private(u32 *pointer, unsigned gic_distributor_address)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
    unsigned tmp;

    /* First disable the distributor so we can write to its config registers */
    tmp = id->control;
    id->control = 0;

    id->enable.set[0] = *pointer;
    ++pointer;
    copy_words(id->priority, pointer, 8);
    pointer += 8;
    copy_words(id->target, pointer, 8);
    pointer += 8;
#if 1 //wschen 2011-12-30
    id->security[0] = *pointer;
    ++pointer;
#endif
    /* Restore just the PPI configurations (SGIs are not configurable) */
    id->configuration[1] = *pointer;
    ++pointer;
    id->pending.set[0] = *pointer;
    id->control = tmp;
}

static void restore_gic_distributor_shared(u32 *pointer, unsigned gic_distributor_address)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
    unsigned num_spis;

    /* First disable the distributor so we can write to its config registers */
    id->control = 0;

    /* Calculate how many SPIs the GIC supports */
    num_spis = 32 * ((id->controller_type) & 0x1f);

    /* TODO: add nonsecure stuff */

    /* Restore rest of GIC configuration */
    if (num_spis) {
        copy_words(id->enable.set + 1, pointer, num_spis / 32);
        pointer += num_spis / 32;
        copy_words(id->priority + 8, pointer, num_spis / 4);
        pointer += num_spis / 4;
        copy_words(id->target + 8, pointer, num_spis / 4);
        pointer += num_spis / 4;
        copy_words(id->configuration + 2, pointer, num_spis / 16);
        pointer += num_spis / 16;
#if 1 //wschen 2011-12-30
        copy_words(id->security + 1, pointer, num_spis / 32);
        pointer += num_spis / 32;
#endif
        copy_words(id->pending.set + 1, pointer, num_spis / 32);

        if (reg_read(SC_WAKEUP_SRC) & WAKE_SRC_KP) {
            int i, j;
            i = MT6577_KP_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = MT6577_KP_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }

        if (reg_read(SC_WAKEUP_SRC) & WAKE_SRC_MSDC1) {
            int i, j;
            i = MT6577_MSDC1_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = MT6577_MSDC1_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }

#ifdef CONFIG_MTK_SC_DEBUG
        if (reg_read(SC_WAKEUP_SRC) & WAKE_SRC_TS) {
            int i, j;
            i = MT6577_TS_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = MT6577_TS_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }

        if (reg_read(SC_WAKEUP_SRC) & WAKE_SRC_LOW_BAT) {
            int i, j;
            i = MT6577_LOWBATTERY_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = MT6577_LOWBATTERY_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }
#endif //CONFIG_MTK_SC_DEBUG

        pointer += num_spis / 32;
    }

    /* We assume the I and F bits are set in the CPSR so that we will not respond to interrupts! */
    /* Restore control register */
    id->control = *pointer;
}

typedef struct
{
    volatile unsigned int control;              /* 0x00 */
    const unsigned int configuration;           /* 0x04 */
    union
    {
        volatile unsigned int w;
        volatile unsigned char b[4];
    } power_status;                             /* 0x08 */
    volatile unsigned int invalidate_all;       /* 0x0c */
    char padding1[48];
    volatile unsigned int filtering_start;      /* 0x40 */
    volatile unsigned int filtering_end;        /* 0x44 */
    char padding2[8];
    volatile unsigned int access_control;       /* 0x50 */
    volatile unsigned int ns_access_control;    /* 0x54 */
} a9_scu_registers;

static void save_a9_scu(u32 *pointer, unsigned scu_address)
{
    a9_scu_registers *scu = (a9_scu_registers *)scu_address;

    pointer[0] = scu->control;
    pointer[1] = scu->power_status.w;
    pointer[2] = scu->filtering_start;
    pointer[3] = scu->filtering_end;
    pointer[4] = scu->access_control;
    pointer[5] = scu->ns_access_control;
}

static void restore_a9_scu(u32 *pointer, unsigned scu_address)
{
    a9_scu_registers *scu = (a9_scu_registers *)scu_address;

    scu->invalidate_all = 0xffff;
    scu->filtering_start = pointer[2];
    scu->filtering_end = pointer[3];
    scu->access_control = pointer[4];
    scu->ns_access_control = pointer[5];
    scu->power_status.w = pointer[1];
    scu->control = pointer[0];
    dsb();
}


struct lockdown_regs
{
    unsigned int d, i;
};

typedef struct
{
    const unsigned cache_id;                    /* 0x000 */
    const unsigned cache_type;                  /* 0x004 */
    char padding1[0x0F8];
    volatile unsigned control;                  /* 0x100 */
    volatile unsigned aux_control;              /* 0x104 */
    volatile unsigned tag_ram_control;          /* 0x108 */
    volatile unsigned data_ram_control;         /* 0x10C */
    char padding2[0x0F0];
    volatile unsigned ev_counter_ctrl;          /* 0x200 */
    volatile unsigned ev_counter1_cfg;          /* 0x204 */
    volatile unsigned ev_counter0_cfg;          /* 0x208 */
    volatile unsigned ev_counter1;              /* 0x20C */
    volatile unsigned ev_counter0;              /* 0x210 */
    volatile unsigned int_mask;                 /* 0x214 */
    const volatile unsigned int_mask_status;    /* 0x218 */
    const volatile unsigned int_raw_status;     /* 0x21C */
    volatile unsigned int_clear;                /* 0x220 */
    char padding3[0x50C];
    volatile unsigned cache_sync;               /* 0x730 */
    char padding4[0x03C];
    volatile unsigned inv_pa;                   /* 0x770 */
    char padding5[0x008];
    volatile unsigned inv_way;                  /* 0x77C */
    char padding6[0x030];
    volatile unsigned clean_pa;                 /* 0x7B0 */
    char padding7[0x004];
    volatile unsigned clean_index;              /* 0x7B8 */
    volatile unsigned clean_way;                /* 0x7BC */
    char padding8[0x030];
    volatile unsigned clean_inv_pa;             /* 0x7F0 */
    char padding9[0x004];
    volatile unsigned clean_inv_index;          /* 0x7F8 */
    volatile unsigned clean_inv_way;            /* 0x7FC */
    char paddinga[0x100];
    volatile struct lockdown_regs lockdown[8];  /* 0x900 */
    char paddingb[0x010];
    volatile unsigned lock_line_en;             /* 0x950 */
    volatile unsigned unlock_way;               /* 0x954 */
    char paddingc[0x2A8];
    volatile unsigned addr_filtering_start;     /* 0xC00 */
    volatile unsigned addr_filtering_end;       /* 0xC04 */
    char paddingd[0x338];
    volatile unsigned debug_ctrl;               /* 0xF40 */
    char paddinge[0x01C];
    volatile unsigned prefetch_ctrl;            /* 0xF60 */
    char paddingf[0x01C];
    volatile unsigned power_ctrl;               /* 0xF80 */
} pl310_registers;


typedef struct
{
    unsigned int aux_control;
    unsigned int tag_ram_control;
    unsigned int data_ram_control;
    unsigned int ev_counter_ctrl;
    unsigned int ev_counter1_cfg;
    unsigned int ev_counter0_cfg;
    unsigned int ev_counter1;
    unsigned int ev_counter0;
    unsigned int int_mask;
    unsigned int lock_line_en;
    struct lockdown_regs lockdown[8];
    unsigned int unlock_way;
    unsigned int addr_filtering_start;
    unsigned int addr_filtering_end;
    unsigned int debug_ctrl;
    unsigned int prefetch_ctrl;
    unsigned int power_ctrl;
} pl310_context;


static void save_pl310(u32 *pointer, unsigned pl310_address)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    pl310_context *context = (pl310_context *)pointer;

    /* TODO: are all these registers are present in earlier PL310 versions? */
    context->aux_control = pl310->aux_control;
    context->tag_ram_control = pl310->tag_ram_control;
    context->data_ram_control = pl310->data_ram_control;
    context->ev_counter_ctrl = pl310->ev_counter_ctrl;
    context->ev_counter1_cfg = pl310->ev_counter1_cfg;
    context->ev_counter0_cfg = pl310->ev_counter0_cfg;
    context->ev_counter1 = pl310->ev_counter1;
    context->ev_counter0 = pl310->ev_counter0;
    context->int_mask = pl310->int_mask;
    context->lock_line_en = pl310->lock_line_en;

    copy_words((unsigned *)context->lockdown, (unsigned *)pl310->lockdown, 16);

    context->addr_filtering_start = pl310->addr_filtering_start;
    context->addr_filtering_end = pl310->addr_filtering_end;
    context->debug_ctrl = pl310->debug_ctrl;
    context->prefetch_ctrl = pl310->prefetch_ctrl;
    context->power_ctrl = pl310->power_ctrl;
}

static void inv_pl310(unsigned pl310_address)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    int i;

    pl310->inv_way = 0xffff;
    dsb();
    while (pl310->inv_way) {
        /* Spin */
        for (i = 10; i > 0; --i) {
            nop();
        }
    }
    dsb();

    pl310->cache_sync = 0;
    dsb();

    while (pl310->cache_sync & C_BIT) {
        for (i = 10; i > 0; --i) {
            nop();
        }
    }

    dmb();
}

/*********************************
* 20120505 marc.huang
* mark inv_range_pl310() to remove build warning
**********************************/
//#define CACHE_LINE_SIZE 32
//static void inv_range_pl310(unsigned start, unsigned size, unsigned pl310_address)
//{
//
//    pl310_registers *pl310 = (pl310_registers *)pl310_address;
//    unsigned addr;
//    int i;
//
//    addr = start & ~(CACHE_LINE_SIZE - 1);
//
//    while (addr <= (size + start)) {
//        pl310->inv_pa = addr;
//        addr += CACHE_LINE_SIZE;
//
//        while (pl310->inv_pa & C_BIT) {
//            for (i = 10; i > 0; --i) {
//                nop();
//            }
//        }
//    }
//    dsb();
//
//    pl310->cache_sync = 0;
//    dsb();
//
//    while (pl310->cache_sync & C_BIT) {
//        for (i = 10; i > 0; --i) {
//            nop();
//        }
//    }
//
//    dmb();
//}

static void restore_pl310(u32 *pointer, unsigned pl310_address, int dormant)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    pl310_context *context = (pl310_context *)pointer;
    int i;

    /* We may need to disable the PL310 if the boot code has turned it on */
    if (pl310->control) {
        /* Wait for the cache to be idle, then disable */
        pl310->cache_sync = 0;
        while (pl310->cache_sync & C_BIT) {
            for (i = 10; i > 0; --i) {
                nop();
            }
        }
        pl310->control = 0;
        dsb();
    }

    /* TODO: are all these registers present in earlier PL310 versions? */
    pl310->aux_control = context->aux_control;
    pl310->tag_ram_control = context->tag_ram_control;
    pl310->data_ram_control = context->data_ram_control;
    pl310->ev_counter_ctrl = context->ev_counter_ctrl;
    pl310->ev_counter1_cfg = context->ev_counter1_cfg;
    pl310->ev_counter0_cfg = context->ev_counter0_cfg;
    pl310->ev_counter1 = context->ev_counter1;
    pl310->ev_counter0 = context->ev_counter0;
    pl310->int_mask = context->int_mask;
    pl310->lock_line_en = context->lock_line_en;

    copy_words((unsigned *)pl310->lockdown, (unsigned *)context->lockdown, 16);

    pl310->addr_filtering_start = context->addr_filtering_start;
    pl310->addr_filtering_end = context->addr_filtering_end;
    pl310->debug_ctrl = context->debug_ctrl;
    pl310->prefetch_ctrl = context->prefetch_ctrl;
    pl310->power_ctrl = context->power_ctrl;
    dsb();

    /*
     * If the RAMs were powered off, we need to invalidate the cache
     */
    if (!dormant) {
        inv_pl310(pl310_address);
    }
}


static void clean_pl310(unsigned pl310_address)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    int i;

    pl310->clean_way = 0xffff;
    dsb();
    while (pl310->clean_way) {
        /* Spin */
        for (i = 10; i > 0; --i) {
            nop();
        }
    }
    dsb();

    pl310->cache_sync = 0;
    dsb();

    while (pl310->cache_sync & C_BIT) {
        for (i = 10; i > 0; --i) {
            nop();
        }
    }

    dmb();
}

/*********************************
* 20120505 marc.huang
* mark disable_pl310() to remove build warning
**********************************/
//static void disable_pl310(unsigned pl310_address)
//{
//    pl310_registers *pl310 = (pl310_registers *)pl310_address;
//    int i;
//
//    if (pl310->control) {
//        /* Wait for the cache to be idle, then disable */
//        pl310->cache_sync = 0;
//        dsb();
//        while (pl310->cache_sync & C_BIT) {
//            for (i = 10; i > 0; --i) {
//                nop();
//            }
//        }
//        pl310->control = 0;
//        dsb();
//    }
//}


static void enable_pl310(unsigned pl310_address)
{
    pl310_registers *pl310 = (pl310_registers *)pl310_address;
    int i;

    pl310->control = 1;
    dsb();

    while (!pl310->control) {
      for (i = 10; i > 0; --i) {
          nop();
      }
    }
    dsb();
}

/**
 * This function saves all the context that will be lost
 * when a CPU and cluster enter a low power state.
 */
static void platform_save_context(struct cpu_cluster *cluster, struct cpu *pcpu)
{
    struct cpu_context *context = &(pcpu->context);
    struct cpu_cluster_context *cluster_context = &(cluster->context);
#if MAX_CPUS != 1
    int cluster_down = 0;
#endif

    /* Save perf. monitors first, so we don't interfere too much with counts */
    save_performance_monitors(context->pmu_data);

    save_a9_timers(context->timer_data, cluster->scu_address);

    save_vfp(context->vfp_data);

    save_gic_interface(context->gic_interface_data, pcpu->ic_address);
    save_gic_distributor_private(context->gic_dist_private_data, cluster->ic_address);

    save_cp15(context->cp15_data);
    save_a9_other(context->other_data);

#if 0 //wschen 2011-07-28
    save_a9_debug(context->debug_data);
#endif

#if MAX_CPUS != 1
    if (cluster->power_state >= STATUS_DORMANT) {
        cluster_down = 1;
    }
#endif

#if MAX_CPUS != 1
    if (cluster_down) {
#endif
        save_a9_global_timer(cluster_context->global_timer_data, cluster->scu_address);

        save_gic_distributor_shared(cluster_context->gic_dist_shared_data, cluster->ic_address);
#if MAX_CPUS != 1
    }
#endif

    save_control_registers(context);
    save_mmu(context->mmu_data);

#if MAX_CPUS != 1
    if (cluster_down) {
#endif
        save_a9_scu(cluster_context->scu_data, cluster->scu_address);

        save_pl310(cluster_context->l2_data, cluster->l2_address);
#if MAX_CPUS != 1
    }
#endif

    dormant_ret_flag = 0;

    save_banked_registers(context->banked_registers);

    if (dormant_ret_flag == 0) {
        clean_dcache_v7_l1();
        clean_pl310(cpu_cluster.l2_address);
    }
}

/**
 * This function restores all the context that was lost
 * when a CPU and cluster entered a low power state. It is called shortly after
 * reset, with the MMU and data cache off.
 *
 */
static void platform_restore_context(struct cpu_cluster *cluster, struct cpu *pcpu)
{
    struct cpu_context *context;
    struct cpu_cluster_context *cluster_context;
#if MAX_CPUS != 1
    int cluster_init;
#endif

    /*
     * At this point we may not write to any data, and we may
     * only read the data that we explicitly cleaned from the L2 above.
     */
    cluster_context = &(cluster->context);
    context = &(pcpu->context);

    restore_cp15(context->cp15_data);

    /* Should we initialize the cluster: are we the first CPU back on, and has the cluster been off? */
#if MAX_CPUS != 1
    cluster_init = (cluster->active_cpus == 0 && cluster->power_state >= STATUS_DORMANT);
#endif


    /* First set up the SCU & L2, if necessary */
#if MAX_CPUS != 1
    if (cluster_init) {
#endif
        restore_a9_scu(cluster_context->scu_data, IO_VIRT_TO_PHYS(cluster->scu_address));

        restore_pl310(cluster_context->l2_data, IO_VIRT_TO_PHYS(cluster->l2_address), cluster->power_state == STATUS_DORMANT);
#if MAX_CPUS != 1
    }
#endif


    /* Next get the MMU back on */
    restore_mmu(context->mmu_data);


    if (cluster->power_state == STATUS_SHUTDOWN) {
        invalidate_icache_v7();
        invalidate_dcache_v7();
    }

    restore_control_registers(context);

    //now MMU is restored, welcome to virtual world
    isb();
    dsb();


    enable_pl310(cpu_cluster.l2_address);
    enable_cache_v7_l1();

   /*
     * MMU and L1 and L2 caches are on, we may now read/write any data.
     * Now we need to restore the rest of this CPU's context
     */

#if 0 //wschen 2011-07-28
    /* Get the debug registers restored, so we can debug most of the code sensibly! */
    restore_a9_debug(cpu.context.debug_data);
#endif

    /* Restore shared items if necessary */
#if MAX_CPUS != 1
    if ((cpu_cluster.active_cpus == 0) && (cpu_cluster.power_state >= STATUS_DORMANT)) {
#endif
        restore_gic_distributor_shared((u32 *)cpu_cluster.context.gic_dist_shared_data, cpu_cluster.ic_address);
        restore_a9_global_timer((u32 *)cpu_cluster.context.global_timer_data, cpu_cluster.scu_address);
#if MAX_CPUS != 1
    }
#endif

    restore_gic_distributor_private((u32 *)cpu.context.gic_dist_private_data, cpu_cluster.ic_address);
    restore_gic_interface((u32 *)cpu.context.gic_interface_data, cpu.ic_address);

    restore_a9_other((u32 *)cpu.context.other_data);

    restore_vfp((u32 *)cpu.context.vfp_data);

    restore_a9_timers((u32 *)cpu.context.timer_data, cpu_cluster.scu_address);

    restore_performance_monitors((u32 *)cpu.context.pmu_data);

    dormant_ret_flag = 1;

    restore_banked_registers((u32 *)cpu.context.banked_registers);
}


int cpu_power_down(int mode)
{
    void (*restore_ptr)(void);
    restore_ptr = cpu_wake_up;
    reg_write(BOOTROM_BOOT_ADDR, __pa(restore_ptr));
    
#if MAX_CPUS != 1
    cpu_cluster.active_cpus--;
#endif

    cpu_cluster.cpu_table->power_state = mode;

#if MAX_CPUS != 1
    if (cpu_cluster.active_cpus == 0) {
        cpu_cluster.power_state = mode;
    }
#else
    cpu_cluster.power_state = mode;
#endif


    platform_save_context((struct cpu_cluster *)&cpu_cluster, (struct cpu *)cpu_cluster.cpu_table);

    return dormant_ret_flag;
}

void cpu_start_restore(void)
{
    struct cpu_cluster *cc = (struct cpu_cluster *)__pa(&cpu_cluster);
    struct cpu *cp = (struct cpu *)__pa(&cpu);

#if 0 //wschen 2011-08-12
    extern int armV7_perf_mon_is_overflow(unsigned int n);
    extern unsigned int armV7_perf_mon_get_cyc_cnt(void);
    extern void armV7_perf_mon_reset(void);
    extern void armV7_perf_mon_enable(unsigned int n);

    armV7_perf_mon_enable(0);
    armV7_perf_mon_reset();
    armV7_perf_mon_enable(1);
#endif

    platform_restore_context(cc, cp);
}

void cpu_dormant_init(void)
{
    //set Boot ROM power-down control to power down
    reg_write(BOOTROM_PWR_CTRL, reg_read(BOOTROM_PWR_CTRL) | 0x1);
}

void cpu_dormant_abort(void)
{
    restore_a9_timers((u32 *)cpu.context.timer_data, cpu_cluster.scu_address);

#if MAX_CPUS != 1
    if ((cpu_cluster.active_cpus == 0) && (cpu_cluster.power_state >= STATUS_DORMANT)) {
#endif
        restore_a9_global_timer((u32 *)cpu_cluster.context.global_timer_data, cpu_cluster.scu_address);
#if MAX_CPUS != 1
    }
#endif


#if MAX_CPUS != 1
    cpu_cluster.active_cpus++;
#endif
    cpu_cluster.power_state = STATUS_RUN;
    cpu_cluster.cpu_table->power_state = STATUS_RUN;

    restore_performance_monitors((u32 *)cpu.context.pmu_data);
}


MODULE_AUTHOR("Wan-Ching Huang <marc.huang@mediatek.com>");
MODULE_DESCRIPTION("MT6577 Dormant/Shutdown Mode Driver $Revision: #1 $");

