#if !defined(__AEE_H__)
#define __AEE_H__

#include <linux/kernel.h>

#define AEE_MODULE_NAME_LENGTH 64
#define AEE_PROCESS_NAME_LENGTH 256
#define AEE_BACKTRACE_LENGTH 4096

typedef enum {
    AE_DEFECT_FATAL,
    AE_DEFECT_EXCEPTION,
    AE_DEFECT_WARNING,
    AE_DEFECT_REMINDING,
    AE_DEFECT_ATTR_END
} AE_DEFECT_ATTR;

typedef enum {
    AE_KE = 0, /* Fatal Exception */
    AE_NE, 
    AE_JE, 
    AE_SWT,
    AE_EE, 
    AE_EXP_ERR_END,
    AE_ANR, /* Error or Warning or Defect */
    AE_RESMON,  
    AE_WRN_ERR_END,
    AE_MANUAL, /* Manual Raise */
    AE_EXP_CLASS_END,

    AE_KERNEL_PROBLEM_REPORT = 1000,
    AE_PROCESS_PROBLEM_REPORT,
} AE_EXP_CLASS; /* General Program Exception Class */

struct aee_oops
{
	AE_DEFECT_ATTR attr;
	AE_EXP_CLASS clazz;

	char module[AEE_MODULE_NAME_LENGTH];
	char process_path[AEE_PROCESS_NAME_LENGTH];
	char backtrace[AEE_BACKTRACE_LENGTH];

	char *detail;
	int detail_len;

	char *console;
	int console_len;

	char *android_main;
	int android_main_len;
	char *android_radio;
	int android_radio_len;
	char *android_system;
	int android_system_len;

	char *userspace_info;
	int  userspace_info_len;
	
	int dump_option;
};

struct aee_kernel_api {
    void (*kernel_exception)(const int db_opt, const char *module, const char *msg);
    void (*kernel_warning)(const int db_opt, const char *module, const char *msg);
    void (*kernel_reminding)(const int db_opt, const char *module, const char *msg);
    void (*md_exception1)(const int *log, int log_size, const int *phy, int phy_size, 
        const char *assert_type, const char *exp_filename, unsigned int exp_linenum, 
        unsigned int fatal1, unsigned int fatal2);
    void (*md_exception2)(const int *log, int log_size, const int *phy, int phy_size, 
        const char *detail);
};

#define RR_CPU_COUNT 2

/* 
   This group of API call by sub-driver module to report reboot reasons
   aee_rr_* stand for previous reboot reason
 */
struct last_reboot_reason {
	uint8_t     wdt_status;
	uint8_t     shutdown_mode;
	uint8_t     in_idle;

	uint32_t    jiffies_current;
	uint32_t    jiffies_idle;
	uint32_t    jiffies_wdk_kick;

	uint32_t    last_irq_enter[RR_CPU_COUNT];
	uint64_t    jiffies_last_irq_enter[RR_CPU_COUNT];

	uint32_t    last_irq_exit[RR_CPU_COUNT];
	uint64_t    jiffies_last_irq_exit[RR_CPU_COUNT];

	uint64_t    jiffies_last_sched[RR_CPU_COUNT];
};

void aee_rr_last(struct last_reboot_reason *lrr);

void aee_rr_rec_current_jiffies(u32 j);
void aee_rr_rec_wdk_kick_jiffies(u32 j);
void aee_rr_rec_idle_jiffies(u32 j);
void aee_rr_rec_shutdown_mode(u8 mode);
void aee_rr_rec_last_irq_enter(int cpu, int irq, u64 j);
void aee_rr_rec_last_irq_exit(int cpu, int irq, u64 j);
void aee_rr_rec_last_sched_jiffies(int cpu, u64 j);

void aee_sram_printk(const char *fmt, ...);

void aee_wdt_irq_info(void);
void aee_wdt_fiq_info(void *arg, void *regs, void *svc_sp);
void aee_trigger_kdb(void);
struct aee_oops *aee_oops_create(AE_DEFECT_ATTR attr, AE_EXP_CLASS clazz, const char *module);
void aee_oops_set_backtrace(struct aee_oops *oops, const char *backtrace);
void aee_oops_set_process_path(struct aee_oops *oops, const char *process_path);
void aee_oops_free(struct aee_oops *oops);

/* DB dump option bits, set relative bit to 1 to include related file in db */
#define DB_OPT_DEFAULT				(0)
#define DB_OPT_FTRACE				(1<<0)
#define DB_OPT_STORAGE_LOG			(1<<1)
#define DB_OPT_NE_JBT_TRACES		(1<<2)
#define DB_OPT_SWT_JBT_TRACES		(1<<3)
#define DB_OPT_VM_TRACES			(1<<4)
#define DB_OPT_DUMPSYS_ACTIVITY		(1<<5)
#define DB_OPT_DUMPSYS_WINDOW		(1<<6)
#define DB_OPT_DUMPSYS_GFXINFO		(1<<7)
#define DB_OPT_BT_RTT_DUMP			(1<<8)
#define DB_OPT_LOW_MEMORY_KILLER	(1<<9)
#define DB_OPT_PROC_MEM				(1<<10)


#define aee_kernel_exception(module, msg...)	\
	aee_kernel_exception_api(__FILE__, __LINE__, DB_OPT_DEFAULT, module, msg)
#define aee_kernel_warning(module, msg...)	\
	aee_kernel_warning_api(__FILE__, __LINE__, DB_OPT_DEFAULT, module, msg)
#define aee_kernel_reminding(module, msg...)	\
	aee_kernel_reminding_api(__FILE__, __LINE__, DB_OPT_DEFAULT, module, msg)

void aee_kernel_exception_api(const char *file, const int line, const int db_opt, const char *module, const char *msg, ...);
void aee_kernel_warning_api(const char *file, const int line, const int db_opt, const char *module, const char *msg, ...);
void aee_kernel_reminding_api(const char *file, const int line, const int db_opt, const char *module, const char *msg, ...);

void aed_md_exception1(const int *log, int log_size, const int *phy, int phy_size, const char *assert_type, const char *exp_filename, unsigned int exp_linenum, unsigned int fatal1, unsigned int fatal2);

#if defined(CONFIG_MTK_AEE_IPANIC)
/* Begin starting panic record */
void ipanic_oops_start(const char *str, int err, struct task_struct *task);
void ipanic_oops_end(void);
#else
#define ipanic_oops_start(str, err, task)
#define ipanic_oops_end()
#endif

#endif // __AEE_H__
