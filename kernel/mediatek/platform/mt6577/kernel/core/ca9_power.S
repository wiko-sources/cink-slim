    .text

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void arm_ca9_dynamic_gating_enable(unsigned int n):
@   Function: enable/disable CA9 dynamic clock gating
@          n: 0 --> disable, otherwise enable
@   Return  : N/A
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global arm_ca9_dynamic_gating_enable
arm_ca9_dynamic_gating_enable:
    TEQ r0, #0
    MRC p15, 0, r0, c15, c0, 0      @ read CA9's Power Control Register
    BICEQ r0, r0, #1                @ disable dynamic clock gating if n == 0
    ORRNE r0, r0, #1                @ enable dynamic clock gating if n != 0
    MCR p15,0, r0, c15, c0, 0       @ write CA9's Power Control Register
    BX lr                           @ return

    .end
