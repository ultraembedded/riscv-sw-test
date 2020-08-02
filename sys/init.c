#include "console.h"
#include "syscalls.h"
#include "sbrk.h"
#include "romfs_tar.h"

#ifndef CONFIG_UART_BAUD
#define CONFIG_UART_BAUD 1000000
#endif

#ifdef CONFIG_ROMFS_TAR_SUPPORT
// ROM filesystem embedded into this executable
extern uint32_t _tarfs_start;
extern uint32_t _tarfs_end;
#endif

//-----------------------------------------------------------------
// init: Early init - run before main()
//-----------------------------------------------------------------
void init(void)
{
    // Initialise the UART
    // Baud rate maybe ignored if the baudrate is fixed in HW
    console_init(CONFIG_UART_BAUD);
    console_putstr("Booting...\n");

    // malloc() init
    sbrk_init();

    // Initialise the newlib syscall interface
    syscall_init();

#ifdef CONFIG_ROMFS_TAR_SUPPORT
    // ROM filesystem embedded into this executable
    romfs_tar_init(&_tarfs_start, &_tarfs_end - &_tarfs_start);
#endif
}
