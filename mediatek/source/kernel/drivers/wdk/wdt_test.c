#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/rtpm_prio.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/rtpm_prio.h>
#include <linux/rtc.h>

extern int nr_cpu_ids;
//int enable_clock(int id, unsigned char *name);
static int test_case  = 0;
module_param(test_case, int, 00664);
static DEFINE_SPINLOCK(wdt_test_lock0);
static DEFINE_SPINLOCK(wdt_test_lock1);
static struct task_struct *wk_tsk[2];// cpu: 2
static int data;
static int kwdt_thread_test(void *arg)
{
	struct sched_param param = { .sched_priority = RTPM_PRIO_WDT};
	int cpu;
	unsigned int flags;

  sched_setscheduler(current, SCHED_FIFO, &param);

  set_current_state(TASK_INTERRUPTIBLE);
  for(;;){ 
  	printk("wulin debug start, cpu:%d\n", cpu);
  	spin_lock(&wdt_test_lock0);
  	cpu = smp_processor_id();
  	spin_unlock(&wdt_test_lock0);

	  if(test_case == (cpu*10+1))//cpu0 Preempt disale
	  {	
	  	printk("CPU:%d, Preempt disable\n", cpu);
	  	spin_lock(&wdt_test_lock1);
	  }
	  if(test_case == (cpu*10+2))//cpu0 Preempt&irq disale
	  {
	  		printk("CPU:%d, irq & Preempt disable\n", cpu);
	  		spin_lock_irqsave(&wdt_test_lock1, flags);
	  }	
  	msleep(5*1000);//5s
  	printk("wulin debug end, cpu:%d\n", cpu);
  	}
  	return 0;
}
static int start_kicker(void)
{

	int i;
	unsigned char name[10] = {0};
	

	for(i = 0; i < nr_cpu_ids; i++){
	sprintf(name, "wdtk-%d", i);
	printk("[WDK]:thread name: %s\n", name);
	wk_tsk[i] = kthread_create(kwdt_thread_test, &data, name);
	if (IS_ERR(wk_tsk[i])) {
		int ret = PTR_ERR(wk_tsk[i]);
		wk_tsk[i] = NULL;
		return ret;
	}
	kthread_bind(wk_tsk[i], i);
	wake_up_process(wk_tsk[i]);
}
	return 0;
}
static int __init test_init(void)
{
	//enable_clock(12, "Vfifo");
	start_kicker();
	return 0;
}

static void __init test_exit(void)
{	

}

module_init(test_init);
module_exit(test_exit);