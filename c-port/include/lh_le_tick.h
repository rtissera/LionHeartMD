/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 * Port of com.b3dgs.lionengine.Tick — frame-tick counter with deferred actions.
 *
 * Lionheart usage: tick.start(); tick.update(extrp); tick.elapsed(N);
 * tick.addAction(runnable, frame).
 */
#ifndef LH_LE_TICK_H
#define LH_LE_TICK_H

#include <stdbool.h>
#include <stdint.h>

#define LH_TICK_MAX_ACTIONS 32

typedef void (*lh_tick_action_fn)(void* user);

typedef struct {
    lh_tick_action_fn fn;
    void*             user;
    int               frame;
    bool              fired;
} lh_tick_action;

typedef struct {
    bool           started;
    int            frame;
    double         extrp_acc;   /* accumulated extrp */
    lh_tick_action actions[LH_TICK_MAX_ACTIONS];
    int            action_count;
} lh_tick;

void lh_tick_init    (lh_tick* t);
void lh_tick_start   (lh_tick* t);
void lh_tick_stop    (lh_tick* t);
void lh_tick_restart (lh_tick* t);
void lh_tick_update  (lh_tick* t, double extrp);
bool lh_tick_started (const lh_tick* t);
bool lh_tick_elapsed (const lh_tick* t, int frames);
int  lh_tick_elapsed_count(const lh_tick* t);
void lh_tick_add_action(lh_tick* t, lh_tick_action_fn fn, void* user, int frame);

#endif
