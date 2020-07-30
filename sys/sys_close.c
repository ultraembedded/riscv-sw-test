#include <sys/types.h>

__attribute__((weak)) int sys_close(int file)
{
    return -1;
}
