#ifndef __GFX_CONSOLE_H__
#define __GFX_CONSOLE_H__

#include <stdint.h>

//-----------------------------------------------------------------
// Prototypes
//-----------------------------------------------------------------
void gfx_console_init(uint16_t *fb, int width, int height, int en);
void gfx_console_enable(int en);
void gfx_console_set_colour(int r, int g, int b);
void gfx_console_set_background(int r, int g, int b);
void gfx_console_clear(void);
int  gfx_console_putc(char ch);

#endif