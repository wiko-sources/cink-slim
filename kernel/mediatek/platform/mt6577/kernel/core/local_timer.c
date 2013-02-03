#include <linux/init.h>
#include <linux/smp.h>
#include <linux/clockchips.h>
#include <asm/irq.h>
#include <asm/smp_twd.h>
#include <asm/localtimer.h>


/*
 * Setup the local clock events for a CPU.
 */
int __cpuinit local_timer_setup(struct clock_event_device *evt)
{
	/* Local timers are not supprted on OMAP4430 ES1.0 */
	evt->irq = GIC_PPI_PRIVATE_TIMER;
	twd_timer_setup(evt);
	return 0;
}




