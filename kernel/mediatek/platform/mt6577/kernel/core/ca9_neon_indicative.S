    .text

    .extern cpu_speed_test_pt_update
    .extern cpu_speed_test_pt_restore
    .extern g_NeonTestMem
    .extern g_iNeonDATA5_CPU1
    .extern g_iNeonDATA6_CPU0    

neon_test_addr:
    .word g_NeonTestMem

nDATA5:
    .word g_iNeonDATA5_CPU1
nDATA6:
    .word g_iNeonDATA6_CPU0
    
    .global ca9_neon_indicative
ca9_neon_indicative:
    STMDB sp!, {r1-r12, r14}  @ save the architecture state
    
    .equ    NFlag,      0x80000000
    .equ    ZFlag,      0x40000000
    .equ    CFlag,      0x20000000
    .equ    VFlag,      0x10000000
    .equ    QFlag,      0x08000000
    .equ    GE3Flag,    0x00080000
    .equ    GE2Flag,    0x00040000
    .equ    GE1Flag,    0x00020000
    .equ    GE0Flag,    0x00010000
    .equ    NEON_CPSR_FLGS_MASK, 0xf90f0000
              
@;    @GBLA    LocalNesting
@;    @GBLA    LocalCount
@;    @GBLA    LocalTemp
@;    @GBLS    l
@;    .set LocalNesting,  0
@;    .set LocalCount,    0
@;    .set LocalTemp,     0
@;    .set l, "_\LocalCount"
@;
@;                WHILE   LocalTemp <= 15
@;    @GBLA    LocalArray$LocalTemp
@;    .set LocalTemp,     LocalTemp + 1
@;                WEND


@*******************************************************************************
@ MACRO definition


@;    .macro LOCAL
@;@$label          LOCAL
@;@                OPT     32+8192                         @ no vars and conditionals
@;@$label
@;        .if       LocalNesting > 15
@;            .exitm
@;        .endif
@;        .set LocalNesting,  LocalNesting + 1
@;        .set LocalCount,    LocalCount + 1
@;        .set LocalArray\LocalNesting,   LocalCount
@;        .set l, "_\LocalCount"  
@;    .endm
@;
@;    .macro LOCAL_END
@;@$label          LOCAL_END
@;@                OPT     32+8192                         @ no vars and conditionals
@;@$label
@;        .if       LocalNesting <= 0
@;            .exitm
@;        .endif
@;        .set LocalNesting,  LocalNesting - 1
@;        .set LocalTemp,     LocalArray\LocalNesting
@;        .set l, "_\LocalTemp"                
@;     
@;    .endm

@$Label          NEON_INTLK_TEST
    .macro NEON_INTLK_TEST sFlag, eFlag, e_dval1, e_dval2, e_dval3, e_dval4, e_dval5, e_dval6, e_reg1, e_reg2, e_reg3, e_reg4, e_reg5, e_reg6, e_rval1, e_rval2, e_rval3, e_rval4, e_rreg1, e_rreg2, e_rreg3, e_rreg4, instr0, instr1, instr2, instr3, instr4, instr5, instr6, instr7

@                LOCAL

        @ Program flag bits of CPSR register
                @.if     \sFlag != '-'
                .ifnc   "\sFlag", "-"
                LDR     R0,=\sFlag
                MSR     CPSR_sf,R0                      @ Set flags
                .else
                MOV     R0,#0
                MSR     CPSR_sf,R0                      @ Reset flags
                .endif
        @ Reset ARM registers
                MOV     R0, #0x0
                MOV     R1, #0x0
                MOV     R2, #0x0
                MOV     R3, #0x0
                MOV     R4, #0x0
                MOV     R5, #0x0
                MOV     R6, #0x0
                MOV     R7, #0x0
                MOV     R8, #0x0
                MOV     R9, #0x0
                MOV     R10, #0x0
                MOV     R11, #0x0
                MOV     R12, #0x0
                MOV     R13, #0x0
                MOV     R14, #0x0

        @ Initialize neon registers
                @;comment out R0 backup
                @;STR     R0,label0
                @LDR R0, =NEON_INTL_DATA_ADDR_1
                LDR R0, neon_test_addr
                LDR R0, [R0]
                VLDMIA  R0!, {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15}
                VLDMIA  R0,  {D16,D17,D18,D19,D20,D21,D22,D23,D24,D25,D26,D27,D28,D29,D30,D31}
                @;comment out R0 restore
                @;LDR     R0,label0
		        @;add R0 restore
		        MOV R0,#0x0

        @ Execute the instruction sequence

0:               
                \instr0
1:
                \instr1
2:               
                \instr2
3:               
                \instr3
4:               
                \instr4
5:               
                \instr5
6:               
                \instr6
7:               
                \instr7

                @;comment out R0,R1,R2 backup
                @;STR r0, label0
                @;STR r1, label1
                @;STR r2, label2

                MRS     R2,CPSR                 @ Copy CPSR in R2 just after
                                                @ executing the IUT
        @ Check CPSR flags
                @.if     \eFlag != '-'
                .ifnc   "\eFlag", "-"
                    LDR     R0,=\eFlag
                    LDR     R1,=NEON_CPSR_FLGS_MASK
                    AND     R1,R1,R2
                    TEQ     R0,R1
                    BNE     Neon_test_fail
                .endif

                @;comment out R0,R1,R2 restore
                @;LDR r0, label0
                @;LDR r1, label1
                @;LDR r2, label2
		        @;ADD r0,r1,r2 restore
		        MOV r0, #0x0
		        MOV r1, #0x0
		        MOV r2, #0x0                

        @ Check for the first ARM destination register
                @.if    \e_rreg1 <> '-' && \e_rval1 <> '-'
                .ifnc   "\e_rreg1", "-"
                    .ifnc   "\e_rval1", "-"
                    MOV     R14,\e_rreg1
                    LDR     R13,=\e_rval1
                    @LDR     R11, =e_rreg1\l
                    LDR     R11, =e_rreg1\@       
                    TEQ     R13,R14
                    BNE     Neon_test_fail
                    .endif
                .endif

        @ Check for the second ARM destination register
                @.if    \e_rreg2 <> '-' && \e_rval2 <> '-'
                .ifnc   "\e_rreg2", "-"
                    .ifnc   "\e_rval2", "-"
                    MOV     R14,\e_rreg2
                    LDR     R13,=\e_rval2
                    @LDR     R11, =e_rreg2\l
                    LDR     R11, =e_rreg2\@
                    TEQ     R13,R14
                    BNE     Neon_test_fail
                    .endif
                .endif

        @ Check for the third ARM destination register
                @.if    \e_rreg3 <> '-' && \e_rval3 <> '-'
                .ifnc   "\e_rreg3", "-"
                    .ifnc   "\e_rval3", "-"
                    MOV     R14,\e_rreg3
                    LDR     R13,=\e_rval3
                    @LDR     R11, =e_rreg3\l
                    LDR     R11, =e_rreg3\@
                    TEQ     R13,R14
                    BNE     Neon_test_fail
                    .endif
                .endif

        @ Check for the fourth ARM destination register
                @.if    \e_rreg4 <> '-' && \e_rval4 <> '-'
                .ifnc   "\e_rreg4", "-"
                    .ifnc   "\e_rval4", "-"
                    MOV     R14,\e_rreg4
                    LDR     R13,=\e_rval4
                    @LDR     R11, =e_rreg4\l
                    LDR     R11, =e_rreg4\@
                    TEQ     R13,R14
                    BNE     Neon_test_fail
                    .endif
                .endif

        @ Check the value of the first output dw register
                @.if       \e_dval1 <> '-' && \e_reg1 <> '-'
               .ifnc    "\e_dval1", "-"
                    .ifnc   "\e_reg1", "-"
                    @LDR     R0,=store_DW1\l
                    LDR     R0,=store_DW1\@
                    LDRD    R10, [R0]
                    VMOV    R0, R1, \e_reg1
                    TEQ     R0,R10
                    TEQEQ   R1,R11
                    BNE     Neon_test_fail
                    .endif
                .endif

        @ Check the value of the second output dw register
                @.if       \e_dval2 <> '-' && \e_reg2 <> '-'
                .ifnc   "\e_dval2", "-"
                    .ifnc   "\e_reg2", "-"
                    @LDR     R0,=store_DW2\l
                    LDR     R0,=store_DW2\@
                    LDRD    R10, [R0]
                    VMOV    R0, R1, \e_reg2
                    TEQ     R0,R10
                    TEQEQ   R1,R11
                    BNE     Neon_test_fail
                    .endif
                .endif

        @ Check the value of the third output dw register
                @.if       \e_dval3 <> '-' && \e_reg3 <> '-'
                .ifnc   "\e_dval3", "-"
                    .ifnc   "\e_reg3", "-"
                    @LDR     R0,=store_DW3\l
                    LDR     R0,=store_DW3\@
                    LDRD    R10, [R0]
                    VMOV    R0, R1, \e_reg3
                    TEQ     R0,R10
                    TEQEQ   R1,R11
                    BNE     Neon_test_fail
                    .endif
                .endif

        @ Check the value of the fourth output dw register
                @.if       \e_dval4 <> '-' && \e_reg4 <> '-'
                .ifnc   "\e_dval4", "-"
                    .ifnc   "\e_reg4", "-"
                    @LDR     R0,=store_DW4\l
                    LDR     R0,=store_DW4\@
                    LDRD    R10, [R0]
                    VMOV    R0, R1, \e_reg4
                    TEQ     R0,R10
                    TEQEQ   R1,R11
                    BNE     Neon_test_fail
                    .endif
                .endif

        @ Check the value of the fifth output dw register
                @.if       \e_dval5 <> '-' && \e_reg5 <> '-'
                .ifnc   "\e_dval5", "-"
                    .ifnc   "\e_reg5", "-"
                    @LDR     R0,=store_DW5\l
                    LDR     R0,=store_DW5\@
                    LDRD    R10, [R0]
                    VMOV    R0, R1, \e_reg5
                    TEQ     R0,R10
                    TEQEQ   R1,R11
                    BNE     Neon_test_fail
                    .endif
                .endif

        @ Check the value of the sixth output dw register
                @.if       \e_dval6 <> '-' && \e_reg6 <> '-'
                .ifnc   "\e_dval6", "-"
                    .ifnc   "\e_reg6", "-"               
                    @LDR     R0,=store_DW6\l
                    LDR     R0,=store_DW6\@
                    LDRD    R10, [R0]
                    VMOV    R0, R1, \e_reg6
                    TEQ     R0,R10
                    TEQEQ   R1,R11
                    BNE     Neon_test_fail
                    .endif
                .endif

                @B       TestDone\l
                B       TestDone\@

                @ALIGN 4
                .align

                @.if       \e_dval1 <> '-' && \e_reg1 <> '-'
                .ifnc   "\e_dval1", "-"
                    .ifnc   "\e_reg1", "-"
@store_DW1\l     DCQ     \e_dval1
store_DW1\@:
                .octa   \e_dval1
                    .endif
                .endif
                @.if       \e_dval2 <> '-' && \e_reg2 <> '-'
                .ifnc   "\e_dval2", "-"
                    .ifnc   "\e_reg2", "-"
@store_DW2\l     DCQ     \e_dval2
store_DW2\@:
                .octa   \e_dval2
                    .endif
                .endif
                @.if       \e_dval3 <> '-' && \e_reg3 <> '-'
                .ifnc   "\e_dval3", "-"
                    .ifnc   "\e_reg3", "-"
@store_DW3\l     DCQ     \e_dval3
store_DW3\@:
                .octa \e_dval3
                    .endif
                .endif
                @.if       \e_dval4 <> '-' && \e_reg4 <> '-'
                .ifnc   "\e_dval4", "-"
                    .ifnc   "\e_reg4", "-"
@store_DW4\l     DCQ     \e_dval4
store_DW4\@:
                .octa   \e_dval4
                    .endif
                .endif
                @.if       \e_dval5 <> '-' && \e_reg5 <> '-'
                .ifnc   "\e_dval5", "-"
                    .ifnc   "\e_reg5", "-"
@store_DW5\l     DCQ     \$e_dval5
store_DW5\@:
                .octa   \e_dval5
                    .endif
                .endif
                @.if       \e_dval6 <> '-' && \e_reg6 <> '-'
                .ifnc   "\e_dval6", "-"
                    .ifnc   "\e_reg6", "-"
@store_DW6\l     DCQ     \e_dval6
store_DW6\@:
                .octa   \e_dval6
                    .endif
                .endif

                @ALIGN
                .align

@Test_pass\l:
@TestDone\l:

@Test_fail\l:
@TestEnd\l:

Test_pass\@:
TestDone\@:

Test_fail\@:
TestEnd\@:

@               LOCAL_END
    .endm

    @; read CPU ID register
    MRC p15,0,r3,c0,c0,5
    AND r3,r3,#0xf
    CMP r3,#0
    BEQ power_loop_cpu0
    CMP r3,#1
    BEQ power_loop_cpu1

@; start the cpu0 infinite loop that maximizes consumption.
power_loop_cpu0: 

@;==================================================================
                @;======= backup R13 (R13 will be used) =======
       	        @LDR  r1, =DATA6;
		        LDR r1, nDATA6
		        LDR r1, [r1]	       	        
		        STR  r13,[r1]
		        @;======= increment =======
		        LDR  r0, =0x0
		        STR  r0,[r1,#0x4]
		        
@;-------------------------------------------------------------------------------
@; Test cases start
@;-------------------------------------------------------------------------------

power_loop_0:
power_loop_1:

@VPADD.I16.U8_B2B_001     
@NEON_INTLK_TEST NFlag+QFlag, NFlag+QFlag, 0x000090F30000C0AB, 0x008200FF00FF0047, 0x008200FF00FF0146, 0x00CA014F0169019C, -, -, D2, D1, D22, D23, -, -, -, -, -, -, -, -, -, -, "VPADDL.I16.U8  Q11,Q0", "VPADDL.I16.U8  D1,D22", "VPADDL.I32.U16 D2,D21"
NEON_INTLK_TEST NFlag+QFlag, NFlag+QFlag, 0x000090F30000C0AB, 0x008200FF00FF0047, 0x008200FF00FF0146, 0x00CA014F0169019C, -, -, D2, D1, D22, D23, -, -, -, -, -, -, -, -, -, -, "VPADDL.U8  Q11,Q0", "VPADDL.U8  D1,D22", "VPADDL.U16 D2,D21"

@VZIP.16_B2B_002 
NEON_INTLK_TEST NFlag+CFlag+GE1Flag, NFlag+CFlag+GE1Flag, 0x7E81359DFB54AA88, 0x1CE1C675DE566687, 0x1C6665619E2C137B, 0x37CF73CF231359EA, 0x01BF0F474BFBBDDF, 0x4872372046B9FF6A, D4, D5, D0, D1, D2, D3, -, -, -, -, -, -, -, -, "VZIP.16  Q0,Q1", "VZIP.16  Q1,Q0", "VZIP.16  Q2,Q0"

@VSUB.I64.I64.S32_B2B_003        
@NEON_INTLK_TEST ZFlag+CFlag+GE2Flag, ZFlag+CFlag+GE2Flag, 0x1D2D85A155133A95, 0xD5502F37731F487A, 0x37CF1CE14417F5B5, 0x2313DE55D51913B8, 0x6206FB8F045A0C0A, 0x690DC509C164D54C, D4, D5, D0, D1, D24, D25, -, -, -, -, -, -, -, -, "VSUBW.I64.I64.S32 Q12,Q7,D0", "VSUBW.I64.I64.S32 Q0,Q1,D24", "VSUBW.I64.I64.S32 Q2,Q3,D2"
NEON_INTLK_TEST ZFlag+CFlag+GE2Flag, ZFlag+CFlag+GE2Flag, 0x1D2D85A155133A95, 0xD5502F37731F487A, 0x37CF1CE14417F5B5, 0x2313DE55D51913B8, 0x6206FB8F045A0C0A, 0x690DC509C164D54C, D4, D5, D0, D1, D24, D25, -, -, -, -, -, -, -, -, "VSUBW.S32 Q12,Q7,D0", "VSUBW.S32 Q0,Q1,D24", "VSUBW.S32 Q2,Q3,D2"

@VTRN.8_B2B_004  
NEON_INTLK_TEST GE2Flag+GE1Flag, GE2Flag+GE1Flag, 0xA13D79028839E3CC, 0xABF18E76F8A599D9, 0xF5883F1F68F343EE, 0x8EF70D4DAF3FA6BE, 0xCF66E18172B9BFFB, 0x132C5654206A47DF, D10, D11, D8, D9, D0, D1, -, -, -, -, -, -, -, -, "VTRN.8  Q0,Q1", "VTRN.8  Q4,Q5", "VTRN.8  Q5,Q4"

@VABA.I16.S8_B2B_005     
@NEON_INTLK_TEST NFlag+CFlag+GE1Flag, NFlag+CFlag+GE1Flag, 0x38681D4A488D01C5, 0x2319DE7F37490FB0, 0x1CAC7F5146DA4C01, 0x9EE1FB7CFF87BE8F, 0x3DCD028F3998CD49, 0xF1B776A4A618D9A0, D2, D3, D0, D1, D8, D9, -, -, -, -, -, -, -, -, "VABAL.I16.S8  Q4,D30,D31", "VABAL.I16.S8  Q0,D8,D10", "VABAL.I16.S8  Q1,D9,D11"
NEON_INTLK_TEST NFlag+CFlag+GE1Flag, NFlag+CFlag+GE1Flag, 0x38681D4A488D01C5, 0x2319DE7F37490FB0, 0x1CAC7F5146DA4C01, 0x9EE1FB7CFF87BE8F, 0x3DCD028F3998CD49, 0xF1B776A4A618D9A0, D2, D3, D0, D1, D8, D9, -, -, -, -, -, -, -, -, "VABAL.S8  Q4,D30,D31", "VABAL.S8  Q0,D8,D10", "VABAL.S8  Q1,D9,D11"

@VTRN.8_B2B_006  
NEON_INTLK_TEST ZFlag+CFlag+GE2Flag, ZFlag+CFlag+GE2Flag, 0xECA37074935E017C, 0xBA763341D6BDC071, 0xD1CF0D88D68F3398, 0xD5D15E5CF70BE66D, 0xAD8E60A614A0BFC3, 0x973D67A82A4225DB, D22, D23, D24, D25, D28, D29, -, -, -, -, -, -, -, -, "VTRN.8  Q12,Q13", "VTRN.8  Q14,Q15", "VTRN.8  Q11,Q12"

                @; read CPU ID register
                MRC    p15,0,r3,c0,c0,5
                AND    r3,r3,#0xf
                CMP    r3,#1
                BEQ    power_loop_1_1
               
@success here

		        @;rstore R13 from DATA6
		        @LDR r1, =DATA6;
		        LDR r1, nDATA6
		        LDR r1, [r1]		
		        LDR r2, =0x100;
		        LDR r0,[r1,#0x4]
                ADD r0,r0, #0x1
		        STR r0,[r1,#0x4]
		        CMP r0,r2
		        BNE power_loop_0
		        
                @;success here
                MOV r0, #0x1

cpu0_fail:       @;fail come in

                @;rstore R13 from DATA6
		        @LDR r1, =DATA6;
		        LDR r1, nDATA6
		        LDR r1, [r1]		
		        LDR r13,[r1]
		        
 @; program Control register, disable  MMU , I$, D$
 @               LDR     r1,=L1_CTL_REG
 @               MRC     p15,0,r2,c1,c0,0                ; read Control reg
 @               BIC     r2,r2,r1
 @               MCR     p15,0,r2,c1,c0,0                ; write Control reg


@@@                @;CPU1 fail, CPU0 always idle
@@@                @;CPU1 pass, CPU0 exit WFE & return to main
@@@               
@@@                WFE     @; waked up by CPU1
@@@
@@@	            @;check max_power correctness	

                LDMIA   sp!, {r1-r12, r14} @;restore the architecture state by cllu
                MOV     pc , r14    @;return ca9_max_power_fp test 

@;====== CPU1  start the infinite loop that maximizes consumption.
power_loop_cpu1:

@; start the infinite loop that maximizes consumption.
                @;======= backup R13 (R13 will be used) =======
       	        @LDR  r1, =DATA5;
		        LDR r1, nDATA5
		        LDR r1, [r1]	       	        
		        STR  r13,[r1]
		        @;======= increment =======
		        LDR  r0, =0x0
		        STR  r0,[r1,#0x4]
		        
@;-------------------------------------------------------------------------------
@; Test cases start
@;-------------------------------------------------------------------------------

                B power_loop_1
                
power_loop_1_1:
              
@;success here               

		        @;rstore R13 from DATA6
		        @LDR  r1, =DATA5;
		        LDR r1, nDATA5
		        LDR r1, [r1]
		        LDR  r2, =0x100;
		        LDR  r0,[r1,#0x4]
                ADD  r0,r0, #0x1
		        STR  r0,[r1,#0x4]
		        CMP  r0,r2
		        BNE power_loop_1

                @;success here
                MOV  r0, #0x1
cpu1_fail:
              	@;rstore R13 from DATA6
		        @LDR  r1, =DATA5;
		        LDR r1, nDATA5
		        LDR r13,[r1]

                @;CPU1 fail, return to main
		        @;CPU1 pass, wake up CPU0, CPU1 idle

		        @;check max_power correctness	
	            LDR r1,=0x1
	            CMP r0,r1
		        BNE fail_case
                
@               SEV     @; wake up CPU0
@		        WFI 
@                
                MOV r0 ,#0x1               
                LDMIA   sp!, {r1-r12, r14} @;restore the architecture state by cllu
                MOV     pc , r14    @;return ca9_max_power_fp test 
                		        
fail_case:      @;Fail here
                MOV r0 ,#0x0              
                LDMIA   sp!, {r1-r12, r14} @;restore the architecture state by cllu
                MOV     pc , r14    @;return ca9_max_power_fp test 

Neon_test_fail: 
	            MOV    r0,#0x0      @;fail signature
                @; read CPU ID register
                MRC    p15,0,r3,c0,c0,5
                AND    r3,r3,#0xf
                CMP    r3,#0
                BEQ    cpu0_fail
                CMP    r3,#1
                BEQ    cpu1_fail
                       
@label0          DCD     0
@label1          DCD     0
@label2          DCD     0

label0:
    .word 0    
label1:
    .word 0
label2:
    .word 0
    
    .end
