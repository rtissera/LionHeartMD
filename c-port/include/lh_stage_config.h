/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.StageConfig (and inlined fragments of
 * EntityConfig / SpawnConfig / Checkpoint / ForegroundConfig).
 *
 * Parses a stage XML descriptor of the form:
 *
 *   <stage>
 *     <music     file="..."/>
 *     <map       file="..." linesPerRaster="N" rasterLineOffset="N"/>
 *     <raster    folder="..."/>
 *     <background type="..."/>
 *     <foreground type="WATER" waterDepth="N" waterOffset="N" .../>
 *     <checkpoints>
 *       <checkpoint tx="..." ty="..." next="..." stx="..." sty="..."/>
 *     </checkpoints>
 *     <boss tx="..." ty="..." tsx="..." tsy="..." next="..."/>
 *     <entities>
 *       <entity file="..." tx="..." ty="..."/>
 *     </entities>
 *     <spawns>
 *       <spawn delay="ms">
 *         <entity file="..." tx="..." ty="...">
 *           <patrol sh="..." sv="..."/>
 *         </entity>
 *       </spawn>
 *     </spawns>
 *   </stage>
 *
 * No reflection: foreground/background types are kept as strings; the
 * caller can map them to enums at use site.
 */
#ifndef LH_STAGE_CONFIG_H
#define LH_STAGE_CONFIG_H

#include "lh_xml.h"

#define LH_STAGE_FILENAME             "stage.xml"

#define LH_STAGE_NODE_ROOT            "stage"
#define LH_STAGE_NODE_MUSIC           "music"
#define LH_STAGE_NODE_MAP             "map"
#define LH_STAGE_NODE_RASTER          "raster"
#define LH_STAGE_NODE_BACKGROUND      "background"
#define LH_STAGE_NODE_FOREGROUND      "foreground"
#define LH_STAGE_NODE_CHECKPOINTS     "checkpoints"
#define LH_STAGE_NODE_CHECKPOINT      "checkpoint"
#define LH_STAGE_NODE_BOSS            "boss"
#define LH_STAGE_NODE_ENTITIES        "entities"
#define LH_STAGE_NODE_SPAWNS          "spawns"
#define LH_STAGE_NODE_SPAWN           "spawn"
#define LH_STAGE_NODE_ENTITY          "entity"
#define LH_STAGE_NODE_PATROL          "patrol"

#define LH_STAGE_PATH_MAX             192
#define LH_STAGE_TYPE_MAX             32
#define LH_STAGE_MAX_ENTITIES         256
#define LH_STAGE_MAX_CHECKPOINTS      32
#define LH_STAGE_MAX_SPAWNS           64
#define LH_STAGE_MAX_SPAWN_ENTITIES   16

/* Single entity placement (used both for static <entities> and for
 * <spawn>'s child entities). */
typedef struct {
    char   file [LH_STAGE_PATH_MAX]; /* media path to entity setup XML */
    double tx;                       /* spawn tile X (fractional ok) */
    double ty;                       /* spawn tile Y */
    bool   has_patrol;
    double patrol_sh;                /* horizontal patrol speed */
    double patrol_sv;                /* vertical patrol speed */
} lh_stage_entity;

/* Time-delayed wave of entities. */
typedef struct {
    int             delay_ms;
    lh_stage_entity entities[LH_STAGE_MAX_SPAWN_ENTITIES];
    int             entity_count;
} lh_stage_spawn;

/* Save / respawn point on the map. */
typedef struct {
    double tx;
    double ty;
    bool   has_next;
    char   next [LH_STAGE_PATH_MAX]; /* next stage media path */
    bool   has_spawn;
    double spawn_tx;
    double spawn_ty;
} lh_stage_checkpoint;

/* Optional boss block. */
typedef struct {
    bool   present;
    double tx;
    double ty;
    double spawn_tx;
    double spawn_ty;
    bool   has_next;
    char   next [LH_STAGE_PATH_MAX];
} lh_stage_boss;

/* Foreground subset — only WATER fields are commonly populated. */
typedef struct {
    char   type        [LH_STAGE_TYPE_MAX];
    bool   has_water_depth;
    int    water_depth;
    bool   has_water_offset;
    int    water_offset;
    bool   has_water_speed;
    double water_speed;
    bool   has_water_depth_speed;
    double water_depth_speed;
    bool   water_effect;
    int    water_raise;
    bool   has_width_max;
    int    width_max;
} lh_stage_foreground;

typedef struct {
    /* Top-level / stage attributes. */
    bool   has_pic;
    char   pic   [LH_STAGE_PATH_MAX];
    bool   has_text;
    char   text  [LH_STAGE_PATH_MAX];
    bool   reload;
    int    reload_min_x;
    int    reload_max_x;

    /* Music + map. */
    char   music_file [LH_STAGE_PATH_MAX];
    char   map_file   [LH_STAGE_PATH_MAX];
    int    lines_per_raster;
    int    raster_line_offset;

    /* Raster folder (foreground recoloring). */
    bool   has_raster_folder;
    char   raster_folder [LH_STAGE_PATH_MAX];

    /* Background + foreground (water). */
    char                background_type [LH_STAGE_TYPE_MAX];
    lh_stage_foreground foreground;

    /* Checkpoints + boss. */
    lh_stage_checkpoint checkpoints [LH_STAGE_MAX_CHECKPOINTS];
    int                 checkpoint_count;
    lh_stage_boss       boss;

    /* Static entities placed on the map. */
    lh_stage_entity     entities    [LH_STAGE_MAX_ENTITIES];
    int                 entity_count;

    /* Time-delayed spawn waves. */
    lh_stage_spawn      spawns      [LH_STAGE_MAX_SPAWNS];
    int                 spawn_count;
} lh_stage_config;

void                   lh_stage_config_init  (lh_stage_config* cfg);
bool                   lh_stage_load         (lh_stage_config* out, const char* media);

/* Accessors (mostly trivial — provided for symmetry with the Java API). */
const char*            lh_stage_music         (const lh_stage_config* cfg);
const char*            lh_stage_map_file      (const lh_stage_config* cfg);
const char*            lh_stage_background    (const lh_stage_config* cfg);
const char*            lh_stage_raster_folder (const lh_stage_config* cfg);
int                    lh_stage_water_level   (const lh_stage_config* cfg); /* -1 if none */
const lh_stage_entity* lh_stage_entity_at     (const lh_stage_config* cfg, int i);
const lh_stage_spawn*  lh_stage_spawn_at      (const lh_stage_config* cfg, int i);

#endif
