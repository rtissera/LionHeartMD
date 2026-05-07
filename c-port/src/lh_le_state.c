/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_state.h"
#include "lh_le_check.h"
#include <string.h>

static void sh_update(lh_feature* f, double extrp)
{
    lh_state_handler* h = (lh_state_handler*)f;
    lh_state_handler_update(h, extrp);
}

static const lh_feature_update_vt s_sh_update_vt = {
    .priority      = 0,
    .update_before = NULL,
    .update        = sh_update,
    .update_after  = NULL,
};

void lh_state_handler_init(lh_state_handler* h)
{
    lh_feature_init(&h->base, LH_FEAT_STATE_HANDLER, NULL);
    h->base.update_vt = &s_sh_update_vt;
    memset(h->table, 0, sizeof(h->table));
    h->current = LH_STATE_NONE;
    h->next    = LH_STATE_NONE;
    h->last    = LH_STATE_NONE;
}

void lh_state_handler_register(lh_state_handler* h, lh_state* s)
{
    lh_check_not_null(h);
    lh_check_not_null(s);
    if (s->id <= LH_STATE_NONE || s->id >= LH_STATE_MAX)
    {
        LH_PANIC("invalid state id");
    }
    h->table[s->id] = s;
}

void lh_state_handler_change(lh_state_handler* h, lh_state_id next)
{
    if (h) h->next = next;
}

bool lh_state_handler_is(const lh_state_handler* h, lh_state_id q)
{
    return h && h->current == q;
}

lh_state_id lh_state_handler_get(const lh_state_handler* h)
{
    return h ? h->current : LH_STATE_NONE;
}

void lh_state_handler_update(lh_state_handler* h, double extrp)
{
    if (!h) return;

    /* Transition pending? */
    if (h->next != LH_STATE_NONE && h->next != h->current)
    {
        lh_state* old = (h->current != LH_STATE_NONE) ? h->table[h->current] : NULL;
        lh_state* nxt = h->table[h->next];
        if (old && old->vt && old->vt->exit) old->vt->exit(old);
        h->last    = h->current;
        h->current = h->next;
        h->next    = LH_STATE_NONE;
        if (nxt && nxt->vt && nxt->vt->enter) nxt->vt->enter(nxt);
    }

    /* Tick current state. */
    if (h->current == LH_STATE_NONE) return;
    lh_state* cur = h->table[h->current];
    if (!cur) return;

    if (cur->vt && cur->vt->update) cur->vt->update(cur, extrp);

    /* Auto-transition via check(). */
    if (cur->vt && cur->vt->check)
    {
        const lh_state_id auto_next = cur->vt->check(cur);
        if (auto_next != LH_STATE_NONE && auto_next != h->current)
        {
            h->next = auto_next;
        }
    }
}
