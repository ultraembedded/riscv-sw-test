#ifndef __CSR_H__
#define __CSR_H__

#include <stdint.h>

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------
#define SR_SIE          (1 << 1)
#define SR_MIE          (1 << 3)
#define SR_SPIE         (1 << 5)
#define SR_MPIE         (1 << 7)

#define IRQ_S_SOFT       1
#define IRQ_M_SOFT       3
#define IRQ_S_TIMER      5
#define IRQ_M_TIMER      7
#define IRQ_S_EXT        9
#define IRQ_M_EXT        11

#define SR_IP_MSIP      (1 << IRQ_M_SOFT)
#define SR_IP_MTIP      (1 << IRQ_M_TIMER)
#define SR_IP_MEIP      (1 << IRQ_M_EXT)
#define SR_IP_SSIP      (1 << IRQ_S_SOFT)
#define SR_IP_STIP      (1 << IRQ_S_TIMER)
#define SR_IP_SEIP      (1 << IRQ_S_EXT)

#define MSTATUS_SIE  0x00000002
#define MSTATUS_MIE  0x00000008
#define MSTATUS_SPIE 0x00000020
#define MSTATUS_MPIE 0x00000080
#define MSTATUS_SPP  0x00000100
#define MSTATUS_MPP  0x00001800
#define MSTATUS_MPRV 0x00020000
#define MSTATUS_SUM  0x00040000
#define MSTATUS_MXR  0x00080000

//-----------------------------------------------------------------
// Helpers:
//-----------------------------------------------------------------
#define csr_read(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define csr_write(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#define csr_set(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define csr_clear(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define csr_swap(reg, val) ({ \
    unsigned long __v = (unsigned long)(val); \
    asm volatile ("csrrw %0, " #reg ", %1" : "=r" (__v) : "rK" (__v) : "memory"); \
    __v; })

//-----------------------------------------------------------------
// csr_set_isr_vector: Set exception vector
//-----------------------------------------------------------------
static inline void csr_set_isr_vector(uint32_t addr)
{
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
    asm volatile ("csrw stvec, %0": : "r" (addr));
#else
    asm volatile ("csrw mtvec, %0": : "r" (addr));
#endif
}
//-----------------------------------------------------------------
// csr_get_time: Get current time (MTIME)
//-----------------------------------------------------------------
static inline uint32_t csr_get_time(void)
{
    uint32_t value;
    asm volatile ("csrr %0, time" : "=r" (value) : );
    return value;
}
//-----------------------------------------------------------------
// csr_set_irq_mask: Enable particular interrupts
//-----------------------------------------------------------------
static inline void csr_set_irq_mask(uint32_t mask)
{
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
    asm volatile ("csrs sie,%0": : "r" (mask));
#else
    asm volatile ("csrs mie,%0": : "r" (mask));
#endif
}
//-----------------------------------------------------------------
// csr_clr_irq_mask: Disable particular interrupts
//-----------------------------------------------------------------
static inline void csr_clr_irq_mask(uint32_t mask)
{
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
    asm volatile ("csrc sie,%0": : "r" (mask));
#else
    asm volatile ("csrc mie,%0": : "r" (mask));
#endif
}
//-----------------------------------------------------------------
// csr_get_irq_pending: Bitmap of pending IRQs
//-----------------------------------------------------------------
static inline uint32_t csr_get_irq_pending(void)
{
    uint32_t value;
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
    asm volatile ("csrr %0, sip" : "=r" (value) : );
#else
    asm volatile ("csrr %0, mip" : "=r" (value) : );
#endif
    return value;
}
//-----------------------------------------------------------------
// csr_set_irq_pending: Set pending interrupts (SW IRQ)
//-----------------------------------------------------------------
static inline void csr_set_irq_pending(uint32_t mask)
{
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
    asm volatile ("csrs sip,%0": : "r" (mask));
#else
    asm volatile ("csrs mip,%0": : "r" (mask));
#endif
}
//-----------------------------------------------------------------
// csr_irq_ack: Clear pending interrupts
//-----------------------------------------------------------------
static inline void csr_irq_ack(uint32_t mask)
{
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
    asm volatile ("csrc sip,%0": : "r" (mask));
#else
    asm volatile ("csrc mip,%0": : "r" (mask));
#endif
}
//-----------------------------------------------------------------
// csr_set_irq_enable: Global IRQ enable
//-----------------------------------------------------------------
static inline void csr_set_irq_enable(void)
{
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
    uint32_t mask = SR_SIE;
    asm volatile ("csrs sstatus,%0": : "r" (mask));
#else
    uint32_t mask = SR_MIE;
    asm volatile ("csrs mstatus,%0": : "r" (mask));
#endif
}
//-----------------------------------------------------------------
// csr_clr_irq_enable: Gloabl IRQ disable
//-----------------------------------------------------------------
static inline void csr_clr_irq_enable(void)
{
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
    uint32_t mask = SR_SIE;
    asm volatile ("csrc sstatus,%0": : "r" (mask));
#else
    uint32_t mask = SR_MIE;
    asm volatile ("csrc mstatus,%0": : "r" (mask));
#endif
}
//-----------------------------------------------------------------
// csr_get_irq_enable: Get current IRQ enable state
//-----------------------------------------------------------------
static inline int csr_get_irq_enable(void)
{
    uint32_t value;    
#ifdef CONFIG_ARCH_RISCV_RUN_AS_SUPER
    uint32_t mask = SR_SIE;
    asm volatile ("csrr %0, sstatus" : "=r" (value) : );
#else
    uint32_t mask = SR_MIE;
    asm volatile ("csrr %0, mstatus" : "=r" (value) : );
#endif
    return (value & mask) != 0;
}

#endif
