/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_entity_checker.h"
#include <string.h>

static void ec_update(lh_feature* f, double extrp)
{
    (void)extrp;
    lh_entity_checker* c = (lh_entity_checker*)f;

    const bool now_u = c->update_pred ? c->update_pred(c->user) : true;
    const bool now_r = c->render_pred ? c->render_pred(c->user) : true;

    if (now_u != c->last_update)
    {
        c->last_update = now_u;
        for (int i = 0; i < c->update_listener_count; i++)
            c->update_listeners[i](now_u, c->user);
    }
    if (now_r != c->last_render)
    {
        c->last_render = now_r;
        for (int i = 0; i < c->render_listener_count; i++)
            c->render_listeners[i](now_r, c->user);
    }
}

static const lh_feature_update_vt s_ec_update_vt = {
    .priority      = 0,
    .update_before = NULL,
    .update        = ec_update,
    .update_after  = NULL,
};

void lh_entity_checker_init(lh_entity_checker* c, void* user)
{
    lh_feature_init(&c->base, LH_FEAT_ENTITY_CHECKER, NULL);
    c->base.update_vt = &s_ec_update_vt;
    c->update_pred = NULL;
    c->render_pred = NULL;
    c->user        = user;
    c->last_update = true;
    c->last_render = true;
    c->update_listener_count = 0;
    c->render_listener_count = 0;
    memset(c->update_listeners, 0, sizeof(c->update_listeners));
    memset(c->render_listeners, 0, sizeof(c->render_listeners));
}

void lh_entity_checker_set_update(lh_entity_checker* c, lh_entity_checker_pred p)
{
    if (c) c->update_pred = p;
}

void lh_entity_checker_set_render(lh_entity_checker* c, lh_entity_checker_pred p)
{
    if (c) c->render_pred = p;
}

void lh_entity_checker_add_update_listener(lh_entity_checker* c, lh_entity_checker_listener l)
{
    if (!c || !l || c->update_listener_count >= LH_ENTITY_CHECKER_MAX_LISTENERS) return;
    c->update_listeners[c->update_listener_count++] = l;
}

void lh_entity_checker_add_render_listener(lh_entity_checker* c, lh_entity_checker_listener l)
{
    if (!c || !l || c->render_listener_count >= LH_ENTITY_CHECKER_MAX_LISTENERS) return;
    c->render_listeners[c->render_listener_count++] = l;
}

void lh_entity_checker_update(lh_entity_checker* c, double extrp)
{
    ec_update(&c->base, extrp);
}

bool lh_entity_checker_is_update_checked(const lh_entity_checker* c)
{
    return c ? c->last_update : true;
}

bool lh_entity_checker_is_render_checked(const lh_entity_checker* c)
{
    return c ? c->last_render : true;
}
