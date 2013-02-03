    .text

    .extern cpu_speed_test_pt_update
    .extern cpu_speed_test_pt_restore
    .extern g_NeonTestMem    
    .extern g_iNeonADDR1_CPU0
    .extern g_iNeonADDR2_CPU0
    .extern g_iNeonADDR3_CPU0
    .extern g_iNeonADDR4_CPU0
    .extern g_iNeonADDR5_CPU0
    .extern g_iNeonADDR6_CPU0
    .extern g_iNeonADDR7_CPU0
    .extern g_iNeonADDR8_CPU0

    .extern g_iNeonADDR1_CPU1
    .extern g_iNeonADDR2_CPU1
    .extern g_iNeonADDR3_CPU1
    .extern g_iNeonADDR4_CPU1
    .extern g_iNeonADDR5_CPU1
    .extern g_iNeonADDR6_CPU1
    .extern g_iNeonADDR7_CPU1
    .extern g_iNeonADDR8_CPU1

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

neon_test_addr:
    .word g_NeonTestMem
    
nADDR1_CPU0:
    .word g_iNeonADDR1_CPU0

nADDR2_CPU0:
    .word g_iNeonADDR2_CPU0

nADDR3_CPU0:
    .word g_iNeonADDR3_CPU0

nADDR4_CPU0:
    .word g_iNeonADDR4_CPU0

nADDR5_CPU0:
    .word g_iNeonADDR5_CPU0

nADDR6_CPU0:
    .word g_iNeonADDR6_CPU0

nADDR7_CPU0:
    .word g_iNeonADDR7_CPU0

nADDR8_CPU0:
    .word g_iNeonADDR8_CPU0


nADDR1_CPU1:               
    .word g_iNeonADDR1_CPU1
                           
nADDR2_CPU1:               
    .word g_iNeonADDR2_CPU1
                           
nADDR3_CPU1:               
    .word g_iNeonADDR3_CPU1
                           
nADDR4_CPU1:               
    .word g_iNeonADDR4_CPU1
                           
nADDR5_CPU1:               
    .word g_iNeonADDR5_CPU1
                           
nADDR6_CPU1:               
    .word g_iNeonADDR6_CPU1
                           
nADDR7_CPU1:               
    .word g_iNeonADDR7_CPU1
                           
nADDR8_CPU1:               
    .word g_iNeonADDR8_CPU1

@final_result:
@    .word 0x0

    .global max_power_neon_loop
max_power_neon_loop:
    STMDB sp!, {r1-r12, r14}  @ save the architecture state
    
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
    LDR r0, nADDR3_CPU0 @=ADDR3
    LDR r0, [r0]
    LDR r1, nADDR4_CPU0 @=ADDR4
    LDR r1, [r1]
    LDR r6, nADDR5_CPU0 @=ADDR5
    LDR r6, [r6]
    LDR r7, nADDR6_CPU0 @=ADDR6
    LDR r7, [r7]
    LDR r8, nADDR7_CPU0 @=ADDR7
    LDR r8, [r8]
    LDR r9, nADDR8_CPU0 @=ADDR8
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
    LDR r0, nADDR3_CPU1 @=ADDR3
    LDR r0, [r0]
    LDR r1, nADDR4_CPU1 @=ADDR4
    LDR r1, [r1]
    LDR r6, nADDR5_CPU1 @=ADDR5
    LDR r6, [r6]
    LDR r7, nADDR6_CPU1 @=ADDR6
    LDR r7, [r7]
    LDR r8, nADDR7_CPU1 @=ADDR7
    LDR r8, [r8]
    LDR r9, nADDR8_CPU1 @=ADDR8
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

init_data_end:
    
@;=============This Part is NEON MAX POWER=========
    @ Enabling Coprocessors
    LDR r0,=0x0FFFFFFF
    MCR p15,0,r0,c1,c0,2

    @ Enable Neon
    MOV R0, #0x40000000
    FMXR FPEXC, r0

    LDR R0, neon_test_addr
    LDR R0, [R0]
    
    VLDMIA R0!, {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15}
    VLDMIA R0, {D16,D17,D18,D19,D20,D21,D22,D23,D24,D25,D26,D27,D28,D29,D30,D31}
@;=================================================   

@@@    @ invalidate SCU tag RAMs
@@@    LDR r10, =PERIPH_BASE + 0xc
@@@
@@@    LDR r11, =0xffff
@@@    STR r11, [r10]

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
    VMLA.I32 Q1, Q1, Q11
    VMLA.I32 Q8, Q0, Q1
                    
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
   	SUB   r10,r11,#2
    VMLA.I32 Q8, Q0, Q1
    VMLA.I32 Q9, Q1, Q2
    LDMIA  r0,{r2,r3}
    LDMDB  r1,{r7,r14}
    LDMIA  r0,{r7,r14}
    MUL    r7,r7,r6
    ADD   r2,r12,#1
    MUL    r10,r8,r9
    LDMDB  r1,{r2,r3}
                
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

@    @CPU1 fail, CPU0 always idle
@    @CPU1 pass, CPU0 exit WFE & return to main
@               
@	LDR r1,=0x34492d7c
@	DSB
@    WFE  @;waked up by CPU1
@               
@	@;check max_power correctness	
@	CMP r0,r1
@	MOVEQ r0, #0x0
@	@; Fail here
@    MOVNE r0 ,#0xFF
    
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
@@@    LDR r10,=PERIPH_BASE + 0xc
@@@    LDR r11,=0xffff
@@@    STR r11,[r10]  @invalidate SCU(background)
    
    LDMIA r0, {r2, r3}
    LDMDB r1, {r4, r14}
    LDMIA r0, {r4, r14}
    MUL r4, r7, r6
    ADDS r2, r12, #1
    MUL r10, r8, r9
    LDMDB r1, {r2, r3}
    SUBS r10, r11, #2
    VMLA.I32 Q1, Q1, Q11
    VMLA.I32 Q8, Q0, Q1    
    
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
	SUB   r10,r11,#2
    VMLA.I32 Q8, Q0, Q1
    VMLA.I32 Q9, Q1, Q2
	LDMIA  r0,{r2,r3}
    LDMDB  r1,{r7,r14}
    LDMIA  r0,{r7,r14}
    MUL    r7,r7,r6
    ADD   r2,r12,#1
    MUL    r10,r8,r9
    LDMDB  r1,{r2,r3}

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
@	LDR r1,=0x34492d7c
@	CMP r0,r1	
@	BNE fail_case
@    SEV @; wake up CPU0
@    WFI 
@fail_case:        @Fail here
@    MOV r0 ,#0xFF
    
    LDMIA sp!, {r1-r12, r14}    @ restore the architecture state by cllu
    MOV pc, r14 @ return ca9_max_power_fp test 
@///////////////////////////////////////////////////
    .end
