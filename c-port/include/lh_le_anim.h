/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.Animation
 *   com.b3dgs.lionengine.AnimState
 *   com.b3dgs.lionengine.Animator (interface)
 *   com.b3dgs.lionengine.AnimatorModel (default impl)
 */
#ifndef LH_LE_ANIM_H
#define LH_LE_ANIM_H

#include <stdbool.h>

#define LH_ANIM_MINIMUM         1
#define LH_ANIM_DEFAULT_NAME    "default"

typedef enum {
    LH_ANIM_STOPPED,
    LH_ANIM_PLAYING,
    LH_ANIM_FINISHED,
    LH_ANIM_REVERSING,
} lh_anim_state;

typedef struct {
    const char* name;
    int         first;       /* 1-based */
    int         last;        /* 1-based, inclusive */
    double      speed;       /* frames per tick */
    bool        reverse;
    bool        repeat;
} lh_animation;

#define LH_ANIMATION(N, F, L, S, REV, REP) \
    ((lh_animation){ .name=(N), .first=(F), .last=(L), .speed=(S), .reverse=(REV), .repeat=(REP) })

/* Animator model — drives one running animation. */
typedef struct {
    const lh_animation* anim;
    lh_anim_state       state;
    double              current;   /* current frame as float, 1-based */
    int                 first;
    int                 last;
    double              speed;
    bool                reverse;
    bool                repeat;
    bool                going_back;
} lh_animator;

void lh_animator_init(lh_animator* a);
void lh_animator_play(lh_animator* a, const lh_animation* anim);
void lh_animator_stop(lh_animator* a);
void lh_animator_update(lh_animator* a, double extrp);
int  lh_animator_get_frame(const lh_animator* a);   /* 1-based current */
int  lh_animator_get_frame_anim(const lh_animator* a); /* 1-based within anim */
lh_anim_state lh_animator_get_state(const lh_animator* a);
void lh_animator_set_anim_speed(lh_animator* a, double speed);
void lh_animator_set_frame(lh_animator* a, int frame);

#endif
