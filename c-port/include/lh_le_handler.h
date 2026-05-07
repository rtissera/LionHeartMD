/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.game.feature.Handler.
 *
 * Manages the lifecycle of a collection of lh_featurable instances.
 * update(extrp) calls every featurable's RoutineUpdate features in priority
 * order; render(g) calls every RoutineRender. add/remove are deferred until
 * the next update_add/update_remove pass to avoid invalidating iteration.
 */
#ifndef LH_LE_HANDLER_H
#define LH_LE_HANDLER_H

#include <stdbool.h>
#include "lh_le_featurable.h"
#include "lh_le_graphic.h"

#define LH_HANDLER_MAX 256

typedef struct {
    lh_featurable* live   [LH_HANDLER_MAX];
    int            live_count;
    lh_featurable* pending_add[LH_HANDLER_MAX];
    int            pending_add_count;
    lh_featurable* pending_remove[LH_HANDLER_MAX];
    int            pending_remove_count;
} lh_handler;

void  lh_handler_init   (lh_handler* h);
void  lh_handler_destroy(lh_handler* h);

/* Defer add/remove until next update tick. */
void  lh_handler_add    (lh_handler* h, lh_featurable* f);
void  lh_handler_remove (lh_handler* h, lh_featurable* f);

/* Apply pending add/remove. Called by update internally; expose for tests. */
void  lh_handler_flush  (lh_handler* h);

/* Per-frame. extrp = extrapolation factor (1.0 = exact rate match). */
void  lh_handler_update (lh_handler* h, double extrp);
void  lh_handler_render (lh_handler* h, lh_graphic* g);

int   lh_handler_size   (const lh_handler* h);

#endif
