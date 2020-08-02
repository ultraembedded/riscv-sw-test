// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//  DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include <stdlib.h>
#include <unistd.h>

#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>

#include <errno.h>
#include <signal.h>

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "d_main.h"
#include "doomdef.h"

static long int screensize = 0;
static char *fbp = 0;

// Blocky mode,
// replace each 320x200 pixel with multiply*multiply pixels.
// According to Dave Taylor, it still is a bonehead thing
// to use ....
static int  multiply=2;

void I_ShutdownGraphics(void)
{
}

//
// I_StartFrame
//
void I_StartFrame (void)
{
}

boolean     mousemoved = false;
boolean     shmFinished;

int scancode(boolean extended, unsigned char keyval)
{
  int rc;

  printf("scancode(%02x, %02x)\n", extended, keyval);
  
  if (extended)
    switch (keyval)
      {
      case 0x75: rc = KEY_UPARROW;    break;
      case 0x6b: rc = KEY_LEFTARROW;  break;
      case 0x72: rc = KEY_DOWNARROW;  break;
      case 0x74: rc = KEY_RIGHTARROW; break;
      case 0x71: rc = KEY_BACKSPACE;  break;
      case 0x5a: rc = KEY_EQUALS;     break;
      case 0x14: rc = KEY_RCTRL;      break;    
      case 0x11: rc = KEY_RALT;       break;
      }
  else
    switch (keyval)
      {
      case 0x76: rc = KEY_ESCAPE;     break;
      case 0x5a: rc = KEY_ENTER;      break;
      case 0x0d: rc = KEY_TAB;        break;
      case 0x05: rc = KEY_F1;         break;
      case 0x06: rc = KEY_F2;         break;
      case 0x04: rc = KEY_F3;         break;
      case 0x0c: rc = KEY_F4;         break;
      case 0x03: rc = KEY_F5;         break;
      case 0x0b: rc = KEY_F6;         break;
      case 0x83: rc = KEY_F7;         break;
      case 0x0a: rc = KEY_F8;         break;
      case 0x01: rc = KEY_F9;         break;
      case 0x09: rc = KEY_F10;        break;
      case 0x78: rc = KEY_F11;        break;
      case 0x07: rc = KEY_F12;        break;
      case 0x66: rc = KEY_BACKSPACE;  break;
      case 0x55: rc = KEY_EQUALS;     break;
      case 0x7b: rc = KEY_MINUS;      break;
      case 0x4e: rc = KEY_MINUS;      break;
      case 0x12: rc = KEY_RSHIFT;     break;
      case 0x59: rc = KEY_RSHIFT;     break;
      case 0x14: rc = KEY_RCTRL;      break;
      case 0x11: rc = KEY_RALT;       break;
      case 0x1c: rc = 'a';            break;
      case 0x32: rc = 'b';            break;
      case 0x21: rc = 'c';            break;
      case 0x23: rc = 'd';            break;
      case 0x24: rc = 'e';            break;
      case 0x2b: rc = 'f';            break;
      case 0x34: rc = 'g';            break;
      case 0x33: rc = 'h';            break;
      case 0x43: rc = 'i';            break;
      case 0x3b: rc = 'j';            break;
      case 0x42: rc = 'k';            break;
      case 0x4b: rc = 'l';            break;
      case 0x3a: rc = 'm';            break;
      case 0x31: rc = 'n';            break;
      case 0x44: rc = 'o';            break;
      case 0x4d: rc = 'p';            break;
      case 0x15: rc = 'q';            break;
      case 0x2d: rc = 'r';            break;
      case 0x1b: rc = 's';            break;
      case 0x2c: rc = 't';            break;
      case 0x3c: rc = 'u';            break;
      case 0x2a: rc = 'v';            break;
      case 0x1d: rc = 'w';            break;
      case 0x22: rc = 'x';            break;
      case 0x35: rc = 'y';            break;
      case 0x1a: rc = 'z';            break;
      case 0x45: rc = '0';            break;
      case 0x16: rc = '1';            break;
      case 0x1e: rc = '2';            break;
      case 0x26: rc = '3';            break;
      case 0x25: rc = '4';            break;
      case 0x2e: rc = '5';            break;
      case 0x36: rc = '6';            break;
      case 0x3d: rc = '7';            break;
      case 0x3e: rc = '8';            break;
      case 0x46: rc = '9';            break;
      case 0x29: rc = ' ';            break;
      }

  return rc;
}

static int prev_key;

void I_GetEvent(void)
{
  event_t event;
  unsigned int rawkey;

  int ch = -1;

  {
      unsigned char buf = 0;
      int len = read(STDIN_FILENO, &buf, 1);
      if (len > 0)
        ch = buf;
  }

  // Key up
  if (prev_key != -1 && ch != prev_key)
  {
    printf("key up %c (%d / 0x%x)\n", prev_key, prev_key, prev_key);  
    event.type = ev_keyup;
    event.data1 = prev_key;
    D_PostEvent(&event);
    prev_key = -1;
  }

  if (ch == -1)
    return ;

  printf("key down %c (%d / 0x%x)\n", ch, ch, ch);  
  rawkey = ch;
  prev_key = ch;

  if (rawkey == 0)
    return;
  event.type = ev_keydown;
  event.data1 = ch;//scancode(rawkey & 0x100, rawkey & 0xff);
  D_PostEvent(&event);
}

//
// I_StartTic
//
void I_StartTic (void)
{
    I_GetEvent();
}


//
// I_UpdateNoBlit
//
void I_UpdateNoBlit (void)
{
    // what is this?
}

//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, screens[0], SCREENWIDTH*SCREENHEIGHT);
}

__attribute__((packed))
struct Color
{
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};

union ColorInt
{
    struct Color col;
    uint32_t raw;
};

static union ColorInt colors[256];

// Takes full 8 bit values.
void I_SetPalette (byte* palette)
{
    byte c;
    // set the X colormap entries
    for (int i=0 ; i<256 ; i++)
    {
        c = gammatable[usegamma][*palette++];
        colors[i].col.r = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].col.g = (c<<8) + c;
        c = gammatable[usegamma][*palette++];
        colors[i].col.b = (c<<8) + c;
    }
}

void I_InitGraphics(void)
{
    static int      firsttime=1;

    if (!firsttime)
        return;
    firsttime = 0;

    screens[0] = (unsigned char *)malloc(SCREENWIDTH*SCREENHEIGHT);

      /* Figure out the size of the screen in bytes */
    screensize = 800 * 600 * 16 / 8;
    printf("The framebuffer device was mapped to memory successfully.\n");
    prev_key = -1;
}


#define vinfo_xoffset 32
#define vinfo_yoffset 50
#define finfo_line_length (800 * (16/8))

static inline int location(int x, int y)
{
    return (x+vinfo_xoffset) * (2 *(16/8)) + (2*(y+vinfo_yoffset)) * finfo_line_length;
}

static inline uint16_t colorTo16bit(struct Color col)
{
    return  (col.r >> 3) << 11 | (col.g >> 2) << 5 | (col.b >> 3);
}

#ifndef CONFIG_FRAMEBUFFER_ADDR
#define CONFIG_FRAMEBUFFER_ADDR 0x03000000
#endif

void I_FinishUpdate (void)
{
    fbp = (char *)(CONFIG_FRAMEBUFFER_ADDR);

    for (int gy=0; gy<SCREENHEIGHT; gy++)
    {
        for (int gx=0; gx<SCREENWIDTH; gx++)
        {
            int fbPos = location(gx,gy);
            uint32_t pixel0 = colorTo16bit(colors[*(screens[0]+gy*SCREENWIDTH+gx)].col);
            pixel0 = (pixel0 << 16) | (pixel0 & 0xFFFF);
            gx++;
            uint32_t pixel1 = colorTo16bit(colors[*(screens[0]+gy*SCREENWIDTH+gx)].col);
            pixel1 = (pixel1 << 16) | (pixel1 & 0xFFFF);

            *((uint32_t*)(fbp+fbPos)) = pixel0;
            *((uint32_t*)(fbp+fbPos+finfo_line_length)) = pixel0;
            *((uint32_t*)(fbp+fbPos+4)) = pixel1;
            *((uint32_t*)(fbp+fbPos+4+finfo_line_length)) = pixel1;
        }
    }
}

