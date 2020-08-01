#include <sys/types.h>
#include <sys/time.h>

#include "timer.h"

static int      epoch_set;
static uint64_t epoch;
static uint32_t last_mtime;
static uint32_t mtime_upper;

__attribute__((weak)) int sys_gettimeofday(struct timeval *tv, struct timezone *tz)
{
    uint32_t mtime_lower = timer_get_mtime();

    // Detect cycle counter wrap.
    // This is ok as long as the timer service is queried at least once per 2^32 cycles.
    if (mtime_lower < last_mtime)
        mtime_upper++;

    uint64_t mtime = mtime_upper;
    mtime <<= 32;
    mtime |= mtime_lower;

    if (!epoch_set)
    {
        epoch_set = 1;
        epoch     = mtime;
    }

    uint64_t usec = ((mtime - epoch) * 1000000) / CONFIG_CPU_HZ;
    if (tv)
    {
        tv->tv_sec  = usec / 1000000;
        tv->tv_usec = usec % 1000000;
    }

    if (tz)
        tz->tz_minuteswest = tz->tz_dsttime = 0;

    return 0;
}
