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
#if !defined(__CA9_SLT__)
#define __CA9_SLT__

#define SLT_LOOP_CNT    (10000)
#define SLT_VA (0x70000000)
//#define __SLT_USE_GFP__

extern int slt_get_section(const unsigned int iCP);
extern void slt_free_section(void);

#endif  /*  __CA9_SLT__ */
