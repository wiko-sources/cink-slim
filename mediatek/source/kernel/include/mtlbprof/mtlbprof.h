#ifndef _MTLBPROF_MTLBPROF_H
#define _MTLBPROF_MTLBPROF_H

#ifdef CONFIG_MTK_LOAD_BALANCE_PROFILER
#define MT_LBPROF_STR_SIZE 128

#define MT_LBPROF_TASK 		1
#define MT_LBPROF_CPULOAD 	2
#define MT_LBPROF_LBSTATUS 	4
#define MT_LBPROF_ALLENABLE 7
/*
Load balance fail reason
*/
#define MT_LBPROF_SUCCESS  					0
#define MT_LBPROF_NO_TRIGGER				-1
#define MT_LBPROF_NOBUSYG_NO_LARGER_THAN	1
#define MT_LBPROF_NOBUSYG_ENOUGH_CAPACITY	2
#define MT_LBPROF_NOBUSYG_CHECK_FAIL		3
#define MT_LBPROF_NOBUSYQ					4
#define MT_LBPROF_FAILED					5

extern int mt_lbprof_flags;

void __mt_lbprof_printf(char *str);
#define mt_lbprof_printf(type, x...)         \
do {                       \
    if (type & mt_lbprof_flags) {		\
		char msg[MT_LBPROF_STR_SIZE];    	 \
    	snprintf(msg, MT_LBPROF_STR_SIZE, x); 	\
    	__mt_lbprof_printf(msg);       \
    	/*printk(KERN_DEBUG x); */         \
    }	\
} while (0)

#define mt_lbprof_stat_inc(rq, field)		do { (rq)->field++; } while (0)
#define mt_lbprof_stat_add(rq, field, amt)	do { (rq)->field += (amt); } while (0)
#define mt_lbprof_stat_set(var, val)		do { var = (val); } while (0)

#else  /* CONFIG_MTK_LOAD_BALANCE_PROFILER */

#define mt_lbprof_printf(type, x...) 		do { } while (0)
#define mt_lbprof_stat_inc(rq, field)		do { } while (0)
#define mt_lbprof_stat_add(rq, field, amt)	do { } while (0)
#define mt_lbprof_stat_set(var, val)		do { } while (0)

#endif /* CONFIG_MTK_LOAD_BALANCE_PROFILER */

extern void mt_lbprof_calc_cpuload(void);
extern void mt_lbprof_tick(void);

#endif 
