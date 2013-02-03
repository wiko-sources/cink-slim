#ifndef __MT6577_DEVS_H__
#define __MT6577_DEVS_H__

#include <board-custom.h>
#include <mach/board.h>

#define CFG_DEV_UART1
#define CFG_DEV_UART2

#ifndef CONFIG_MT6577_FPGA
#define CFG_DEV_UART3
#define CFG_DEV_UART4
#endif

/*
 * Define constants.
 */

#define MTK_UART_SIZE 0x100

/*
 * Define function prototype.
 */

extern int mt6577_board_init(void);

#endif  /* !__MT6577_DEVS_H__ */

