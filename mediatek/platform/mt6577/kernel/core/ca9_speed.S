    .text

    .extern cpu_speed_test_pt_update
    .extern cpu_speed_test_pt_restore
    .extern g_iADDR1_CPU0
    .extern g_iADDR2_CPU0
    .extern g_iADDR3_CPU0
    .extern g_iADDR4_CPU0
    .extern g_iADDR5_CPU0
    .extern g_iADDR6_CPU0
    .extern g_iADDR7_CPU0
    .extern g_iADDR8_CPU0

    .extern g_iADDR1_CPU1
    .extern g_iADDR2_CPU1
    .extern g_iADDR3_CPU1
    .extern g_iADDR4_CPU1
    .extern g_iADDR5_CPU1
    .extern g_iADDR6_CPU1
    .extern g_iADDR7_CPU1
    .extern g_iADDR8_CPU1    

    @.equ DEBUG_BASE, 0xC0170000
    .equ DEBUG_BASE, 0xF0170000
    @.equ PERIPH_BASE, 0xC000A000    @ Base address for SCU + periph
    .equ PERIPH_BASE, 0xF000A000    @ Base address for SCU + periph
    .equ TIMER_BASE, PERIPH_BASE + 0x600
    @
    @ NOTE:TAG1 must be &__SO_MEM_TEST + 0x30000
    @
    @.equ TAG1, 0x09000000   @ Data section 1
    @.equ TAG1, 0x09A30000   @ Data section 1
    @.equ TAG1, 0x02430000   @ Data section 1
    @.equ TAG1, 0xC2030000   @ Data section 1
    @.equ TAG1, 0x02430000   @ Data section 1
    @.equ ADDR1, TAG1 + 0x00000
    @.equ ADDR2, TAG1 + 0x02000
    @.equ ADDR3, TAG1 + 0x04000
    @.equ ADDR4, TAG1 + 0x06000
    @.equ ADDR5, TAG1 + 0x08000
    @.equ ADDR6, TAG1 + 0x0A000
    @.equ ADDR7, TAG1 + 0x0C000
    @.equ ADDR8, TAG1 + 0x0E000

    @.equ RESULT_R0, ADDR1 + 8
    @.equ RESULT_R1, ADDR2 + 8
    @.equ RESULT_R2, 0x55555555
    @.equ RESULT_R3, 0x55555555
    @.equ RESULT_R4, 0xFAFAFAFB
    @.equ RESULT_R10, 0x5F5F5F60
    @.equ RESULT, RESULT_R0 + RESULT_R1 + RESULT_R2 + RESULT_R3 + RESULT_R4 + RESULT_R10

nADDR1_CPU0:
    .word g_iADDR1_CPU0

nADDR2_CPU0:
    .word g_iADDR2_CPU0

nADDR3_CPU0:
    .word g_iADDR3_CPU0

nADDR4_CPU0:
    .word g_iADDR4_CPU0

nADDR5_CPU0:
    .word g_iADDR5_CPU0

nADDR6_CPU0:
    .word g_iADDR6_CPU0

nADDR7_CPU0:
    .word g_iADDR7_CPU0

nADDR8_CPU0:
    .word g_iADDR8_CPU0


nADDR1_CPU1:
    .word g_iADDR1_CPU1

nADDR2_CPU1:
    .word g_iADDR2_CPU1

nADDR3_CPU1:
    .word g_iADDR3_CPU1

nADDR4_CPU1:
    .word g_iADDR4_CPU1

nADDR5_CPU1:
    .word g_iADDR5_CPU1

nADDR6_CPU1:
    .word g_iADDR6_CPU1

nADDR7_CPU1:
    .word g_iADDR7_CPU1

nADDR8_CPU1:
    .word g_iADDR8_CPU1
@final_result:
@    .word 0x0

    .global max_power_loop
max_power_loop:
    STMDB sp!, {r1-r12, r14}  @ save the architecture state

    @ calculate the correct test result value first for final evaluation
@    MOV r0, #0
@    LDR r1, =RESULT_R0
@    ADD r0, r0, r1
@    LDR r1, =RESULT_R1
@    ADD r0, r0, r1
@    LDR r1, =RESULT_R2
@    ADD r0, r0, r1
@    LDR r1, =RESULT_R3
@    ADD r0, r0, r1
@    LDR r1, =RESULT_R4
@    ADD r0, r0, r1
@    LDR r1, =RESULT_R10
@    ADD r0, r0, r1
@    STR r0, final_result

@@@    @ Enable Timer and watchdog
@@@    LDR r0, =0xffffffff
@@@    LDR r1, =TIMER_BASE
@@@    STR r0, [r1]    @ set load value for timer
@@@    MOV r0, #3
@@@    STR r0, [r1, #8]    @ timer enable + auto-reload
@@@    LDR r0, =0xffffffff
@@@    LDR r1, =TIMER_BASE
@@@    STR r0, [r1, #0x20] @ set load value for wdog
@@@    MOV r0, #3
@@@    STR r0, [r1, #0x28] @ wdog enable + auto-reload in timer mode
@@@
@@@    @ Enable system metrics counters to count clk cycles
@@@    @ count cache reads on each counter
@@@    MOV r0, #0x0
@@@    MCR p15, 0, r0, c9, c12, 5
@@@    MOV r0, #0x6
@@@    MCR p15, 0, r0, c9, c13, 1
@@@    MOV r0, #0x1
@@@    MCR p15, 0, r0, c9, c12, 5
@@@    MOV r0, #0x6
@@@    MCR p15, 0, r0, c9, c13, 1
@@@
@@@    MOV r0, #0x2
@@@    MCR p15, 0, r0, c9, c12, 5
@@@    MOV r0, #0x6
@@@    MCR p15, 0, r0, c9, c13, 1
@@@
@@@    MOV r0, #0x3
@@@    MCR p15, 0, r0, c9, c12, 5
@@@    MOV r0, #0x6
@@@    MCR p15, 0, r0, c9, c13, 1
@@@
@@@    MOV r0, #0x4
@@@    MCR p15, 0, r0, c9, c12, 5
@@@    MOV r0, #0x6
@@@    MCR p15, 0, r0, c9, c13, 1
@@@
@@@    MOV r0, #0x5
@@@    MCR p15, 0, r0, c9, c12, 5
@@@    MOV r0, #0x6
@@@    MCR p15, 0, r0, c9, c13, 1
@@@
@@@    @ reset + enable counters & event bus
@@@    MOV r0, #0x17
@@@    MCR p15, 0, r0, c9, c12, 0
@@@    LDR r0, =0x8000003f
@@@    MCR p15, 0, r0, c9, c12, 1
@@@
@@@    @ DEBUG BASE: 0xC017_0000
@@@    @ Enable monitor mode
@@@    MOV r0, #0x8000
@@@    @@MCR p14, 0, r0, c0, c2, 2 @ reg#=34, offset=0x088
@@@    LDR r1, =DEBUG_BASE + 0x88
@@@    STR r0, [r1]
@@@
@@@    @ Enable breakpoint unit logic
@@@    LDR	r0, =0xfffffff0
@@@    @@MCR p14, 0, r0, c0, c0, 4 @ reg#=64, offset=0x100
@@@    LDR r1, =DEBUG_BASE + 0x100
@@@    STR r0, [r1]
@@@    MOV r0, #1
@@@    @@MCR p14, 0, r0, c0, c0, 5 @ reg#=80, offset=0x140
@@@    LDR r1, =DEBUG_BASE + 0x140
@@@    STR r0, [r1]
@@@
@@@    LDR	r0, =0xfffffff0
@@@    @@MCR p14, 0, r0, c0, c1, 4 @ reg#=65, offset=0x104
@@@    LDR r1, =DEBUG_BASE + 0x104
@@@    STR r0, [r1]
@@@    MOV	r0, #1
@@@    @@MCR p14, 0, r0, c0, c1, 5 @ reg#=81, offset=0x144
@@@    LDR r1, =DEBUG_BASE + 0x144
@@@    STR r0, [r1]
@@@
@@@    LDR	r0, =0xfffffff0
@@@    @@MCR p14, 0, r0, c0, c2, 4 @ reg#=66, offset=0x108
@@@    LDR r1, =DEBUG_BASE + 0x108
@@@    STR r0, [r1]
@@@    MOV	r0, #1
@@@    @@MCR p14, 0, r0, c0, c2, 5 @ reg#=82, offset=0x148
@@@    LDR r1, =DEBUG_BASE + 0x148
@@@    STR r0, [r1]
@@@
@@@    LDR	r0, =0xfffffff0
@@@    @@MCR p14, 0, r0, c0, c3, 4 @ reg#=67, offset=0x10C
@@@    LDR r1, =DEBUG_BASE + 0x10C
@@@    STR r0, [r1]
@@@    MOV	r0, #1
@@@    @@MCR p14, 0, r0, c0, c3, 5 @ reg#=83, offset=0x14C
@@@    LDR r1, =DEBUG_BASE + 0x14C
@@@    STR r0, [r1]
@@@
@@@    LDR	r0, =0xfffffff0
@@@    @@MCR p14, 0, r0, c0, c4, 4 @ reg#=68, offset=0x110
@@@    LDR r1, =DEBUG_BASE + 0x110
@@@    STR r0, [r1]
@@@    MOV	r0, #1
@@@    @@MCR p14, 0, r0, c0, c4, 5 @ reg#=84, offset=0x150
@@@    LDR r1, =DEBUG_BASE + 0x150
@@@    STR r0, [r1]
@@@
@@@    LDR	r0, =0xfffffff0
@@@    @@MCR p14, 0, r0, c0, c5, 4 @ reg#=69, offset=0x114
@@@    LDR r1, =DEBUG_BASE + 0x114
@@@    STR r0, [r1]
@@@    MOV	r0, #1
@@@    @@MCR p14, 0, r0, c0, c5, 5 @ reg#=85, offset=0x154
@@@    LDR r1, =DEBUG_BASE + 0x154
@@@    STR r0, [r1]
@@@
@@@    @ Enable watchpoint unit logic
@@@    LDR	r0, =0xffff3ff0
@@@    @@MCR p14, 0, r0, c0, c0, 6 @ reg#=96, offset=0x180
@@@    LDR r1, =DEBUG_BASE + 0x180
@@@    STR r0, [r1]
@@@    LDR	r0, =0x08000019
@@@    @@MCR p14, 0, r0, c0, c0, 7 @ reg#=112, offset=0x1C0
@@@    LDR r1, =DEBUG_BASE + 0x1C0
@@@    STR r0, [r1]
@@@
@@@    LDR	r0, =0xfffff3f0
@@@    @@MCR p14, 0, r0, c0, c1, 6 @ reg#=97, offset=0x184
@@@    LDR r1, =DEBUG_BASE + 0x184
@@@    STR r0, [r1]
@@@    LDR	r0, =0x08000019
@@@    @@MCR p14, 0, r0, c0, c1, 7 @ reg#=113, offset=0x1C4
@@@    LDR r1, =DEBUG_BASE + 0x1C4
@@@    STR r0, [r1]
@@@
@@@    @ Give access to all coprocessors
@@@    LDR	r0, =0x0FFFFFFF
@@@    MCR p15, 0, r0, c1, c0, 2
@@@
@@@    @ Chicken bit to desactivate high level clk gating
@@@    MRC p15, 0, r0, c1, c0, 1
@@@    ORR r0, r0, #0x2000
@@@    ORR r0, r0, #0x20000
@@@    MCR p15, 0, r0, c1, c0, 1
@@@
@@@    MRC p15, 0, r0, c15, c0, 1
@@@    ORR r0, r0, #1 << 13
@@@    ORR r0, r0, #1 << 16
@@@    MCR p15, 0, r0, c15, c0, 1
@@@
@@@    MRC p15, 0, r0, c15, c0, 2
@@@    ORR r0, r0, #2
@@@    MCR p15, 0, r0, c15, c0, 2

    @
    @ update Page table
    @
@    BL cpu_speed_test_pt_update

    @ read CPU ID register
    MRC p15,0,r3,c0,c0,5
    AND r3,r3,#0xf
    CMP r3,#0
    BEQ init_data_cpu0
    CMP r3,#1
    BEQ init_data_cpu1

init_data_cpu0:    
    @ init data in other ways of data RAM
    LDR r0, nADDR3_CPU0 @=ADDR3_CPU0
    LDR r0, [r0]
    LDR r1, nADDR4_CPU0 @=ADDR4_CPU0
    LDR r1, [r1]
    LDR r6, nADDR5_CPU0 @=ADDR5_CPU0
    LDR r6, [r6]
    LDR r7, nADDR6_CPU0 @=ADDR6_CPU0
    LDR r7, [r7]
    LDR r8, nADDR7_CPU0 @=ADDR7_CPU0
    LDR r8, [r8]
    LDR r9, nADDR8_CPU0 @=ADDR8_CPU0
    LDR r9, [r9]

    LDR r10, =0xAAAAAAAA
    LDR r14, =0x55555555

    MOV r2, r10
    MOV r3, r10
    MOV r4, r14
    MOV r5, r14

    STRD r2, [r0, #8]
    STRD r4, [r7]
    MCR  p15, 0, r0, c7, c10, 4 @ Drain WB

    STRD r2, [r1, #8]
    STRD r4, [r8]
    MCR  p15, 0, r0, c7, c10, 4 @ Drain WB

    STRD r2, [r6, #8]
    STRD r4, [r9]
    MCR  p15, 0, r0, c7, c10, 4 @ Drain WB

    @ init the 2 addresses used for data
    @LDR r0, =ADDR1_CPU0 - 8
    LDR r0, nADDR1_CPU0
    LDR r0, [r0]
    MOV r2, #8
    SUB r0, r0, r2

    MOV r2, r10
    MOV r3, r2
    MOV r4, r2
    MOV r5, r2
    MOV r6, r2
    MOV r7, r2
    MOV r8, r2
    MOV r9, r2
    MOV r10, r2
    MOV r11, r2
    STMIA  r0, {r2, r3, r4, r5, r6, r7, r8, r9}

    @LDR r1, =ADDR2_CPU0 - 0x10
    LDR r1, nADDR2_CPU0
    LDR r1, [r1]
    MOV r2, #0x10
    SUB r1, r1, r2

    MOV r2, r14
    MOV r3, r2
    MOV r4, r2
    MOV r5, r2
    MOV r6, r2
    MOV r7, r2
    MOV r8, r2
    MOV r9, r2
    MOV r10, r2
    MOV r11, r2
    STMIA r1, {r4, r5, r6, r7, r8, r9, r10, r11}

    MCR p15, 0, r0, c7, c10, 4  @ Drain WB
    B init_data_end

init_data_cpu1:    
    @ init data in other ways of data RAM
    LDR r0, nADDR3_CPU1 @=ADDR3_CPU1
    LDR r0, [r0]
    LDR r1, nADDR4_CPU1 @=ADDR4_CPU1
    LDR r1, [r1]
    LDR r6, nADDR5_CPU1 @=ADDR5_CPU1
    LDR r6, [r6]
    LDR r7, nADDR6_CPU1 @=ADDR6_CPU1
    LDR r7, [r7]
    LDR r8, nADDR7_CPU1 @=ADDR7_CPU1
    LDR r8, [r8]
    LDR r9, nADDR8_CPU1 @=ADDR8_CPU1
    LDR r9, [r9]

    LDR r10, =0xAAAAAAAA
    LDR r14, =0x55555555

    MOV r2, r10
    MOV r3, r10
    MOV r4, r14
    MOV r5, r14

    STRD r2, [r0, #8]
    STRD r4, [r7]
    MCR  p15, 0, r0, c7, c10, 4 @ Drain WB

    STRD r2, [r1, #8]
    STRD r4, [r8]
    MCR  p15, 0, r0, c7, c10, 4 @ Drain WB

    STRD r2, [r6, #8]
    STRD r4, [r9]
    MCR  p15, 0, r0, c7, c10, 4 @ Drain WB

    @ init the 2 addresses used for data
    @LDR r0, =ADDR1_CPU1 - 8
    LDR r0, nADDR1_CPU1
    LDR r0, [r0]
    MOV r2, #8
    SUB r0, r0, r2

    MOV r2, r10
    MOV r3, r2
    MOV r4, r2
    MOV r5, r2
    MOV r6, r2
    MOV r7, r2
    MOV r8, r2
    MOV r9, r2
    MOV r10, r2
    MOV r11, r2
    STMIA  r0, {r2, r3, r4, r5, r6, r7, r8, r9}

    @LDR r1, =ADDR2_CPU1 - 0x10
    LDR r1, nADDR2_CPU1
    LDR r1, [r1]
    MOV r2, #0x10
    SUB r1, r1, r2

    MOV r2, r14
    MOV r3, r2
    MOV r4, r2
    MOV r5, r2
    MOV r6, r2
    MOV r7, r2
    MOV r8, r2
    MOV r9, r2
    MOV r10, r2
    MOV r11, r2
    STMIA r1, {r4, r5, r6, r7, r8, r9, r10, r11}

    MCR p15, 0, r0, c7, c10, 4  @ Drain WB
     
@@@ 
@@@     @ invalidate SCU tag RAMs
@@@     LDR r10, =PERIPH_BASE + 0xc
@@@ 
@@@     LDR r11, =0xffff
@@@     STR r11, [r10]

init_data_end:

    @ read CPU ID register
    MRC p15,0,r3,c0,c0,5
    AND r3,r3,#0xf
    CMP r3,#0
    BEQ power_loop_cpu0
    CMP r3,#1
    BEQ power_loop_cpu1

@ start the cpu0 infinite loop that maximizes consumption.
power_loop_cpu0: 

    @ start the infinite loop that maximizes consumption.

    LDR r7, =0x55555555
    LDR r6, =0x0f0f0f0f

    LDR r8, =0xaaaaaaaa
    LDR r9, =0xf0f0f0f0

    LDR r11, =0x00000000
    LDR r12, =0xffffffff
    LDR r4, =0xf0f0f0f0

    @loop count initialize
    MOV r5, #0x100

    @LDR r0, =ADDR1_CPU0 + 8
    LDR r0, nADDR1_CPU0
    LDR r0, [r0]
    MOV r4, #8
    ADD r0, r0, r4

    @LDR r1, =ADDR2_CPU0 + 8
    LDR r1, nADDR2_CPU0
    LDR r1, [r1]
    ADD r1, r1, r4

    LDR r12, =0xfffffff8
    LDR r4, =0xfffffff8

    .align 5 
	NOP
	NOP
	
power_loop:
    @;LDR r10,=PERIPH_BASE + 0xc
    @;LDR r11,=0xffff
    @;STR r11,[r10]  @invalidate SCU(background)
    
    LDMIA r0, {r2, r3}
    LDMDB r1, {r4, r14}
    LDMIA r0, {r4, r14}
    MUL r4, r7, r6
    ADDS r2, r12, #1
    MUL r10, r8, r9
    LDMDB r1, {r2, r3}
    SUBS r10, r11, #2

    LDMIA r0, {r2, r3}
    LDMDB r1, {r4, r14}
    LDMIA r0, {r4, r14}
    MUL r4, r7, r6
    ADDS r2, r12, #1
    MUL r10, r8, r9
    LDMDB r1, {r2, r3}
    SUBS  r10, r11, #2

    LDMIA r0, {r2, r3}
    LDMDB r1, {r4, r14}
    LDMIA r0, {r4, r14}
    MUL r4, r7, r6
    @;ADDS r2, r12, #1
    SUBS r5, r5, #1
    MUL r10, r8, r9
    LDMDB r1, {r2, r3}

    BNE power_loop

    @signature the value of the registers
    ADD r0, r0, r1
    ADD r0, r0, r2
    ADD r0, r0, r3
    ADD r0, r0, r4
    ADD r0, r0, r10        

    STMDB sp!, {r0-r3}
@    bl cpu_speed_test_pt_restore
    LDMIA sp!, {r0-r3}

    @@CPU1 fail, CPU0 always idle
    @@CPU1 pass, CPU0 exit WFE & return to main
    @           
	@LDR r1,=0x8907550d
	@DSB
    @WFE  @;waked up by CPU1
    @           
	@@;check max_power correctness	
	@CMP r0,r1
	@MOVEQ r0, #0x1
	@@; Fail here
    @MOVNE r0 ,#0x0
    
    LDMIA sp!, {r1-r12, r14}    @ restore the architecture state by cllu
    MOV pc, r14 @ return ca9_max_power_fp test 

@====== CPU1  start the infinite loop that maximizes consumption.
power_loop_cpu1: 

    @ start the infinite loop that maximizes consumption.

    LDR r7, =0x55555555
    LDR r6, =0x0f0f0f0f

    LDR r8, =0xaaaaaaaa
    LDR r9, =0xf0f0f0f0

    LDR r11, =0x00000000
    LDR r12, =0xffffffff
    LDR r4, =0xf0f0f0f0

    @loop count initialize
    MOV r5, #0x100

    @LDR r0, =ADDR1_CPU1 + 8
    LDR r0, nADDR1_CPU1
    LDR r0, [r0]
    MOV r4, #8
    ADD r0, r0, r4

    @LDR r1, =ADDR2_CPU1 + 8
    LDR r1, nADDR2_CPU1
    LDR r1, [r1]
    ADD r1, r1, r4

    LDR r12, =0xfffffff8
    LDR r4, =0xfffffff8

    .align 5 
	NOP
	NOP
	
power_loop_1:
    @@@ LDR r10,=PERIPH_BASE + 0xc
    @@@ LDR r11,=0xffff
    @@@ STR r11,[r10]  @invalidate SCU(background)
    
    LDMIA r0, {r2, r3}
    LDMDB r1, {r4, r14}
    LDMIA r0, {r4, r14}
    MUL r4, r7, r6
    ADDS r2, r12, #1
    MUL r10, r8, r9
    LDMDB r1, {r2, r3}
    SUBS r10, r11, #2

    LDMIA r0, {r2, r3}
    LDMDB r1, {r4, r14}
    LDMIA r0, {r4, r14}
    MUL r4, r7, r6
    ADDS r2, r12, #1
    MUL r10, r8, r9
    LDMDB r1, {r2, r3}
    SUBS  r10, r11, #2

    LDMIA r0, {r2, r3}
    LDMDB r1, {r4, r14}
    LDMIA r0, {r4, r14}
    MUL r4, r7, r6
    @;ADDS r2, r12, #1
    SUBS r5, r5, #1
    MUL r10, r8, r9
    LDMDB r1, {r2, r3}

    BNE power_loop_1

    @signature the value of the registers
    ADD r0, r0, r1
    ADD r0, r0, r2
    ADD r0, r0, r3
    ADD r0, r0, r4
    ADD r0, r0, r10        

    STMDB sp!, {r0-r3}
@    bl cpu_speed_test_pt_restore
    LDMIA sp!, {r0-r3}

@    @CPU1 fail, return to main
@	@CPU1 pass, wake up CPU0, CPU1 idle             
@               
@	@;check max_power correctness	
@	LDR r1,=0x8907550d
@	CMP r0,r1	
@	BNE fail_case
@    
@    MOV r0 ,#1
@    LDMIA sp!, {r1-r12, r14}    @ restore the architecture state by cllu
@    MOV pc, r14 @ return ca9_max_power_fp test 
@        
@    SEV @; wake up CPU0
@    WFI 
@    
@fail_case:        @Fail here
@    MOV r0 ,#0
    
    LDMIA sp!, {r1-r12, r14}    @ restore the architecture state by cllu
    MOV pc, r14 @ return ca9_max_power_fp test 

    .end
