/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of:
 *   com.b3dgs.lionengine.graphic.drawable.Sprite
 *   com.b3dgs.lionengine.graphic.drawable.SpriteAnimated
 *   com.b3dgs.lionengine.graphic.drawable.SpriteTiled
 *
 * One struct, one vtable-less union; Java's separate classes collapse to
 * sprite kind discriminator since Lionheart doesn't subclass for behavior.
 */
#ifndef LH_LE_SPRITE_H
#define LH_LE_SPRITE_H

#include <stdbool.h>
#include "lh_le_anim.h"
#include "lh_le_graphic.h"
#include "lh_le_image_buffer.h"
#include "lh_le_transform.h"

typedef enum {
    LH_SPRITE_PLAIN,
    LH_SPRITE_ANIMATED,
    LH_SPRITE_TILED,
} lh_sprite_kind;

typedef struct {
    lh_sprite_kind   kind;
    lh_image_buffer* surface;     /* not owned */
    bool             owns_surface;
    int              tile_w;
    int              tile_h;      /* for ANIMATED/TILED */
    int              cols;        /* for ANIMATED */
    int              rows;        /* for ANIMATED */
    int              frame;       /* 1-based current frame */
    /* location */
    double           x, y;
    int              offset_x;
    int              offset_y;    /* setFrameOffsets */
    bool             mirror_h;
    bool             mirror_v;
    double           alpha;       /* 0..1 */
    lh_transform     transform;
    /* animator (kind=ANIMATED) */
    lh_animator      animator;
} lh_sprite;

/* Constructors */
lh_sprite* lh_sprite_load_plain (SDL_Renderer* g, const char* path);
lh_sprite* lh_sprite_load_animated(SDL_Renderer* g, const char* path, int cols, int rows);
lh_sprite* lh_sprite_load_tiled  (SDL_Renderer* g, const char* path, int tw, int th);
lh_sprite* lh_sprite_from_buffer (lh_image_buffer* buf, lh_sprite_kind kind, int cols_or_tw, int rows_or_th);

/* Property setters mirroring Java method names */
void lh_sprite_set_location     (lh_sprite* s, double x, double y);
void lh_sprite_set_frame        (lh_sprite* s, int frame);  /* tiled/animated */
void lh_sprite_set_frame_offsets(lh_sprite* s, int ox, int oy);
void lh_sprite_set_mirror       (lh_sprite* s, bool h, bool v);
void lh_sprite_set_alpha        (lh_sprite* s, double a);
void lh_sprite_set_transform    (lh_sprite* s, lh_transform t);

int  lh_sprite_get_width    (const lh_sprite* s);
int  lh_sprite_get_height   (const lh_sprite* s);
int  lh_sprite_get_tile_width (const lh_sprite* s);
int  lh_sprite_get_tile_height(const lh_sprite* s);

/* Anim convenience (animated kind) */
void lh_sprite_play_anim   (lh_sprite* s, const lh_animation* anim);
void lh_sprite_update_anim (lh_sprite* s, double extrp);
lh_anim_state lh_sprite_anim_state(const lh_sprite* s);

void lh_sprite_render(const lh_sprite* s, lh_graphic* g);

void lh_sprite_destroy(lh_sprite* s);

#endif
