#include <sys/types.h>

__attribute__((weak)) off_t sys_lseek(int file, off_t ptr, int dir)
{
    return -1;
}
