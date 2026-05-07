/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of UtilMath / UtilRandom / UtilConversion subsets.
 */
#include "lh_le_util.h"
#include "lh_le_constant.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double lh_math_clamp_d(double v, double min, double max)
{
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

int lh_math_clamp_i(int v, int min, int max)
{
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

bool lh_math_is_between(double v, double min, double max)
{
    return v >= min && v <= max;
}

double lh_math_wrap_d(double v, double min, double max)
{
    const double range = max - min;
    if (range <= 0.0) return min;
    double r = fmod(v - min, range);
    if (r < 0.0) r += range;
    return r + min;
}

double lh_math_wrap_angle(double angle)
{
    return lh_math_wrap_d(angle, 0.0, (double)LH_LE_MAX_DEGREE);
}

int lh_math_wrap_i(int v, int min, int max)
{
    const int range = max - min;
    if (range <= 0) return min;
    int r = (v - min) % range;
    if (r < 0) r += range;
    return r + min;
}

int lh_math_get_rounded(double v, int round)
{
    if (round == 0) return (int)v;
    return ((int)v / round) * round;
}

double lh_math_get_distance(double x1, double y1, double x2, double y2)
{
    const double dx = x2 - x1;
    const double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

int lh_math_get_sign(double v)
{
    if (v < 0.0) return -1;
    if (v > 0.0) return  1;
    return 0;
}

int lh_math_get_closest_square_mult(int total)
{
    int n = 1;
    while (n * n < total) n++;
    return n;
}

double lh_math_sin(double angle_deg)
{
    return sin(angle_deg * M_PI / 180.0);
}

double lh_math_cos(double angle_deg)
{
    return cos(angle_deg * M_PI / 180.0);
}

/* --- Random --- */

static uint32_t s_rng_state = 0xDEADBEEFu;

void lh_random_seed(uint32_t s)
{
    s_rng_state = s ? s : 1u;
}

/* xorshift32 — fast, deterministic, sufficient for game RNG. */
static uint32_t xorshift32(void)
{
    uint32_t x = s_rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    s_rng_state = x;
    return x;
}

int lh_random_int(int max)
{
    if (max <= 0) return 0;
    return (int)(xorshift32() % (uint32_t)max);
}

double lh_random_double(void)
{
    return (double)xorshift32() / (double)0xFFFFFFFFu;
}

bool lh_random_bool(void)
{
    return (xorshift32() & 1u) != 0;
}

/* --- Conversion --- */

int lh_conv_bool_to_int(bool b)
{
    return b ? 1 : 0;
}

uint8_t lh_conv_to_unsigned_byte(int v)
{
    return (uint8_t)(v & 0xFF);
}

int lh_conv_from_unsigned_byte(uint8_t v)
{
    return (int)v;
}

char* lh_conv_to_title_case(const char* s)
{
    if (!s) return NULL;
    const size_t n = strlen(s);
    char* out = (char*)malloc(n + 1);
    if (!out) return NULL;
    for (size_t i = 0; i < n; i++)
    {
        const unsigned char c = (unsigned char)s[i];
        out[i] = (char)(i == 0 ? toupper(c) : tolower(c));
    }
    out[n] = '\0';
    return out;
}
