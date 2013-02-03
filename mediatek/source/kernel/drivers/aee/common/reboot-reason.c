#include <linux/aee.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

#define RR_PROC_NAME "reboot-reason"

static struct proc_dir_entry *aee_rr_file;

struct last_reboot_reason aee_rr_last_rec;

#define WDT_NORMAL_BOOT 0
#define WDT_HW_REBOOT 1
#define WDT_SW_REBOOT 2

#ifdef CONFIG_LOCAL_WDT
enum wk_wdt_type {
        WK_WDT_LOC_TYPE,
        WK_WDT_EXT_TYPE
};
extern void mpcore_wk_wdt_stop(void);
extern void mpcore_wdt_restart(enum wk_wdt_type type);
extern void mtk_wdt_restart(enum wk_wdt_type type);
#else
extern void mtk_wdt_restart(void);
#endif

void aee_rr_last(struct last_reboot_reason *lrr)
{
	memcpy(&aee_rr_last_rec, lrr, sizeof(struct last_reboot_reason));
}

static ssize_t aee_rr_read_reboot_reason(char *page, char **start, off_t off, 
					 int count,int *eof, void *data)
{
	return sprintf(page, "WDT status: %d\nshutdown mode: %u\njiffies (current, wdk-kick, idle, in-idle-count): %lu, %lu, %lu, %lu\n"
		       "last irq enter  [%d, %llu], [%d, %llu]\n" 
		       "last irq exit   [%d, %llu], [%d, %llu]\n" 
		       "last sched      [%llu], [%llu]\n",
		       aee_rr_last_rec.wdt_status, aee_rr_last_rec.shutdown_mode,
		       (unsigned long) aee_rr_last_rec.jiffies_current, 
		       (unsigned long) aee_rr_last_rec.jiffies_wdk_kick, 
		       (unsigned long) aee_rr_last_rec.jiffies_idle, (unsigned long) aee_rr_last_rec.in_idle,
		       aee_rr_last_rec.last_irq_enter[0], aee_rr_last_rec.jiffies_last_irq_enter[0], aee_rr_last_rec.last_irq_enter[1], aee_rr_last_rec.jiffies_last_irq_enter[1],
		       aee_rr_last_rec.last_irq_exit[0], aee_rr_last_rec.jiffies_last_irq_exit[0], aee_rr_last_rec.last_irq_exit[1], aee_rr_last_rec.jiffies_last_irq_exit[1],
		       aee_rr_last_rec.jiffies_last_sched[0], aee_rr_last_rec.jiffies_last_sched[1]) + 1;
}

void aee_rr_proc_init(struct proc_dir_entry *aed_proc_dir)
{
	aee_rr_file = create_proc_read_entry(RR_PROC_NAME, 
					     0444, aed_proc_dir, 
					     aee_rr_read_reboot_reason,
					     NULL);
	if (aee_rr_file == NULL) {
		printk(KERN_ERR "%s: Can't create rr proc entry\n", __func__);
	}
}
EXPORT_SYMBOL(aee_rr_proc_init);

void aee_rr_proc_done(struct proc_dir_entry *aed_proc_dir)
{
	remove_proc_entry(RR_PROC_NAME, aed_proc_dir);
}
EXPORT_SYMBOL(aee_rr_proc_done);

void aee_stop_nested_panic(struct pt_regs *regs)
{
	struct thread_info *thread = current_thread_info();

#ifdef CONFIG_LOCAL_WDT
	/* stop the local WDT and restart the global WDT */
	mpcore_wk_wdt_stop();
	mtk_wdt_restart(WK_WDT_EXT_TYPE);
#else
	/* restart the WDT */
	mtk_wdt_restart();
#endif

	local_irq_disable();
	preempt_disable();

	aee_sram_printk("Nested panic\n");

	aee_sram_printk("Log for the previous panic:\n");
	aee_sram_printk("pc: %08lx lr: %08lx psr: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_pc, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_lr, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_cpsr);
	aee_sram_printk("sp: %08lx ip: %08lx fp: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_sp, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_ip, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_fp);
	aee_sram_printk("r10: %08lx r9: %08lx r8: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_r10, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r9, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r8);
	aee_sram_printk("r7: %08lx r6: %08lx r5: %08lx r4: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_r7, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r6, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r5, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r4);
	aee_sram_printk("r3: %08lx r2: %08lx r1: %08lx r0: %08lx\n",
			((struct pt_regs *)thread->regs_on_excp)->ARM_r3, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r2, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r1, 
			((struct pt_regs *)thread->regs_on_excp)->ARM_r0);

	aee_sram_printk("Log for the current panic:\n");
	aee_sram_printk("pc: %08lx lr: %08lx psr: %08lx\n",
			regs->ARM_pc, 
			regs->ARM_lr, 
			regs->ARM_cpsr);
	aee_sram_printk("sp: %08lx ip: %08lx fp: %08lx\n",
			regs->ARM_sp, 
			regs->ARM_ip, 
			regs->ARM_fp);
	aee_sram_printk("r10: %08lx r9: %08lx r8: %08lx\n",
			regs->ARM_r10, 
			regs->ARM_r9, 
			regs->ARM_r8);
	aee_sram_printk("r7: %08lx r6: %08lx r5: %08lx r4: %08lx\n",
			regs->ARM_r7, 
			regs->ARM_r6, 
			regs->ARM_r5, 
			regs->ARM_r4);
	aee_sram_printk("r3: %08lx r2: %08lx r1: %08lx r0: %08lx\n",
			regs->ARM_r3, 
			regs->ARM_r2, 
			regs->ARM_r1, 
			regs->ARM_r0);

	printk("Nested panic\n");

	printk("Log for the previous panic:\n");
	__show_regs((struct pt_regs *)thread->regs_on_excp);

	printk("Log for the current panic:\n");
	__show_regs(regs);

	/* waiting for the WDT timeout */
	while (1);
}

