/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_anim.h"
#include <string.h>

void lh_animator_init(lh_animator* a)
{
    memset(a, 0, sizeof(*a));
    a->state   = LH_ANIM_STOPPED;
    a->current = 1.0;
    a->first   = 1;
    a->last    = 1;
}

void lh_animator_play(lh_animator* a, const lh_animation* anim)
{
    a->anim       = anim;
    a->first      = anim->first;
    a->last       = anim->last;
    a->speed      = anim->speed;
    a->reverse    = anim->reverse;
    a->repeat     = anim->repeat;
    a->current    = (double)anim->first;
    a->going_back = false;
    a->state      = LH_ANIM_PLAYING;
}

void lh_animator_stop(lh_animator* a)
{
    a->state = LH_ANIM_STOPPED;
}

void lh_animator_update(lh_animator* a, double extrp)
{
    if (a->state != LH_ANIM_PLAYING && a->state != LH_ANIM_REVERSING)
        return;

    if (a->going_back)
        a->current -= a->speed * extrp;
    else
        a->current += a->speed * extrp;

    if (!a->going_back && a->current >= (double)a->last + 1.0)
    {
        if (a->reverse)
        {
            a->going_back = true;
            a->current    = (double)a->last;
            a->state      = LH_ANIM_REVERSING;
        }
        else if (a->repeat)
        {
            a->current = (double)a->first;
        }
        else
        {
            a->current = (double)a->last;
            a->state   = LH_ANIM_FINISHED;
        }
    }
    else if (a->going_back && a->current < (double)a->first)
    {
        if (a->repeat)
        {
            a->going_back = false;
            a->current    = (double)a->first;
            a->state      = LH_ANIM_PLAYING;
        }
        else
        {
            a->current = (double)a->first;
            a->state   = LH_ANIM_FINISHED;
        }
    }
}

int lh_animator_get_frame(const lh_animator* a)
{
    return (int)a->current;
}

int lh_animator_get_frame_anim(const lh_animator* a)
{
    return (int)a->current - a->first + 1;
}

lh_anim_state lh_animator_get_state(const lh_animator* a)
{
    return a->state;
}

void lh_animator_set_anim_speed(lh_animator* a, double speed)
{
    a->speed = speed;
}

void lh_animator_set_frame(lh_animator* a, int frame)
{
    a->current = (double)frame;
}
