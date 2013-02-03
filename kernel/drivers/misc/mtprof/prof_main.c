#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/kallsyms.h>
#include <linux/utsname.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include "prof_ctl.h"
#include "prof_mem.h"

#include <linux/pid.h>
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

#ifdef CONFIG_MTPROF_CPUTIME
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

static unsigned long usec_low(unsigned long long usec)
{
    if ((long long)usec < 0)
	usec = -usec;

    return do_div(usec, 1000);
}

#define SPLIT_US(x) usec_high(x), usec_low(x)

#endif

static void print_task(struct seq_file *m, struct task_struct *p)
{
    SEQ_printf(m, "%15s %5d %9Ld %5d ",
	p->comm, p->pid,
	(long long)(p->nvcsw + p->nivcsw),
	p->prio);
#ifdef CONFIG_SCHEDSTATS
    SEQ_printf(m, "%9Ld.%06ld %9Ld.%06ld %9Ld.%06ld\n",
	SPLIT_NS(p->se.vruntime),
	SPLIT_NS(p->se.sum_exec_runtime),
	SPLIT_NS(p->se.statistics.sum_sleep_runtime));
#else
    SEQ_printf(m, "%15Ld %15Ld %15Ld.%06ld %15Ld.%06ld %15Ld.%06ld\n",
	0LL, 0LL, 0LL, 0L, 0LL, 0L, 0LL, 0L);
#endif
}
/*========================================================================*/
/* Real work */
/*========================================================================*/
/* 1. sched info */
MT_DEBUG_ENTRY(sched_debug);
static int mt_sched_debug_show(struct seq_file *m, void *v)
{
    struct task_struct *g, *p;
    unsigned long flags;
    SEQ_printf(m, "=== mt Scheduler Profiling ===\n");
    SEQ_printf(m,
	    "\nrunnable tasks:\n"
	    "            task   PID   switches  prio"
	    "     exec-runtime         sum-exec        sum-sleep\n"
	    "------------------------------------------------------"
	    "----------------------------------------------------\n");
    read_lock_irqsave(&tasklist_lock, flags);

    do_each_thread(g, p) {
	print_task(m, p);
    } while_each_thread(g, p);

    read_unlock_irqrestore(&tasklist_lock, flags);
    return 0;
}
static ssize_t mt_sched_debug_write(struct file *filp, const char *ubuf,
	   size_t cnt, loff_t *data)
{
    return cnt;
}

/* 2. cputime */
MT_DEBUG_ENTRY(cputime);
static int mt_cputime_show(struct seq_file *m, void *v)
{
#ifdef CONFIG_MTPROF_CPUTIME
	struct mt_proc_struct *mtproc = mt_proc_head;
	unsigned long long end_ts;
	unsigned long long total_excul_time = 0, thread_time = 0;
	u32 div_value;
	struct task_struct *tsk;
	struct task_struct *idle;
	struct mtk_isr_info *mtk_isr = NULL;
	char status;
	
	if(mt_proc_head == NULL)
	{
		SEQ_printf(m,"Please enable cputime again!\n");
	}
	else
	{
		if(0 == prof_end_ts || mtsched_enabled)
	{
		end_ts = sched_clock();
	}
	else
	{
		end_ts = prof_end_ts; 
	}
	prof_dur_ts = end_ts  - prof_start_ts;



		if(0 == cpu0_idletime_end || mtsched_enabled)
		{
			cpu0_idletime_end = mtprof_get_cpu_idle(0);
		}

		if(0 == cpu0_iowait_end || mtsched_enabled)
		{
			cpu0_iowait_end = mtprof_get_cpu_iowait(0);
		}
		
#ifndef CONFIG_SMP
		SEQ_printf(m,"iowait time(us): %llu\n",cpu0_iowait_end - cpu0_iowait_start);
#else
		if(0 == cpu1_iowait_end || mtsched_enabled)
		{
			cpu1_iowait_end = mtprof_get_cpu_iowait(1);
		}
		SEQ_printf(m,"iowait time(us): %llu , %llu\n",cpu0_iowait_end - cpu0_iowait_start, 
			cpu1_iowait_end - cpu1_iowait_start);
#endif

		thread_time = cputime_sub(cpu0_idletime_end, cpu0_idletime_start) * 1000;
		do_div(prof_dur_ts, 1000000);
		do_div(thread_time, prof_dur_ts);
		div_value = (u32)thread_time;

		

		idle = idle_task(0);
	SEQ_printf(m, "-----------------------------------------------\n");
	SEQ_printf(m, "        Duration: %10Ld.%06ld ms \n", SPLIT_NS(end_ts - prof_start_ts));
	SEQ_printf(m, "        --------------------------------\n");
	SEQ_printf(m, "           Start: %10Ld.%06ld ms \n", SPLIT_NS(prof_start_ts));
	SEQ_printf(m, "             End: %10Ld.%06ld ms \n", SPLIT_NS(end_ts));
	/*
	SEQ_printf(m, "    SCHED_IOWAIT: %10Ld.%06ld ms \n", SPLIT_US(
	cpu0_iowait_end == 0 ? mtprof_get_cpu_iowait(0) - cpu0_iowait_start:
	cpu0_iowait_end - cpu0_iowait_start
	));*/
	SEQ_printf(m, "-----------------------------------------------\n");
	SEQ_printf(m, "         Process:Status:   PID:  TGID:          CPUtime:       Percenta%%:          Elapsed:   User: Kernel:  ISR  type:   ISRTime:\n");
		SEQ_printf(m, "            Idle:     L:     0:     0:%10Ld.%06ld:%10d.%04d%%:%10Ld.%06ld:      0:      0:%7d:%10Ld.%06ld:\n",
		SPLIT_US(cpu0_idletime_end - cpu0_idletime_start),
		div_value /10000,
		div_value % 10000,
		SPLIT_NS(end_ts - prof_start_ts),
		idle->se.mtk_isr_count,
		SPLIT_NS(idle->se.mtk_isr_time)
				);

		total_excul_time = (cpu0_idletime_end - cpu0_idletime_start) * 1000;
		
#ifdef CONFIG_SMP
		if(0 == cpu1_idletime_end || mtsched_enabled)
		{
			cpu1_idletime_end = mtprof_get_cpu_idle(1);
		}
		thread_time = cputime_sub(cpu1_idletime_end, cpu1_idletime_start) * 1000;
		do_div(thread_time, prof_dur_ts);
		div_value = (u32)thread_time;
		idle = idle_task(1);
		SEQ_printf(m, "          Idle-1:     L:    -1:     0:%10Ld.%06ld:%10d.%04d%%:%10Ld.%06ld:      0:      0:%7d:%10Ld.%06ld:\n",
		SPLIT_US(cpu1_idletime_end - cpu1_idletime_start),
		div_value /10000,
		div_value % 10000,
		SPLIT_NS(end_ts - prof_start_ts),
		idle->se.mtk_isr_count,
		SPLIT_NS(idle->se.mtk_isr_time)
					);
		total_excul_time += (cpu1_idletime_end - cpu1_idletime_start) * 1000;

#endif
	//for(i=0;i<proc_count; i++)
	while(mtproc != NULL)
	{
		/* Record new cputime*/
		tsk = find_task_by_vpid(mtproc->pid);

		if(tsk!=NULL)
		{
			/* update cputime */
			if(mtsched_enabled)
			{
				mtproc->cputime = tsk->se.sum_exec_runtime;// - tsk->se.mtk_isr_time;
				mtproc->isr_time = tsk->se.mtk_isr_time;				
				mt_task_times(tsk, &mtproc->utime, &mtproc->stime);
				mtproc->utime = cputime_sub(mtproc->utime, mtproc->utime_init);
				mtproc->stime = cputime_sub(mtproc->stime, mtproc->stime_init);
			}
			status = 'L';    
		}
		else
		{
			status = 'D';						
		}

		if(mtsched_enabled)
		{
			if(mtproc->cputime >=  (mtproc->cputime_init + mtproc->isr_time))
			{
				thread_time = cputime_sub( mtproc->cputime- mtproc->isr_time, mtproc->cputime_init);
					mtproc->cost_cputime = thread_time;
				do_div(thread_time,  prof_dur_ts);
				mtproc->cputime_percen_6 = thread_time;
			}
			else
			{
					mtproc->cost_cputime = 0;
				mtproc->cputime_percen_6 = 0;
			}
		}
		

		total_excul_time += (mtproc->cputime-mtproc->cputime_init);

		SEQ_printf(m,"%16s:     %c:%6d:%6d:%10Ld.%06ld:%10d.%04d%%:%10Ld.%06ld:%7u:%7u:%7d:%10Ld.%06ld:\n", 
			mtproc->comm,  status, mtproc->pid, mtproc->tgid,
				SPLIT_NS(mtproc->cost_cputime),
			mtproc->cputime_percen_6 /10000,
			mtproc->cputime_percen_6 %10000,
			SPLIT_NS(mtproc->prof_end== 0? end_ts - mtproc->prof_start: mtproc->prof_end -  mtproc->prof_start),
			cputime_to_msecs(mtproc->utime),
			cputime_to_msecs(mtproc->stime),
			mtproc->isr_count,
			SPLIT_NS(mtproc->isr_time));
		
		mtproc = mtproc->next;
	}

	SEQ_printf(m,"********************\n");

		idle = idle_task(0);
		mtk_isr = idle->se.mtk_isr;
		if(idle->se.mtk_isr_count != 0)
		{
			SEQ_printf(m, "thread name:            idle, thread id: 0, total ISR type %d:\n",idle->se.mtk_isr_count);
		}
		while(mtk_isr != NULL)
		{
			SEQ_printf(m, "ISR name: %16s: number: %d: count: %d: total time: %10Ld.%06ld:\n",mtk_isr->isr_name, mtk_isr->isr_num,
				mtk_isr->isr_count,SPLIT_NS(mtk_isr->isr_time));
			mtk_isr = mtk_isr->next;
		}
#ifdef CONFIG_SMP
		idle = idle_task(1);
		mtk_isr = idle->se.mtk_isr;
		if(idle->se.mtk_isr_count != 0)
		{
			SEQ_printf(m, "thread name:          idle-1, thread id: 0, total ISR type %d:\n",  idle->se.mtk_isr_count);
		}
		while(mtk_isr != NULL)
		{
			SEQ_printf(m, "ISR name: %16s: number: %d: count: %d: total time: %10Ld.%06ld:\n",mtk_isr->isr_name, mtk_isr->isr_num,
				mtk_isr->isr_count,SPLIT_NS(mtk_isr->isr_time));
			mtk_isr = mtk_isr->next;
		}
#endif
	
		
	mtproc = mt_proc_head;
	while(mtproc != NULL)
	{
		mtk_isr = mtproc->mtk_isr;
		if(mtproc->isr_count != 0)
		{
			SEQ_printf(m, "thread name: %16s, thread id: %d, total ISR type %d:\n", mtproc->comm, mtproc->pid, mtproc->isr_count);
		}
		while(mtk_isr != NULL)
		{
			SEQ_printf(m, "ISR name: %16s: number: %d: count: %d: total time: %10Ld.%06ld:\n",mtk_isr->isr_name, mtk_isr->isr_num,
				mtk_isr->isr_count,SPLIT_NS(mtk_isr->isr_time));
			mtk_isr = mtk_isr->next;
		}
		mtproc = mtproc->next;
	}
	SEQ_printf(m,"********************\n");
	SEQ_printf(m,"All the thread total execult time is:%10Ld.%06ld.\n", SPLIT_NS(total_excul_time));
	}

#endif
	return 0;
}

static ssize_t mt_cputime_write(struct file *filp, const char *ubuf,
	   size_t cnt, loff_t *data)
{
#ifdef CONFIG_MTPROF_CPUTIME
    char buf[64];
    unsigned long val;
    int ret;

    if (cnt >= sizeof(buf))
	return -EINVAL;

    if (copy_from_user(&buf, ubuf, cnt))
	return -EFAULT;
	
printk("mtsched_proc input stream:%s, count %d.\n", buf, cnt);

    buf[cnt] = 0;

    ret = strict_strtoul(buf, 10, &val);
    if (ret < 0)
	return ret;
    printk("mtsched_proc input stream:%s\n", buf);
//    val = !!val;
    //0: off, 1:on
    mt_cputime_switch(val);
#endif
    return cnt;

}

/* 3. mem prof*/
MT_DEBUG_ENTRY(memprof);
static int mt_memprof_show(struct seq_file *m, void *v)
{
	/*
	
	unsigned long long avg_good_ns = good_pages_time;
	unsigned long long avg_bad_ns = bad_pages_time;

	SEQ_printf(m, "----------------------------------------\n");
	SEQ_printf(m, "Total Mem AllocPage Latency(ms): %Ld.%06ld \n", SPLIT_NS(good_pages_time + bad_pages_time));
	SEQ_printf(m, "----------------------------------------\n");

	if (num_good_pages) {
		SEQ_printf(m, "Alloc Good Pages Latency(ms): %Ld.%06lu \n", SPLIT_NS(good_pages_time));
		do_div(avg_good_ns, num_good_pages);
		SEQ_printf(m, "Good pages: %lu\n", num_good_pages);
		SEQ_printf(m, "Avg. Good Page Latency(ns): %llu\n", avg_good_ns);
	}
	if (num_bad_pages) {
		SEQ_printf(m, "Alloc Bad Pages Latency(ms): %Ld.%06lu \n", SPLIT_NS(bad_pages_time));
		do_div(avg_bad_ns, num_bad_pages);
		SEQ_printf(m, "Bad pages: %lu\n", num_bad_pages);
		SEQ_printf(m, "Avg. Bad Page Latency(ns): %llu\n", avg_bad_ns);
	}
*/
    return 0; 
}

static ssize_t mt_memprof_write(struct file *filp, const char *ubuf,
	size_t cnt, loff_t *data)
{
/*
    char buf[64];
    unsigned long val;
    int ret;

    if (cnt >= sizeof(buf))
	return -EINVAL;

    if (copy_from_user(&buf, ubuf, cnt))
	return -EFAULT;

    buf[cnt] = 0;

    ret = strict_strtoul(buf, 10, &val);
    if (ret < 0)
	return ret;
    printk("allocpages input stream:%s\n", buf);
    val = !!val;
    //0: off, 1:on
    mt_memprof_switch(val);
*/
    return cnt;
}
/* 4. prof status*/
MT_DEBUG_ENTRY(status);
#define MT_CPUTIME 1
#define MT_MEMPROF 2
unsigned long mtprof_status = 0;
static int mt_status_show(struct seq_file *m, void *v)
{
    SEQ_printf(m, "%lu\n", mtprof_status);
    return 0; 
}

static ssize_t mt_status_write(struct file *filp, const char *ubuf,
	size_t cnt, loff_t *data)
{
    char buf[64];
    unsigned long val;
    int ret;
    if (cnt >= sizeof(buf))
	return -EINVAL;

    if (copy_from_user(&buf, ubuf, cnt))
	return -EFAULT;

    buf[cnt] = 0;

    ret = strict_strtoul(buf, 10, &val);
    if (ret < 0)
	return ret;
    //0: off, 1:on
    printk("[mtprof] status = 0x%x\n", (unsigned int)mtprof_status);
    mtprof_status = val;
    printk("[mtprof] new status = 0x%x\n", (unsigned int)mtprof_status);
    return cnt;
}
/*-------------------------------------------------------------------*/
static int __init init_mtsched_prof(void)
{
    struct proc_dir_entry *pe;
    if (!proc_mkdir("mtprof", NULL)){
	return -1;
    }
    pe = proc_create("mtprof/sched", 0444, NULL, &mt_sched_debug_fops);
    if (!pe)
	return -ENOMEM;
    pe = proc_create("mtprof/cputime", 0664, NULL, &mt_cputime_fops);
    if (!pe)
	return -ENOMEM;
#ifdef CONFIG_MT_ENG_BUILD
    pe = proc_create("mtprof/memprof", 0666, NULL, &mt_memprof_fops);
    if (!pe)
	return -ENOMEM;
    pe = proc_create("mtprof/status", 0666, NULL, &mt_status_fops);
    if (!pe)
	return -ENOMEM;
#endif
    //start_record_task();
    return 0;
}
__initcall(init_mtsched_prof);
