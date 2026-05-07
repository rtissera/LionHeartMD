/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionheart.Settings.
 *
 * Persistent user preferences read at boot from lionheart.properties.
 * Phase 3: minimal field set covering everything the Loading + Menu
 * sequences need. Filter type is honored at parse time but always
 * resolves to FilterNone (per pass-2 strip).
 */
#ifndef LH_SETTINGS_H
#define LH_SETTINGS_H

#include <stdbool.h>

/* Property key constants — match Java Settings string keys for round-trip
 * compat with existing user .properties files. */
#define LH_PROP_FILENAME            "lionheart.properties"
#define LH_PROP_LANG                "lang"
#define LH_PROP_RES_WIDTH           "resolution.width"
#define LH_PROP_RES_HEIGHT          "resolution.height"
#define LH_PROP_RES_RATE            "resolution.rate"
#define LH_PROP_RES_WINDOWED        "resolution.windowed"
#define LH_PROP_VOLUME_MUSIC        "volume.music"
#define LH_PROP_VOLUME_SFX          "volume.sfx"
#define LH_PROP_FILTER              "filter"
#define LH_PROP_GAMEPLAY_TWOBUTTONS "gameplay.twobuttons"
#define LH_PROP_RASTER_TYPE         "raster.type"
#define LH_PROP_RASTER_CHECK        "raster.check"
#define LH_PROP_HUD_VISIBLE         "hud.visible"
#define LH_PROP_HUD_SWORD           "hud.sword"
#define LH_PROP_FLICKER_BACKGROUND  "flicker.background"
#define LH_PROP_FLICKER_FOREGROUND  "flicker.foreground"
#define LH_PROP_ZOOM                "zoom"
#define LH_PROP_FLAG_STRATEGY       "flag.strategy"
#define LH_PROP_FLAG_PARALLEL       "flag.parallel"
#define LH_PROP_FLAG_VSYNC          "flag.vsync"
#define LH_PROP_FLAG_DEBUG          "flag.debug"

typedef enum {
    LH_FILTER_TYPE_NONE = 0,
    LH_FILTER_TYPE_BLUR,
    LH_FILTER_TYPE_HQ2X,
    LH_FILTER_TYPE_HQ3X,
    LH_FILTER_TYPE_SCANLINE,
    LH_FILTER_TYPE_CRT
} lh_filter_type;

typedef enum {
    LH_RASTER_NONE = 0,
    LH_RASTER_DIRECT,
    LH_RASTER_CACHE
} lh_raster_type;

typedef struct {
    /* lang */
    char    lang[8];

    /* resolution */
    int     res_width;
    int     res_height;
    int     res_rate;
    bool    res_windowed;

    /* volume 0..100 */
    int     volume_music;
    int     volume_sfx;

    /* filter */
    lh_filter_type filter;

    /* gameplay */
    bool    two_buttons;

    /* raster */
    lh_raster_type raster_type;
    bool    raster_check;

    /* hud */
    bool    hud_visible;
    bool    hud_sword;

    /* flicker */
    bool    flicker_background;
    bool    flicker_foreground;

    /* zoom 0.8..1.3 */
    double  zoom;

    /* flags */
    int     flag_strategy;
    bool    flag_parallel;
    bool    flag_vsync;
    bool    flag_debug;
} lh_settings;

/* Singleton accessor (mirrors Java Settings.getInstance). */
lh_settings* lh_settings_get(void);

void lh_settings_load_default(lh_settings* s);
bool lh_settings_load_file   (lh_settings* s, const char* path);
bool lh_settings_save_file   (const lh_settings* s, const char* path);

#endif
