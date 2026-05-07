/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_handler.h"
#include "lh_le_check.h"
#include <string.h>

void lh_handler_init(lh_handler* h)
{
    memset(h, 0, sizeof(*h));
}

void lh_handler_destroy(lh_handler* h)
{
    if (!h) return;
    /* Handler does not own featurables. Caller frees. */
    memset(h, 0, sizeof(*h));
}

void lh_handler_add(lh_handler* h, lh_featurable* f)
{
    lh_check_not_null(h);
    lh_check_not_null(f);
    if (h->pending_add_count >= LH_HANDLER_MAX)
    {
        LH_PANIC("handler pending_add overflow");
    }
    h->pending_add[h->pending_add_count++] = f;
}

void lh_handler_remove(lh_handler* h, lh_featurable* f)
{
    lh_check_not_null(h);
    lh_check_not_null(f);
    if (h->pending_remove_count >= LH_HANDLER_MAX)
    {
        LH_PANIC("handler pending_remove overflow");
    }
    h->pending_remove[h->pending_remove_count++] = f;
}

void lh_handler_flush(lh_handler* h)
{
    if (!h) return;

    /* Apply additions: append to live[]. */
    for (int i = 0; i < h->pending_add_count; i++)
    {
        if (h->live_count >= LH_HANDLER_MAX)
        {
            LH_PANIC("handler live overflow");
        }
        h->live[h->live_count++] = h->pending_add[i];
    }
    h->pending_add_count = 0;

    /* Apply removals: swap-remove. */
    for (int r = 0; r < h->pending_remove_count; r++)
    {
        lh_featurable* victim = h->pending_remove[r];
        for (int i = 0; i < h->live_count; i++)
        {
            if (h->live[i] == victim)
            {
                h->live[i] = h->live[--h->live_count];
                h->live[h->live_count] = NULL;
                break;
            }
        }
    }
    h->pending_remove_count = 0;
}

/* Per-featurable update walk. Iterates feature slots and dispatches
 * RoutineUpdate hooks (update_before -> update -> update_after).
 * No priority sort here — features are visited in feature-id order, which
 * matches the Java `addFeature` order in well-behaved entities. Phase 8+
 * will introduce a sorted index when the entity feature graph stabilizes. */
static void update_one(lh_featurable* fb, double extrp)
{
    if (!fb) return;
    /* Phase 1: update_before. */
    for (int i = 1; i < LH_FEAT_MAX; i++)
    {
        lh_feature* f = fb->features[i];
        if (f && f->update_vt && f->update_vt->update_before)
            f->update_vt->update_before(f);
    }
    /* Phase 2: update. */
    for (int i = 1; i < LH_FEAT_MAX; i++)
    {
        lh_feature* f = fb->features[i];
        if (f && f->update_vt && f->update_vt->update)
            f->update_vt->update(f, extrp);
    }
    /* Phase 3: update_after. */
    for (int i = 1; i < LH_FEAT_MAX; i++)
    {
        lh_feature* f = fb->features[i];
        if (f && f->update_vt && f->update_vt->update_after)
            f->update_vt->update_after(f);
    }
}

static void render_one(lh_featurable* fb, lh_graphic* g)
{
    if (!fb) return;
    for (int i = 1; i < LH_FEAT_MAX; i++)
    {
        lh_feature* f = fb->features[i];
        if (f && f->render_vt && f->render_vt->render)
            f->render_vt->render(f, g);
    }
}

void lh_handler_update(lh_handler* h, double extrp)
{
    if (!h) return;

    lh_handler_flush(h);

    for (int i = 0; i < h->live_count; i++)
    {
        update_one(h->live[i], extrp);
    }
}

void lh_handler_render(lh_handler* h, lh_graphic* g)
{
    if (!h) return;

    for (int i = 0; i < h->live_count; i++)
    {
        render_one(h->live[i], g);
    }
}

int lh_handler_size(const lh_handler* h)
{
    return h ? h->live_count : 0;
}
