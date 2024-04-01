/*****************************************************************************
* EPD_printf.c
******************************************************************************/
#include "EPD_printf.h"
#include "EPD_1in54_V2.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

int EPD_init() {
    int ret = 0;
    if ((ret = DEV_Module_Init())) {
        return ret;
    }
    EPD_1IN54_V2_Init();
    EPD_1IN54_V2_Clear();
    return 0;
}

int EPD_print(char* str)
{
    UBYTE *Image;
    UWORD Imagesize;
    unsigned WidthBytes = 0;

    WidthBytes = (EPD_1IN54_V2_WIDTH % 8 == 0) ?
            (EPD_1IN54_V2_WIDTH / 8 )
            : (EPD_1IN54_V2_WIDTH / 8 + 1);

    Imagesize = WidthBytes * EPD_1IN54_V2_HEIGHT;
    Image = (UBYTE *)malloc(Imagesize);
    if (Image == NULL) {
        printf("Failed to apply for memory...\r\n");
        return -1;
    }

    Paint_NewImage(Image, EPD_1IN54_V2_WIDTH, EPD_1IN54_V2_HEIGHT, 270, WHITE);
    Paint_SelectImage(Image);
    Paint_Clear(WHITE);
    //EPD_1IN54_V2_Display(Image);
    Paint_DrawString_EN(0, 0, str, &Font20, WHITE, BLACK);
    EPD_1IN54_V2_Display(Image);
    free(Image);
    return 0;
}

int EPD_printf(const char *fmt, ...) {
    char ch_buf[2048] = {0};
    va_list args;
    va_start(args, fmt);
    vsnprintf(ch_buf, sizeof(ch_buf), fmt, args);
    EPD_print(ch_buf);
    va_end(args);
}
