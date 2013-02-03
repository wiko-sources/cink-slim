/* 
 *
 * (C) Copyright 2009 
 * MediaTek <www.mediatek.com>
 * James Lo <james.lo@mediatek.com>
 *
 * mt6329 I2C Device Driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/i2c.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/kthread.h>
#include <linux/wakelock.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/xlog.h>

#include <mach/mt6577_typedefs.h>

#include <asm/uaccess.h>

#include <mach/pmic_mt6329_hw.h>
#include <mach/pmic_mt6329_sw.h> 
#include <mach/upmu_common_sw.h>

#include <mach/mt6577_pm_ldo.h>

/**********************************************************
  *
  *   [Chip specific BUCKs list] 
  *
  *********************************************************/
upmu_buck_profile_entry upmu_buck_profile[] =
{
/* VPROC */     
	{	BANK0_VPROC_CON0,  
		32, 
		{						   
			UPMU_VOLT_0_7_0_0_V, UPMU_VOLT_0_7_2_5_V, UPMU_VOLT_0_7_5_0_V, UPMU_VOLT_0_7_7_5_V,
			UPMU_VOLT_0_8_0_0_V, UPMU_VOLT_0_8_2_5_V, UPMU_VOLT_0_8_5_0_V, UPMU_VOLT_0_8_7_5_V,
			UPMU_VOLT_0_9_0_0_V, UPMU_VOLT_0_9_2_5_V, UPMU_VOLT_0_9_5_0_V, UPMU_VOLT_0_9_7_5_V,
			UPMU_VOLT_1_0_0_0_V, UPMU_VOLT_1_0_2_5_V, UPMU_VOLT_1_0_5_0_V, UPMU_VOLT_1_0_7_5_V,
			UPMU_VOLT_1_1_0_0_V, UPMU_VOLT_1_1_2_5_V, UPMU_VOLT_1_1_5_0_V, UPMU_VOLT_1_1_7_5_V,
			UPMU_VOLT_1_2_0_0_V, UPMU_VOLT_1_2_2_5_V, UPMU_VOLT_1_2_5_0_V, UPMU_VOLT_1_2_7_5_V,
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V,
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V
       	}
	},
	
/* VCORE */    
	{	BANK0_VCORE_CON0,  
		32, 
		{						   
			UPMU_VOLT_0_7_0_0_V, UPMU_VOLT_0_7_2_5_V, UPMU_VOLT_0_7_5_0_V, UPMU_VOLT_0_7_7_5_V,
			UPMU_VOLT_0_8_0_0_V, UPMU_VOLT_0_8_2_5_V, UPMU_VOLT_0_8_5_0_V, UPMU_VOLT_0_8_7_5_V,
			UPMU_VOLT_0_9_0_0_V, UPMU_VOLT_0_9_2_5_V, UPMU_VOLT_0_9_5_0_V, UPMU_VOLT_0_9_7_5_V,
			UPMU_VOLT_1_0_0_0_V, UPMU_VOLT_1_0_2_5_V, UPMU_VOLT_1_0_5_0_V, UPMU_VOLT_1_0_7_5_V,
			UPMU_VOLT_1_1_0_0_V, UPMU_VOLT_1_1_2_5_V, UPMU_VOLT_1_1_5_0_V, UPMU_VOLT_1_1_7_5_V,
			UPMU_VOLT_1_2_0_0_V, UPMU_VOLT_1_2_2_5_V, UPMU_VOLT_1_2_5_0_V, UPMU_VOLT_1_2_7_5_V,
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V,
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_3_0_0_V
       	}
	},

/* VRF18 */    
	{	BANK0_VRF18_CON0,  
		32, 
		{						   
			UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_5_2_0_V, UPMU_VOLT_1_5_4_0_V, UPMU_VOLT_1_5_6_0_V,	UPMU_VOLT_1_5_8_0_V, 
			UPMU_VOLT_1_6_0_0_V, UPMU_VOLT_1_6_2_0_V, UPMU_VOLT_1_6_4_0_V, UPMU_VOLT_1_6_6_0_V, UPMU_VOLT_1_6_8_0_V, 
			UPMU_VOLT_1_7_0_0_V, UPMU_VOLT_1_7_2_0_V, UPMU_VOLT_1_7_4_0_V, UPMU_VOLT_1_7_6_0_V, UPMU_VOLT_1_7_8_0_V,
			UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_1_8_2_0_V, UPMU_VOLT_1_8_4_0_V, UPMU_VOLT_1_8_6_0_V, UPMU_VOLT_1_8_8_0_V,
			UPMU_VOLT_1_9_0_0_V, UPMU_VOLT_1_9_2_0_V, UPMU_VOLT_1_9_4_0_V, UPMU_VOLT_1_9_6_0_V, UPMU_VOLT_1_9_8_0_V,
			UPMU_VOLT_2_0_0_0_V, UPMU_VOLT_2_0_2_0_V, UPMU_VOLT_2_0_4_0_V, UPMU_VOLT_2_0_6_0_V, UPMU_VOLT_2_0_8_0_V,
			UPMU_VOLT_2_1_0_0_V, UPMU_VOLT_2_1_2_0_V
       	}
	},

/* VIO18 */
	{	BANK0_VM_CON0,  
		32, 
		{						   
			UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_5_2_0_V, UPMU_VOLT_1_5_4_0_V, UPMU_VOLT_1_5_6_0_V,	UPMU_VOLT_1_5_8_0_V, 
			UPMU_VOLT_1_6_0_0_V, UPMU_VOLT_1_6_2_0_V, UPMU_VOLT_1_6_4_0_V, UPMU_VOLT_1_6_6_0_V, UPMU_VOLT_1_6_8_0_V, 
			UPMU_VOLT_1_7_0_0_V, UPMU_VOLT_1_7_2_0_V, UPMU_VOLT_1_7_4_0_V, UPMU_VOLT_1_7_6_0_V, UPMU_VOLT_1_7_8_0_V,
			UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_1_8_2_0_V, UPMU_VOLT_1_8_4_0_V, UPMU_VOLT_1_8_6_0_V, UPMU_VOLT_1_8_8_0_V,
			UPMU_VOLT_1_9_0_0_V, UPMU_VOLT_1_9_2_0_V, UPMU_VOLT_1_9_4_0_V, UPMU_VOLT_1_9_6_0_V, UPMU_VOLT_1_9_8_0_V,
			UPMU_VOLT_2_0_0_0_V, UPMU_VOLT_2_0_2_0_V, UPMU_VOLT_2_0_4_0_V, UPMU_VOLT_2_0_6_0_V, UPMU_VOLT_2_0_8_0_V,
			UPMU_VOLT_2_1_0_0_V, UPMU_VOLT_2_1_2_0_V
       	}
	},
	
/* VPA */
	{	BANK0_VPA_CON0,  
		32, 
		{						   
			UPMU_VOLT_0_9_0_0_V, UPMU_VOLT_1_0_0_0_V, UPMU_VOLT_1_1_0_0_V, UPMU_VOLT_1_2_0_0_V,
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_4_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_6_0_0_V,
			UPMU_VOLT_1_7_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_1_9_0_0_V, UPMU_VOLT_2_0_0_0_V,
			UPMU_VOLT_2_1_0_0_V, UPMU_VOLT_2_2_0_0_V, UPMU_VOLT_2_3_0_0_V, UPMU_VOLT_2_4_0_0_V,
			UPMU_VOLT_2_5_0_0_V, UPMU_VOLT_2_6_0_0_V, UPMU_VOLT_2_7_0_0_V, UPMU_VOLT_2_8_0_0_V,
			UPMU_VOLT_2_9_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_1_0_0_V, UPMU_VOLT_3_2_0_0_V,
			UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_3_4_0_0_V, UPMU_VOLT_3_4_0_0_V, UPMU_VOLT_3_4_0_0_V,
			UPMU_VOLT_3_4_0_0_V, UPMU_VOLT_3_4_0_0_V, UPMU_VOLT_3_4_0_0_V, UPMU_VOLT_3_4_0_0_V
       	}
	}	
};

/**********************************************************
  *
  *   [Chip specific Digital & Analog LDOs list] 
  *
  *********************************************************/
upmu_ldo_profile_entry upmu_ldo_profile[] =
{
/* 0: VM12_1 */    
	{	BANK0_LDO_VM12_1,  
		1, 
		{						   
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
       	}
	},
	
/* 1: VM12_2 */    
	{	BANK0_LDO_VM12_2,  
		1, 
		{						   
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
       	}
	},	

/* 2: VM12_INT */	
	{	BANK0_LDO_VM12_INT,  
		1, 
		{						   
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 3: VIO28 */	
	{	BANK0_LDO_VIO28,  
		1, 
		{						   
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 4: VSIM */ 
	{	BANK0_LDO_VSIM,  
		2, 
		{						   
			UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 5: VSIM2 */ 
	{	BANK0_LDO_VSIM2,  
		8, 
		{						   
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V,
			UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_3_3_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 6: VUSB */ 
	{	BANK0_LDO_VUSB,  
		1, 
		{						   
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 7: VCAMD */ 
	{	BANK0_LDO_VCAMD,  
		8, 
		{						   
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V,
			UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_1_2_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 8: VCAM_IO */ 
	{	BANK0_LDO_VCAM_IO,  
		8, 
		{						   
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V,
			UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_3_3_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 9: VCAM_AF */ 
	{	BANK0_LDO_VCAM_AF,	
		8, 
		{						   
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V,
			UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_3_3_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 10: VMC */ 
	{	BANK0_LDO_VMC,	
		8, 
		{						   
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V,
			UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_3_3_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 11: VMCH */ 
	{	BANK0_LDO_VMCH,	
		8, 
		{						   
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V,
			UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_3_3_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 12: VGP */ 
	{	BANK0_LDO_VGP, 
		8, 
		{						   
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V,
			UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_3_3_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 13: VGP2 */ 
	{	BANK0_LDO_VGP2, 
		8, 
		{						   
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V,
			UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_3_3_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 14: VIBR */ 
	{	BANK0_LDO_VIBR, 
		8, 
		{						   
			UPMU_VOLT_1_3_0_0_V, UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V,
			UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_3_0_0_0_V, UPMU_VOLT_3_3_0_0_V, UPMU_VOLT_3_3_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 15: VRF */ 
	{	BANK0_LDO_VRF, 
		1, 
		{						   
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 16: VTCXO */ 
	{	BANK0_LDO_VTCXO, 
		1, 
		{						   
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 17: VA1 */ 
	{	BANK0_LDO_VA1, 
		4, 
		{						   
			UPMU_VOLT_2_5_0_0_V, UPMU_VOLT_2_1_0_0_V, UPMU_VOLT_2_0_0_0_V, UPMU_VOLT_1_8_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 18: VA2 */ 
	{	BANK0_LDO_VA2, 
		2, 
		{						   
			UPMU_VOLT_2_5_0_0_V, UPMU_VOLT_2_8_0_0_V, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 19: VCAMA */ 
	{	BANK0_LDO_VCAMA, 
		4, 
		{						   
			UPMU_VOLT_1_5_0_0_V, UPMU_VOLT_1_8_0_0_V, UPMU_VOLT_2_5_0_0_V, UPMU_VOLT_2_8_0_0_V,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	},

/* 20: VRTC */ 
	{	BANK0_LDO_VRTC, 
		1, 
		{						   
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX,
			UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX, UPMU_VOLT_MAX
		}
	}
	
};

/**********************************************************
  *
  *   [I2C Slave Setting] 
  *
  *********************************************************/
#if 0
#define mt6329_BANK0_SLAVE_ADDR_WRITE	0xC0
#define mt6329_BANK0_SLAVE_ADDR_Read	0xC1
#define mt6329_BANK1_SLAVE_ADDR_WRITE	0xC2
#define mt6329_BANK1_SLAVE_ADDR_Read	0xC3

/* Addresses to scan */
//static unsigned short normal_i2c[] = { mt6329_BANK0_SLAVE_ADDR_WRITE,  I2C_CLIENT_END };
//static unsigned short normal_i2c[] = { mt6329_BANK1_SLAVE_ADDR_WRITE,  I2C_CLIENT_END }; //test

//static unsigned short ignore = I2C_CLIENT_END;

static struct i2c_client *new_client = NULL;

static const struct i2c_device_id mt6329_i2c_id[] = {{"mt6329_pmic",0},{}};   
//static unsigned short force[] = {2, 0xC0, I2C_CLIENT_END, I2C_CLIENT_END}; // id=2 for using I2C dual
static unsigned short force[] = {2, mt6329_BANK0_SLAVE_ADDR_WRITE, I2C_CLIENT_END, I2C_CLIENT_END}; // id=2 for using I2C dual
//static unsigned short force[] = {2, mt6329_BANK1_SLAVE_ADDR_WRITE, I2C_CLIENT_END, I2C_CLIENT_END}; // id=2 for using I2C dual // test

static const unsigned short * const forces[] = { force, NULL };              
static struct i2c_client_address_data addr_data = { .forces = forces,}; 
#endif

//static int mt6329_driver_detect(struct i2c_client *client, int kind, struct i2c_board_info *info);
//static int mt6329_driver_probe(struct i2c_client *client, const struct i2c_device_id *id);

static struct i2c_client *new_client = NULL;

/**********************************************************
  *
  *   [I2C Function For Read/Write PMIC] 
  *
  *********************************************************/

static DEFINE_MUTEX(pmic_i2c_access_mutex_0);

ssize_t mt6329_read_byte(u8 cmd, u8 *returnData)
{
    char     cmd_buf[1]={0x00};
    char     readData = 0;
    int     ret=0;

	mutex_lock(&pmic_i2c_access_mutex_0);
	
#if 0
    cmd_buf[0] = cmd;
    ret = i2c_master_send(new_client, &cmd_buf[0], 1);
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_read_byte] PMIC sends command error!! \n");	
        return 0;
    }
    ret = i2c_master_recv(new_client, &readData, 1);
    if (ret < 0) 
	{	
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_read_byte] PMIC reads data error!! \n");	
        return 0;
    } 
    //xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_read_byte] Reg[%x]=0x%x \n", cmd, readData);
    *returnData = readData;
#else
	new_client->addr = ((new_client->addr) & I2C_MASK_FLAG) | I2C_WR_FLAG;	

	cmd_buf[0] = cmd;
    ret = i2c_master_send(new_client, &cmd_buf[0], (1<<8 | 1));
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_read_byte:write&read] PMIC sends command error!! \n");	
		new_client->addr = new_client->addr & I2C_MASK_FLAG;
		mutex_unlock(&pmic_i2c_access_mutex_0);
        return 0;
    }
	readData = cmd_buf[0];
    //xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[mt6329_read_byte:write&read] Reg[%x]=0x%x \n", cmd, readData);
    *returnData = readData;

	new_client->addr = new_client->addr & I2C_MASK_FLAG;
#endif	
    
	mutex_unlock(&pmic_i2c_access_mutex_0);
    return 1;
}

int mt6329_multi_read_byte(u8 cmd, u8 *returnData, int len)
{
    char cmd_buf[1]={0x00};
    char readData[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
	int index=0;
    int ret=0;

    cmd_buf[0] = cmd;
    ret = i2c_master_send(new_client, &cmd_buf[0], 1);
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_multi_read_byte] PMIC sends command error!! \n");	
        return 0;
    } 
	ret = i2c_master_recv(new_client, readData, len);
    if (ret < 0) {
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_multi_read_byte] PMIC reads data error!! \n");
        return 0;
    }
	
    for(index=0 ; index<len ; index++)
    {
    	returnData[index] = readData[index];
    }
    
    return 1;
}

ssize_t mt6329_write_byte(u8 cmd, u8 writeData)
{
    char    write_data[2] = {0};
    int    ret=0;
    
	mutex_lock(&pmic_i2c_access_mutex_0);
	
    write_data[0] = cmd;
    write_data[1] = writeData;
    
    ret = i2c_master_send(new_client, write_data, 2);
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_write_byte] sends command error!! \n");
		mutex_unlock(&pmic_i2c_access_mutex_0);
        return 0;
    }
    
	mutex_unlock(&pmic_i2c_access_mutex_0);
    return 1;
}

int mt6329_multi_write_byte(u8 *cmd, u8 *writeData, int len)
{
    char write_data[8] = {0};
	int count=0;
	int index=0;
    int ret=0;

	for(count=0 ; count<len ; count++)
	{
    	write_data[index] = cmd[count];
		index++;
    	write_data[index] = writeData[count];
		index++;
	}

	//check
	for(index=0 ; index<8 ; index++)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[mt6329_mult_write_byte] %x\n", write_data[index]);
	}
	
    ret = i2c_master_send(new_client, write_data, len*2);
    if (ret < 0) 
	{
        xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[mt6329_mult_write_byte] sends command error!! \n");
        return 0;
    }
    
    return 1;
}

/**********************************************************
  *
  *   [PMIC Variable] 
  *
  *********************************************************/
kal_uint8 pmic6329_reg[PMIC_REG_NUM] = {0};

/**********************************************************
  *
  *   [PMIC Function] 
  *
  *********************************************************/
extern int mt_i2c_polling_read(int port, unsigned char addr, unsigned char *buffer, int len);
extern int mt_i2c_polling_write(int port, unsigned char addr, unsigned char *buffer, int len);
extern int mt_i2c_polling_writeread(int port, unsigned char addr, unsigned char *buffer, int write_len, int read_len);

int g_switch_to_i2c_polling_mode=0;
//int g_switch_to_i2c_polling_mode=1;

kal_uint32 pmic_read_interface (kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	//kal_uint32 ret_code = PMIC_TEST_PASS;
	kal_uint8 pmic6329_reg = 0;

	kal_uint8 chip_slave_address = 0xC0;
    kal_uint8 buffer = 0x0;    
    int result_tmp;

if(g_switch_to_i2c_polling_mode == 0)
{
	mt6329_read_byte(RegNum, &pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_read_interface] Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
	
	pmic6329_reg &= (MASK << SHIFT);
	*val = (pmic6329_reg >> SHIFT);
	
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_read_interface] val=0x%x\n", *val);
}
else
{
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "$r");
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_read_interface] g_switch_to_i2c_polling_mode = 1\r\n");
	
	buffer = RegNum;	    
	result_tmp = mt_i2c_polling_writeread(2,chip_slave_address,&buffer,1,1);

    buffer &= (MASK << SHIFT);
    *val = (buffer >> SHIFT);

	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_read_interface] Reg[0x%x]=0x%x, val=0x%x\n", RegNum, buffer, *val);
}

	return PMIC_TEST_PASS;
}

kal_uint32 pmic_config_interface (kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	//kal_uint32 ret_code = PMIC_TEST_PASS;
	kal_uint8 pmic6329_reg = 0;

	kal_uint8 chip_slave_address = 0xC0;
    kal_uint8 buffer = 0x0;    
    int result_tmp;
	kal_uint8 write_data[2];

if(g_switch_to_i2c_polling_mode == 0)
{
	mt6329_read_byte(RegNum, &pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_config_interface] Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
	
	pmic6329_reg &= ~(MASK << SHIFT);
	pmic6329_reg |= (val << SHIFT);

	mt6329_write_byte(RegNum, pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_config_interface] write Reg[%x]=0x%x\n", RegNum, pmic6329_reg);

	// Check
	//mt6329_read_byte(RegNum, &pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_config_interface] Check Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
}
else
{
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "$c");
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_config_interface] g_switch_to_i2c_polling_mode = 1\r\n");
	
	buffer = RegNum;
	result_tmp = mt_i2c_polling_writeread(2,chip_slave_address,&buffer,1,1);
	
	buffer &= ~(MASK << SHIFT);
	buffer |= (val << SHIFT);
	pmic6329_reg = buffer;

	write_data[0] = RegNum;
    write_data[1] = pmic6329_reg;
	
	result_tmp = mt_i2c_polling_write(2,chip_slave_address,write_data,2);

	//Check
	//buffer = RegNum;
	//result_tmp = mt_i2c_polling_writeread(2,chip_slave_address,&buffer,1,1);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_config_interface] Reg[0x%x]=0x%x, val=0x%x\n", RegNum, buffer, pmic6329_reg);	
}

	return PMIC_TEST_PASS;
}

kal_uint32 pmic_int_en_interface (kal_uint8 RegNum, kal_uint8 sel, kal_bool enable)
{
	//kal_uint32 ret_code = PMIC_TEST_PASS;
	kal_uint8 pmic6329_reg = 0;

	mt6329_read_byte(RegNum, &pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_int_en_interface] Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
	
	if (enable){
		pmic6329_reg |= (U8)sel;
	}else{
		pmic6329_reg &= ~((U8)sel);
	}
	
	mt6329_write_byte(RegNum, pmic6329_reg);
	//xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_int_en_interface] write Reg[%x]=0x%x\n", RegNum, pmic6329_reg);
	
	// Check
	mt6329_read_byte(RegNum, &pmic6329_reg);
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_int_en_interface] Check Reg[%x]=0x%x\n", RegNum, pmic6329_reg);

	return PMIC_TEST_PASS;
}

void pmic_get_chip_version_v2(void)
{
    kal_uint16 eco_version = 0;
    kal_uint8 tmp8;
    //kal_bool result_tmp;
    
    /* Low part of CID */
    //result_tmp = mt6329_read_byte(CID_1_REG_INDEX, &tmp8);
    tmp8=upmu_get_cid0();
    eco_version |= tmp8;

    /* High part of CID */
    //result_tmp = mt6329_read_byte(CID_2_REG_INDEX, &tmp8);
    tmp8=upmu_get_cid1();
    eco_version |= (tmp8 << 8);

#if 1	
    if (eco_version == PMIC6326_E1_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_v2] PMIC6326_ECO_1_VERSION !!\n ");
    }
    else if (eco_version == PMIC6326_E2_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_v2] PMIC6326_ECO_2_VERSION !!\n ");
    }
    else if (eco_version == PMIC6326_E3_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_v2] PMIC6326_ECO_3_VERSION !!\n ");
    }
    else if (eco_version == PMIC6326_E4_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_v2] PMIC6326_ECO_4_VERSION !!\n ");
    }
    else
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version_v2] UNKNOWN VERSION !!\n ");
    }
#endif	
	
}

void pmic_get_chip_version(void)
{
    kal_uint16 eco_version = 0;
    kal_uint8 tmp8;
    kal_bool result_tmp;
    
    /* Low part of CID */
	result_tmp = mt6329_read_byte(CID_1_REG_INDEX, &tmp8);	
    eco_version |= tmp8;

    /* High part of CID */
    result_tmp = mt6329_read_byte(CID_2_REG_INDEX, &tmp8);	
    eco_version |= (tmp8 << 8);

#if 1	
    if (eco_version == PMIC6326_E1_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version] PMIC6326_ECO_1_VERSION !!\n ");
    }
    else if (eco_version == PMIC6326_E2_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version] PMIC6326_ECO_2_VERSION !!\n ");
    }
    else if (eco_version == PMIC6326_E3_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version] PMIC6326_ECO_3_VERSION !!\n ");
    }
    else if (eco_version == PMIC6326_E4_CID_CODE)
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version] PMIC6326_ECO_4_VERSION !!\n ");
    }
    else
    {
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_get_chip_version] UNKNOWN VERSION !!\n ");
    }
#endif	
	
}

void pmic_dump_register(void)
{
	int i=0;
    for (i=0;i<PMIC_REG_NUM;i++)
	{
		mt6329_read_byte(i, &pmic6329_reg[i]);
        xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[pmic_dump_register] Reg[0x%X]=0x%X\n", i, pmic6329_reg[i]);        
    }
}

//==============================================================================
// Test Code : Start
//==============================================================================
int pmic_interface_test(void)
{	
	kal_uint32 ret=0;
	kal_uint8 temp_val;

	//For MT6573
#if 1	
	ret=pmic_read_interface(0x0, &temp_val, 0xFF, 0x0);
	if(temp_val == 0xb)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test] pmic_read_interface : PASS\n");		
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test] pmic_read_interface : FAIL(%d)\n", temp_val);
		return 0;
	}
	
	ret=pmic_config_interface(0x81, 0x3, 0x7, 0x4);
	ret=pmic_read_interface(0x81, &temp_val, 0x7, 0x4);
	if(temp_val == 0x3)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test] pmic_config_interface : PASS\n");
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test] pmic_config_interface : FAIL(%d)\n", temp_val);
		return 0;
	}

	ret=pmic_int_en_interface(PMIC_INIT_CTRL_1, INT1_EN_ALL, KAL_FALSE);
	ret=pmic_read_interface(PMIC_INIT_CTRL_1, &temp_val, 0xFF, 0x0);
	if(temp_val == 0x0)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test] pmic_int_en_interface : PASS\n");
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test] pmic_int_en_interface : FAIL(%d)\n", temp_val);
		return 0;
	}

	ret=pmic_int_en_interface(PMIC_INIT_CTRL_1, INT1_EN_ALL, KAL_TRUE);
	ret=pmic_read_interface(PMIC_INIT_CTRL_1, &temp_val, 0xFF, 0x0);
	if(temp_val == 0xFF)
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test] pmic_int_en_interface : PASS\n");
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_interface_test] pmic_int_en_interface : FAIL(%d)\n", temp_val);
		return 0;
	}
#endif

	return 0;
}


void pmic_buck_interface_test(void)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_buck_interface_test] \n");
	
	//upmu_buck_csl(BUCK_VPROC, 0x2);

	upmu_buck_vosel(BUCK_VPROC, UPMU_VOLT_0_7_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");	
	upmu_buck_vosel(BUCK_VPROC, UPMU_VOLT_1_0_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	upmu_buck_vosel(BUCK_VPROC, UPMU_VOLT_1_3_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");

	upmu_buck_vosel(BUCK_VCORE, UPMU_VOLT_0_7_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	upmu_buck_vosel(BUCK_VCORE, UPMU_VOLT_1_0_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	upmu_buck_vosel(BUCK_VCORE, UPMU_VOLT_1_3_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");

	upmu_buck_vosel(BUCK_VRF18, UPMU_VOLT_1_5_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	upmu_buck_vosel(BUCK_VRF18, UPMU_VOLT_1_9_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	upmu_buck_vosel(BUCK_VRF18, UPMU_VOLT_2_1_2_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");

	upmu_buck_vosel(BUCK_VIO18, UPMU_VOLT_1_5_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	upmu_buck_vosel(BUCK_VIO18, UPMU_VOLT_1_9_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	upmu_buck_vosel(BUCK_VIO18, UPMU_VOLT_2_1_2_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");

	upmu_buck_vosel(BUCK_VPA, UPMU_VOLT_0_9_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	upmu_buck_vosel(BUCK_VPA, UPMU_VOLT_2_1_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	upmu_buck_vosel(BUCK_VPA, UPMU_VOLT_3_4_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "##\n");
	
}

void pmic_ldo_interface_test(void)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","[pmic_ldo_interface_test] \n");
	
	upmu_ldo_en(LDO_VM12_1		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VM12_2		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VM12_INT	  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VIO28		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VSIM		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VSIM2		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VUSB		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VCAMD		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VCAM_IO 	  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VCAM_AF 	  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VMC 		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VMCH		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VGP 		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VGP2		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VIBR		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VRF 		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VTCXO		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VA1 		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VA2 		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VCAMA		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_en(LDO_VRTC		  , 0x1);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");

	upmu_ldo_vosel(LDO_VSIM, UPMU_VOLT_3_0_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VSIM2, UPMU_VOLT_3_0_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_2_5_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VCAM_IO, UPMU_VOLT_1_8_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VCAM_AF, UPMU_VOLT_2_8_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VMC, UPMU_VOLT_1_5_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VMCH, UPMU_VOLT_1_8_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VGP, UPMU_VOLT_1_8_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VGP2, UPMU_VOLT_2_8_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VIBR, UPMU_VOLT_3_3_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VA1, UPMU_VOLT_2_1_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VA2, UPMU_VOLT_2_8_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	upmu_ldo_vosel(LDO_VCAMA, UPMU_VOLT_2_5_0_0_V);xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC","##\n");
	
	
}

void pmic_auxadc_test(void)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_auxadc_test] \n");

	upmu_auxadc_get_ch0_data_7_0();	
	upmu_auxadc_get_ch0_ready();
	upmu_auxadc_get_ch0_data_9_8();	
}

void pmic_chr_test(void)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_chr_test] \n");
	
	upmu_chr_ovp_trim(0x7);
}
//==============================================================================
// Test Code : End
//==============================================================================

//==============================================================================
// DCT PMIC APIs : Start
//==============================================================================
void dct_pmic_VM12_1_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VM12_1_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VM12_1,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VM12_1,0x0);
	}
}

void dct_pmic_VM12_2_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VM12_2_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VM12_2,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VM12_2,0x0);
	}
}

void dct_pmic_VM12_INT_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VM12_INT_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VM12_INT,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VM12_INT,0x0);
	}	
}

void dct_pmic_VIO28_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VIO28_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VIO28,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VIO28,0x0);
	}	
}

void dct_pmic_VSIM_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VSIM_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VSIM,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VSIM,0x0);
	}	
}

void dct_pmic_VSIM2_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VSIM2_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VSIM2,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VSIM2,0x0);
	}	
}

void dct_pmic_VUSB_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VUSB_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VUSB,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VUSB,0x0);
	}	
}

void dct_pmic_VCAMD_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VCAMD_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VCAMD,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VCAMD,0x0);
	}
}

void dct_pmic_VCAM_IO_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VCAM_IO_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VCAM_IO,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VCAM_IO,0x0);
	}
}

void dct_pmic_VCAM_AF_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VCAM_AF_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VCAM_AF,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VCAM_AF,0x0);
	}
}

void dct_pmic_VMC_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VMC_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VMC,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VMC,0x0);
	}
}

void dct_pmic_VMCH_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VMCH_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VMCH,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VMCH,0x0);
	}
}

void dct_pmic_VGP_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VGP_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VGP,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VGP,0x0);
	}
}

void dct_pmic_VGP2_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VGP2_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VGP2,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VGP2,0x0);
	}
}

void dct_pmic_VIBR_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VIBR_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VIBR,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VIBR,0x0);
	}
}

void dct_pmic_VRF_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VRF_enable] %d\n", dctEnable);

	upmu_ldo_on_ctrl(LDO_VRF, 0x0);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VRF,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VRF,0x0);
	}
}

void dct_pmic_VTCXO_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VTCXO_enable] %d\n", dctEnable);

	upmu_ldo_on_ctrl(LDO_VTCXO, 0x0);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VTCXO,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VTCXO,0x0);
	}
}

void dct_pmic_VA1_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VA1_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VA1,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VA1,0x0);
	}
}

void dct_pmic_VA2_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VA2_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VA2,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VA2,0x0);
	}
}

void dct_pmic_VCAMA_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VCAMA_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VCAMA,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VCAMA,0x0);
	}
}

void dct_pmic_VRTC_enable(kal_bool dctEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VRTC_enable] %d\n", dctEnable);

	if(dctEnable == KAL_TRUE)
	{
		upmu_ldo_en(LDO_VRTC,0x1);
	}
	else
	{
		upmu_ldo_en(LDO_VRTC,0x0);
	}
}

//==============================================================================

void dct_pmic_VM12_1_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VM12_1_sel] No vlotage can setting on E1 !\n");
}

void dct_pmic_VM12_2_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VM12_2_sel] No vlotage can setting on E1 !\n");
}

void dct_pmic_VM12_INT_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VM12_INT_sel] No vlotage can setting on E1 !\n");
}

void dct_pmic_VIO28_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VIO28_sel] No vlotage can setting on E1 !\n");
}

void dct_pmic_VSIM_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VSIM_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VSIM, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){		
		upmu_ldo_vosel(LDO_VSIM, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VSIM, UPMU_VOLT_3_0_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VSIM2_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VSIM2_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VSIM2, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_3_0_0_V){		
		upmu_ldo_vosel(LDO_VSIM2, UPMU_VOLT_1_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VSIM2, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VSIM2, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VSIM2, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VSIM2, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VSIM2, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_3_0_0_V){
		upmu_ldo_vosel(LDO_VSIM2, UPMU_VOLT_3_3_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VUSB_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VUSB_sel] No vlotage can setting on E1 !\n");
}

void dct_pmic_VCAMD_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VCAMD_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_3_0_0_V){		
		upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_1_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_3_0_0_V){
		upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_3_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_2_0_0_V){
		upmu_ldo_vosel(LDO_VCAMD, UPMU_VOLT_1_2_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VCAM_IO_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VCAM_IO_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VCAM_IO, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_3_0_0_V){		
		upmu_ldo_vosel(LDO_VCAM_IO, UPMU_VOLT_1_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_IO, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_IO, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_IO, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_IO, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_IO, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_3_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_IO, UPMU_VOLT_3_3_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VCAM_AF_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VCAM_AF_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VCAM_AF, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_3_0_0_V){		
		upmu_ldo_vosel(LDO_VCAM_AF, UPMU_VOLT_1_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_AF, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_AF, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_AF, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_AF, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_AF, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_3_0_0_V){
		upmu_ldo_vosel(LDO_VCAM_AF, UPMU_VOLT_3_3_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VMC_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VMC_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VMC, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_3_0_0_V){		
		upmu_ldo_vosel(LDO_VMC, UPMU_VOLT_1_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VMC, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VMC, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VMC, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VMC, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VMC, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_3_0_0_V){
		upmu_ldo_vosel(LDO_VMC, UPMU_VOLT_3_3_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_LDO_VMCH_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_LDO_VMCH_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VMCH, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_3_0_0_V){		
		upmu_ldo_vosel(LDO_VMCH, UPMU_VOLT_1_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VMCH, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VMCH, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VMCH, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VMCH, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VMCH, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_3_0_0_V){
		upmu_ldo_vosel(LDO_VMCH, UPMU_VOLT_3_3_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VGP_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VGP_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VGP, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_3_0_0_V){		
		upmu_ldo_vosel(LDO_VGP, UPMU_VOLT_1_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VGP, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VGP, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VGP, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VGP, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VGP, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_3_0_0_V){
		upmu_ldo_vosel(LDO_VGP, UPMU_VOLT_3_3_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VGP2_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VGP2_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VGP2, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_3_0_0_V){		
		upmu_ldo_vosel(LDO_VGP2, UPMU_VOLT_1_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VGP2, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VGP2, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VGP2, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VGP2, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VGP2, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_3_0_0_V){
		upmu_ldo_vosel(LDO_VGP2, UPMU_VOLT_3_3_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VIBR_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VIBR_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VIBR, UPMU_VOLT_3_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_3_0_0_V){		
		upmu_ldo_vosel(LDO_VIBR, UPMU_VOLT_1_3_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VIBR, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VIBR, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VIBR, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VIBR, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_0_0_0_V){
		upmu_ldo_vosel(LDO_VIBR, UPMU_VOLT_3_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_3_3_0_0_V){
		upmu_ldo_vosel(LDO_VIBR, UPMU_VOLT_3_3_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VRF_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VRF_sel] No vlotage can setting on E1 !\n");
}

void dct_pmic_VTCXO_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VTCXO_sel] No vlotage can setting on E1 !\n");
}

void dct_pmic_VA1_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VA1_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VA1, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VA1, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_1_0_0_V){
		upmu_ldo_vosel(LDO_VA1, UPMU_VOLT_2_1_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_0_0_0_V){
		upmu_ldo_vosel(LDO_VA1, UPMU_VOLT_2_0_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VA1, UPMU_VOLT_1_8_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VA2_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VA2_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VA2, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VA2, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VA2, UPMU_VOLT_2_8_0_0_V);
	}	
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VCAMA_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "****[dct_pmic_VCAMA_sel] value=%d \n", volt);

	if(volt == VOL_DEFAULT)	{
		upmu_ldo_vosel(LDO_VCAMA, UPMU_VOLT_2_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_5_0_0_V){
		upmu_ldo_vosel(LDO_VCAMA, UPMU_VOLT_1_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_1_8_0_0_V){
		upmu_ldo_vosel(LDO_VCAMA, UPMU_VOLT_1_8_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_5_0_0_V){
		upmu_ldo_vosel(LDO_VCAMA, UPMU_VOLT_2_5_0_0_V);
	}
	else if(volt == UPMU_VOLT_2_8_0_0_V){
		upmu_ldo_vosel(LDO_VCAMA, UPMU_VOLT_2_8_0_0_V);
	}
	else{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "Error Setting %d. DO nothing.\r\n", volt);
	}
}

void dct_pmic_VRTC_sel(kal_uint32 volt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[dct_pmic_VRTC_sel] No vlotage can setting on E1 !\n");
}

//==============================================================================
// DCT PMIC APIs : End
//==============================================================================

void pmic_ldo_enable(MT65XX_POWER powerId, kal_bool powerEnable)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_ldo_enable] Receive powerId %d, action is %d\n", powerId, powerEnable);

	//Need integrate with DCT : using DCT APIs

	if(powerId == MT65XX_POWER_LDO_VM12_1)			{	dct_pmic_VM12_1_enable(powerEnable);	}
	else if(powerId == MT65XX_POWER_LDO_VM12_2)		{	dct_pmic_VM12_2_enable(powerEnable);	}
	else if(powerId == MT65XX_POWER_LDO_VM12_INT)	{	dct_pmic_VM12_INT_enable(powerEnable);	}
	else if(powerId == MT65XX_POWER_LDO_VIO28)		{	dct_pmic_VIO28_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VSIM)		{	dct_pmic_VSIM_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VSIM2)		{	dct_pmic_VSIM2_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VUSB)		{	dct_pmic_VUSB_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VCAMD)		{	dct_pmic_VCAMD_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VCAM_IO)	{	dct_pmic_VCAM_IO_enable(powerEnable);	}
	else if(powerId == MT65XX_POWER_LDO_VCAM_AF)	{	dct_pmic_VCAM_AF_enable(powerEnable);	}
	else if(powerId == MT65XX_POWER_LDO_VMC)		{	dct_pmic_VMC_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VMCH)		{	dct_pmic_VMCH_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VGP)		{	dct_pmic_VGP_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VGP2)		{	dct_pmic_VGP2_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VIBR)		{	dct_pmic_VIBR_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VRF)		{	dct_pmic_VRF_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VTCXO)		{	dct_pmic_VTCXO_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VA1)		{	dct_pmic_VA1_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VA2)		{	dct_pmic_VA2_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VCAMA)		{	dct_pmic_VCAMA_enable(powerEnable);		}
	else if(powerId == MT65XX_POWER_LDO_VRTC)		{	dct_pmic_VRTC_enable(powerEnable);		}
	else
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[pmic_ldo_enable] UnKnown powerId (%d)\n", powerId);
	}		
}

void pmic_ldo_vol_sel(MT65XX_POWER powerId, MT65XX_POWER_VOLTAGE powerVolt)
{
	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[pmic_ldo_vol_sel] Receive powerId %d, action is %d\n", powerId, powerVolt);

	//Need integrate with DCT : using DCT APIs

	if(powerId == MT65XX_POWER_LDO_VM12_1)			{	dct_pmic_VM12_1_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VM12_2)		{	dct_pmic_VM12_2_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VM12_INT)	{	dct_pmic_VM12_INT_sel(powerVolt);	}
	else if(powerId == MT65XX_POWER_LDO_VIO28)		{	dct_pmic_VIO28_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VSIM)		{	dct_pmic_VSIM_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VSIM2)		{	dct_pmic_VSIM2_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VUSB)		{	dct_pmic_VUSB_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VCAMD)		{	dct_pmic_VCAMD_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VCAM_IO)	{	dct_pmic_VCAM_IO_sel(powerVolt);	}
	else if(powerId == MT65XX_POWER_LDO_VCAM_AF)	{	dct_pmic_VCAM_AF_sel(powerVolt);	}
	else if(powerId == MT65XX_POWER_LDO_VMC)		{	dct_pmic_VMC_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VMCH)		{	dct_pmic_LDO_VMCH_sel(powerVolt);	}
	else if(powerId == MT65XX_POWER_LDO_VGP)		{	dct_pmic_VGP_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VGP2)		{	dct_pmic_VGP2_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VIBR)		{	dct_pmic_VIBR_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VRF)		{	dct_pmic_VRF_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VTCXO)		{	dct_pmic_VTCXO_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VA1)		{	dct_pmic_VA1_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VA2)		{	dct_pmic_VA2_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VCAMA)		{	dct_pmic_VCAMA_sel(powerVolt);		}
	else if(powerId == MT65XX_POWER_LDO_VRTC)		{	dct_pmic_VRTC_sel(powerVolt);		}
	else
	{
		xlog_printk(ANDROID_LOG_WARN, "Power/PMIC", "[pmic_ldo_ldo_vol_sel] UnKnown powerId (%d)\n", powerId);
	}
}

#if 0
static int mt6329_driver_detect(struct i2c_client *client, int kind, struct i2c_board_info *info) 
{         
    strcpy(info->type, "mt6329_pmic");                                                         

	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_driver_detect] \n");
	
    return 0;                                                                                       
}
#endif

static int mt6329_driver_probe(struct i2c_client *client, const struct i2c_device_id *id) 
{             
	  //int err=0;

    xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_driver_probe] !\n");
#if 0
	if (!(new_client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL))) {
        err = -ENOMEM;
        goto exit;
    }	
    memset(new_client, 0, sizeof(struct i2c_client));
#endif
	new_client = client;
	
    return 0;                                                                                       

//exit_kfree:
//    kfree(new_client);
//exit:
//    return err;

}

static int mt6329_driver_remove(struct i2c_client *client)
{
	kfree(new_client);
	return 0;
}

static const struct i2c_device_id mt6329_i2c_id[] = 
{
	{"mt6329_pmic",0},
	{}
};
//	MODULE_DEVICE_TABLE(i2c, mt6329_i2c_id);

struct i2c_driver mt6329_driver = {                       
    .probe = mt6329_driver_probe,                                   
	.remove = mt6329_driver_remove,	
	.driver = {
		.name = "mt6329_pmic",
	},
    .id_table = mt6329_i2c_id,                             
};

#define PMIC_I2C_BUSNUM 2
static struct i2c_board_info __initdata pmic_dev = { I2C_BOARD_INFO("mt6329_pmic", 0x60) };

static int __init mt6329_init(void)
{		
	xlog_printk(ANDROID_LOG_INFO, "Power/PMIC", "[mt6329_init] init start\n");	
	i2c_register_board_info(PMIC_I2C_BUSNUM, &pmic_dev, 1);
	i2c_add_driver(&mt6329_driver);	
	return 0;		
}

static void __exit mt6329_exit(void)
{
	i2c_del_driver(&mt6329_driver);
}

module_init(mt6329_init);
module_exit(mt6329_exit);
   
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MediaTek I2C mt6329 Driver");
MODULE_AUTHOR("James Lo<james.lo@mediatek.com>");

