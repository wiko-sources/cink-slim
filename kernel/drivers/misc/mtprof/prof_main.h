
#include <linux/sched.h>

struct mt_proc_struct{
    int pid;
    int tgid;
    int index;
    u64 cputime;
    u64 cputime_init;
    u64 prof_start;
    u64 prof_end;
	u64 cost_cputime;
	u32 cputime_percen_6;
	u64 isr_time;
	int   isr_count;
	struct mtk_isr_info  *mtk_isr;

    cputime_t utime_init;
    cputime_t utime;
    cputime_t stime_init;
    cputime_t stime;
    char comm[TASK_COMM_LEN];
    struct mt_proc_struct * next;
};
// #define PROC_NUM 512
