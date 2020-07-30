#include <sys/types.h>

__attribute__((weak)) int sys_open(const char *name, int flags, int mode)
{
    return -1;
}
