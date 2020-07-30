#include <sys/types.h>

__attribute__((weak)) ssize_t sys_read(int file, void *ptr, size_t len)
{
    return -1;
}
