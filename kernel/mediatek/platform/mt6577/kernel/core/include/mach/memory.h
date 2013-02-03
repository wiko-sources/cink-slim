#ifndef __MT6577_MEMORY_H__
#define __MT6577_MEMORY_H__

/*
 * Define constants.
 */
#if defined (MODEM_3G)
#define PHYS_OFFSET 0x01600000
#elif defined (MODEM_2G)
#define PHYS_OFFSET 0x00A00000
#else
#define PHYS_OFFSET 0x01600000
#endif

/*
 * Define macros.
 */

/* IO_VIRT = 0xF0000000 | IO_PHYS[27:0] */
#define IO_VIRT_TO_PHYS(v) (0xC0000000 | ((v) & 0x0fffffff))

/*  audio analog hw register, [0xFD114000:0xFD114FFF] <-> [0xD1140000:0xD1140FFF]   */
#define IO_ABB_MDSYS_VIRT_TO_PHYS(v) (0xD1140000 | ((v) & 0x00000fff))

#define IO_VER_VIRT_TO_PHYS(v) (v)

#endif  /* !__MT6577_MEMORY_H__ */
