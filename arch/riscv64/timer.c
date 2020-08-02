#include "timer.h"
#include "csr.h"

//-----------------------------------------------------------------
// cpu_timer_get_count:
//-----------------------------------------------------------------
static inline unsigned long cpu_timer_get_count(void)
{    
    unsigned long value;
#ifdef CONFIG_ARCH_RISCV_USE_MCYCLE
    asm volatile ("csrr %0, mcycle" : "=r" (value) : );
#else
    asm volatile ("csrr %0, cycle" : "=r" (value) : );
#endif
    return value;
}
//--------------------------------------------------------------------------
// timer_init:
//--------------------------------------------------------------------------
void timer_init(void)
{

}
//--------------------------------------------------------------------------
// timer_sleep:
//--------------------------------------------------------------------------
void timer_sleep(int timeMs)
{
    t_time t = timer_now();

    while (timer_diff(timer_now(), t) < timeMs)
        ;
}
//--------------------------------------------------------------------------
// timer_now:
//--------------------------------------------------------------------------
t_time timer_now(void)
{
    return cpu_timer_get_count() / (CONFIG_CPU_HZ / 1000);
}
//--------------------------------------------------------------------------
// timer_set_mtimecmp: Non-std mtimecmp support
//--------------------------------------------------------------------------
void timer_set_mtimecmp(t_time next)
{
    csr_write(0x7c0, next);
}
//--------------------------------------------------------------------------
// timer_get_mtime:
//--------------------------------------------------------------------------
t_time timer_get_mtime(void)
{
    return csr_read(0xc00);
}
