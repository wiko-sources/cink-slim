/*******************************************************************************
 *
 * Filename:
 * ---------
 *   auddrv_headset.h
 *
 * Project:
 * --------
 *   Android Audio driver
 *
 * Description:
 * ------------
 *   Headset driver header
 *
 * Author:
 * -------
 *   Chipeng    (mtk02308)
 *   Stan Huang (mtk01728)
 *
 *
 *------------------------------------------------------------------------------
 * $Revision$
 * $Modtime:$
 * $Log:$
 *
 *******************************************************************************/


#ifndef _AUDDRV_HEADSET_H_
#define _AUDDRV_HEADSET_H_

/*****************************************************************************
*                E X T E R N A L   R E F E R E N C E S
******************************************************************************
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/workqueue.h>
#include <mach/mt6577_typedefs.h>
#include <linux/switch.h>
#include "auddrv_def.h"


/*****************************************************************************
*                          C O N S T A N T S
******************************************************************************
*/

/*****************************************************************************
*                         D A T A   T Y P E S
******************************************************************************
*/

/*****************************************************************************
*                        F U N C T I O N   D E F I N I T I O N
******************************************************************************
*/
//void Sound_Headset_Set_Gpio(void);
//void Sound_Headset_Unset_Gpio(void);

#endif


