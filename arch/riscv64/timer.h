#ifndef __TIMER_H__
#define __TIMER_H__

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------
#ifndef CONFIG_CPU_HZ
#define CONFIG_CPU_HZ         25000000
#endif

//-----------------------------------------------------------------
// Types
//-----------------------------------------------------------------
typedef unsigned int   t_time;

//-----------------------------------------------------------------
// Prototypes:
//-----------------------------------------------------------------

// General timer
void            timer_init(void);
t_time          timer_now(void);
static int      timer_diff(t_time a, t_time b) { return (int)(a - b); } 
void            timer_sleep(int timeMs);
void            timer_set_mtimecmp(t_time next);
t_time          timer_get_mtime(void);

#endif
