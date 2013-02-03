#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/kallsyms.h>
#include <linux/utsname.h>
#include <asm/uaccess.h>
#include <linux/tick.h>
#include "prof_main.h"
#include "prof_mem.h"
struct mt_proc_struct *mt_proc_curr = NULL;
struct mt_proc_struct *mt_proc_head = NULL;
int proc_count = 0;
int mtsched_enabled = 0;
unsigned long long prof_start_ts, prof_end_ts, prof_dur_ts;
static DEFINE_MUTEX(mt_cputime_lock);
static DEFINE_MUTEX(mt_memprof_lock);

unsigned long long cpu0_idletime_start, cpu0_iowait_start;
unsigned long long cpu0_idletime_end, cpu0_iowait_end;

#ifdef CONFIG_SMP
unsigned long long cpu1_idletime_start, cpu1_iowait_start; 
unsigned long long cpu1_idletime_end, cpu1_iowait_end;
#endif
void mt_cputime_switch(int on);
unsigned long long mtprof_get_cpu_idle(int cpu)
{
    unsigned long long *unused = 0;
    return get_cpu_idle_time_us(cpu, unused);
}
unsigned long long mtprof_get_cpu_iowait(int cpu)
{
    unsigned long long *unused = 0;
    return get_cpu_iowait_time_us(cpu, unused);
}
void mt_task_times(struct task_struct *p, cputime_t *ut, cputime_t *st){
    task_times(p,ut,st);
}
/********************
     MT cputime prof
*********************/
#ifdef CONFIG_MTPROF_CPUTIME
void setup_mtproc_info(struct task_struct *p, unsigned long long ts)
{
	struct mt_proc_struct *mtproc;

	if(0 == mtsched_enabled)
	{
		return;
	}
	/*	
	if(proc_count>= PROC_NUM){
	return;
	}
	*/   
	mtproc = kmalloc(sizeof(struct mt_proc_struct), GFP_KERNEL);
	if(!mtproc)
	{
		printk("mtproc unable to kmalloc\n");
		return;
	}
	memset(mtproc, 0,sizeof(struct mt_proc_struct));
	proc_count++; 

	p->se.mtk_isr_time = 0;
	mtproc->pid = p->pid;
	mtproc->tgid = p->tgid;
	mtproc->index = proc_count;
	mtproc->cputime =  p->se.sum_exec_runtime;
	mtproc->cputime_init = p->se.sum_exec_runtime;
	mtproc->prof_start = ts;
	mtproc->prof_end = 0;
	mtproc->isr_time = 0;
	p->se.mtk_isr = NULL;
	p->se.mtk_isr_time = 0;
	p->se.mtk_isr_count = 0;
	mtproc->next = NULL;
	mt_task_times(p,&mtproc->utime_init, &mtproc->stime_init);
	strcpy(mtproc->comm, p->comm);
	if(mt_proc_head != NULL)
	{
		mt_proc_curr->next = mtproc;
		mt_proc_curr = mtproc;
	}
	else
	{
		mt_proc_head = mtproc;
		mt_proc_curr = mtproc;	
	}
	//   mt_proc[proc_count - 1] = mtproc;
}
void save_mtproc_info(struct task_struct *p, unsigned long long ts)
{
	struct mt_proc_struct *mtproc;

	mutex_lock(&mt_cputime_lock);
	if(0 == mtsched_enabled)
	{
		mutex_unlock(&mt_cputime_lock);
		return;
	}

	/*
	if(proc_count>= PROC_NUM){
	mutex_unlock(&mt_cputime_lock);
	return;
	}
	*/   
	mutex_unlock(&mt_cputime_lock);

	mtproc = kmalloc(sizeof(struct mt_proc_struct), GFP_KERNEL);
	if(!mtproc)
	{
		printk("mtproc unable to kmalloc\n");
		return;
	}
	memset(mtproc, 0,sizeof(struct mt_proc_struct));
	mutex_lock(&mt_cputime_lock);
	proc_count++; 

	mtproc->pid = p->pid;
	mtproc->tgid = p->tgid;
	mtproc->index = proc_count;
	mtproc->cputime =  p->se.sum_exec_runtime ;
	mtproc->cputime_init = p->se.sum_exec_runtime;
	mtproc->isr_time = 0;
	p->se.mtk_isr = NULL;
	p->se.mtk_isr_time = 0;
	p->se.mtk_isr_count = 0;
	mtproc->prof_start = ts;
	mtproc->prof_end = 0;
	mtproc->next = NULL;	
	mt_task_times(p,&mtproc->utime_init, &mtproc->stime_init);
	strcpy(mtproc->comm, p->comm);
	//   mt_proc[proc_count - 1] = mtproc;
	if(mt_proc_head != NULL)
	{
		mt_proc_curr->next = mtproc;
		mt_proc_curr = mtproc;
	}
	else
	{
		mt_proc_head = mtproc;
		mt_proc_curr = mtproc;  
	}

	mutex_unlock(&mt_cputime_lock);
}
void end_mtproc_info(struct task_struct *p)
{
	struct mt_proc_struct *mtproc = mt_proc_head;

	mutex_lock(&mt_cputime_lock);
	//check profiling enable flag
	if(0 == mtsched_enabled)
	{
		mutex_unlock(&mt_cputime_lock);
		return;
	}

	//may waste time...
	while(mtproc != NULL)
	{
		if(p->pid != mtproc->pid)
		{
			mtproc = mtproc->next;
		}
		else
		{
			break;
		}
	}
	/*	
	for(i=0;i<proc_count; i++){
	if(p->pid == mt_proc[i]->pid)
	break;
	}
	*/
	if(mtproc == NULL)
	// if(i == proc_count)
	{
		printk("pid:%d can't be found in mtsched proc_info.\n",p->pid);
		mutex_unlock(&mt_cputime_lock);	
		return;
	}
	mtproc->prof_end = sched_clock();
	/* update cputime */
	mtproc->cputime = p->se.sum_exec_runtime;
	mtproc->isr_time = p->se.mtk_isr_time;
	mtproc->isr_count = p->se.mtk_isr_count;
	mtproc->mtk_isr = p->se.mtk_isr;
	p->se.mtk_isr = NULL;
	mt_task_times(p,&mtproc->utime, &mtproc->stime);
	mtproc->utime = cputime_sub(mtproc->utime, mtproc->utime_init);
	mtproc->stime = cputime_sub(mtproc->stime, mtproc->stime_init);

	mutex_unlock(&mt_cputime_lock);
	return;
}
void set_mtprof_comm(char* comm, int pid)
{
     struct mt_proc_struct *mtproc = mt_proc_head;

    mutex_lock(&mt_cputime_lock);
    if(0 == mtsched_enabled){
	mutex_unlock(&mt_cputime_lock);
	return;
    }

   while(mtproc != NULL)
   {
	if(pid != mtproc->pid)
	{
		mtproc = mtproc->next;
	}
	else
	{
		break;
	}
   }

   if(mtproc == NULL)
   {
        printk("[mtprof] no matching pid\n");
	mutex_unlock(&mt_cputime_lock);
	return;
   }


    memset(mtproc->comm, 0, TASK_COMM_LEN);
    wmb();
    strlcpy(mtproc->comm, comm, TASK_COMM_LEN);
    mutex_unlock(&mt_cputime_lock);
    
}
void start_record_task(void)
{
    unsigned long long ts;

    struct task_struct *g, *p;
    unsigned long flags;
    int cpu = 0;
    mtsched_enabled = 1;
    prof_start_ts = sched_clock();
    cpu0_idletime_start = mtprof_get_cpu_idle(cpu);// cpu'0', notified SMP
    cpu0_iowait_start = mtprof_get_cpu_iowait(cpu); 
#ifdef CONFIG_SMP
	cpu1_idletime_start = mtprof_get_cpu_idle(1);
	cpu1_iowait_start = mtprof_get_cpu_iowait(1);
#endif
    ts = sched_clock();
//    for_each_online_cpu(cpu){
	read_lock_irqsave(&tasklist_lock, flags);
	do_each_thread(g, p) {
	    setup_mtproc_info(p, ts);
	} while_each_thread(g, p);

	read_unlock_irqrestore(&tasklist_lock, flags);
 //   }
}
void stop_record_task(void)
{
	struct mt_proc_struct *mtproc = mt_proc_head;
	struct task_struct *tsk;
	unsigned long long cost_cputime = 0;

	mtsched_enabled = 0;
	prof_end_ts = sched_clock();

	prof_dur_ts = cputime_sub(prof_end_ts,  prof_start_ts);
	do_div(prof_dur_ts, 1000000);		// put prof_dur_ts to ms

	cpu0_idletime_end = mtprof_get_cpu_idle(0);// prepare for SMP
	cpu0_iowait_end = mtprof_get_cpu_iowait(0);

#ifdef CONFIG_SMP
	cpu1_idletime_end = mtprof_get_cpu_idle(1);
	cpu1_iowait_end = mtprof_get_cpu_iowait(1);
#endif	

	//for(i=0;i<proc_count; i++)
	while(mtproc != NULL)
	{
		tsk = find_task_by_vpid(mtproc->pid);
		if(tsk!=NULL)
		{
			mtproc->cputime = tsk->se.sum_exec_runtime;
			mtproc->isr_time = tsk->se.mtk_isr_time;
			mtproc->isr_count = tsk->se.mtk_isr_count;
			mt_task_times(tsk, &mtproc->utime, &mtproc->stime);
			mtproc->utime = cputime_sub(mtproc->utime, mtproc->utime_init);
			mtproc->stime = cputime_sub(mtproc->stime, mtproc->stime_init);			
			mtproc->mtk_isr = tsk->se.mtk_isr;
			tsk->se.mtk_isr_count = 0;
			tsk->se.mtk_isr_time = 0;
			tsk->se.mtk_isr = NULL;
			
		}
		
		
		if(mtproc->cputime >=  (mtproc->cputime_init + mtproc->isr_time))
		{
			cost_cputime = cputime_sub( mtproc->cputime-mtproc->isr_time, mtproc->cputime_init);
			mtproc->cost_cputime = cost_cputime;
			do_div(cost_cputime,  prof_dur_ts);
			mtproc->cputime_percen_6 = cost_cputime;
		}
		else
		{
			mtproc->cost_cputime = 0;
			mtproc->cputime_percen_6 = 0;
		}
		
		mtproc = mtproc->next;
	}
}
void reset_record_task(void){
	struct mt_proc_struct *mtproc = mt_proc_head;
	struct mt_proc_struct *mtproc_next;
	struct mtk_isr_info * mtk_isr_current, *mtk_isr_next;
	struct task_struct *idle;
	
	while(mtproc != NULL)
	{
		mtk_isr_current = mtproc->mtk_isr;
		while(mtk_isr_current != NULL)
		{
			mtk_isr_next = mtk_isr_current->next;
			kfree(mtk_isr_current->isr_name);
			kfree(mtk_isr_current);
			mtk_isr_current = mtk_isr_next;
		}
		mtproc_next = mtproc->next;
		kfree(mtproc);
		mtproc = mtproc_next;
	}
	proc_count = 0;
	prof_end_ts = 0;

	cpu0_idletime_start = 0;cpu0_iowait_start = 0;
	cpu0_idletime_end = 0;cpu0_iowait_end = 0;

	idle = idle_task(0);
	mtk_isr_current = idle->se.mtk_isr;
	while(mtk_isr_current != NULL)
	{
		mtk_isr_next = mtk_isr_current->next;
		kfree(mtk_isr_current->isr_name);
		kfree(mtk_isr_current);
		mtk_isr_current = mtk_isr_next;
	}
	idle->se.mtk_isr_time = 0;
	idle->se.mtk_isr_count = 0;
	idle->se.mtk_isr = NULL;
	
	
#ifdef CONFIG_SMP
	cpu1_idletime_start = 0;cpu1_iowait_start = 0;
	cpu1_idletime_end = 0;cpu1_iowait_end = 0;
	idle = idle_task(1);
	mtk_isr_current = idle->se.mtk_isr;
	while(mtk_isr_current != NULL)
	{
		mtk_isr_next = mtk_isr_current->next;
		kfree(mtk_isr_current->isr_name);
		kfree(mtk_isr_current);
		mtk_isr_current = mtk_isr_next;
	}
	idle->se.mtk_isr_time = 0;
	idle->se.mtk_isr_count = 0;	
	idle->se.mtk_isr = NULL;
#endif

	mt_proc_head =NULL;  mt_proc_curr = NULL;
}

void mt_cputime_switch(int on)
{
	printk("Original mtsched enabled = %d, on = %d.\n", mtsched_enabled, on);
    mutex_lock(&mt_cputime_lock);

	if(mtsched_enabled == 1)
	{
		if(on == 0)
		{
			stop_record_task();
		}
	}
	else
	{
		if(on == 1)
		{
			reset_record_task();
			start_record_task();
		}
		else if(on == 2)
		{
			reset_record_task();
		}
	}

/*
	if (mtsched_enabled ^ on) {
	if (on) {
	reset_record_task();
	start_record_task();
	} else {
	stop_record_task();
	}
	}
*/	
    mutex_unlock(&mt_cputime_lock);
    printk("Current mtsched enabled = %d\n", mtsched_enabled);
}

#else  //CONFIG_MTPROF_CPUTIME
void setup_mtproc_info(struct task_struct *p, unsigned long long ts){
    return;
}
void save_mtproc_info(struct task_struct *p, unsigned long long ts){
    return;
}
void end_mtproc_info(struct task_struct *p){
    return;
}
void set_mtprof_comm(char* comm, int pid){
    return;
}
void start_record_task(void){
    return;
}
void stop_record_task(void){
    return;
}
void reset_record_task(void){
    return;
}
void mt_cputime_switch(int on){
    return;
}
#endif//end of CONFIG_MTPROF_CPUTIME

/********************
     MT mem prof
*********************/
/*
void reset_allocpages_prof(void)
{
    num_good_pages = 0;
    num_bad_pages = 0;
    good_pages_time = 0;
    bad_pages_time = 0;
}

void start_allocpages_prof(void)
{
    mt_memprof_enabled = 1;
}

void stop_allocpages_prof(void)
{
    mt_memprof_enabled = 0;
}

void mt_memprof_switch(int on)
{
    printk("Original memprof enabled = %d\n", mt_memprof_enabled);
    mutex_lock(&mt_memprof_lock);
    if (mt_memprof_enabled ^ on) {
	if (on) {
	    reset_allocpages_prof();
	    start_allocpages_prof();
	} else {
	    stop_allocpages_prof();
	}
    }
    mutex_unlock(&mt_memprof_lock);
    printk("Current memprof enabled = %d\n", mt_memprof_enabled);

}
*/
