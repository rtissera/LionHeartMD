/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of Constant.java static initializer.
 */
#include "lh_constant.h"

lh_color_rgba LH_ALPHAS_BLACK[LH_UNSIGNED_BYTE];
lh_color_rgba LH_ALPHAS_WHITE[LH_UNSIGNED_BYTE];

void lh_constant_init(void)
{
    for (int i = 0; i < LH_UNSIGNED_BYTE; i++)
    {
        LH_ALPHAS_BLACK[i] = (lh_color_rgba){0, 0, 0, (uint8_t)i};
        LH_ALPHAS_WHITE[i] = (lh_color_rgba){255, 255, 255, (uint8_t)i};
    }
}
