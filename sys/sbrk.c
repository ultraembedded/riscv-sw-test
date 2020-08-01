#include <sys/types.h>
#include <assert.h>

//-------------------------------------------------------------
// Defines:
//-------------------------------------------------------------
#ifndef CONFIG_HEAP_MEMSIZE
#define CONFIG_HEAP_MEMSIZE         (4 << 20)
#endif

//-------------------------------------------------------------
// Locals:
//-------------------------------------------------------------
static uint8_t _malloc_mem[CONFIG_HEAP_MEMSIZE];
static uint8_t *_mem_alloc = _malloc_mem;

//-----------------------------------------------------------------
// sbrk_init:
//-----------------------------------------------------------------
void sbrk_init(void)
{
    _mem_alloc = _malloc_mem;
}
//-------------------------------------------------------------
// sbrk: Memory chunk allocator
//-------------------------------------------------------------
caddr_t _sbrk(int incr)
{
    uint8_t *p = _mem_alloc;
    _mem_alloc += incr;

    assert ((_mem_alloc - _malloc_mem) < CONFIG_HEAP_MEMSIZE);

    return (caddr_t)p;
}
