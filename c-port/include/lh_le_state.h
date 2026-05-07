/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.game.feature.state.State
 *   com.b3dgs.lionengine.game.feature.state.StateAbstract
 *   com.b3dgs.lionengine.game.feature.state.StateHandler
 *
 * Closed-world state machine. Each State subclass becomes an entry in a
 * static dispatch table indexed by lh_state_id; StateHandler swaps between
 * them via id rather than reflective Class<? extends State> lookup.
 *
 * Codegen target (phase 4 asset pipeline): scan
 * lionheart-game object state java sources to populate the full enum.
 * For now the subset Loading/Menu need (none — sequences don't use states).
 */
#ifndef LH_LE_STATE_H
#define LH_LE_STATE_H

#include "lh_le_feature.h"

/* Closed-world state IDs. Phase-2c reservation; full table comes from
 * codegen of 62 lionheart object/state/State*.java files. */
typedef enum {
    LH_STATE_NONE = 0,
    LH_STATE_IDLE,
    LH_STATE_WALK,
    LH_STATE_RUN,
    LH_STATE_JUMP,
    LH_STATE_FALL,
    LH_STATE_LAND,
    LH_STATE_ATTACK,
    LH_STATE_HURT,
    LH_STATE_DIE,
    /* ... 62 total per audit. Reserve up to 96. */
    LH_STATE_MAX = 96
} lh_state_id;

typedef struct lh_state lh_state;

/* State vtable. enter/update/exit are required; check returns the next
 * state id (or LH_STATE_NONE to stay). */
typedef struct {
    void        (*enter) (lh_state* s);
    void        (*update)(lh_state* s, double extrp);
    void        (*exit)  (lh_state* s);
    lh_state_id (*check) (lh_state* s);
} lh_state_vt;

struct lh_state {
    const lh_state_vt* vt;
    lh_state_id        id;
    void*              owner;     /* concrete entity ctx ptr */
};

/* StateHandler — feature that owns the active state and transitions. */
typedef struct {
    lh_feature  base;
    lh_state*   table[LH_STATE_MAX];   /* registered state instances */
    lh_state_id current;
    lh_state_id next;
    lh_state_id last;
} lh_state_handler;

void  lh_state_handler_init    (lh_state_handler* h);
void  lh_state_handler_register(lh_state_handler* h, lh_state* s);
void  lh_state_handler_change  (lh_state_handler* h, lh_state_id next);
bool  lh_state_handler_is      (const lh_state_handler* h, lh_state_id q);
lh_state_id lh_state_handler_get(const lh_state_handler* h);
void  lh_state_handler_update  (lh_state_handler* h, double extrp);

#endif
