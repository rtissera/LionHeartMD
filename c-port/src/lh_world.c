/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_world.h"

#include "lh_constant.h"
#include "lh_extension.h"
#include "lh_folder.h"
#include "lh_le_check.h"
#include "lh_medias.h"

#include <stdio.h>
#include <string.h>

bool lh_world_init(lh_world* w,
                   const lh_game_config* game,
                   const lh_init_config* init,
                   int view_w, int view_h)
{
    if (!w) return false;
    memset(w, 0, sizeof(*w));

    if (game) w->game = *game;     else lh_game_config_default(&w->game);
    if (init) w->init = *init;     else lh_init_config_default(&w->init);

    lh_services_init        (&w->services);
    lh_handler_init         (&w->handler);
    lh_camera_init          (&w->camera);
    lh_camera_set_view      (&w->camera, 0, 0, view_w, view_h, view_h);
    lh_camera_tracker_init  (&w->tracker, &w->camera);
    lh_factory_init         (&w->factory);
    lh_landscape_init       (&w->landscape, view_w, view_h);
    lh_checkpoint_handler_init(&w->checkpoints);
    lh_load_next_stage_init (&w->next_stage);

    /* Publish aggregated services — features look these up by id. */
    lh_services_add(&w->services, LH_SVC_HANDLER,             &w->handler);
    lh_services_add(&w->services, LH_SVC_CAMERA,              &w->camera);
    lh_services_add(&w->services, LH_SVC_CAMERA_TRACKER,      &w->tracker);
    lh_services_add(&w->services, LH_SVC_VIEWER,              &w->camera.viewer);
    lh_services_add(&w->services, LH_SVC_FACTORY,             &w->factory);
    lh_services_add(&w->services, LH_SVC_CHECKPOINT_HANDLER,  &w->checkpoints);
    lh_services_add(&w->services, LH_SVC_LOAD_NEXT_STAGE,     &w->next_stage);
    lh_services_add(&w->services, LH_SVC_GAME_CONFIG,         &w->game);
    lh_services_add(&w->services, LH_SVC_INIT_CONFIG,         &w->init);

    return true;
}

static void load_collisions(lh_world* w, const char* parent_dir)
{
    char path[LH_MEDIA_PATH_MAX];
    /* Lionheart convention: <parent>/formulas.xml + groups.xml */
    snprintf(path, sizeof(path), "%s/formulas.xml", parent_dir);
    if (lh_medias_exists(path))
    {
        lh_collision_formula_config_load(&w->formulas, path);
    }
    snprintf(path, sizeof(path), "%s/collisions.xml", parent_dir);
    if (lh_medias_exists(path))
    {
        lh_collision_group_config_load(&w->groups, path);
    }
    snprintf(path, sizeof(path), "%s/groups.xml", parent_dir);
    if (lh_medias_exists(path))
    {
        lh_tile_groups_config_load(&w->tile_groups, path);
    }
}

bool lh_world_load_stage(lh_world* w, const char* media)
{
    if (!w || !media) return false;

    /* 1. Stage XML. */
    if (!lh_stage_load(&w->stage, media)) return false;

    /* 2. Tile sheets, if stage refers to a level dir. */
    if (w->stage.map_file[0])
    {
        char parent[LH_MEDIA_PATH_MAX];
        snprintf(parent, sizeof(parent), "%s", w->stage.map_file);
        char* slash = strrchr(parent, '/');
        if (slash) *slash = '\0';

        char sheets_path[LH_MEDIA_PATH_MAX];
        snprintf(sheets_path, sizeof(sheets_path), "%s/sheets.xml", parent);
        if (lh_medias_exists(sheets_path))
        {
            lh_tile_sheets_load(&w->sheets, sheets_path);
        }
        load_collisions(w, parent);
    }

    /* 3. Checkpoints sourced from stage. */
    lh_checkpoint_handler_load(&w->checkpoints, &w->stage);

    /* 4. Static entity spawning — TODO phase 8 (factory registry empty). */
    for (int i = 0; i < w->stage.entity_count; i++)
    {
        /* Placeholder: factory_create returns NULL until registered. */
        const char* media_ref = w->stage.entities[i].file;
        lh_featurable* fb = lh_factory_create(&w->factory, media_ref, NULL);
        (void)fb;
        /* Full impl: if (fb) lh_handler_add(&w->handler, fb); */
    }

    /* 5. Camera limits — pixel bounds from map dimensions if loaded. */
    const int map_w = lh_map_tile_get_in_tile_width (&w->map) * w->sheets.tile_width;
    const int map_h = lh_map_tile_get_in_tile_height(&w->map) * w->sheets.tile_height;
    if (map_w > 0 && map_h > 0)
    {
        lh_camera_set_limits(&w->camera, 0.0, (double)map_w, 0.0, (double)map_h);
    }

    w->loaded = true;
    return true;
}

void lh_world_update(lh_world* w, double extrp)
{
    if (!w || w->paused) return;

    lh_camera_tracker_update     (&w->tracker, extrp);
    lh_handler_update            (&w->handler, extrp);
    lh_landscape_update          (&w->landscape, extrp, &w->camera);
    lh_checkpoint_handler_update (&w->checkpoints, extrp);
    /* Stage transition timer. */
    lh_load_next_stage_update    (&w->next_stage, (int)(extrp * (1000.0 / LH_RES_RATE)));
}

void lh_world_render(lh_world* w, lh_graphic* g)
{
    if (!w || !g) return;

    lh_landscape_render_background(&w->landscape, g, &w->camera);
    /* TODO phase 7+: draw the active tile map here once a renderer is
     * wired. For now Handler renders any Featurable that registered a
     * render_vt; landscape covers the rest. */
    lh_handler_render(&w->handler, g);
    lh_landscape_render_foreground(&w->landscape, g, &w->camera);
}

void lh_world_destroy(lh_world* w)
{
    if (!w) return;
    lh_landscape_destroy   (&w->landscape);
    lh_handler_destroy     (&w->handler);
    lh_services_destroy    (&w->services);
    lh_map_tile_destroy    (&w->map);
    /* lh_factory has no destructor — entries are static C strings. */
}

void lh_world_set_paused(lh_world* w, bool p) { if (w) w->paused = p; }
bool lh_world_is_paused (const lh_world* w)   { return w && w->paused; }
