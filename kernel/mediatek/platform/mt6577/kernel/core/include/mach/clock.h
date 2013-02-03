/*
 *  linux/arch/arm/mach-versatile/clock.h
 *
 *  Copyright (C) 2004 ARM Limited.
 *  Written by Deep Blue Solutions Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _CLOCK_H
#define _CLOCK_H

#include <asm/hardware/icst.h>

struct clk {
	unsigned long		rate;
	const struct icst_params *params;
	u32			oscoff;
	void			*data;
	void			(*setvco)(struct clk *, struct icst_vco vco);
};

#endif
