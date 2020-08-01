#include "console.h"
#include "syscalls.h"
#include "sbrk.h"

#ifndef CONFIG_UART_BAUD
#define CONFIG_UART_BAUD 1000000
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
}
