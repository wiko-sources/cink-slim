#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/kallsyms.h>
#include <linux/utsname.h>
#include <asm/uaccess.h>
#include "prof_ctl.h"
#include "prof_mem.h"
#include <linux/module.h>
#include <linux/pid.h>

#include <linux/irq.h>
#include <linux/interrupt.h>

#include <linux/mt_sched_mon.h>
#include <linux/stacktrace.h>
#ifdef CONFIG_MTK_SCHED_TRACERS
#include <trace/events/sched.h>
#include <linux/mt_trace.h>
#endif

#include <linux/aee.h>


#define WARN_ISR_DUR     3000000
#define WARN_SOFTIRQ_DUR 10000000
#define WARN_TASKLET_DUR 10000000
#define WARN_HRTIMER_DUR 3000000
#define WARN_STIMER_DUR  10000000

enum mt_event_type {
    evt_ISR = 1,
    evt_SOFTIRQ = 2,
    evt_TASKLET,
    evt_HRTIMER,
    evt_STIMER,
};

DEFINE_PER_CPU(struct sched_block_event, ISR_mon);
DEFINE_PER_CPU(struct sched_block_event, SoftIRQ_mon);
DEFINE_PER_CPU(struct sched_block_event, tasklet_mon);
DEFINE_PER_CPU(struct sched_block_event, hrt_mon);
DEFINE_PER_CPU(struct sched_block_event, sft_mon);
DEFINE_PER_CPU(struct sched_stop_event, IRQ_disable_mon);
DEFINE_PER_CPU(struct sched_stop_event, Preempt_disable_mon);
DEFINE_PER_CPU(int, mt_timer_irq);
DEFINE_PER_CPU(int, mtsched_mon_enabled);

//TIMER debug
DEFINE_PER_CPU(int, mtsched_mon_enabled);
DEFINE_PER_CPU(unsigned long long, local_timer_ts);
DEFINE_PER_CPU(unsigned long long, local_timer_te);
DEFINE_PER_CPU(unsigned long long, timer_ack_ts);

void mt_sched_monitor_switch(int on);
static DEFINE_MUTEX(mt_sched_mon_lock);
#define SEQ_printf(m, x...)	    \
 do {			    \
    if (m)		    \
	seq_printf(m, x);	\
    else		    \
	printk(x);	    \
 } while (0)

#define MT_DEBUG_ENTRY(name) \
static int mt_##name##_show(struct seq_file *m, void *v);\
static int mt_##name##_write(struct file *filp, const char *ubuf, size_t cnt, loff_t *data);\
static int mt_##name##_open(struct inode *inode, struct file *file) \
{ \
    return single_open(file, mt_##name##_show, inode->i_private); \
} \
\
static const struct file_operations mt_##name##_fops = { \
    .open = mt_##name##_open, \
    .write = mt_##name##_write,\
    .read = seq_read, \
    .llseek = seq_lseek, \
    .release = single_release, \
};\
void mt_##name##_switch(int on);

/*
 * Ease the printing of nsec fields:
 */
static long long nsec_high(unsigned long long nsec)
{
    if ((long long)nsec < 0) {
	nsec = -nsec;
	do_div(nsec, 1000000);
	return -nsec;
    }
    do_div(nsec, 1000000);

    return nsec;
}

static unsigned long nsec_low(unsigned long long nsec)
{
    if ((long long)nsec < 0)
	nsec = -nsec;

    return do_div(nsec, 1000000);
}
#define SPLIT_NS(x) nsec_high(x), nsec_low(x)

#ifdef CONFIG_MT_SCHED_MONITOR
static long long usec_high(unsigned long long usec)
{
    if ((long long)usec < 0) {
    usec = -usec;
    do_div(usec, 1000);
    return -usec;
    }
    do_div(usec, 1000);

    return usec;
}

/* --------------------------------------------------- */
/* Real work */
static const char* isr_name(int irq)
{
    struct irqaction *action;
    struct irq_desc *desc;

    desc = irq_to_desc(irq);
    if(!desc)
        return NULL;
    else{
        action = desc->action;
        if(!action)
            return NULL;
        else
            return action->name;
    }
}
static void event_duration_check(struct sched_block_event *b)
{
    unsigned long long t_dur;
    t_dur = b->last_te - b->last_ts;
    switch(b->type){
        case evt_ISR:
            if(t_dur > WARN_ISR_DUR){
                printk("[ISR DURATION WARN] IRQ[%d:%s], dur:%llu ns > %d ms,(s:%llu,e:%llu)\n", (int)b->last_event, isr_name(b->last_event), t_dur, WARN_ISR_DUR/1000000, b->last_ts, b->last_te);
            }
            break;
        case evt_SOFTIRQ:
            if(t_dur > WARN_SOFTIRQ_DUR){
                struct sched_block_event *b_isr;
                b_isr = & __raw_get_cpu_var(ISR_mon); 
                printk("[SOFTIRQ DURATION WARN] SoftIRQ:%d, dur:%llu ns > %d ms,(s:%llu,e:%llu)\n", (int)b->last_event, t_dur, WARN_SOFTIRQ_DUR/1000000, b->last_ts, b->last_te);
                if(b_isr->last_ts > b->last_ts) // ISR occur during Tasklet
                {
                    printk(" IRQ occurrs in this duration, IRQ[%d:%s], dur:%llu ns (s:%llu, e:%llu)\n\n",(int)b_isr->last_event, isr_name(b_isr->last_event), b_isr->last_te - b_isr->last_ts ,b_isr->last_ts, b_isr->last_te);     
                }
            }
            break;
        case evt_TASKLET:
            if(t_dur > WARN_TASKLET_DUR){
                struct sched_block_event *b_isr;
                b_isr = & __raw_get_cpu_var(ISR_mon); 
                printk("[TASKLET DURATION WARN] Tasklet:%pS, dur:%llu ns > %d ms,(s:%llu,e:%llu)\n", (void *)b->last_event, t_dur, WARN_TASKLET_DUR/1000000, b->last_ts, b->last_te);
                if(b_isr->last_ts > b->last_ts) // ISR occur during Tasklet
                {
                    printk(" IRQ occurrs in this duration, IRQ[%d:%s], dur:%llu ns (s:%llu, e:%llu)\n\n",(int)b_isr->last_event, isr_name(b_isr->last_event), b_isr->last_te - b_isr->last_ts ,b_isr->last_ts, b_isr->last_te);     
                }
            }
            break;
        case evt_HRTIMER:
            if(t_dur > WARN_HRTIMER_DUR){
                printk("[HRTIMER DURATION WARN] HRTIMER:%pS, dur:%llu ns > %d ms,(s:%llu,e:%llu)\n", (void *)b->last_event, t_dur, WARN_HRTIMER_DUR/1000000, b->last_ts, b->last_te);
            }
            break;
        case evt_STIMER:
            if(t_dur > WARN_STIMER_DUR){
                struct sched_block_event *b_isr;
                b_isr = & __raw_get_cpu_var(ISR_mon); 
                printk("[STIMER DURATION WARN] SoftTIMER:%pS, dur:%llu ns > %d ms,(s:%llu,e:%llu)\n", (void *)b->last_event, t_dur, WARN_STIMER_DUR/1000000, b->last_ts, b->last_te);
                if(b_isr->last_ts > b->last_ts) // ISR occur during Softtimer
                {
                    printk(" IRQ occurrs in this duration, IRQ[%d:%s], dur:%llu ns (s:%llu, e:%llu)\n\n",(int)b_isr->last_event, isr_name(b_isr->last_event), b_isr->last_te - b_isr->last_ts ,b_isr->last_ts, b_isr->last_te);     
                }
            }
            break;

    }
}
static void reset_event_count(struct sched_block_event *b)
{
    b->last_count = b->cur_count;
    b->cur_count = 0;
}
// ISR monitor
void mt_trace_ISR_start(int irq)
{
    struct sched_block_event *b;
#ifdef CONFIG_MTK_SCHED_TRACERS
	struct task_struct *tsk= __raw_get_cpu_var(mtk_next_task);
    if (unlikely(!sched_stopped))
        trace_int_switch(tsk, irq, 1);
#endif
    b = & __raw_get_cpu_var(ISR_mon);

    b->cur_ts = sched_clock();
    b->cur_event = (unsigned long)irq;
    aee_rr_rec_last_irq_enter(smp_processor_id(), irq, b->cur_ts);
 
}
void mt_trace_ISR_end(int irq)
{
    struct sched_block_event *b;
#ifdef CONFIG_MTK_SCHED_TRACERS
	struct task_struct *tsk= __raw_get_cpu_var(mtk_next_task);
    if (unlikely(!sched_stopped))
        trace_int_switch(tsk, irq, 0);
#endif
    b = & __raw_get_cpu_var(ISR_mon);

    WARN_ON(b->cur_event != irq);
    b->last_event = b->cur_event;
    b->last_ts = b->cur_ts;
    b->last_te = sched_clock();
    b->cur_event = 0;
    b->cur_ts = 0;
    event_duration_check(b);

    //reset HRTimer function counter
    b = & __raw_get_cpu_var(hrt_mon);
    reset_event_count(b);
    aee_rr_rec_last_irq_exit(smp_processor_id(), irq, b->last_te);

}

// SoftIRQ monitor
void mt_trace_SoftIRQ_start(int sq_num)
{

    struct sched_block_event *b;
    b = & __raw_get_cpu_var(SoftIRQ_mon);

    b->cur_ts = sched_clock();
    b->cur_event = (unsigned long)sq_num;
}
void mt_trace_SoftIRQ_end(int sq_num)
{
    struct sched_block_event *b;
    b = & __raw_get_cpu_var(SoftIRQ_mon);

    WARN_ON(b->cur_event != sq_num);
    b->last_event = b->cur_event;
    b->last_ts = b->cur_ts;
    b->last_te = sched_clock();
    b->cur_event = 0;
    b->cur_ts = 0;
    event_duration_check(b);

    //reset soft timer function counter
    b = & __raw_get_cpu_var(sft_mon);
    reset_event_count(b);
    //reset tasklet function counter
    b = & __raw_get_cpu_var(tasklet_mon);
    reset_event_count(b);
}

// Tasklet monitor
void mt_trace_tasklet_start(void *func)
{
    struct sched_block_event *b;
    b = & __raw_get_cpu_var(tasklet_mon);

    b->cur_ts = sched_clock();
    b->cur_event = (unsigned long)func;
    b->cur_count++;
}
void mt_trace_tasklet_end(void *func)
{
    struct sched_block_event *b;
    b = & __raw_get_cpu_var(tasklet_mon);

    WARN_ON(b->cur_event != (unsigned long)func);
    b->last_event = b->cur_event;
    b->last_ts = b->cur_ts;
    b->last_te = sched_clock();
    b->cur_event = 0;
    b->cur_ts = 0;
    event_duration_check(b);
}
// HRTimer monitor
void mt_trace_hrt_start(void *func)
{
    struct sched_block_event *b;
    b = & __raw_get_cpu_var(hrt_mon);

    b->cur_ts = sched_clock();
    b->cur_event = (unsigned long)func;
    b->cur_count++;
}
void mt_trace_hrt_end(void *func)
{
    struct sched_block_event *b;
    b = & __raw_get_cpu_var(hrt_mon);

    WARN_ON(b->cur_event != (unsigned long)func);
    b->last_event = b->cur_event;
    b->last_ts = b->cur_ts;
    b->last_te = sched_clock();
    b->cur_event = 0;
    b->cur_ts = 0;
    event_duration_check(b);
}
// SoftTimer monitor
void mt_trace_sft_start(void *func)
{
    struct sched_block_event *b;
    b = & __raw_get_cpu_var(sft_mon);

    b->cur_ts = sched_clock();
    b->cur_event = (unsigned long)func;
    b->cur_count++;
}
void mt_trace_sft_end(void *func)
{
    struct sched_block_event *b;
    b = & __raw_get_cpu_var(sft_mon);

    WARN_ON(b->cur_event != (unsigned long)func);
    b->last_event = b->cur_event;
    b->last_ts = b->cur_ts;
    b->last_te = sched_clock();
    b->cur_event = 0;
    b->cur_ts = 0;
    event_duration_check(b);
}
// Preempt off monitor
void MT_trace_preempt_off(void)
{
    struct sched_stop_event *e;
    e = & __raw_get_cpu_var(Preempt_disable_mon);

    e->cur_ts = sched_clock();
}
void MT_trace_preempt_on(void)
{
    struct sched_stop_event *e;
    e = & __raw_get_cpu_var(Preempt_disable_mon);
    e->last_ts = e->cur_ts;
    e->cur_ts = 0 ;
    e->last_te = sched_clock();

}
// IRQ off monitor
void MT_trace_irq_off(void)
{
    struct sched_stop_event *e;
    e = & __raw_get_cpu_var(IRQ_disable_mon);

    e->cur_ts = sched_clock();
}
void MT_trace_irq_on(void)
{
    struct sched_stop_event *e;
    e = & __raw_get_cpu_var(IRQ_disable_mon);
    e->last_ts = e->cur_ts;
    e->cur_ts = 0 ;
    e->last_te = sched_clock();

}
void mt_show_last_irq_counts(void);
void mt_show_current_irq_counts(void);
void mt_show_timer_info(void);
void mt_save_irq_counts(void);
void mt_dump_sched_traces(void)
{
    struct sched_block_event *b_isr, *b_sq, *b_tk, *b_hrt, *b_sft;
    struct sched_stop_event *e_irq, *e_pmpt;
    int cpu;
    for_each_possible_cpu(cpu){
    
    b_isr = & per_cpu(ISR_mon, cpu);
    b_sq = & per_cpu(SoftIRQ_mon, cpu);
    b_tk = & per_cpu(tasklet_mon, cpu);
    b_hrt = & per_cpu(hrt_mon, cpu);
    b_sft = & per_cpu(sft_mon, cpu);

    e_pmpt = & per_cpu(Preempt_disable_mon, cpu);
    e_irq = & per_cpu(IRQ_disable_mon, cpu);
    printk("===CPU[%d]==\n", cpu);
    b_isr->cur_event == 0 ?
    printk("[ISR]last_irq:%d, dur:%llu ns (s:%llu, e:%llu)\n\n",
        (int)b_isr->last_event, b_isr->last_te - b_isr->last_ts ,b_isr->last_ts, b_isr->last_te):
    printk("[In ISR]Current irq:%d, Start:%llu (elapsed: %llu ns), last irq#:%d, last_start:%llu, last_end:%llu\n\n",
        (int)b_isr->cur_event, b_isr->cur_ts, sched_clock() - b_isr->cur_ts, (int)b_isr->last_event, b_isr->last_ts, b_isr->last_te);

    b_sq->cur_event == 0?
    printk("[Softirq]last#:%d, dur:%llu ns (s:%llu, e:%llu)\n\n",
        (int)b_sq->last_event, b_sq->last_te - b_sq->last_ts,b_sq->last_ts, b_sq->last_te):
    printk("[In Softirq]Current softirq#:%d, Start:%llu(elapsed: %llu ns), last softirq#:%d(dur:%llu ns), last_start:%llu, last_end:%llu\n\n",
        (int)b_sq->cur_event, b_sq->cur_ts, sched_clock() -b_sq->cur_ts, (int)b_sq->last_event,b_sq->last_te - b_sq->last_ts ,b_sq->last_ts, b_sq->last_te);


    b_tk->cur_event == 0?
    printk("[Tasklet]\n Occurs %d times in last SoftIRQ duration\n last fn:%pS, dur:%llu ns (s:%llu, e:%llu)\n\n",
        (int)b_tk->last_count, (void *)b_tk->last_event, b_tk->last_te - b_tk->last_ts,b_tk->last_ts, b_tk->last_te):
    printk("[In Tasklet]\n Occurs: cur:%d, last:%d\n Current:%pS, Start:%llu(elapsed: %llu ns), last#:%pS(dur:%llu ns), last_start:%llu, last_end:%llu\n\n",
        (int)b_tk->cur_count, (int)b_tk->last_count, (void *)b_tk->cur_event, b_tk->cur_ts, sched_clock() -b_tk->cur_ts, (void *)b_tk->last_event,b_tk->last_te - b_tk->last_ts ,b_tk->last_ts, b_tk->last_te);

    b_hrt->cur_event == 0?
    printk("[HRTimer]\n Occurs %d times in last ISR duration\n last fn:%pS, dur:%llu ns (s:%llu, e:%llu)\n\n",
        (int)b_hrt->last_count, (void *)b_hrt->last_event, b_hrt->last_te - b_hrt->last_ts,b_hrt->last_ts, b_hrt->last_te):
    printk("[In HRTimer]\n Occurs: cur:%d, last:%d\n Current:%pS, Start:%llu(elapsed: %llu ns), last#:%pS(dur:%llu ns), last_start:%llu, last_end:%llu\n\n",
        (int)b_tk->cur_count, (int)b_tk->last_count, (void *)b_hrt->cur_event, b_hrt->cur_ts, sched_clock() -b_hrt->cur_ts, (void *)b_hrt->last_event,b_hrt->last_te - b_hrt->last_ts ,b_hrt->last_ts, b_hrt->last_te);

    b_sft->cur_event == 0?
    printk("[SoftTimer]\n Occurs %d times in last SoftIRQ duration\n last fn:%pS, dur:%llu ns (s:%llu, e:%llu)\n\n",
        (int)b_sft->last_count, (void *)b_sft->last_event, b_sft->last_te - b_sft->last_ts,b_sft->last_ts, b_sft->last_te):
    printk("[In SoftTimer]\n Occurs: cur:%d, last:%d\n Current:%pS, Start:%llu(elapsed: %llu ns), last#:%pS(dur:%llu ns), last_start:%llu, last_end:%llu\n\n",
        (int)b_sft->cur_count, (int)b_sft->last_count, (void *)b_sft->cur_event, b_sft->cur_ts, sched_clock() -b_sft->cur_ts, (void *)b_sft->last_event,b_sft->last_te - b_sft->last_ts ,b_sft->last_ts, b_sft->last_te);

/****  Dump Stop Events ****/
    e_irq->cur_ts == 0?
    printk("[IRQ disable] last duration:%llu ns (s: %llu, e: %llu)\n\n", 
        e_irq->last_te - e_irq->last_ts, e_irq->last_ts, e_irq->last_te):
    printk("[IRQ disable] cur_ts:%llu(elapsed:%llu ns), last duration:%llu ns(s: %llu, e: %llu)\n\n", 
        e_irq->cur_ts, sched_clock() - e_irq->cur_ts, e_irq->last_te - e_irq->last_ts, e_irq->last_ts, e_irq->last_te);

   
    e_pmpt->cur_ts == 0?
    printk("[Preempt disable] last duration:%llu ns(s: %llu, e: %llu)\n\n", 
        e_pmpt->last_te - e_pmpt->last_ts, e_pmpt->last_ts, e_pmpt->last_te):
    printk("[Preempt disable] cur_ts:%llu(elapsed:%llu ns), last duration:%llu ns(s: %llu, e: %llu)\n\n", 
        e_pmpt->cur_ts, sched_clock() - e_pmpt->cur_ts, e_pmpt->last_te - e_pmpt->last_ts, e_pmpt->last_ts, e_pmpt->last_te);
    }    
    mt_show_current_irq_counts();
    mt_show_timer_info();
}
#include <linux/irqnr.h>
#include <linux/kernel_stat.h>
#include <asm/hardirq.h>

static int mt_irq_count[NR_IRQS];
#ifdef CONFIG_SMP
static int mt_local_irq_count[NR_CPUS][NR_IPI];
#endif
#ifdef CONFIG_LOCAL_TIMERS
static int mt_local_timer_count[NR_CPUS];
#endif
unsigned long long mt_save_irq_count_time;

static DEFINE_SPINLOCK(mt_irq_count_lock);
void mt_save_irq_counts(void)
{
    int irq, cpu;
    unsigned long flags;

    //do not refresh data in 10ms
    if(sched_clock() - mt_save_irq_count_time < 10000000)
        return;

    spin_lock_irqsave(&mt_irq_count_lock, flags);
    if(smp_processor_id() != 0){ //only record by CPU#0
        spin_unlock_irqrestore(&mt_irq_count_lock, flags);
        return;
    }
    mt_save_irq_count_time = sched_clock();
    for(irq = 0; irq< NR_IRQS; irq++){
        mt_irq_count[irq]=kstat_irqs_cpu(irq, 0);
    }
    for(cpu = 0; cpu < 2; cpu++){
#ifdef CONFIG_SMP
        for(irq = 0; irq< NR_IPI; irq++){
            mt_local_irq_count[cpu][irq] =  __get_irq_stat(cpu, ipi_irqs[irq]);
        }
#endif
#ifdef CONFIG_LOCAL_TIMERS
        mt_local_timer_count[cpu] =  __get_irq_stat(cpu, local_timer_irqs);
#endif
    }
    spin_unlock_irqrestore(&mt_irq_count_lock, flags);
}
void mt_show_last_irq_counts(void)
{
    int irq, cpu;
    unsigned long flags;
    spin_lock_irqsave(&mt_irq_count_lock, flags);
    printk("Last irq counts record at [%llu] ns\n", mt_save_irq_count_time);
    for(irq = 0; irq< NR_IRQS; irq++){
        if(mt_irq_count[irq] != 0)
            printk("[%3d] = %8d\n", irq, mt_irq_count[irq]);
    }

    for(cpu = 0; cpu < 2; cpu++){
#ifdef CONFIG_SMP
        for(irq = 0; irq< NR_IPI; irq++){
            if(mt_local_irq_count[cpu][irq] != 0)
                printk("(CPU#%d)IPI[%3d] = %8d\n", cpu, irq, mt_local_irq_count[cpu][irq]);
        }
#endif
#ifdef CONFIG_LOCAL_TIMERS
        if(mt_local_timer_count[cpu] != 0)
            printk("(CPU#%d)local timer = %8d\n", cpu, mt_local_timer_count[cpu]);
#endif
    }
    spin_unlock_irqrestore(&mt_irq_count_lock, flags);
}
void mt_show_current_irq_counts(void)
{
    int irq, cpu, count;
    unsigned long flags;
    unsigned long long t_cur, t_diff;
    t_cur = sched_clock();
    spin_lock_irqsave(&mt_irq_count_lock, flags);

    t_diff = t_cur - mt_save_irq_count_time;
    printk("=========================================\nIRQ Status:\n");
    printk("Current irq counts record at [%llu] ns.(last at %llu, diff:+%llu ns)\n", t_cur, mt_save_irq_count_time, t_diff);
    for(irq = 0; irq< NR_IRQS; irq++){
        count = kstat_irqs_cpu(irq, 0);
        if(count != 0)
            printk(" IRQ[%3d:%14s] = %8d, (+%d times in %lld us) \n", irq, isr_name(irq), count, count - mt_irq_count[irq], usec_high(t_diff));
    }
    for(cpu = 0; cpu < 2; cpu++){
#ifdef CONFIG_SMP
        printk("Local IRQ on CPU#%d: \n",cpu);
        for(irq = 0; irq< NR_IPI; irq++){
            count =  __get_irq_stat(cpu, ipi_irqs[irq]);
            if(count != 0)
                printk(" IRQ[%2d:  IPI] = %8d,(+%d times in %lld us)\n", irq, count, count - mt_local_irq_count[cpu][irq], usec_high(t_diff));
        }
#endif
#ifdef CONFIG_LOCAL_TIMERS
        count =  __get_irq_stat(cpu, local_timer_irqs);
        if(count != 0)
            printk(" IRQ[29:Timer] = %8d,(+%d times in %lld us)\n", count, count - mt_local_timer_count[cpu], usec_high(t_diff));
#endif
    }
    spin_unlock_irqrestore(&mt_irq_count_lock, flags);
}
void mt_show_timer_info(void)
{
    int cpu;
    for_each_possible_cpu(cpu){
        printk("[TimerISR#%d] last timer ISR start:%llu ns, end:%llu ns (ack timer:%llu ns)\n", cpu, per_cpu(local_timer_ts, cpu), per_cpu(local_timer_te, cpu), per_cpu(timer_ack_ts, cpu));
        if(0 == per_cpu(timer_ack_ts, cpu))
            printk("[TimerISR#%d] Warngin!!no ack timer and do not enter ipi_timer\n!!!", cpu);
    }
}
#else
void mt_trace_ISR_start(int id){}
void mt_trace_ISR_end(int id){}
void mt_trace_SoftIRQ_start(int id){}
void mt_trace_SoftIRQ_end(int id){}
void mt_trace_tasklet_start(void *func){}
void mt_trace_tasklet_end(void *func){}
void mt_trace_hrt_start(void *func){}
void mt_trace_hrt_end(void *func){}
void mt_trace_sft_start(void *func){}
void mt_trace_sft_end(void *func){}

void MT_trace_irq_on(void){}
void MT_trace_irq_off(void){}
void MT_trace_preempt_on(void){}
void MT_trace_preempt_off(void){}
void mt_dump_sched_traces(void){}
void mt_save_irq_counts(void){}
void mt_show_last_irq_counts(void){}
void mt_show_current_irq_counts(void){}
void mt_show_timer_info(void){}
#endif
/**/
#define TIME_3MS  3000000
#define TIME_10MS 10000000
static DEFINE_PER_CPU(int, MT_tracing_cpu);
static DEFINE_PER_CPU(unsigned long long, t_irq_on);
static DEFINE_PER_CPU(unsigned long long, t_irq_off);
void MT_trace_softirqs_on(unsigned long ip);
void MT_trace_softirqs_off(unsigned long ip);

void MT_trace_hardirqs_on(void)
{
    unsigned long long t_diff, t_on, t_off;
    if(unlikely(__raw_get_cpu_var(mtsched_mon_enabled) & 0x2)){
        if( 0 == current->pid) /* Ignore swap thread */
            return;
        if(__raw_get_cpu_var(MT_tracing_cpu) == 1) 
        {
            MT_trace_irq_on();
            t_on = sched_clock();
            t_off = __raw_get_cpu_var(t_irq_off);
            t_diff = t_on - t_off;

            __raw_get_cpu_var(t_irq_on) = t_on;
            if(t_diff > TIME_10MS){
                printk(KERN_WARNING "[Sched Latency:IRQ Disable Monitor][%d:%s] Duration: %lld.%lu ms > 10 ms(s:%lld, e:%lld)\n",current->pid, current->comm, SPLIT_NS(t_diff), t_off, t_on);
                dump_stack();
            }
            __raw_get_cpu_var(t_irq_off) = 0; 
        }
        __raw_get_cpu_var(MT_tracing_cpu) = 0;
    }
}
EXPORT_SYMBOL(MT_trace_hardirqs_on);
void MT_trace_hardirqs_off(void)
{
    if(unlikely(__raw_get_cpu_var(mtsched_mon_enabled) & 0x2)){
        if( 0 == current->pid) /* Ignore swap thread */
            return;
        if(__raw_get_cpu_var(MT_tracing_cpu) == 0){
            MT_trace_irq_off();
            __raw_get_cpu_var(t_irq_off) = sched_clock();
        }
        __raw_get_cpu_var(MT_tracing_cpu) = 1;
    }
}
EXPORT_SYMBOL(MT_trace_hardirqs_off);
/* --------------------------------------------------- */
/*                     Define Proc entry               */
/* --------------------------------------------------- */
MT_DEBUG_ENTRY(sched_monitor);
static int mt_sched_monitor_show(struct seq_file *m, void *v)
{
    int cpu;
    SEQ_printf(m, "=== mt Scheduler monitoring ===\n");
    SEQ_printf(m, " 0: Disable All\n 1: [Preemption] Monitor\n 2: [IRQ disable] Monitor\n 3: Enable All\n");
	
    for_each_possible_cpu(cpu){
	SEQ_printf(m, "  Scheduler Monitor:%d (CPU#%d)\n",per_cpu(mtsched_mon_enabled, cpu), cpu);
    }
    
    return 0;
}
static ssize_t mt_sched_monitor_write(struct file *filp, const char *ubuf,
	   size_t cnt, loff_t *data)
{
    char buf[64];
    int val;
    int ret;
    if (cnt >= sizeof(buf))
	return -EINVAL;

    if (copy_from_user(&buf, ubuf, cnt))
	return -EFAULT;

    buf[cnt] = 0;

    ret = strict_strtoul(buf, 10, (unsigned long*)&val);
    if (ret < 0)
	return ret;
    //0: off, 1:on
    //val = !!val;
    mt_sched_monitor_switch(val);
    printk(" to %d\n", val);
    if(val == 9 )
        mt_dump_sched_traces();
    return cnt;
}
void reset_sched_monitor(void)
{}
void start_sched_monitor(void)
{
}
void stop_sched_monitor(void)
{
}
void mt_sched_monitor_switch(int on)
{
    int cpu;
    preempt_disable_notrace();
    mutex_lock(&mt_sched_mon_lock);
    for_each_possible_cpu(cpu){
        printk("[mtprof] sched monitor on CPU#%d switch from %d to %d\n", cpu, per_cpu(mtsched_mon_enabled, cpu), on);
        per_cpu(mtsched_mon_enabled, cpu)= on; // 0x1 || 0x2, IRQ & Preempt
    }
    mutex_unlock(&mt_sched_mon_lock);
    preempt_enable_notrace();
}

static int __init init_mtsched_mon(void)
{
    struct proc_dir_entry *pe;
    int cpu;
#ifdef CONFIG_MT_SCHED_MONITOR
    struct sched_block_event *b;
#endif
    for_each_possible_cpu(cpu){
        per_cpu(mtsched_mon_enabled, cpu)= 0; // 0x1 || 0x2, IRQ & Preempt
        per_cpu(MT_tracing_cpu, cpu) = 0;
    }
    if (!proc_mkdir("mtmon", NULL)){
	return -1;
    }
#ifdef CONFIG_MT_ENG_BUILD
    pe = proc_create("mtmon/sched_mon", 0666, NULL, &mt_sched_monitor_fops);
    if (!pe)
	return -ENOMEM;
#ifdef CONFIG_MT_SCHED_MONITOR
    b = & __raw_get_cpu_var(ISR_mon);
    b->type = evt_ISR ;
    b = & __raw_get_cpu_var(SoftIRQ_mon);
    b->type = evt_SOFTIRQ ;
    b = & __raw_get_cpu_var(tasklet_mon);
    b->type = evt_TASKLET ;
    b = & __raw_get_cpu_var(hrt_mon);
    b->type = evt_HRTIMER ;
    b = & __raw_get_cpu_var(sft_mon);
    b->type = evt_STIMER ;
#endif
#else
    pe = NULL;
#endif
    //start_record_task();
    return 0;
}
__initcall(init_mtsched_mon);
