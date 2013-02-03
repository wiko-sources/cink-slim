#ifdef CONFIG_MTK_LOAD_BALANCE_PROFILER

#include <linux/poll.h>
#include <mtlbprof/mtlbprof.h>

#define MT_LBPROF_VERSION 2

DECLARE_WAIT_QUEUE_HEAD(mt_lbprof_log_wait);

#define MT_LBPROF_LOG_ACTION_CLOSE          0
#define MT_LBPROF_LOG_ACTION_OPEN           1
#define MT_LBPROF_LOG_ACTION_READ           2
/* Return number of unread characters in the log buffer */
#define MT_LBPROF_LOG_ACTION_SIZE_UNREAD    3

#define MT_LBPROF_LOG_SIZE (1024*64)
char mt_lbprof_log[MT_LBPROF_LOG_SIZE];
static int lblog_rd = 0, lblog_wt = 0;
static DEFINE_SPINLOCK(mt_lbprof_spinlock);
int mt_lbprof_flags = 3;

void __mt_lbprof_printf(char *str)
{
	unsigned long long ts;
	char msg[MT_LBPROF_STR_SIZE];
	int len, log_cnt;
	
	ts = sched_clock();
	spin_lock(&mt_lbprof_spinlock);

	sprintf(msg, "%10Lu.%06lu %s", SPLIT_NS(ts), str);

	len = strlen(msg); 
	log_cnt = 0;
	while(log_cnt < len ){	
		mt_lbprof_log[lblog_wt] = msg[log_cnt];

		log_cnt ++ ;
		lblog_wt++;
		if(lblog_wt >= MT_LBPROF_LOG_SIZE){
			lblog_wt = 0;
		}
	}

	spin_unlock(&mt_lbprof_spinlock);
}

#define MT_LBPROC_ENTRY(name) \
static int mt_##name##_show(struct seq_file *m, void *v);\
static int mt_##name##_write(struct file *filp, const char *ubuf, size_t cnt, loff_t *data);\
static unsigned int mt_##name##_poll(struct file *file, poll_table *wait); \
static int mt_##name##_open(struct inode *inode, struct file *file) \
{ \
    return single_open(file, mt_##name##_show, inode->i_private); \
} \
\
static const struct file_operations mt_##name##_fops = { \
    .open = mt_##name##_open, \
    .write = mt_##name##_write,\
    .read = seq_read, \
    .poll = mt_##name##_poll, \
    .llseek = seq_lseek, \
    .release = single_release, \
};\

/* 1. /proc/mtlbprof/flags*/
MT_LBPROC_ENTRY(lbprof_flags);
static int mt_lbprof_flags_show(struct seq_file *seq, void *v)
{
	seq_printf(seq,"%d\n", mt_lbprof_flags); /*ytchang*/

	return 0;
}

static ssize_t mt_lbprof_flags_write(struct file *filp, const char *ubuf,
           size_t cnt, loff_t *data)
{
	char buf[MT_LBPROF_STR_SIZE];
	size_t copy_size = cnt;
	int flags;

	if (cnt >= sizeof(buf)) {
		copy_size = MT_LBPROF_STR_SIZE - 1;
	}

	if (copy_from_user(&buf, ubuf, copy_size))
		return -EFAULT;

	if (1 != sscanf(buf, "%d", &flags)) 
		return -EPERM;		/*ytchang*/

	if (flags & ~MT_LBPROF_ALLENABLE)
		return -EPERM; /*ytchang*/

	spin_lock(&mt_lbprof_spinlock);
	mt_lbprof_flags = flags;
	spin_unlock(&mt_lbprof_spinlock);

	return cnt;
}

static unsigned int mt_lbprof_flags_poll(struct file *file, poll_table *wait)
{
	return 0;
}

/* 2. /proc/mtlbprof/log*/
//MT_LBPROC_ENTRY(lbprof_log);

static int mt_lbprof_log_open(struct inode * inode, struct file * file);
int mt_lbprof_do_log(int type, char __user *buf, int len);
static ssize_t mt_lbprof_log_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos);
static unsigned int mt_lbprof_log_poll(struct file *file, poll_table *wait);
static int mt_lbprof_log_release(struct inode * inode, struct file * file);
static const struct file_operations mt_lbprof_log_fops = {
	.open		= mt_lbprof_log_open,
	.read		= mt_lbprof_log_read,
	.poll		= mt_lbprof_log_poll,
	.release	= mt_lbprof_log_release,
	.llseek		= generic_file_llseek,
};


static int mt_lbprof_log_open(struct inode * inode, struct file * file)
{
	return mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_OPEN, NULL, 0);	
}

int mt_lbprof_do_log(int type, char __user *buf, int len)
{
	int error = 0, i, log_cnt=0;
	
	switch (type) {
	case MT_LBPROF_LOG_ACTION_CLOSE:	/* Close log */
		break;
	case MT_LBPROF_LOG_ACTION_OPEN:	/* Open log */
		break;
	case MT_LBPROF_LOG_ACTION_READ:	/* Read from log */
		error = -EINVAL;
		if (!buf || len < 0)
			goto out;	
		error = 0;
		if (!len)
			goto out;
		if (!access_ok(VERIFY_WRITE, buf, len)) {
			error = -EFAULT;
			goto out;
		}
					
		error = wait_event_interruptible(mt_lbprof_log_wait,
			(lblog_rd - lblog_wt));

		if (error)
			goto out;

		while (!error && (lblog_rd != lblog_wt) && (log_cnt < len ) ){			
			error = __put_user(mt_lbprof_log[lblog_rd], &buf[log_cnt]);
			if(error)
				break;
						
			lblog_rd++;
			if(lblog_rd >= MT_LBPROF_LOG_SIZE)
				lblog_rd = 0;
							
			log_cnt++;
			cond_resched();
		}		
						
		if (!error)
			error = log_cnt;	

		break;																					
	/* Number of chars in the log buffer */
	case MT_LBPROF_LOG_ACTION_SIZE_UNREAD:
		error = lblog_wt - lblog_rd;
		break;
	default:
		error = -EINVAL;
		break;	
	}
out:
	return error;
}

static ssize_t mt_lbprof_log_read(struct file *file, char __user *buf,
			 size_t count, loff_t *ppos)
{
	if ((file->f_flags & O_NONBLOCK) &&
	    !mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_SIZE_UNREAD, NULL, 0))
		return -EAGAIN;
	return mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_READ, buf, count);	
}


static unsigned int mt_lbprof_log_poll(struct file *file, poll_table *wait)
{
	poll_wait(file, &mt_lbprof_log_wait, wait);
	if (mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_SIZE_UNREAD, NULL, 0))
		return POLLIN | POLLRDNORM;
	return 0;	
}

static int mt_lbprof_log_release(struct inode * inode, struct file * file)
{
	(void) mt_lbprof_do_log(MT_LBPROF_LOG_ACTION_CLOSE, NULL, 0);
	return 0;
}


/* 3. /proc/mtlbprof/lbstat*/
MT_LBPROC_ENTRY(lbprof_lbstat);
static int mt_lbprof_lbstat_show(struct seq_file *seq, void *v)
{
	int cpu;
	seq_printf(seq, "----------------------------------------------------------\n");
	seq_printf(seq, "		Load balance PROF (version %d)\n", MT_LBPROF_VERSION);
	seq_printf(seq, "----------------------------------------------------------\n");
	seq_printf(seq, "timestamp(ms) %10Lu.%06lu\n", SPLIT_NS(sched_clock()));
	
	for_each_online_cpu (cpu) {

		struct sched_domain *sd;
		int dcount = 0;

		/* domain-specific stats */
		seq_printf(seq,"cpu%d\n", cpu);				
		rcu_read_lock();
		for_each_domain (cpu, sd) {
			enum cpu_idle_type itype;
			seq_printf(seq, "domain%d \n", dcount++);
			for (itype = CPU_IDLE; itype < CPU_MAX_IDLE_TYPES; itype++) {
				seq_printf(seq, " %10u %10u %10u %10u %10u %10u",
					sd->lb_count[itype],
					sd->lb_balanced[itype],
					sd->lb_failed[itype],
					sd->lb_gained[itype],
					sd->lb_nobusyq[itype],
					sd->lb_nobusyg[itype]);

				seq_printf(seq, " %10u %10u %10u %10u %10u %10u %10u %10u\n",
					sd->mt_lbprof_nobusyg_no_larger_than[itype],
					sd->mt_lbprof_nobusyg_enough_capacity[itype],
					sd->mt_lbprof_nobusyg_check_fail[itype],
					sd->mt_lbprof_nobusyg_check_max_load[itype],
					sd->mt_lbprof_nobusyg_check_this_load[itype],
					sd->mt_lbprof_failed_migrations_affine[itype],
					sd->mt_lbprof_failed_migrations_running[itype],
					sd->mt_lbprof_failed_migrations_hot[itype]);

			}
			seq_printf(seq,
			   " %10u %10u\n %10u %10u\n %10u %10u \n",
				sd->mt_lbprof_fork_count, sd->mt_lbprof_fork_balanced,
				sd->mt_lbprof_exec_count, sd->mt_lbprof_exec_balanced,
				sd->mt_lbprof_wake_count, sd->mt_lbprof_wake_balanced);
		}
		rcu_read_unlock();
	}
	
	return 0;
}

static ssize_t mt_lbprof_lbstat_write(struct file *filp, const char *ubuf,
           size_t cnt, loff_t *data)
{
	return cnt;
}

static unsigned int mt_lbprof_lbstat_poll(struct file *file, poll_table *wait)
{
	return 0;
}

/* ---------------------------------------------------------- */
int __init mt_lbprof_proc_init(void)
{
	struct proc_dir_entry *pe;

	pe = proc_create("mtprof/lb_flags", 0644, NULL, &mt_lbprof_flags_fops);
	if (!pe)
		return -ENOMEM;

	pe = proc_create("mtprof/lb_log", 0444, NULL, &mt_lbprof_log_fops);
	if (!pe)
		return -ENOMEM;

	pe = proc_create("mtprof/lb_stats", 0444, NULL, &mt_lbprof_lbstat_fops);
	if (!pe)
		return -ENOMEM;

	return 0;
}
late_initcall(mt_lbprof_proc_init);

#ifndef arch_idle_time
#define arch_idle_time(cpu) 0
#endif

struct cpuload_stat {
	struct cpu_usage_stat cpustat;
	int load;
	unsigned long last_update;
};

static DEFINE_PER_CPU(struct cpuload_stat, old_cpuload_stat);

#define TRIMz(x)  ((tz = (unsigned long long)(x)) < 0 ? 0 : tz)
void mt_lbprof_calc_cpuload(void)
{	
	cputime64_t user, nice, system, idle, iowait, irq, softirq, steal;
	unsigned long long u, u_frme, s, s_frme, n, n_frme, i, w, x, x_frme, y, y_frme, z, z_frme, tz;
	unsigned i_frme, w_frme, tot_frme;
	int load, cpu, online_cpu=0;
	struct cpuload_stat *old_cpuload;
	unsigned int lb_count = 0, lb_balanced = 0, lb_failed = 0, lb_gained = 0, lb_nobusyq = 0, lb_nobusyg = 0;
	unsigned int mt_lbprof_nobusyg_no_larger_than = 0, mt_lbprof_nobusyg_enough_capacity = 0, mt_lbprof_nobusyg_check_fail = 0;
	unsigned int mt_lbprof_failed_migrations_affine = 0, mt_lbprof_failed_migrations_running = 0, mt_lbprof_failed_migrations_hot = 0;					

	for_each_online_cpu(cpu) { /*ytchang*/
//		printk(KERN_DEBUG "debug %u %u %u\n", cpu, jiffies,lb_cpu_stat_sav_cpu(cpu).last_update +100);
//		if(time_before(jiffies, lb_cpu_stat_sav_cpu(cpu).last_update + 100)){
		old_cpuload = &per_cpu(old_cpuload_stat, cpu);
		if (jiffies < (old_cpuload->last_update + 100)) 	/* ytchang */
			return;
	}

	for_each_online_cpu(cpu) {
		old_cpuload = &per_cpu(old_cpuload_stat, cpu);
		online_cpu++;
		user = nice = system = idle = iowait =
			irq = softirq = steal = cputime64_zero;
	//	guest = guest_nice = cputime64_zero;
		
		user = kstat_cpu(cpu).cpustat.user;
		nice = kstat_cpu(cpu).cpustat.nice;
		system = kstat_cpu(cpu).cpustat.system;
		idle = kstat_cpu(cpu).cpustat.idle;
		idle = cputime64_add(idle, arch_idle_time(cpu));
		iowait = kstat_cpu(cpu).cpustat.iowait;
		irq = kstat_cpu(cpu).cpustat.irq;
		softirq = kstat_cpu(cpu).cpustat.softirq;
		steal = kstat_cpu(cpu).cpustat.steal;
	//	guest = kstat_cpu(this_cpu).cpustat.guest;
	//	guest_nice = cputime64_add(guest_nice,
	//	kstat_cpu(this_cpu).cpustat.guest_nice);

		u = (unsigned long long)cputime64_to_clock_t(user);
		n = (unsigned long long)cputime64_to_clock_t(nice);
		s = (unsigned long long)cputime64_to_clock_t(system);
		i = (unsigned long long)cputime64_to_clock_t(idle);
		w = (unsigned long long)cputime64_to_clock_t(iowait);
		x = (unsigned long long)cputime64_to_clock_t(irq);
		y = (unsigned long long)cputime64_to_clock_t(softirq);
		z = (unsigned long long)cputime64_to_clock_t(steal);
	//	guest_time=(unsigned long long)cputime64_to_clock_t(guest);

	  	u_frme = u - old_cpuload->cpustat.user;
	  	s_frme = s - old_cpuload->cpustat.system;
	  	n_frme = n - old_cpuload->cpustat.nice;
	  	i_frme = TRIMz(i - old_cpuload->cpustat.idle);
	  	w_frme = w - old_cpuload->cpustat.iowait;
	  	x_frme = x - old_cpuload->cpustat.irq;
  		y_frme = y - old_cpuload->cpustat.softirq;
		z_frme = z - old_cpuload->cpustat.steal;

		tot_frme = u_frme + s_frme + n_frme + i_frme + w_frme + x_frme + y_frme + z_frme;
		if (tot_frme < 1) {
			load = old_cpuload->load; // if tot_frme not changed, use load at last time
			//iowa[id] = iowa_sav[id];
		} else {
			load = 10000 - ((i_frme + w_frme)* 10000 / tot_frme );
			//load=10000-((idle + iowait)* 10000 / tot_frme );
			//iowa[id] = w_frme[id] / (float)tot_frme[id] * 100.0;
		}
		old_cpuload->cpustat.user  = u;
		old_cpuload->cpustat.system = s;
		old_cpuload->cpustat.nice = n;
		old_cpuload->cpustat.idle = i;
		old_cpuload->cpustat.iowait = w;
		old_cpuload->cpustat.irq = x;
		old_cpuload->cpustat.softirq = y;
		old_cpuload->cpustat.steal = z;
		old_cpuload->load = load;
		old_cpuload->last_update = jiffies;		
	}


	for_each_online_cpu (cpu) {
		struct sched_domain *sd;
		enum cpu_idle_type itype;
		rcu_read_lock();
		for_each_domain (cpu, sd) {
			for (itype = CPU_IDLE; itype < CPU_MAX_IDLE_TYPES; itype++) {
				lb_count += sd->lb_count[itype];
				lb_balanced += sd->lb_balanced[itype];
				lb_failed += sd->lb_failed[itype];
				lb_gained += sd->lb_gained[itype];
				lb_nobusyq += sd->lb_nobusyq[itype];
				lb_nobusyg += sd->lb_nobusyg[itype];
				mt_lbprof_nobusyg_no_larger_than += sd->mt_lbprof_nobusyg_no_larger_than[itype];
				mt_lbprof_nobusyg_enough_capacity += sd->mt_lbprof_nobusyg_enough_capacity[itype];
				mt_lbprof_nobusyg_check_fail += sd->mt_lbprof_nobusyg_check_fail[itype];
				mt_lbprof_failed_migrations_affine += sd->mt_lbprof_failed_migrations_affine[itype];
				mt_lbprof_failed_migrations_running += sd->mt_lbprof_failed_migrations_running[itype];
				mt_lbprof_failed_migrations_hot += sd->mt_lbprof_failed_migrations_hot[itype];
			}
		}
		rcu_read_unlock();				
	}				
	
	/*
	mt_lbprof_printf(MT_LBPROF_CPULOAD, "load: %5u %5u\n",
		per_cpu(old_cpuload_stat, 0).load, per_cpu(old_cpuload_stat, 1).load);
	*/
			
	mt_lbprof_printf(MT_LBPROF_CPULOAD, "load: %5u %5u %6u %6u %6u %6u %6u %6u %6u %6u %6u %6u %6u %6u\n",
		per_cpu(old_cpuload_stat, 0).load, per_cpu(old_cpuload_stat, 1).load,
		lb_count, 
		lb_balanced, 
		lb_failed, 
		lb_gained,
		lb_nobusyq,
		lb_nobusyg,
		mt_lbprof_nobusyg_no_larger_than,
		mt_lbprof_nobusyg_enough_capacity,
		mt_lbprof_nobusyg_check_fail,
		mt_lbprof_failed_migrations_affine,
		mt_lbprof_failed_migrations_running,
		mt_lbprof_failed_migrations_hot);		
}

void mt_lbprof_tick(void)
{
	wake_up_interruptible(&mt_lbprof_log_wait);
}

#else /* CONFIG_MTK_LOAD_BALANCE_PROFILER */

void mt_lbprof_calc_cpuload(void)
{
}

void mt_lbprof_tick(void)
{
}

#endif /* CONFIG_MTK_LOAD_BALANCE_PROFILER */