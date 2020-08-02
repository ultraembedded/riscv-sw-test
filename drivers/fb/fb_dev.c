#include "fb_dev.h"

// Driver for frame buffer device:
// https://github.com/ultraembedded/core_dvi_framebuffer
#ifdef CONFIG_FRAMEBUFFER

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------
#define VIDEO_CONFIG            0x0
    #define VIDEO_CONFIG_INT_EN_SOF                    1
    #define VIDEO_CONFIG_INT_EN_SOF_SHIFT              1
    #define VIDEO_CONFIG_INT_EN_SOF_MASK               0x1

    #define VIDEO_CONFIG_ENABLE                        0
    #define VIDEO_CONFIG_ENABLE_SHIFT                  0
    #define VIDEO_CONFIG_ENABLE_MASK                   0x1

#define VIDEO_STATUS            0x4
    #define VIDEO_STATUS_Y_POS_SHIFT                   16
    #define VIDEO_STATUS_Y_POS_MASK                    0xffff

    #define VIDEO_STATUS_H_POS_SHIFT                   0
    #define VIDEO_STATUS_H_POS_MASK                    0xffff

#define VIDEO_FRAME_BUFFER      0x8
    #define VIDEO_FRAME_BUFFER_ADDR_SHIFT              8
    #define VIDEO_FRAME_BUFFER_ADDR_MASK               0xffffff

//-----------------------------------------------------------------
// Locals
//-----------------------------------------------------------------
static volatile uint32_t *m_fb;
static int m_fb_width;
static int m_fb_height;

//-----------------------------------------------------------------
// fbdev_init: Initialise frame buffer peripheral
//-----------------------------------------------------------------
void fbdev_init(uint32_t base_addr, uint32_t frame_buffer, int width, int height, int enable)
{
    m_fb = (volatile uint32_t *)base_addr;

    m_fb_width  = width;
    m_fb_height = height;

    m_fb[VIDEO_FRAME_BUFFER/4] = frame_buffer;
    m_fb[VIDEO_CONFIG/4]       = 1 << VIDEO_CONFIG_ENABLE_SHIFT;
}
//-----------------------------------------------------------------
// fbdev_set_framebuffer: Set new frame buffer address
//-----------------------------------------------------------------
void fbdev_set_framebuffer(uint32_t frame_buffer)
{
    m_fb[VIDEO_FRAME_BUFFER/4] = frame_buffer;
}
//-----------------------------------------------------------------
// fbdev_draw_pixel: Draw pixel
//-----------------------------------------------------------------
void fbdev_draw_pixel(uint16_t *frame, int x, int y, uint16_t colour)
{
    int offset = x + (y * m_fb_width);
    frame[offset] = colour;
}
//-----------------------------------------------------------------
// fbdev_fill_screen: Fill screen with a particular colour
//-----------------------------------------------------------------
void fbdev_fill_screen(uint16_t *frame, uint16_t value)
{
    for (int y=0;y<m_fb_height;y++)
        for (int x=0;x<m_fb_width;x++)
            frame[x + (y * m_fb_width)] = value;
}
//-----------------------------------------------------------------
// fbdev_clear_screen: Clear screen (fill with black)
//-----------------------------------------------------------------
void fbdev_clear_screen(uint16_t *frame)
{
    fbdev_fill_screen(frame, RGB565(0,0,0));
}
//-----------------------------------------------------------------
// fbdev_draw_bitmap: Draw a bitmap
//-----------------------------------------------------------------
void fbdev_draw_bitmap(uint16_t *frame, uint8_t* bitmap, int x, int y, int width, int height, uint16_t colour, uint16_t bg_colour)
{
    int w_bytes = width / 8;
    for(int v=0; v<height; v++)
        for (int u=0; u<width; u++)
        {
            if (bitmap[w_bytes * v + (u >> 3)] & (1 << (~u & 7)))
                fbdev_draw_pixel(frame, x + u, y + v, colour);
            else
                fbdev_draw_pixel(frame, x + u, y + v, bg_colour);
        }
}

#endif
