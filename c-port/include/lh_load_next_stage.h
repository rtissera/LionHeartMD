/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.LoadNextStage.
 *
 * Tracks a pending stage transition. World/entity code calls request() to
 * schedule a stage change with optional delay; Scene polls is_pending()
 * each frame and consume()s when ready, terminating the current sequence
 * with the next stage's media path.
 */
#ifndef LH_LOAD_NEXT_STAGE_H
#define LH_LOAD_NEXT_STAGE_H

#include <stdbool.h>

#define LH_NEXT_STAGE_MEDIA_MAX 256

typedef struct {
    char   media [LH_NEXT_STAGE_MEDIA_MAX];
    int    delay_ms;
    int    elapsed_ms;
    bool   pending;
} lh_load_next_stage;

void  lh_load_next_stage_init   (lh_load_next_stage* lns);
void  lh_load_next_stage_request(lh_load_next_stage* lns, const char* media, int delay_ms);
void  lh_load_next_stage_update (lh_load_next_stage* lns, int elapsed_ms);
bool  lh_load_next_stage_is_ready(const lh_load_next_stage* lns);
const char* lh_load_next_stage_consume(lh_load_next_stage* lns); /* returns media + clears pending; NULL if not ready */
void  lh_load_next_stage_cancel (lh_load_next_stage* lns);

#endif
