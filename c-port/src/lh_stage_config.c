/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_stage_config.h"

#include <stdio.h>
#include <string.h>

static void copy_attr_string(char* dst, size_t cap,
                             const lh_xml_node* n, const char* attr)
{
    const char* v = lh_xml_attr_string(n, attr, "");
    snprintf(dst, cap, "%s", v ? v : "");
}

static void parse_foreground(lh_stage_foreground* fg, const lh_xml_node* root)
{
    memset(fg, 0, sizeof(*fg));
    fg->water_effect = true;

    lh_xml_node* node = lh_xml_node_first_child((lh_xml_node*)root,
                                                LH_STAGE_NODE_FOREGROUND);
    if (!node) return;

    copy_attr_string(fg->type, sizeof(fg->type), node, "type");

    if (lh_xml_attr_has(node, "waterDepth"))
    {
        fg->has_water_depth = true;
        fg->water_depth     = lh_xml_attr_int(node, "waterDepth", 0);
    }
    if (lh_xml_attr_has(node, "waterOffset"))
    {
        fg->has_water_offset = true;
        fg->water_offset     = lh_xml_attr_int(node, "waterOffset", 0);
    }
    if (lh_xml_attr_has(node, "waterSpeed"))
    {
        fg->has_water_speed = true;
        fg->water_speed     = lh_xml_attr_double(node, "waterSpeed", 0.0);
    }
    if (lh_xml_attr_has(node, "waterDepthSpeed"))
    {
        fg->has_water_depth_speed = true;
        fg->water_depth_speed     = lh_xml_attr_double(node, "waterDepthSpeed", 0.0);
    }
    fg->water_effect = lh_xml_attr_bool(node, "waterEffect", true);
    fg->water_raise  = lh_xml_attr_int (node, "raise", 0);

    if (lh_xml_attr_has(node, "widthMax"))
    {
        fg->has_width_max = true;
        fg->width_max     = lh_xml_attr_int(node, "widthMax", 0);
    }
}

static void parse_entity(lh_stage_entity* out, const lh_xml_node* node)
{
    memset(out, 0, sizeof(*out));
    copy_attr_string(out->file, sizeof(out->file), node, "file");
    out->tx = lh_xml_attr_double(node, "tx", 0.0);
    out->ty = lh_xml_attr_double(node, "ty", 0.0);

    lh_xml_node* patrol = lh_xml_node_first_child((lh_xml_node*)node,
                                                  LH_STAGE_NODE_PATROL);
    if (patrol)
    {
        out->has_patrol = true;
        out->patrol_sh  = lh_xml_attr_double(patrol, "sh", 0.0);
        out->patrol_sv  = lh_xml_attr_double(patrol, "sv", 0.0);
    }
}

static void parse_checkpoints(lh_stage_config* cfg, const lh_xml_node* root)
{
    lh_xml_node* parent = lh_xml_node_first_child((lh_xml_node*)root,
                                                  LH_STAGE_NODE_CHECKPOINTS);
    if (!parent) return;

    for (lh_xml_node* c = lh_xml_node_first_child(parent, LH_STAGE_NODE_CHECKPOINT);
         c;
         c = lh_xml_node_next_sibling(c, LH_STAGE_NODE_CHECKPOINT))
    {
        if (cfg->checkpoint_count >= LH_STAGE_MAX_CHECKPOINTS) break;
        lh_stage_checkpoint* cp = &cfg->checkpoints[cfg->checkpoint_count++];
        memset(cp, 0, sizeof(*cp));
        cp->tx = lh_xml_attr_double(c, "tx", 0.0);
        cp->ty = lh_xml_attr_double(c, "ty", 0.0);
        if (lh_xml_attr_has(c, "next"))
        {
            cp->has_next = true;
            copy_attr_string(cp->next, sizeof(cp->next), c, "next");
        }
        if (lh_xml_attr_has(c, "stx") && lh_xml_attr_has(c, "sty"))
        {
            cp->has_spawn = true;
            cp->spawn_tx  = lh_xml_attr_double(c, "stx", 0.0);
            cp->spawn_ty  = lh_xml_attr_double(c, "sty", 0.0);
        }
    }
}

static void parse_boss(lh_stage_config* cfg, const lh_xml_node* root)
{
    lh_xml_node* boss = lh_xml_node_first_child((lh_xml_node*)root,
                                                LH_STAGE_NODE_BOSS);
    if (!boss) return;
    if (!(lh_xml_attr_has(boss, "tx") && lh_xml_attr_has(boss, "ty"))) return;

    cfg->boss.present  = true;
    cfg->boss.tx       = lh_xml_attr_double(boss, "tx", 0.0);
    cfg->boss.ty       = lh_xml_attr_double(boss, "ty", 0.0);
    cfg->boss.spawn_tx = lh_xml_attr_double(boss, "tsx", 0.0);
    cfg->boss.spawn_ty = lh_xml_attr_double(boss, "tsy", 0.0);
    if (lh_xml_attr_has(boss, "next"))
    {
        cfg->boss.has_next = true;
        copy_attr_string(cfg->boss.next, sizeof(cfg->boss.next), boss, "next");
    }
}

static void parse_entities(lh_stage_config* cfg, const lh_xml_node* root)
{
    lh_xml_node* parent = lh_xml_node_first_child((lh_xml_node*)root,
                                                  LH_STAGE_NODE_ENTITIES);
    if (!parent) return;
    for (lh_xml_node* e = lh_xml_node_first_child(parent, LH_STAGE_NODE_ENTITY);
         e;
         e = lh_xml_node_next_sibling(e, LH_STAGE_NODE_ENTITY))
    {
        if (cfg->entity_count >= LH_STAGE_MAX_ENTITIES) break;
        parse_entity(&cfg->entities[cfg->entity_count++], e);
    }
}

static void parse_spawns(lh_stage_config* cfg, const lh_xml_node* root)
{
    lh_xml_node* parent = lh_xml_node_first_child((lh_xml_node*)root,
                                                  LH_STAGE_NODE_SPAWNS);
    if (!parent) return;
    for (lh_xml_node* s = lh_xml_node_first_child(parent, LH_STAGE_NODE_SPAWN);
         s;
         s = lh_xml_node_next_sibling(s, LH_STAGE_NODE_SPAWN))
    {
        if (cfg->spawn_count >= LH_STAGE_MAX_SPAWNS) break;
        lh_stage_spawn* sp = &cfg->spawns[cfg->spawn_count++];
        memset(sp, 0, sizeof(*sp));
        sp->delay_ms = lh_xml_attr_int(s, "delay", 0);

        for (lh_xml_node* e = lh_xml_node_first_child(s, LH_STAGE_NODE_ENTITY);
             e;
             e = lh_xml_node_next_sibling(e, LH_STAGE_NODE_ENTITY))
        {
            if (sp->entity_count >= LH_STAGE_MAX_SPAWN_ENTITIES) break;
            parse_entity(&sp->entities[sp->entity_count++], e);
        }
    }
}

void lh_stage_config_init(lh_stage_config* cfg)
{
    if (!cfg) return;
    memset(cfg, 0, sizeof(*cfg));
    cfg->reload_min_x      = 0;
    cfg->reload_max_x      = 0x7fffffff;
    cfg->lines_per_raster  = 2;
    cfg->raster_line_offset= 1;
    cfg->foreground.water_effect = true;
}

bool lh_stage_load(lh_stage_config* out, const char* media)
{
    if (!out || !media) return false;
    lh_stage_config_init(out);

    lh_xml_doc* doc = lh_xml_load(media);
    if (!doc) return false;

    const lh_xml_node* root = lh_xml_root(doc);
    if (!root || strcmp(lh_xml_node_name(root), LH_STAGE_NODE_ROOT) != 0)
    {
        lh_xml_free(doc);
        return false;
    }

    /* Stage-level attributes. */
    if (lh_xml_attr_has(root, "pic"))
    {
        out->has_pic = true;
        copy_attr_string(out->pic, sizeof(out->pic), root, "pic");
    }
    if (lh_xml_attr_has(root, "text"))
    {
        out->has_text = true;
        copy_attr_string(out->text, sizeof(out->text), root, "text");
    }
    out->reload       = lh_xml_attr_bool(root, "reload",     false);
    out->reload_min_x = lh_xml_attr_int (root, "reloadMinX", 0);
    out->reload_max_x = lh_xml_attr_int (root, "reloadMaxX", 0x7fffffff);

    /* Music / map. */
    {
        lh_xml_node* music = lh_xml_node_first_child((lh_xml_node*)root,
                                                     LH_STAGE_NODE_MUSIC);
        if (music) copy_attr_string(out->music_file, sizeof(out->music_file),
                                    music, "file");
    }
    {
        lh_xml_node* map = lh_xml_node_first_child((lh_xml_node*)root,
                                                   LH_STAGE_NODE_MAP);
        if (map)
        {
            copy_attr_string(out->map_file, sizeof(out->map_file), map, "file");
            out->lines_per_raster   = lh_xml_attr_int(map, "linesPerRaster",   2);
            out->raster_line_offset = lh_xml_attr_int(map, "rasterLineOffset", 1);
        }
    }

    /* Raster folder. */
    {
        lh_xml_node* raster = lh_xml_node_first_child((lh_xml_node*)root,
                                                      LH_STAGE_NODE_RASTER);
        if (raster && lh_xml_attr_has(raster, "folder"))
        {
            out->has_raster_folder = true;
            copy_attr_string(out->raster_folder, sizeof(out->raster_folder),
                             raster, "folder");
        }
    }

    /* Background. */
    {
        lh_xml_node* bg = lh_xml_node_first_child((lh_xml_node*)root,
                                                  LH_STAGE_NODE_BACKGROUND);
        if (bg) copy_attr_string(out->background_type,
                                 sizeof(out->background_type), bg, "type");
    }

    parse_foreground (&out->foreground, root);
    parse_checkpoints(out, root);
    parse_boss       (out, root);
    parse_entities   (out, root);
    parse_spawns     (out, root);

    lh_xml_free(doc);
    return true;
}

const char* lh_stage_music(const lh_stage_config* cfg)
{
    return (cfg && cfg->music_file[0]) ? cfg->music_file : NULL;
}

const char* lh_stage_map_file(const lh_stage_config* cfg)
{
    return (cfg && cfg->map_file[0]) ? cfg->map_file : NULL;
}

const char* lh_stage_background(const lh_stage_config* cfg)
{
    return (cfg && cfg->background_type[0]) ? cfg->background_type : NULL;
}

const char* lh_stage_raster_folder(const lh_stage_config* cfg)
{
    return (cfg && cfg->has_raster_folder) ? cfg->raster_folder : NULL;
}

int lh_stage_water_level(const lh_stage_config* cfg)
{
    if (!cfg || !cfg->foreground.has_water_depth) return -1;
    return cfg->foreground.water_depth;
}

const lh_stage_entity* lh_stage_entity_at(const lh_stage_config* cfg, int i)
{
    if (!cfg || i < 0 || i >= cfg->entity_count) return NULL;
    return &cfg->entities[i];
}

const lh_stage_spawn* lh_stage_spawn_at(const lh_stage_config* cfg, int i)
{
    if (!cfg || i < 0 || i >= cfg->spawn_count) return NULL;
    return &cfg->spawns[i];
}
