#include <linux/linkage.h>
#include <linux/init.h>

    __CPUINIT

ENTRY(mt_secondary_startup)
    mov r0, #0

    /* invaliate I-Cache */
    mcr p15, 0, r0, c7, c5, 0
    dsb

    /* invalidate I + D TLBs */
    mcr p15, 0, r0, c8, c7, 0

    /* invaliate D-Cache */
    mov r0, #0
    mov r1, #3
    mov r2, #0xFF
1:
    mov r1, #3
2:
    mov r0, #0
    orr r0, r0, r1, lsl #30
    orr r0, r0, r2, lsl #5
    mcr p15, 0, r0, c7, c6, #2
    subs r1, r1, #1
    bge 2b
    subs r2, r2, #1
    bge 1b
    dsb
    isb

    b secondary_startup
END(mt_secondary_startup)

