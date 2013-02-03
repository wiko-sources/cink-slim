    .text

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void armV7_perf_mon_enable(unsigned int n):
@   Function: enable/disable ARMv7 performance monitor
@          n: 0 --> disable performance monitors
@             otherwise, enable performance monitors
@   Return  : N/A
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global armV7_perf_mon_enable
armV7_perf_mon_enable:
    TEQ r0, #0
    MRCNE   p15, 0, r1, c9, c12, 1  @ Read Count Enable Set Register
    LDR     r2, =0x8000003F         @ Enable cycle counter + 6 event counters
    ORRNE   r1, r1, r2              @ Set counter enable bits
    MCRNE   p15, 0, r1, c9, c12, 1  @ Write Count Enable Set Register
    MRC p15, 0, r1, c9, c12, 0
    BICEQ r1, r1, #1
    ORRNE r1, r1, #1
    MCR p15, 0, r1, c9, c12, 0
    BX lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int armV7_perf_mon_number():
@   Function: get the available event counters
@   Return  : return the available event counters
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global armV7_perf_mon_number
armV7_perf_mon_number:
    MRC p15, 0, r0, c9, c12, 0
    LSR r0, #11
    LDR r1, =0xFFFFFFE0
    BIC r0, r0, r1
    BX lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void armV7_perf_mon_reset():
@   Function: reset ALL ARMv7 performance monitor counters
@   Return  : N/A
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global armV7_perf_mon_reset
armV7_perf_mon_reset:
    MRC p15, 0, r0, c9, c12, 0
    ORR r0, r0, #6
    MCR p15, 0, r0, c9, c12, 0
    BX lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int armV7_perf_mon_get_cyc_cnt():
@   Function: get processor cycle count
@   Return  : processor cycle count
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global armV7_perf_mon_get_cyc_cnt
armV7_perf_mon_get_cyc_cnt:
    MRC p15, 0, r0, c9, c13, 0
    BX lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void armV7_perf_mon_set_cyc_cnt_divider(unsigned int n):
@   Function: set processor cycle count divider, either 1 or 64
@          n: 0 --> divider = 1
@             otherwise, divider = 64
@   Return  : N/A
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global armV7_perf_mon_set_cyc_cnt_divider
armV7_perf_mon_set_cyc_cnt_divider:
    MRC p15, 0, r1, c9, c12, 0
    TEQ r0, #0
    BICEQ r1, r1, #8
    ORRNE r1, r1, #8
    MCR p15, 0, r1, c9, c12, 0
    BX lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int armV7_perf_mon_get_cyc_cnt_divider():
@   Function: get processor cycle count divider
@   Return  : 0 --> 1, otherwise 64
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global armV7_perf_mon_get_cyc_cnt_divider
armV7_perf_mon_get_cyc_cnt_divider:
    MRC p15, 0, r0, c9, c12, 0
    LDR r1, =0xFFFFFFF7
    BIC r0, r0, r1
    BX lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ void armV7_perf_mon_select_event(unsigned int i, unsigned t):
@   Function: select event type for a specific event counter
@          i: event counter index 0 ~ 5
@          t: event type
@   Return  : N/A
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global armV7_perf_mon_select_event
armV7_perf_mon_select_event:
    MCR     p15, 0, r0, c9, c12, 5  @ Write Performance Counter Selection Register
    MCR     p15, 0, r1, c9, c13, 1  @ Write Event Selection Register
    BX lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int armV7_perf_mon_get_event_counter(unsigned int i):
@   Function: get the value of a specific event counter
@          i: event counter index 0 ~ 5
@   Return  : N/A
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global armV7_perf_mon_get_event_counter
armV7_perf_mon_get_event_counter:
    MCR     p15, 0, r0, c9, c12, 5  @ Write Performance Counter Selection Register
    MRC     p15, 0, r0, c9, c13, 2  @ Read Event Counter Register
    BX lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ unsigned int armV7_perf_mon_is_overflow(unsigned int n):
@   Function: get the specified event count value
@          n: event counter index, 0xFF means processor cycle count's index
@   Return  : overflow flag of the specified event counter or processor cycle count
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    .global armV7_perf_mon_is_overflow
armV7_perf_mon_is_overflow:
    MRC     p15, 0, r1, c9, c12, 3  @ Read Overflow Flag Status Register
    CMP     r0, #0xFF
    LSRLT   r1, r0                  @ Shift so ECx overflow bit is bit 0
    LSRGE   r1, #31                 @ Shift so CC overflow bit is bit 0
    AND     r0, r1, #1
    BX lr

    .end
