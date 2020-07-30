#include <sys/types.h>

__attribute__((weak)) ssize_t sys_write(int file, const void *ptr, size_t len)
{
    return -1;
}
