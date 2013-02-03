#ifndef _MT6577_DORMANT_
#define _MT6577_DORMANT_

#define MAX_CPUS 1

/* 
 * Power status
 */
#define STATUS_RUN      0
#define STATUS_STANDBY  1
#define STATUS_DORMANT  2
#define STATUS_SHUTDOWN 3
#define DORMANT_MODE  STATUS_DORMANT
#define SHUTDOWN_MODE STATUS_SHUTDOWN


/* input value:
 * DORMANT_MODE / SHUTDOWN_MODE
 *
 * return value: 
 * 0 : execute wfi then power down
 * 1 : wake up and resume
 */
extern int cpu_power_down(int mode);
extern void cpu_dormant_init(void);
extern void cpu_dormant_abort(void);

#endif

