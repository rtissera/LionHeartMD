/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * LionEngine subset: Loader + Sequence pattern.
 * Java equivalents:
 *   com.b3dgs.lionengine.graphic.engine.Loader
 *   com.b3dgs.lionengine.graphic.engine.Sequence
 *   com.b3dgs.lionengine.graphic.engine.Sequencable
 *   com.b3dgs.lionengine.Config / Resolution
 *
 * Sequence vtable mirrors Sequence's load/update/render/onTerminated.
 */
#ifndef LH_ENGINE_H
#define LH_ENGINE_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

/* Resolution: width, height, refresh rate (Hz). */
typedef struct {
    int width;
    int height;
    int rate;
} lh_resolution;

/* Config: window/fullscreen + source/output resolution. Mirrors LionEngine Config. */
typedef struct {
    lh_resolution source;       /* internal render resolution */
    lh_resolution output;       /* window/screen resolution */
    bool          windowed;
    int           depth;        /* bit depth, default 32 */
    const char*   icon_path;    /* optional */
    const char*   title;
} lh_config;

struct lh_sequence;
typedef struct lh_sequence lh_sequence;

/* Sequence vtable. Each method may be NULL. */
typedef struct {
    void (*load)      (lh_sequence* self);
    void (*update)    (lh_sequence* self, double extrp);
    void (*render)    (lh_sequence* self, SDL_Renderer* g);
    void (*on_resolution_changed)(lh_sequence* self, int w, int h, int rate);
    void (*on_terminated)(lh_sequence* self, bool has_next);
    void (*destroy)   (lh_sequence* self);
} lh_sequence_vt;

/* Sequence base. Concrete sequences embed this as first field. */
struct lh_sequence {
    const lh_sequence_vt* vt;
    lh_resolution         resolution;
    int                   rate;       /* target tick rate Hz */
    bool                  end_request;
    /* next sequence factory (set via lh_sequence_end_with) */
    lh_sequence* (*next_factory)(void* args);
    void*                 next_args;
    void*                 user;       /* user data slot */
};

/* Loader: starts the run loop on the given initial sequence factory. */
typedef struct {
    SDL_Window*   window;
    SDL_Renderer* renderer;
    lh_config     config;
    lh_sequence*  current;
    bool          running;
} lh_loader;

/* Init/teardown SDL & window. */
int  lh_loader_init(lh_loader* l, const lh_config* cfg);
void lh_loader_shutdown(lh_loader* l);

/* Run loop until current sequence ends with no next. */
int  lh_loader_run(lh_loader* l, lh_sequence* initial);

/* Sequence helpers (called from inside a Sequence). */
void lh_sequence_init(lh_sequence* s,
                      const lh_sequence_vt* vt,
                      const lh_resolution* res);

/* Request end with no next: return to caller, exit run loop. */
void lh_sequence_end(lh_sequence* s);

/* Request end and chain to next sequence built from factory(args). */
void lh_sequence_end_with(lh_sequence* s,
                          lh_sequence* (*factory)(void* args),
                          void* args);

#endif /* LH_ENGINE_H */
