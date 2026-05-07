/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of subsets actually used:
 *   com.b3dgs.lionengine.UtilMath
 *   com.b3dgs.lionengine.UtilRandom
 *   com.b3dgs.lionengine.UtilConversion
 */
#ifndef LH_LE_UTIL_H
#define LH_LE_UTIL_H

#include <stdbool.h>
#include <stdint.h>

/* UtilMath */
double lh_math_clamp_d   (double v, double min, double max);
int    lh_math_clamp_i   (int    v, int    min, int    max);
bool   lh_math_is_between(double v, double min, double max);
double lh_math_wrap_d    (double v, double min, double max);
double lh_math_wrap_angle(double angle);
int    lh_math_wrap_i    (int    v, int    min, int    max);
int    lh_math_get_rounded   (double v, int round);
double lh_math_get_distance  (double x1, double y1, double x2, double y2);
int    lh_math_get_sign      (double v);
int    lh_math_get_closest_square_mult(int total);
double lh_math_sin (double angle_deg);
double lh_math_cos (double angle_deg);

/* UtilRandom */
void   lh_random_seed(uint32_t s);
int    lh_random_int  (int max);          /* [0, max) */
double lh_random_double(void);            /* [0, 1) */
bool   lh_random_bool (void);

/* UtilConversion */
int     lh_conv_bool_to_int(bool b);
uint8_t lh_conv_to_unsigned_byte  (int v);
int     lh_conv_from_unsigned_byte(uint8_t v);
/* Returns malloc'd uppercase-first lowercase-rest copy. Caller frees. */
char*   lh_conv_to_title_case(const char* s);

#endif
