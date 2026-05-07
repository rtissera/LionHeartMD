/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_load_next_stage.h"
#include <stdio.h>
#include <string.h>

void lh_load_next_stage_init(lh_load_next_stage* lns)
{
    if (!lns) return;
    memset(lns, 0, sizeof(*lns));
}

void lh_load_next_stage_request(lh_load_next_stage* lns, const char* media, int delay_ms)
{
    if (!lns || !media) return;
    snprintf(lns->media, sizeof(lns->media), "%s", media);
    lns->delay_ms   = delay_ms < 0 ? 0 : delay_ms;
    lns->elapsed_ms = 0;
    lns->pending    = true;
}

void lh_load_next_stage_update(lh_load_next_stage* lns, int elapsed_ms)
{
    if (!lns || !lns->pending) return;
    lns->elapsed_ms += elapsed_ms;
}

bool lh_load_next_stage_is_ready(const lh_load_next_stage* lns)
{
    return lns && lns->pending && lns->elapsed_ms >= lns->delay_ms;
}

const char* lh_load_next_stage_consume(lh_load_next_stage* lns)
{
    if (!lh_load_next_stage_is_ready(lns)) return NULL;
    lns->pending = false;
    return lns->media;
}

void lh_load_next_stage_cancel(lh_load_next_stage* lns)
{
    if (!lns) return;
    lns->pending = false;
    lns->elapsed_ms = 0;
}
