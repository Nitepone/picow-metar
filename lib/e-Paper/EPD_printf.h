/*****************************************************************************
* EPD_printf.h
******************************************************************************/
#ifndef _EPD_TEST_H_
#define _EPD_TEST_H_

//#include "DEV_Config.h"
#include <GUI_Paint.h>
#include "GUI_BMPfile.h"
//#include <ImageData.h>
#include "Debug.h"
#include <stdlib.h> // malloc() free()


int EPD_init(void);
int EPD_print(char* str);
int EPD_printf(const char* fmt, ...);

#endif