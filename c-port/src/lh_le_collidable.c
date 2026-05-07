/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_collidable.h"
#include <string.h>

void lh_collidable_init(lh_collidable* c)
{
    lh_feature_init(&c->base, LH_FEAT_COLLIDABLE, NULL);
    c->box_count      = 0;
    c->listener_count = 0;
    c->enabled        = true;
    c->debug_visible  = false;
    c->group          = 0;
    c->accept_mask    = ~0;
    memset(c->boxes,     0, sizeof(c->boxes));
    memset(c->listeners, 0, sizeof(c->listeners));
}

void lh_collidable_set_enabled (lh_collidable* c, bool e)        { if (c) c->enabled       = e; }
void lh_collidable_set_visibility(lh_collidable* c, bool v)      { if (c) c->debug_visible = v; }
void lh_collidable_set_group   (lh_collidable* c, int g)         { if (c) c->group         = g; }
void lh_collidable_set_accept  (lh_collidable* c, int mask)      { if (c) c->accept_mask   = mask; }

void lh_collidable_add_box(lh_collidable* c, const lh_collision* box)
{
    if (!c || !box || c->box_count >= LH_COLLIDABLE_MAX_BOXES) return;
    c->boxes[c->box_count++] = *box;
}

void lh_collidable_clear_boxes(lh_collidable* c)
{
    if (c) c->box_count = 0;
}

void lh_collidable_add_listener(lh_collidable* c, lh_collidable_listener l)
{
    if (!c || !l || c->listener_count >= LH_COLLIDABLE_MAX_LISTENERS) return;
    c->listeners[c->listener_count++] = l;
}

static bool box_overlaps(double ax, double ay, const lh_collision* a,
                         double bx, double by, const lh_collision* b)
{
    const double l1 = ax + a->offset_x;
    const double t1 = ay + a->offset_y;
    const double r1 = l1 + a->width;
    const double b1 = t1 + a->height;
    const double l2 = bx + b->offset_x;
    const double t2 = by + b->offset_y;
    const double r2 = l2 + b->width;
    const double b2 = t2 + b->height;
    return r1 > l2 && r2 > l1 && b1 > t2 && b2 > t1;
}

int lh_collidable_test_overlap(const lh_collidable* a, double ax, double ay,
                               const lh_collidable* b, double bx, double by)
{
    if (!a || !b || !a->enabled || !b->enabled) return -1;
    if (!(a->accept_mask & b->group) && !(b->accept_mask & a->group)) return -1;

    for (int i = 0; i < a->box_count; i++)
    {
        for (int j = 0; j < b->box_count; j++)
        {
            if (box_overlaps(ax, ay, &a->boxes[i], bx, by, &b->boxes[j]))
            {
                return i;
            }
        }
    }
    return -1;
}

void lh_collidable_notify_collided(lh_collidable* self, lh_feature* other,
                                   const lh_collision* with, const lh_collision* by)
{
    if (!self) return;
    for (int i = 0; i < self->listener_count; i++)
    {
        self->listeners[i](&self->base, other, with, by);
    }
}
