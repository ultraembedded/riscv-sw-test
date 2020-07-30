#include "console.h"

#define WEAK __attribute__((weak))

//-----------------------------------------------------------------
// Console stubs
//-----------------------------------------------------------------
WEAK void console_init(uint32_t baud) { }
WEAK void console_putchar(int ch) { }
WEAK int  console_getchar(void) { return -1; }
WEAK void console_putstr(const char *s)
{
	while (*s)
		console_putchar(*s++);
}