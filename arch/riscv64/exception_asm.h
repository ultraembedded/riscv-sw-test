//-----------------------------------------------------------------
// ISR Vector
//-----------------------------------------------------------------
.global isr_vector
.global exception_return
isr_vector:
#ifdef EXCEPTION_SP_FROM_MSCRATCH
    csrrw sp, mscratch, sp
#endif
    addi sp, sp, -(35*8)

    // Save registers
    sd x31, (8*34)(sp)
    sd x30, (8*33)(sp)
    sd x29, (8*32)(sp)
    sd x28, (8*31)(sp)
    sd x27, (8*30)(sp)
    sd x26, (8*29)(sp)
    sd x25, (8*28)(sp)
    sd x24, (8*27)(sp)
    sd x23, (8*26)(sp)
    sd x22, (8*25)(sp)
    sd x21, (8*24)(sp)
    sd x20, (8*23)(sp)
    sd x19, (8*22)(sp)
    sd x18, (8*21)(sp)
    sd x17, (8*20)(sp)
    sd x16, (8*19)(sp)
    sd x15, (8*18)(sp)
    sd x14, (8*17)(sp)
    sd x13, (8*16)(sp)
    sd x12, (8*15)(sp)
    sd x11, (8*14)(sp)
    sd x10, (8*13)(sp)
    sd x9,  (8*12)(sp)
    sd x8,  (8*11)(sp)
    sd x7,  (8*10)(sp)
    sd x6,  (8* 9)(sp)
    sd x5,  (8* 8)(sp)
    sd x4,  (8* 7)(sp)
    sd x3,  (8* 6)(sp)
    sd x2,  (8* 5)(sp) // SP
    sd x1,  (8* 4)(sp) // RA
    //sd x0,  (8* 3)(sp) // ZERO

    csrr s0, mcause
    sd s0,  (8* 2)(sp)

    csrr s0, mstatus
    sd s0,  (8* 1)(sp)

    csrr s0, mepc
    sd s0,  (8* 0)(sp)

    // Call ISR handler
    mv a0, sp
    jal exception_handler

exception_return:
    mv sp, a0

    // Restore registers
    ld s0,  (8* 0)(sp)
    csrw mepc, s0

    ld s0,  (8* 1)(sp)
    csrw mstatus, s0

    //ld s0,  (8* 2)(sp)
    //csrw mcause, s0

    // ld(HOLE): x0 / ZERO
    ld x1,  (8* 4)(sp)
    // ld(HOLE): x2 / SP
    ld x3,  (8* 6)(sp)
    ld x4,  (8* 7)(sp)
    ld x5,  (8* 8)(sp)
    ld x6,  (8* 9)(sp)
    ld x7,  (8*10)(sp)
    ld x8,  (8*11)(sp)
    ld x9,  (8*12)(sp)
    ld x10, (8*13)(sp)
    ld x11, (8*14)(sp)
    ld x12, (8*15)(sp)
    ld x13, (8*16)(sp)
    ld x14, (8*17)(sp)
    ld x15, (8*18)(sp)
    ld x16, (8*19)(sp)
    ld x17, (8*20)(sp)
    ld x18, (8*21)(sp)
    ld x19, (8*22)(sp)
    ld x20, (8*23)(sp)
    ld x21, (8*24)(sp)
    ld x22, (8*25)(sp)
    ld x23, (8*26)(sp)
    ld x24, (8*27)(sp)
    ld x25, (8*28)(sp)
    ld x26, (8*29)(sp)
    ld x27, (8*30)(sp)
    ld x28, (8*31)(sp)
    ld x29, (8*32)(sp)
    ld x30, (8*33)(sp)
    ld x31, (8*34)(sp)

    addi sp, sp, (35*8)
#ifdef EXCEPTION_SP_FROM_MSCRATCH
    csrrw sp, mscratch, sp
#endif
    mret

.global exception_syscall
exception_syscall:
    ecall
    ret
