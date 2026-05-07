/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_settings.h"
#include "lh_constant.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

static lh_settings s_instance;
static bool        s_initialized = false;

static void apply_defaults(lh_settings* s)
{
    memset(s, 0, sizeof(*s));
    snprintf(s->lang, sizeof(s->lang), "%s", "en");
    s->res_width          = LH_RES_OUT_W;
    s->res_height         = LH_RES_OUT_H;
    s->res_rate           = LH_RES_RATE;
    s->res_windowed       = true;
    s->volume_music       = 100;
    s->volume_sfx         = 100;
    s->filter             = LH_FILTER_TYPE_NONE;
    s->two_buttons        = false;
    s->raster_type        = LH_RASTER_CACHE;
    s->raster_check       = false;
    s->hud_visible        = true;
    s->hud_sword          = true;
    s->flicker_background = false;
    s->flicker_foreground = false;
    s->zoom               = 1.0;
    s->flag_strategy      = 0;
    s->flag_parallel      = true;
    s->flag_vsync         = true;
    s->flag_debug         = false;
}

lh_settings* lh_settings_get(void)
{
    if (!s_initialized)
    {
        apply_defaults(&s_instance);
        s_initialized = true;
    }
    return &s_instance;
}

void lh_settings_load_default(lh_settings* s)
{
    if (s) apply_defaults(s);
}

/* Trim trailing whitespace + newline. */
static void rtrim(char* s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' || s[n-1] == ' ' || s[n-1] == '\t'))
        s[--n] = '\0';
}

static bool parse_bool(const char* v)
{
    return v && (strcasecmp(v, "true") == 0 || strcmp(v, "1") == 0);
}

static void apply_kv(lh_settings* s, const char* key, const char* val)
{
    if      (strcmp(key, LH_PROP_LANG) == 0)                snprintf(s->lang, sizeof(s->lang), "%s", val);
    else if (strcmp(key, LH_PROP_RES_WIDTH) == 0)           s->res_width    = atoi(val);
    else if (strcmp(key, LH_PROP_RES_HEIGHT) == 0)          s->res_height   = atoi(val);
    else if (strcmp(key, LH_PROP_RES_RATE) == 0)            s->res_rate     = atoi(val);
    else if (strcmp(key, LH_PROP_RES_WINDOWED) == 0)        s->res_windowed = parse_bool(val);
    else if (strcmp(key, LH_PROP_VOLUME_MUSIC) == 0)        s->volume_music = atoi(val);
    else if (strcmp(key, LH_PROP_VOLUME_SFX) == 0)          s->volume_sfx   = atoi(val);
    else if (strcmp(key, LH_PROP_FILTER) == 0)              s->filter       = (lh_filter_type)atoi(val);
    else if (strcmp(key, LH_PROP_GAMEPLAY_TWOBUTTONS) == 0) s->two_buttons  = parse_bool(val);
    else if (strcmp(key, LH_PROP_RASTER_TYPE) == 0)         s->raster_type  = (lh_raster_type)atoi(val);
    else if (strcmp(key, LH_PROP_RASTER_CHECK) == 0)        s->raster_check = parse_bool(val);
    else if (strcmp(key, LH_PROP_HUD_VISIBLE) == 0)         s->hud_visible  = parse_bool(val);
    else if (strcmp(key, LH_PROP_HUD_SWORD) == 0)           s->hud_sword    = parse_bool(val);
    else if (strcmp(key, LH_PROP_FLICKER_BACKGROUND) == 0)  s->flicker_background = parse_bool(val);
    else if (strcmp(key, LH_PROP_FLICKER_FOREGROUND) == 0)  s->flicker_foreground = parse_bool(val);
    else if (strcmp(key, LH_PROP_ZOOM) == 0)                s->zoom         = atof(val);
    else if (strcmp(key, LH_PROP_FLAG_STRATEGY) == 0)       s->flag_strategy = atoi(val);
    else if (strcmp(key, LH_PROP_FLAG_PARALLEL) == 0)       s->flag_parallel = parse_bool(val);
    else if (strcmp(key, LH_PROP_FLAG_VSYNC) == 0)          s->flag_vsync    = parse_bool(val);
    else if (strcmp(key, LH_PROP_FLAG_DEBUG) == 0)          s->flag_debug    = parse_bool(val);
}

bool lh_settings_load_file(lh_settings* s, const char* path)
{
    if (!s || !path) return false;
    apply_defaults(s);

    FILE* fp = fopen(path, "r");
    if (!fp) return false;

    char line[512];
    while (fgets(line, sizeof(line), fp))
    {
        rtrim(line);
        char* p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '#' || *p == '!') continue;       /* comment / blank */

        char* eq = strchr(p, '=');
        if (!eq) continue;
        *eq = '\0';
        char* key = p;
        char* val = eq + 1;
        while (*val == ' ' || *val == '\t') val++;
        rtrim(key);
        apply_kv(s, key, val);
    }
    fclose(fp);
    return true;
}

bool lh_settings_save_file(const lh_settings* s, const char* path)
{
    if (!s || !path) return false;
    FILE* fp = fopen(path, "w");
    if (!fp) return false;

    fprintf(fp, "%s=%s\n", LH_PROP_LANG, s->lang);
    fprintf(fp, "%s=%d\n", LH_PROP_RES_WIDTH,  s->res_width);
    fprintf(fp, "%s=%d\n", LH_PROP_RES_HEIGHT, s->res_height);
    fprintf(fp, "%s=%d\n", LH_PROP_RES_RATE,   s->res_rate);
    fprintf(fp, "%s=%s\n", LH_PROP_RES_WINDOWED, s->res_windowed ? "true" : "false");
    fprintf(fp, "%s=%d\n", LH_PROP_VOLUME_MUSIC, s->volume_music);
    fprintf(fp, "%s=%d\n", LH_PROP_VOLUME_SFX,   s->volume_sfx);
    fprintf(fp, "%s=%d\n", LH_PROP_FILTER, (int)s->filter);
    fprintf(fp, "%s=%s\n", LH_PROP_GAMEPLAY_TWOBUTTONS, s->two_buttons ? "true" : "false");
    fprintf(fp, "%s=%d\n", LH_PROP_RASTER_TYPE, (int)s->raster_type);
    fprintf(fp, "%s=%s\n", LH_PROP_RASTER_CHECK, s->raster_check ? "true" : "false");
    fprintf(fp, "%s=%s\n", LH_PROP_HUD_VISIBLE,  s->hud_visible  ? "true" : "false");
    fprintf(fp, "%s=%s\n", LH_PROP_HUD_SWORD,    s->hud_sword    ? "true" : "false");
    fprintf(fp, "%s=%s\n", LH_PROP_FLICKER_BACKGROUND, s->flicker_background ? "true" : "false");
    fprintf(fp, "%s=%s\n", LH_PROP_FLICKER_FOREGROUND, s->flicker_foreground ? "true" : "false");
    fprintf(fp, "%s=%g\n", LH_PROP_ZOOM, s->zoom);
    fprintf(fp, "%s=%d\n", LH_PROP_FLAG_STRATEGY, s->flag_strategy);
    fprintf(fp, "%s=%s\n", LH_PROP_FLAG_PARALLEL, s->flag_parallel ? "true" : "false");
    fprintf(fp, "%s=%s\n", LH_PROP_FLAG_VSYNC,    s->flag_vsync    ? "true" : "false");
    fprintf(fp, "%s=%s\n", LH_PROP_FLAG_DEBUG,    s->flag_debug    ? "true" : "false");

    fclose(fp);
    return true;
}
