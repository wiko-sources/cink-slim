    .text

    .equ Z_bit, 1 << 11 @ Prediction on
    .extern cpu_speed_test_pt_update
    .extern cpu_speed_test_pt_restore
    .extern g_NeonTestMem
    .extern g_iNeonDATA1_CPU0
    .extern g_iNeonDATA2_CPU1

neon_test_addr:
    .word g_NeonTestMem

nDATA1:
    .word g_iNeonDATA1_CPU0

nDATA2:
    .word g_iNeonDATA2_CPU1
        
    .global ca9_neon
ca9_neon:
    STMDB sp!, {r1-r12, r14}  @ save the architecture state

@    MCR p15,0,r0,c7,c5,6  @ invalidate btac
@    @ switch on prediction
@    LDR r1,=Z_bit
@    MRC p15,0,r0,c1,c0,0    @ read Control reg
@    ORR r0,r0,r1
@    MCR p15,0,r0,c1,c0,0    @ write Control reg
@
@    @ set test page to L1C WBWA Normal, L2C NC
@@    BL cpu_speed_test_pt_update
@
@    @ Enabling Coprocessors
@    LDR r0,=0x0FFFFFFF
@    MCR p15,0,r0,c1,c0,2
@
@    @ Enable Neon
@    MOV R0, #0x40000000
@    FMXR FPEXC, r0

    @; read CPU ID register
    MRC p15,0,r3,c0,c0,5
    AND r3,r3,#0xf
    CMP r3,#0
    BEQ power_loop_cpu0
    CMP r3,#1
    BEQ power_loop_cpu1

@; start the cpu0 infinite loop that maximizes consumption.
power_loop_cpu0:     
               
@; start the infinite loop that maximizes consumption.

    LDR R0, neon_test_addr
    LDR R0, [R0]
    @LDR R3, =DATA1
    LDR R3, nDATA1
    LDR R3, [R3]
    
    VLDMIA R0!, {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15}
    VLDMIA R0, {D16,D17,D18,D19,D20,D21,D22,D23,D24,D25,D26,D27,D28,D29,D30,D31}

    MOV R7,#0x0

    B power_neon_loop

power_neon_loop:
    VMLA.I32 Q8, Q0, Q1
    VMLA.I32 Q9, Q1, Q2
    VMLA.I32 Q10, Q10, Q3
    VMLAL.U16 Q11, D1, D8
    @VST1.64 {D16}, [R3@64]
    VST1.64 {D16}, [R3] @ store 2 Words to [R3]
    VMLA.I32 Q5, Q0, Q1
    VMLA.I32 Q6, Q1, Q0
    VMLA.I32 Q7, Q9, Q5
    VMLAL.U16 Q0, D5, D6
    VMLA.I32 Q1, Q1, Q11
    VMLA.I32 Q10, Q7, Q5
    @VST1.64 {D0}, [R3@64]
    VST1.64 {D0}, [R3] @ store 2 Words to [R3]

    ADD R7, R7, #0x1

    CMP R7, #0x100

    BNE power_neon_loop
    
    MOV r0,#0x0
    VSTMIA  R3!, {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15}
    VSTMIA  R3,  {D16,D17,D18,D19,D20,D21,D22,D23,D24,D25,D26,D27,D28,D29,D30,D31}
    @LDR R3, =DATA1
    LDR R3, nDATA1
    LDR R3, [R3]
    
	LDMIA   R3!, {r4-r11} @0~7
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @8~15
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @16~23
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @24~31
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @32~39
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @40~47
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @48~55
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @56~63
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	
    @;CPU1 fail, CPU0 always idle
    @;CPU1 pass, CPU0 exit WFE & return to main
	       
	LDR r1,=0xe2080bdf
	@DSB
    @;WFE     @; waked up by CPU1
	
	@;check max_power correctness	
	CMP r0,r1
	MOVEQ r0, #0x1
	@; Fail here
    MOVNE r0 ,#0x0
                   
    LDMIA   sp!, {r1-r12, r14} @restore the architecture state by cllu
    MOV     pc , r14    @return ca9_max_power_fp test 
     
@;====== CPU1  start the infinite loop that maximizes consumption.
power_loop_cpu1:

@; start the infinite loop that maximizes consumption.
    LDR R0, neon_test_addr
    LDR R0, [R0]
    @LDR R3, =DATA2
    LDR R3, nDATA2
    LDR R3, [R3]
    
    VLDMIA R0!, {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15}
    VLDMIA R0, {D16,D17,D18,D19,D20,D21,D22,D23,D24,D25,D26,D27,D28,D29,D30,D31}

    MOV R7,#0x0
    B power_neon_loop_1
    .align 6

power_neon_loop_1:
    VMLA.I32 Q8, Q0, Q1
    VMLA.I32 Q9, Q1, Q2
    VMLA.I32 Q10, Q10, Q3
    VMLAL.U16 Q11, D1, D8
    @VST1.64 {D16}, [R3@64]
    VST1.64 {D16}, [R3] @ store 2 Words to [R3]
    VMLA.I32 Q5, Q0, Q1
    VMLA.I32 Q6, Q1, Q0
    VMLA.I32 Q7, Q9, Q5
    VMLAL.U16 Q0, D5, D6
    VMLA.I32 Q1, Q1, Q11
    VMLA.I32 Q10, Q7, Q5
    @VST1.64 {D0}, [R3@64]
    VST1.64 {D0}, [R3] @ store 2 Words to [R3]

    ADD R7, R7, #0x1

    CMP R7, #0x100

    BNE power_neon_loop_1

@;signature the value of the registers    
    MOV r0,#0x0
    VSTMIA  R3!, {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15}
    VSTMIA  R3,  {D16,D17,D18,D19,D20,D21,D22,D23,D24,D25,D26,D27,D28,D29,D30,D31}
    @LDR R3, =DATA2
    LDR R3, nDATA2
    LDR R3, [R3]
    
	LDMIA   R3!, {r4-r11} @0~7
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @8~15
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @16~23
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @24~31
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @32~39
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @40~47
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @48~55
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	LDMIA   R3!, {r4-r11} @56~63
	ADD  r0,r0,r4
	ADD  r0,r0,r5
	ADD  r0,r0,r6
	ADD  r0,r0,r7
	ADD  r0,r0,r8
	ADD  r0,r0,r9
	ADD  r0,r0,r10
	ADD  r0,r0,r11
	
    @;CPU1 fail, return to main
    @;CPU1 pass, wake up CPU0, CPU1 idle

	@;check max_power correctness		       
	LDR r1,=0xe2080bdf
	CMP r0,r1
	BNE fail_case
	
	MOV r0 ,#0x1

    LDMIA   sp!, {r1-r12, r14} @restore the architecture state by cllu
    MOV     pc , r14    @return ca9_max_power_fp test 
    
    @;SEV @; wake up CPU0
    @;WFI 
		 	
fail_case:  @;Fail here
    MOV r0 ,#0x0

    LDMIA   sp!, {r1-r12, r14} @restore the architecture state by cllu
    MOV     pc , r14    @return ca9_max_power_fp test 
    .end
