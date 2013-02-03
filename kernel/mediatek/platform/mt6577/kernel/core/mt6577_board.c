#include <linux/init.h>
#include <linux/delay.h>
#include "mach/mt6577_devs.h"
#include "mach/mt6577_pm_common.h"

/*
 * board_init: entry point for board initialization.
 * Always return 1.
 */
extern void custom_board_init(void);
static __init int board_init(void)
{
    /*
     * NoteXXX: There are two board init related functions.
     *          One is mt65xx_board_init() and another is custom_board_init().
     *
     *          mt65xx_board_init() is used for chip-dependent code.
     *          It is suggested to put driver code in this function to do:
     *          1). Capability structure of platform devices.
     *          2). Define platform devices with their resources.
     *          3). Register MT65XX platform devices.
     *
     *          custom_board_init() is used for customizable code.
     *          It is suggested to put driver code in this function to do:
     *          1). Initialize board (PINMUX, GPIOs).
     *          2). Perform board specific initialization:
     *              1-1). Register MT65xx platform devices.
     *              1-2). Register non-MT65xx platform devices.
     *                    (e.g. external peripheral GPS, BT, ¡K etc.)
     */
    
    // power management initialization
#ifndef CONFIG_MT6577_FPGA
   mt6577_power_management_init();
#endif  

# if 1
    custom_board_init();
#endif
    
   mt6577_board_init();

    /* init low power PMIC setting after PMIC is ready */
#ifndef CONFIG_MT6577_FPGA
    mt6577_pmic_low_power_init();
#endif
    /* BUG BUG: temporarily marked since customization is not ready yet */


    return 0;
}

late_initcall(board_init);

