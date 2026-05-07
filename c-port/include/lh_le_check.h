/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of com.b3dgs.lionengine.Check (subset actually used).
 */
#ifndef LH_LE_CHECK_H
#define LH_LE_CHECK_H

#include <stdio.h>
#include <stdlib.h>

#define LH_PANIC(fmt, ...) do { \
    fprintf(stderr, "lh panic %s:%d: " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
    abort(); \
} while (0)

#define lh_check_not_null(p) do { \
    if ((p) == NULL) LH_PANIC("notNull failed"); \
} while (0)

#define lh_check_superior_or_equal(v, min) do { \
    if (!((v) >= (min))) LH_PANIC("superiorOrEqual: %g < %g", (double)(v), (double)(min)); \
} while (0)

#define lh_check_inferior_or_equal(v, max) do { \
    if (!((v) <= (max))) LH_PANIC("inferiorOrEqual: %g > %g", (double)(v), (double)(max)); \
} while (0)

#define lh_check_superior_strict(v, min) do { \
    if (!((v) > (min))) LH_PANIC("superiorStrict: %g <= %g", (double)(v), (double)(min)); \
} while (0)

#endif
