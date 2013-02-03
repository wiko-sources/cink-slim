#include <asm/mach/time.h>
#include <mach/timer.h>
#include <asm/smp_twd.h>

extern struct mt65xx_clock mtk_gpt;

struct mt65xx_clock *mtk_clocks[] =
{
    &mtk_gpt,
};

static void __init mtk_timer_init(void)
{
    int i;
    struct mt65xx_clock *clock;
    int ret;
    #ifdef CONFIG_LOCAL_TIMERS
		//twd_base = ioremap(0xF000a600, SZ_256);
		twd_base = (void __iomem*)0xF000a600;
    #endif

    for (i = 0; i < ARRAY_SIZE(mtk_clocks); i++) {
        clock = mtk_clocks[i];

        clock->init_func();

        if (clock->clocksource.name) {
            ret = clocksource_register(&(clock->clocksource));
            if (ret) {
                printk(KERN_ERR "mtk_timer_init: clocksource_register failed for %s\n", clock->clocksource.name);
            }
        }

        ret = setup_irq(clock->irq.irq, &(clock->irq));
        if (ret) {
            printk(KERN_ERR "mtk_timer_init: setup_irq failed for %s\n", clock->irq.name);
        }

        if (clock->clockevent.name)
            clockevents_register_device(&(clock->clockevent));
    }
}

struct sys_timer mt6577_timer = {
    .init = mtk_timer_init,
};

