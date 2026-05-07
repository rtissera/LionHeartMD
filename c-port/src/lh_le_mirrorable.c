/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_mirrorable.h"

void lh_mirrorable_init(lh_mirrorable* m)
{
    lh_feature_init(&m->base, LH_FEAT_MIRRORABLE, NULL);
    m->current = LH_MIRROR_NONE;
    m->next    = LH_MIRROR_NONE;
}

void lh_mirrorable_set(lh_mirrorable* m, lh_mirror next)
{
    if (m) m->next = next;
}

lh_mirror lh_mirrorable_get(const lh_mirrorable* m)
{
    return m ? m->current : LH_MIRROR_NONE;
}

bool lh_mirrorable_is(const lh_mirrorable* m, lh_mirror q)
{
    return m && m->current == q;
}

void lh_mirrorable_update(lh_mirrorable* m, double extrp)
{
    (void)extrp;
    if (!m) return;
    m->current = m->next;
}
