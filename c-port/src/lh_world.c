/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_world.h"

#include "lh_asset_cache.h"
#include "lh_constant.h"
#include "lh_extension.h"
#include "lh_folder.h"
#include "lh_input.h"
#include "lh_le_check.h"
#include "lh_map_tile_persister.h"
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

/* Load each sheet PNG referenced by sheets.xml into the asset cache and
 * cache the surface pointer in w->sheet_surfaces[i]. Sheet paths in the
 * config are relative to parent_dir (the level dir holding sheets.xml). */
static void load_sheet_surfaces(lh_world* w, const char* parent_dir)
{
    for (int i = 0; i < w->sheets.count && i < LH_TILE_SHEETS_MAX; i++)
    {
        char path[LH_MEDIA_PATH_MAX];
        snprintf(path, sizeof(path), "%s/%s", parent_dir, w->sheets.paths[i]);
        w->sheet_surfaces[i] = lh_asset_get_image(path);
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
            load_sheet_surfaces(w, parent);
        }
        load_collisions(w, parent);

        /* Load the binary .lvl tile grid. Resolve relative path. */
        char lvl_path[LH_MEDIA_PATH_MAX];
        if (lh_medias_resolve(w->stage.map_file, lvl_path, sizeof(lvl_path)))
        {
            lh_map_tile_persister_load(&w->map, lvl_path);
        }
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

    /* 4b. Hero spawn at first checkpoint (or stage origin). Phase 7+
     * placeholder; phase 8 swaps for the full Valdyn featurable. */
    {
        const lh_coord c = lh_checkpoint_handler_get_current(&w->checkpoints);
        const int tw = w->sheets.tile_width  > 0 ? w->sheets.tile_width  : 16;
        const int th = w->sheets.tile_height > 0 ? w->sheets.tile_height : 16;
        const double sx = (c.x > 0.0 ? c.x : 4.0) * tw;
        const double sy = (c.y > 0.0 ? c.y : 4.0) * th;
        lh_hero_init(&w->hero, sx, sy);

        /* Load Valdyn sprite atlas (13 cols x 12 rows per Valdyn.xml frames). */
        lh_image_buffer* atlas = lh_asset_get_image(
            "com/b3dgs/lionheart/hero/valdyn/Valdyn.png");
        if (atlas)
        {
            lh_hero_load_sprite(&w->hero, atlas, 13, 12);
        }

        /* Camera tracks the hero. */
        lh_camera_tracker_track(&w->tracker, &w->hero.transformable);
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

    /* Hero physics + formula-driven tile collision. */
    lh_input_update();
    const lh_tile_collision_ctx coll = {
        .map         = &w->map,
        .tile_groups = &w->tile_groups,
        .coll_groups = &w->groups,
        .formulas    = &w->formulas,
        .tile_w      = w->sheets.tile_width  > 0 ? w->sheets.tile_width  : 16,
        .tile_h      = w->sheets.tile_height > 0 ? w->sheets.tile_height : 16,
    };
    lh_hero_update(&w->hero, &coll, extrp);

    lh_camera_tracker_update     (&w->tracker, extrp);
    lh_handler_update            (&w->handler, extrp);
    lh_landscape_update          (&w->landscape, extrp, &w->camera);
    lh_checkpoint_handler_update (&w->checkpoints, extrp);
    /* Stage transition timer. */
    lh_load_next_stage_update    (&w->next_stage, (int)(extrp * (1000.0 / LH_RES_RATE)));
}

/* Draw the visible tile range. Each map cell packs (sheet_id << 16) |
 * tile_num; -1 = empty. tile_num is 1-based per Java convention. */
static void render_tiles(lh_world* w, lh_graphic* g)
{
    if (!w || !g) return;
    const int tw = w->sheets.tile_width;
    const int th = w->sheets.tile_height;
    if (tw <= 0 || th <= 0) return;

    const int in_w = lh_map_tile_get_in_tile_width (&w->map);
    const int in_h = lh_map_tile_get_in_tile_height(&w->map);
    if (in_w <= 0 || in_h <= 0) return;

    /* Camera-relative visible window in tile coords. */
    const int cx = (int)w->camera.x;
    const int cy = (int)w->camera.y;
    const int vw = w->camera.width;
    const int vh = w->camera.height;
    int tx0 = cx / tw;
    int ty0 = cy / th;
    int tx1 = (cx + vw + tw - 1) / tw + 1;
    int ty1 = (cy + vh + th - 1) / th + 1;
    if (tx0 < 0) tx0 = 0;
    if (ty0 < 0) ty0 = 0;
    if (tx1 > in_w) tx1 = in_w;
    if (ty1 > in_h) ty1 = in_h;

    for (int ty = ty0; ty < ty1; ty++)
    {
        for (int tx = tx0; tx < tx1; tx++)
        {
            const int32_t packed = lh_map_tile_get_tile(&w->map, tx, ty);
            if (packed < 0) continue;

            const int sheet_id = (int)((uint32_t)packed >> 16);
            const int tile_num = (int)((uint32_t)packed & 0xFFFFu);
            if (sheet_id < 0 || sheet_id >= w->sheets.count) continue;
            lh_image_buffer* sheet = w->sheet_surfaces[sheet_id];
            if (!sheet || sheet->width <= 0) continue;

            const int cols    = sheet->width / tw;
            if (cols <= 0) continue;
            const int idx     = tile_num > 0 ? tile_num - 1 : 0;
            const int sx      = (idx % cols) * tw;
            const int sy      = (idx / cols) * th;
            const int dx      = tx * tw - cx;
            const int dy      = ty * th - cy;

            lh_graphic_draw_image_region(g, sheet, sx, sy, tw, th, dx, dy);
        }
    }
}

void lh_world_render(lh_world* w, lh_graphic* g)
{
    if (!w || !g) return;

    lh_landscape_render_background(&w->landscape, g, &w->camera);
    render_tiles(w, g);
    lh_hero_render(&w->hero, g, &w->camera);
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
