/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of com.b3dgs.lionengine.graphic.ColorRgba.
 */
#ifndef LH_LE_COLOR_H
#define LH_LE_COLOR_H

#include "lh_constant.h"   /* lh_color_rgba defined here */

#define LH_RGB(r,g,b)      ((lh_color_rgba){(uint8_t)(r),(uint8_t)(g),(uint8_t)(b),0xFF})
#define LH_RGBA(r,g,b,a)   ((lh_color_rgba){(uint8_t)(r),(uint8_t)(g),(uint8_t)(b),(uint8_t)(a)})

#define LH_COLOR_BLACK       LH_RGB(0,0,0)
#define LH_COLOR_WHITE       LH_RGB(255,255,255)
#define LH_COLOR_RED         LH_RGB(255,0,0)
#define LH_COLOR_GREEN       LH_RGB(0,255,0)
#define LH_COLOR_BLUE        LH_RGB(0,0,255)
#define LH_COLOR_TRANSPARENT LH_RGBA(0,0,0,0)

#endif
