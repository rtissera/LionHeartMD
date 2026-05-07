/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.CheckpointHandler.
 *
 * Tracks per-stage checkpoint waypoints. The hero's current checkpoint is
 * the last one passed (transformable.x crosses checkpoint.x with same y
 * proximity). On death, hero respawns at current checkpoint.x/y. End-of-
 * stage checkpoint may carry an optional next-stage media reference.
 */
#ifndef LH_CHECKPOINT_HANDLER_H
#define LH_CHECKPOINT_HANDLER_H

#include <stdbool.h>
#include "lh_le_transformable.h"
#include "lh_stage_config.h"

#define LH_CHECKPOINT_MAX 32
#define LH_CHECKPOINT_MEDIA_MAX 128

typedef struct {
    double x, y;
} lh_coord;

typedef struct {
    int    id;
    double x, y;
    char   next_stage[LH_CHECKPOINT_MEDIA_MAX];   /* empty = no transition */
} lh_checkpoint;

typedef struct {
    lh_checkpoint           checkpoints[LH_CHECKPOINT_MAX];
    int                     count;
    int                     current;             /* index into checkpoints[] */
    const lh_transformable* tracked;              /* hero transformable */
} lh_checkpoint_handler;

void          lh_checkpoint_handler_init     (lh_checkpoint_handler* h);
void          lh_checkpoint_handler_load     (lh_checkpoint_handler* h,
                                              const lh_stage_config* stage);
void          lh_checkpoint_handler_register (lh_checkpoint_handler* h,
                                              const lh_transformable* tf);
void          lh_checkpoint_handler_update   (lh_checkpoint_handler* h, double extrp);
lh_coord      lh_checkpoint_handler_get_current(const lh_checkpoint_handler* h);
const char*   lh_checkpoint_handler_get_next_stage(const lh_checkpoint_handler* h);
int           lh_checkpoint_handler_count    (const lh_checkpoint_handler* h);

#endif
