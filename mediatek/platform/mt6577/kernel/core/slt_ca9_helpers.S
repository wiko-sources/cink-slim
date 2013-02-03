    .text

    .global my_get_ttbcr
my_get_ttbcr:
    MRC p15, 0, r0, c2, c0, 2
    MOV pc, lr

    .global slt_get_ttbr0
slt_get_ttbr0:
    MRC p15,0,r0,c2,c0,0
    MOV pc, lr

    .global slt_dsb
slt_dsb:
    dsb
    MOV pc, lr

    .global slt_isb
slt_isb:
    isb
    MOV pc, lr

    .global slt_invalidate_tlb
slt_invalidate_tlb:
    @   Uses unified TLB, so we do not have to specify the I/D/Unified TLB.
    @      Their results are the same.
    MCR p15,0,r0,c8,c7,0
    DSB
    ISB

    MOV pc, lr

    .global slt_get_nmrr
slt_get_nmrr:
    MRC p15,0,r0,c10,c2,1

    MOV pc, lr

    .global slt_set_nmrr
slt_set_nmrr:
    MCR p15,0,r0,c10,c2,1

    MOV pc, lr

    .global slt_l1c_clean_mva_to_pou
slt_l1c_clean_mva_to_pou:
    MCR p15,0,r0,c7,c11,1

    MOV pc, lr

    .global slt_va_to_pa
slt_va_to_pa:
    MCR p15,0,r0,c7,c8,1
    isb
    MRC p15,0,r0,c7,c4,0

    MOV pc, lr

    .end
