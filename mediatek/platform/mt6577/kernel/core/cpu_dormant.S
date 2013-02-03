#include <asm/memory.h>
#include <mach/mt6577_reg_base.h>

        .equ CA9_SCU_ICD, 0x1000                    @@ GIC Distributor offset from SCU_BASE
        .equ CA9_SCU_ICC, 0x100                     @@ GIC CPU Interface offset from SCU_BASE
        .equ CA9_SCU_TIM64, 0x200                   @@ (64-bit) timer block offset from SCU_BASE

        @ SCU_BASE offsets:
        .equ SCU_Ctl, 0x0
        .equ SCU_Config, 0x4
        .equ SCU_PwrStatus, 0x8
        .equ SCU_InvAll, 0xC
        .equ SCU_FiltStart, 0x40
        .equ SCU_FiltEnd, 0x44
        .equ SCU_SAC, 0x50
        .equ SCU_SSAC, 0x54

        @ Global timer offsets
        .equ TIM64_CntLo, 0x0
        .equ TIM64_CntHi, 0x4
        .equ TIM64_Ctl, 0x8
        .equ TIM64_Status, 0xC
        .equ TIM64_CmpLo, 0x10
        .equ TIM64_CmpHi, 0x14
        .equ TIM64_AutoInc, 0x18

        @ Aliases for mode encodings - do not change
        .equ MODE_USR, 0x10
        .equ MODE_FIQ, 0x11
        .equ MODE_IRQ, 0x12
        .equ MODE_SVC, 0x13
        .equ MODE_ABT, 0x17
        .equ MODE_UND, 0x1B
        .equ MODE_SYS, 0x1F

        .equ MODE_MON, 0x16 @@ A-profile (Security Extensions) only
        .equ SCR_NS, 0x01 @@ A-profile (Security Extensions) only

        @ Maintain correlation between these offsets and struct appf_cpu_context in appf_types.h

        .equ ENDIAN_OFFSET, 0x00 @@ flag for CPSR.E bit status
        .equ ACTLR_OFFSET, 0x04 @@ CP15 register content
        .equ SCTLR_OFFSET, 0x08 @@ CP15 register content
        .equ CPACR_OFFSET, 0x0C @@ CP15 register content

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global save_performance_monitors

save_performance_monitors:
        .func

        PUSH    {r4, r8, r9, r10}

        @ Ignore:
        @        Count Enable Clear Register
        @        Software Increment Register
        @        Interrupt Enable Clear Register

        MRC     p15, 0, r8, c9, c12, 0          @ PMon: Control Register
        BIC     r1, r8, #1
        MCR     p15, 0, r1, c9, c12, 0	        @ disable counter updates from here
        ISB                                     @ 0b0 => PMCR<0>
        MRC     p15, 0, r9, c9, c12, 3          @ PMon: Overflow Flag Status Reg
        MRC     p15, 0, r10, c9, c12, 5         @ PMon: Event Counter Selection Reg
        STM     r0!, {r8-r10}
        UBFX    r9, r8, #11, #5                 @ extract # of event counters, N
        TST     r9, r9
        BEQ     1f

0:
        SUBS    r9, r9, #1                      @ decrement N
        MCR     p15, 0, r9, c9, c12, 5          @ PMon: select CounterN
        ISB
        MRC     p15, 0, r3, c9, c13, 1          @ PMon: save Event Type register
        MRC     p15, 0, r4, c9, c13, 2          @ PMon: save Event Counter register
        STM     r0!, {r3, r4}
        BNE     0b

1:
        MRC     p15, 0, r1, c9, c13, 0          @ PMon: Cycle Count Register
        MRC     p15, 0, r2, c9, c14, 0          @ PMon: User Enable Register
        MRC     p15, 0, r3, c9, c14, 1          @ PMon: Interrupt Enable Set Reg
        MRC     p15, 0, r4, c9, c12, 1          @ PMon: Count Enable Set Register
        STM     r0!, {r1-r4}

        POP     {r4, r8, r9, r10}
        bx      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_performance_monitors

restore_performance_monitors:
        .func

        PUSH    {r4-r5, r8-r10, lr}
        @ NOTE: all counters disabled by PMCR<0> == 0 on reset

        @ Restore performance counters
        LDM     r0!, {r8-r10}                   @ recover first block of PMon context
                                                @ (PMCR, PMOVSR, PMSELR)
        MOV     r1, #0                          @ generate register of all 0's
        MVN     r2, #0                          @ generate register of all 1's
        MCR     p15, 0, r2, c9, c14, 2          @ disable all counter related interrupts
        MCR     p15, 0, r2, c9, c12, 3          @ clear all overflow flags
        ISB

        UBFX    r12, r8, #11, #5                @ extract # of event counters, N (0-31)
        TST     r12, r12
        BEQ     20f
        MOV     r3, r12	                        @ for N >0, generate a 2nd copy of N
        MOV     r4, #1
        LSL     r4, r4, r3
        SUB     r4, r4, #1                      @ set bits<N-1:0> to all 1's

0:
        SUBS    r3, r3, #1                      @ decrement N
        MCR     p15, 0, r3, c9, c12, 5          @ select Event CounterN
        ISB
        MRC     p15, 0, r5, c9, c13, 1          @ read Event Type register
        BFC     r5, #0, #8
        MCR     p15, 0, r5, c9, c13, 1          @ set Event Type to 0x0
        MCR     p15, 0, r2, c9, c13, 2          @ set Event Counter to all 1's
        ISB
        BNE     0b

        MOV     r3, #1
        BIC     r5, r9, #1<<31
        MCR	p15, 0, r5, c9, c12, 1          @ enable Event Counters
                                                @ (PMOVSR bits set)
        MCR     p15, 0, r3, c9, c12, 0          @ set the PMCR global enable bit
        ISB
        MCR     p15, 0, r9, c9, c12, 4          @ set event count overflow bits
        ISB
        MCR     p15, 0, r4, c9, c12, 2          @ disable Event Counters

        @ restore the event counters
10:
        SUBS    r12, r12, #1                    @ decrement N
        MCR     p15, 0, r12, c9, c12, 5         @ select Event CounterN
        ISB
        LDM     r0!, {r3-r4}
        MCR     p15, 0, r3, c9, c13, 1          @ restore Event Type
        MCR     p15, 0, r4, c9, c13, 2          @ restore Event Counter
        ISB
        BNE     10b

20:
        TST     r9, #0x80000000                 @ check for cycle count overflow flag
        BEQ     40f
        MCR     p15, 0, r2, c9, c13, 0          @ set Cycle Counter to all 1's
        ISB
        MOV     r3, #0x80000000
        MCR     p15, 0, r3, c9, c12, 1          @ enable the Cycle Counter
        ISB

30:
        MRC     p15, 0, r4, c9, c12, 3          @ check cycle count overflow now set
        MOVS    r4, r4                          @ test bit<31>
        BPL     30b
        MCR     p15, 0, r3, c9, c12, 2          @ disable the Cycle Counter

40:
        MCR     p15, 0, r1, c9, c12, 0          @ clear the PMCR global enable bit
        ISB

        @ restore the remaining PMon registers
        LDM     r0!, {r1-r4}
        MCR     p15, 0, r1, c9, c13, 0          @ restore Cycle Count Register
        MCR     p15, 0, r2, c9, c14, 0          @ restore User Enable Register
        MCR     p15, 0, r3, c9, c14, 1          @ restore Interrupt Enable Set Reg
        MCR     p15, 0, r4, c9, c12, 1          @ restore Count Enable Set Register
        MCR     p15, 0, r10, c9, c12, 5         @ restore Event Counter Selection
        ISB
        MCR     p15, 0, r8, c9, c12, 0          @ restore the PM Control Register
        ISB

        POP     {r4-r5, r8-r10, pc}
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global save_vfp
save_vfp:
        .func
        @ FPU state save/restore.
        @ FPSID,MVFR0 and MVFR1 don't get serialized/saved (Read Only).
        MRC     p15, 0, r3, c1, c0, 2           @ CPACR allows CP10 and CP11 access
        ORR     r2, r3, #0xF00000
        MCR     p15, 0, r2, c1, c0, 2
        ISB
        MRC     p15, 0, r2, c1, c0, 2
        AND     r2, r2, #0xF00000
        CMP     r2, #0xF00000
        BEQ     0f
        MOVS    r2, #0
        B       2f

0:
        @ Save configuration registers and enable.
        FMRX    r12, FPEXC
        STR     r12, [r0], #4                   @ Save the FPEXC
        @ Enable FPU access to save/restore the other registers.
        LDR     r2, =0x40000000
        FMXR    FPEXC, r2
        FMRX    r2, FPSCR
        STR     r2, [r0], #4                    @ Save the FPSCR
        @ Store the VFP-D16 registers.
        VSTM    r0!, {D0-D15}
        @ Check for Advanced SIMD/VFP-D32 support
        FMRX    r2, MVFR0
        AND     r2, r2, #0xF                    @ extract the A_SIMD bitfield
        CMP     r2, #0x2
        BLT     1f
        @ Store the Advanced SIMD/VFP-D32 additional registers.
        VSTM    r0!, {D16-D31}

        @ IMPLEMENTATION DEFINED: save any subarchitecture defined state
        @ NOTE: Don't change the order of the FPEXC and CPACR restores
1:
        FMXR    FPEXC, r12                      @ Restore the original En bit of FPU.
2:
        MCR     p15, 0, r3, c1, c0, 2           @ Restore the original CPACR value.
        BX      lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


        .global restore_vfp
restore_vfp:
        .func
        @ FPU state save/restore. Obviously FPSID,MVFR0 and MVFR1 don't get
        @ serialized (RO).
        @ Modify CPACR to allow CP10 and CP11 access
        MRC     p15, 0, r1, c1, c0, 2
        ORR     r2, r1, #0x00F00000
        MCR     p15, 0, r2, c1, c0, 2
        @ Enable FPU access to save/restore the rest of registers.
        LDR     r2, =0x40000000
        FMXR    FPEXC, r2
        @ Recover FPEXC and FPSCR. These will be restored later.
        LDM     r0!, {r3, r12}
        @ Restore the VFP-D16 registers.
        VLDM    r0!, {D0-D15}
        @ Check for Advanced SIMD/VFP-D32 support
        FMRX    r2, MVFR0
        AND     r2, r2, #0xF                    @ extract the A_SIMD bitfield
        CMP     r2, #0x2
        BLT     0f

        @ Store the Advanced SIMD/VFP-D32 additional registers.
        VLDM    r0!, {D16-D31}

        @ IMPLEMENTATION DEFINED: restore any subarchitecture defined state

0:
        @ Restore configuration registers and enable.
        @ Restore FPSCR _before_ FPEXC since FPEXC could disable FPU
        @ and make setting FPSCR unpredictable.
        FMXR    FPSCR, r12
        FMXR    FPEXC, r3                       @ Restore FPEXC after FPSCR
        @ Restore CPACR
        MCR     p15, 0, r1, c1, c0, 2
        BX      lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


        .global save_cp15
save_cp15:
        .func
        @ CSSELR  Cache Size Selection Register
        MRC     p15, 2, r3, c0, c0, 0
        STR     r3, [r0], #4

        @ IMPLEMENTATION DEFINED - proprietary features:
        @ (CP15 register 15, TCM support, lockdown support, etc.)

        @ NOTE: IMP DEF registers might have save and restore order that relate
        @ to other CP15 registers or logical grouping requirements and can
        @ therefore occur at any point in this sequence.
        BX      lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


        .global restore_cp15
restore_cp15:
        .func
        @ CSSELR  Cache Size Selection Register
        LDR     r3, [r0], #4
        MCR     p15, 2, r3, c0, c0, 0

        bx      lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


        .global save_a9_other
save_a9_other:
        .func
        MRC     p15, 0, r1, c15, c0, 0          @ Read Power Control Register
        STR     r1, [r0], #4

        MRC     p15, 0, r3, c0, c0, 0           @ Read Main ID Register
        UBFX    r3, r3, #20, #4	                @ Extract major version number
        CMP     r3, #2
        BLT     1f                              @ PLE only possible in r2p0 onwards
        MRC     p15, 0, r3, c11, c0, 0          @ Read PLE IDR
        CMP     r3, #0
        BEQ     1f                              @ No PLE present

        MRC     p15, 0, r1, c11, c1, 0          @ Read PLE UAR
        MRC	p15, 0, r2, c11, c1, 1          @ Read PLE PCR
        STM	r0!, {r1, r2}

1:
        bx      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


        .global restore_a9_other
restore_a9_other:
        .func
        LDR     r1, [r0], #4
        ANDS	r1, r1, #0x01                   @ We only restore the Dynamic Clock gating bit
        MCR     p15, 0, r1, c15, c0, 0          @ Write Power Control Register

        MRC     p15, 0, r3, c0, c0,0            @ Read Main ID Register
        UBFX    r3, r3, #20, #4                 @ Extract major version number
        CMP     r3, #2
        BLT     1f                              @ PLE only possible in r2p0 onwards
        MRC     p15, 0, r3, c11, c0, 0          @ Read PLE IDR
        CMP     r3, #0
        BEQ     1f                              @ No PLE present

        LDM     r0!, {r1, r2}
        MCR     p15, 0, r1, c11, c1, 0          @ Write PLE UAR
        MCR     p15, 0, r2, c11, c1, 1          @ Write PLE PCR

1:
        bx      lr
        .endfunc


        .if 0                                   @ disable debug now

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global save_a9_debug
save_a9_debug:
        .func
        @ TODO
        bx      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_a9_debug
restore_a9_debug:
        .func
        @ TODO
        bx      lr
        .endfunc

        .endif

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global save_a9_global_timer
save_a9_global_timer:
        .func
        PUSH    {r4, r5}
        MOVW    r12, #CA9_SCU_TIM64
        ADD     r1, r1, r12
        LDR     r2, [r1, #TIM64_Ctl]            @ 64-bit timer control
        BIC     r3, r2, #0xF
        STR     r3, [r1, #TIM64_Ctl]            @ disable the features
        @ the registers are now frozen for the context save
        LDR     r3, [r1, #TIM64_AutoInc]        @ Autoincrement register
        LDR     r4, [r1, #TIM64_CmpLo]          @ comparator - lo word
        LDR     r5, [r1, #TIM64_CmpHi]          @ comparator - hi word
        STM     r0!, {r2-r5}
        LDR     r2, [r1, #TIM64_CntLo]          @ counter - lo word
        LDR     r3, [r1, #TIM64_CntHi]          @ counter - hi word
        STM     r0!, {r2-r3}
        POP     {r4, r5}
        BX      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_a9_global_timer
restore_a9_global_timer:
        .func
        PUSH    {r4, r5}
        MOVW    r12, #CA9_SCU_TIM64
        ADD     r1, r1, r12
        LDM     r0!, {r2-r5}
        STR     r3, [r1, #TIM64_AutoInc]        @ Autoincrement register
        STR     r4, [r1, #TIM64_CmpLo]          @ comparator - lo word
        STR     r5, [r1, #TIM64_CmpHi]          @ comparator - hi word
        LDM     r0!, {r3-r4}
        STR     r3, [r1, #TIM64_CntLo]          @ counter - lo word
        STR     r4, [r1, #TIM64_CntHi]          @ counter - hi word
        STR     r2, [r1, #TIM64_Ctl]            @ restore the control last

        POP     {r4, r5}
        BX      LR
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global save_control_registers
save_control_registers:
        .func
        @ ACTLR - Auxiliary Control Register
        MRC     p15, 0, r1, c1, c0, 1
        @ SCTLR - System Control Register
        MRC     p15, 0, r2, c1, c0, 0
        @ CPACR - Coprocessor Access Control Register
        MRC     p15, 0, r3, c1, c0, 2
        STR     r1, [r0, #ACTLR_OFFSET]         @ fixed address
        STR     r2, [r0, #SCTLR_OFFSET]         @ fixed address
        STR     r3, [r0, #CPACR_OFFSET]         @ fixed address
        BX      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_control_registers
restore_control_registers:
        .func
        LDR     r1, [r0, #ACTLR_OFFSET]         @ fixed address
        LDR     r2, [r0, #SCTLR_OFFSET]         @ fixed address
        LDR     r3, [r0, #CPACR_OFFSET]         @ fixed address
        @ ACTLR - Auxiliary Control Register
        MCR     p15, 0, r1, c1, c0, 1
        dsb
        @ CPACR - Coprocessor Access Control Register
        MCR     p15, 0, r3, c1, c0, 2
        dsb

        adr     r3, new_stack
        ldr     r3, [r3, #0]
        mov     sp, r3

        add     lr, lr, #PAGE_OFFSET
        sub     lr, lr, #PHYS_OFFSET

        add     r11, r11, #PAGE_OFFSET
        sub     r11, r11, #PHYS_OFFSET

        @ SCTLR - System Control Register
        bic     r2, #4                          @ Clear C bit
        bic     r2, #0x1000                     @ Clear I bit

        MCR     p15, 0, r2, c1, c0, 0
        mov     pc, lr
        isb
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global cpu_wake_up
cpu_wake_up:
        .func

        .if 0
1:
        nop
        b       1b
        .endif

        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop

        adr     r0, new_stack
        ldr     r0, [r0, #0]
        sub     r0, r0, #PAGE_OFFSET
        add     r0, r0, #PHYS_OFFSET
        mov     sp, r0

        .if 0

        ldr     r4, .L2
        mov     r5, #82
        str     r5, [r4, #0]
        mov     r5, #69
        str     r5, [r4, #0]
        mov     r5, #83
        str     r5, [r4, #0]
        mov     r5, #69
        str     r5, [r4, #0]
        mov     r5, #84
        str     r5, [r4, #0]
        mov     r5, #10
        str     r5, [r4, #0]
        mov     r5, #13
        str     r5, [r4, #0]
        .endif

        b       cpu_start_restore
        .endfunc

        .if 0
.L2:
                .word -1056915456
        .endif
new_stack:      .word nstack+1020
nstack:         .space 1024


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .global enable_cache_v7_l1
enable_cache_v7_l1:
        .func

        push    {r4}
        mrc     p15, 0, r4, c1, c0, 0
        orr     r4, #4                          @ Set C bit
        orr     r4, #0x1000                     @ Set I bit
        mcr     p15, 0, r4, c1, c0, 0
        dsb
        pop     {r4}
        bx      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .global disable_cache_v7_l1
disable_cache_v7_l1:
        .func
        push    {r4}

        mrc     p15, 0, r4, c1, c0, 0
        bic     r4, #4                          @ Clear C bit
        bic     r4, #0x1000                     @ Clear I bit
        mcr     p15, 0, r4, c1, c0, 0
        dsb
        pop     {r4}
        bx      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global save_mmu
save_mmu:
        .func

        PUSH    {r4, r5, r6, r7}
        @ ASSUMPTION: no useful fault address / fault status information

        MRC     p15, 0, r4, c12, c0, 0          @ VBAR
        MRC     p15, 0, r5, c2, c0, 0           @ TTBR0
        MRC     p15, 0, r6, c2, c0, 1           @ TTBR1
        MRC     p15, 0, r7, c2, c0, 2           @ TTBCR
        STM     r0!, {r4-r7}

        MRC     p15, 0, r4, c3, c0, 0           @ DACR
        MRC     p15, 0, r5, c7, c4, 0           @ PAR
        MRC     p15, 0, r6, c10, c2, 0          @ PRRR
        MRC     p15, 0, r7, c10, c2, 1          @ NMRR
        STM     r0!, {r4-r7}

        @ TODO: IMPLEMENTATION DEFINED - TCM, lockdown and performance monitor support
        @     CP15 registers 9 and 11

        MRC     p15, 0, r4, c13, c0, 1          @ CONTEXTIDR
        MRC     p15, 0, r5, c13, c0, 2          @ TPIDRURW
        MRC     p15, 0, r6, c13, c0, 3          @ TPIDRURO
        MRC     p15, 0, r7, c13, c0, 4          @ TPIDRPRW
        STM     r0!, {r4-r7}

        POP     {r4, r5, r6, r7}
        bx      lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_mmu
restore_mmu:
        .func

        PUSH    {r4, r5, r6, r7}
        LDM     r0!, {r4-r7}
        MCR     p15, 0, r4, c12, c0, 0          @ VBAR
        MCR     p15, 0, r5, c2, c0, 0           @ TTBR0
        MCR     p15, 0, r6, c2, c0, 1           @ TTBR1
        MCR     p15, 0, r7, c2, c0, 2           @ TTBCR

        LDM     r0!, {r4-r7}
        MCR     p15, 0, r4, c3, c0, 0           @ DACR
        MCR     p15, 0, r5, c7, c4, 0           @ PAR
        MCR     p15, 0, r6, c10, c2, 0          @ PRRR
        MCR     p15, 0, r7, c10, c2, 1          @ NMRR

        @ TODO: IMPLEMENTATION DEFINED - TCM, lockdown and performance monitor support
        @     CP15 registers 9 and 11

        LDM     r0!, {r4-r7}
        MCR     p15, 0, r4, c13, c0, 1          @ CONTEXTIDR
        MCR     p15, 0, r5, c13, c0, 2          @ TPIDRURW
        MCR     p15, 0, r6, c13, c0, 3          @ TPIDRURO
        MCR     p15, 0, r7, c13, c0, 4          @ TPIDRPRW

        POP     {r4, r5, r6, r7}
        bx      lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .if 0


        .global write_actlr
write_actlr:
        .func
        mcr     p15, 0, r0, c1, c0, 1
        bx      lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global read_actlr
read_actlr:
        .func
        mrc     p15, 0, r0, c1, c0, 1
        bx      lr
        .endfunc

        .endif

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


        @ This function takes three arguments
        @ r0: Destination start address (must be word aligned)
        @ r1: Source start address (must be word aligned)
        @ r2: Number of words to copy
        @ Return value is updated destination pointer (first unwritten word)

        .global copy_words
copy_words:
        .func
        push    {r3}
        cmp     r2, #0
        beq     1f
2:
        ldr     r3, [r1], #4
        str     r3, [r0], #4
        subs    r2, r2, #1
        bne     2b
1:
        pop     {r3}
        bx      lr

        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


        .global save_banked_registers
save_banked_registers:
        .func
        .if  0      @ TODO should be SECURITY_EXTNS_ValidS
        @ Monitor Mode in use? A-profile ONLY
        CPS     #MODE_MON                       @ switch to Monitor mode
        STR     SP, [r0], #4                    @ save the User SP
        STR     LR, [r0], #4                    @ save the User LR
        .endif

        PUSH    {r3, lr}

        CPS     #MODE_SYS                       @ switch to System mode
        STR     SP, [r0], #4                    @ save the Monitor SP
        STR     LR, [r0], #4                    @ save the Monitor LR
        CPS     #MODE_ABT                       @ switch to Abort mode
        STR     SP, [r0], #4                    @ save the current SP
        MRS     r3, SPSR
        STM     r0!, {r3, LR}                   @ save the current SPSR, LR
        CPS     #MODE_UND                       @ switch to Undefined mode
        STR     SP, [r0], #4                    @ save the current SP
        MRS     r3, SPSR
        STM     r0!, {r3, LR}                   @ save the current SPSR, LR
        CPS     #MODE_IRQ                       @ switch to IRQ mode
        STR     SP, [r0], #4                    @ save the current SP
        MRS     r3, SPSR
        STM     r0!, {r3, LR}                   @ save the current SPSR, LR
        CPS     #MODE_FIQ                       @ switch to FIQ mode
        STR     SP, [r0], #4                    @ save the current SP
        MRS     r3, SPSR
        STM     r0!, {r8-r12, LR}               @ save the current SPSR,r8-r12,LR
        CPS     #MODE_SVC                       @ switch back to Supervisor mode

        POP     {r3, lr}
        STR     SP, [r0], #4                    @ save the current SP
        MRS     r3, SPSR
        STM     r0!, {r3, r4-r12, LR}           @ save the current SPSR,r4-r12,LR
        dsb
        BX      lr
        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

        .global restore_banked_registers
restore_banked_registers:
        .func
        .if 0
        @ A-profile ONLY
        CPS     #MODE_MON                       @ switch to Monitor mode
        LDR     SP, [r0], #4                    @ restore the Monitor SP
        LDR     LR, [r0], #4                    @ restore the Monitor LR
        .endif

        CPS     #MODE_SYS                       @ switch to System mode
        LDR     SP, [r0], #4                    @ restore the User SP
        LDR     LR, [r0], #4                    @ restore the User LR
        CPS     #MODE_ABT                       @ switch to Abort mode
        LDR     SP, [r0], #4                    @ restore the current SP
        LDM     r0!, {r3, LR}                   @ restore the current LR
        MSR     SPSR_fsxc, r3                   @ restore the current SPSR
        CPS     #MODE_UND                       @ switch to Undefined mode
        LDR     SP, [r0], #4                    @ restore the current SP
        LDM     r0!, {r3, LR}                   @ restore the current LR
        MSR     SPSR_fsxc, r3                   @ restore the current SPSR
        CPS     #MODE_IRQ                       @ switch to IRQ mode
        LDR     SP, [r0], #4                    @ restore the current SP
        LDM     r0!, {r3, LR}                   @ restore the current LR
        MSR     SPSR_fsxc, r3                   @ restore the current SPSR
        CPS     #MODE_FIQ                       @ switch to FIQ mode
        LDR     SP, [r0], #4                    @ restore the current SP
        LDM     r0!, {r8-r12, LR}               @ restore the current r8-r12,LR
        MSR     SPSR_fsxc, r3                   @ restore the current SPSR
        CPS     #MODE_SVC                       @ switch back to Supervisor mode

        LDR     SP, [r0], #4                    @ restore the current SP
        LDM     r0!, {r3, r4-r12, LR}           @ restore the current r4-r12,LR
        MSR     SPSR_fsxc, r3                   @ restore the current SPSR
        dsb

        BX      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        @ This function cleans the whole L1 data cache
        .global clean_dcache_v7_l1
clean_dcache_v7_l1:
        .func

        push    {r0-r7, r9-r11}
        dmb                                     @ ensure ordering with previous memory accesses
        mrc     p15, 1, r0, c0, c0, 1           @ read clidr
        ands    r3, r0, #0x7000000              @ extract loc from clidr
        mov     r3, r3, lsr #23                 @ left align loc bit field
        beq     finished_a                      @ if loc is 0, then no need to clean
        mov     r10, #0                         @ start clean at cache level 0
loop_a:
        add     r2, r10, r10, lsr #1            @ work out 3x current cache level
        mov     r1, r0, lsr r2                  @ extract cache type bits from clidr
        and     r1, r1, #7                      @ mask of the bits for current cache only
        cmp     r1, #2                          @ see what cache we have at this level
        blt     skip_a                          @ skip if no cache, or just i-cache
        mcr     p15, 2, r10, c0, c0, 0          @ select current cache level in cssr
        isb                                     @ isb to sych the new cssr&csidr
        mrc     p15, 1, r1, c0, c0, 0           @ read the new csidr
        and     r2, r1, #7                      @ extract the length of the cache lines
        add     r2, r2, #4                      @ add 4 (line length offset)
        ldr     r4, =0x3ff
        ands    r4, r4, r1, lsr #3              @ find maximum number on the way size
        clz     r5, r4                          @ find bit position of way size increment
        ldr     r7, =0x7fff
        ands    r7, r7, r1, lsr #13             @ extract max number of the index size
loop_b:
        mov     r9, r4                          @ create working copy of max way size
loop_c:
        orr     r11, r10, r9, lsl r5            @ factor way and cache number into r11
        orr     r11, r11, r7, lsl r2            @ factor index number into r11
        mcr     p15, 0, r11, c7, c14, 2         @ clean & invalidate by set/way
        subs    r9, r9, #1                      @ decrement the way
        bge     loop_c
        subs    r7, r7, #1                      @ decrement the index
        bge     loop_b
skip_a:
        add     r10, r10, #2                    @ increment cache number
        cmp     r3, r10
        bgt     loop_a
finished_a:
        mov     r10, #0                         @ swith back to cache level 0
        mcr     p15, 2, r10, c0, c0, 0          @ select current cache level in cssr
        dsb
        isb

        pop     {r0-r7, r9-r11}
        mov     pc, lr

        .endfunc


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .global invalidate_icache_v7
invalidate_icache_v7:
        .func
        mov     r0, #0
        mcr     p15, 0, r0, c7, c5, 0           @ ICIALLU
        bx      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .global invalidate_dcache_v7
invalidate_dcache_v7:
        .func
        @ Must iterate over the caches in order to synthesise a complete clean
        @ of data/unified cache
        push    {r4-r11}
        mrc     p15, 1, r0, c0, c0, 1           @ read clidr
        ands    r3, r0, #0x7000000              @ extract loc from clidr
        mov     r3, r3, lsr #23                 @ left align loc bit field
        beq     finished                        @ if loc is 0, then no need to clean
        mov     r10, #0                         @ start clean at cache level 0 (in r10)
loop1:
        add     r2, r10, r10, lsr #1            @ work out 3x current cache level
        mov     r12, r0, lsr r2                 @ extract cache type bits from clidr
        and     r12, r12, #7                    @ mask of bits for current cache only
        cmp     r12, #2                         @ see what cache we have at this level
        blt     skip                            @ skip if no cache, or just i-cache
        mcr     p15, 2, r10, c0, c0, 0          @ select current cache level in cssr
        mov     r12, #0
        mcr     p15, 0, r12, c7, c5, 4          @ prefetch flush to sync new cssr&csidr
        mrc     p15, 1, r12, c0, c0, 0          @ read the new csidr
        and     r2, r12, #7                     @ extract the length of the cache lines
        add     r2, r2, #4                      @ add 4 (line length offset)
        ldr     r6, =0x3ff
        ands    r6, r6, r12, lsr #3             @ find maximum number on the way size
        clz     r5, r6                          @ find bit pos of way size increment
        ldr     r7, =0x7fff
        ands    r7, r7, r12, lsr #13            @ extract max number of the index size
loop2:
        mov     r8, r6                          @ create working copy of max way size
loop3:
        orr     r11, r10, r8, lsl r5            @ factor way and cache number into r11
        orr     r11, r11, r7, lsl r2            @ factor index number into r11
        mcr     p15, 0, r11, c7, c6, 2          @ invalidate by set/way
        subs    r8, r8, #1                      @ decrement the way
        bge     loop3
        subs    r7, r7, #1                      @ decrement the index
        bge     loop2
skip:
        add     r10, r10, #2                    @ increment cache number
        cmp     r3, r10
        bgt     loop1
finished:
        mov     r10, #0

        mcr     p15, 0, r10, c7, c10, 4         @ drain write buffer
        mcr     p15, 0, r10, c8, c7, 0          @ invalidate I + D TLBs
        mcr     p15, 0, r10, c2, c0, 2          @ TTB control register
        pop     {r4-r11}
        bx      lr
        .endfunc

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
        .macro dcache_line_size, reg, tmp
        mrc     p15, 1, \tmp, c0, c0, 0         @ read CSIDR
        and     \tmp, \tmp, #7                  @ cache line size encoding
        mov     \reg, #16                       @ size offset
        mov     \reg, \reg, lsl \tmp            @ actual cache line size
        .endm

        .global inv_cache_range_v7_l1
inv_cache_range_v7_l1:
        .func
        push    {r2-r3}
        dcache_line_size r2, r3
        sub     r3, r2, #1
        bic     r0, r0, r3
1:
        mcr     p15, 0, r0, c7, c7, 1           @ invalidate D/U line
        add     r0, r0, r2
        cmp     r0, r1
        blo     1b
        pop     {r2-r3}
        dsb
        mov     pc, lr
        .endfunc

        .global get_current_stack
get_current_stack:
        .func
        str     sp, [r0, #0]
        dsb
        mov     pc, lr
        .endfunc

