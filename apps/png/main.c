#include <stdio.h>
#include "upng.h"
#include "fb_dev.h"

#ifndef CONFIG_FRAMEBUFFER_BASE
#define CONFIG_FRAMEBUFFER_BASE 0x95000000
#endif

#define FRAME_BUFFER_OFFSET 0x03000000
#define FRAME_BUFFER_WIDTH  800
#define FRAME_BUFFER_HEIGHT 600

int main(int argc, char** argv)
{
    fbdev_init(CONFIG_FRAMEBUFFER_BASE, FRAME_BUFFER_OFFSET, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 1);

    upng_t* upng = upng_new_from_file("/images/test_image.png");
    if (upng != NULL)
    {
        upng_decode(upng);

        unsigned width  = upng_get_width(upng);
        unsigned height = upng_get_height(upng);
        unsigned depth  = upng_get_bpp(upng) / 8;

        printf("PNG size:   %ux%ux%u (%u)\n", width, height, upng_get_bpp(upng), upng_get_size(upng));
        printf("PNG format: %u\n", upng_get_format(upng));

        if (upng_get_error(upng) == UPNG_EOK && upng_get_format(upng) == UPNG_RGB8)
        {
            uint8_t *pixels = (uint8_t *)upng_get_buffer(upng);

            for (unsigned y = 0; y != height; ++y)
            {
                for (unsigned x = 0; x != width; ++x)
                {
                    uint8_t r, g, b;
                    r = pixels[y * width * depth + x * depth + (depth - 2 - 1)];
                    g = pixels[y * width * depth + x * depth + (depth - 1 - 1)];
                    b = pixels[y * width * depth + x * depth + (depth - 0 - 1)];

                    fbdev_draw_pixel((uint16_t*)FRAME_BUFFER_OFFSET, x, y, RGB565(r,g,b));
                }
            }
        }

        upng_free(upng);
    }
    while (1)
        ;
    return 0;
}
