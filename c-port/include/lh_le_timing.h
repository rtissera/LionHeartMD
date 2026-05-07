/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of com.b3dgs.lionengine.Timing (millisecond timer).
 */
#ifndef LH_LE_TIMING_H
#define LH_LE_TIMING_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool     started;
    uint64_t start_ms;
    uint64_t accum_ms;     /* paused accumulator */
    bool     paused;
} lh_timing;

void     lh_timing_init   (lh_timing* t);
void     lh_timing_start  (lh_timing* t);
void     lh_timing_stop   (lh_timing* t);
void     lh_timing_pause  (lh_timing* t);
void     lh_timing_unpause(lh_timing* t);
void     lh_timing_restart(lh_timing* t);
bool     lh_timing_started(const lh_timing* t);
bool     lh_timing_elapsed(const lh_timing* t, uint64_t ms);
uint64_t lh_timing_elapsed_ms(const lh_timing* t);

#endif
