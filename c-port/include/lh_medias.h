/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.Medias.
 *
 * Asset path resolution. Converts logical media paths (e.g.
 * "com/b3dgs/lionheart/sprite/foo.png") into filesystem paths under the
 * configured resources root (default: ./assets/com/b3dgs/lionheart/...).
 */
#ifndef LH_MEDIAS_H
#define LH_MEDIAS_H

#include <stdbool.h>
#include <stddef.h>

#define LH_MEDIA_PATH_MAX 512

/* Set the resources root directory (e.g. "./assets" or absolute path).
 * Subsequent lh_media_resolve calls prefix this. */
void        lh_medias_set_root(const char* root);
const char* lh_medias_get_root(void);

/* Resolve a logical media path into out (absolute or relative to cwd).
 * Returns true on success, false if out buffer too small. */
bool lh_medias_resolve(const char* logical, char* out, size_t out_len);

/* Build a logical media path by joining segments with '/'. Up to 6 segs;
 * NULL terminates the list. Mirrors Java Medias.create(parent, child...).
 * Output is in a static buffer overwritten on next call. Use
 * lh_medias_create_into for thread-safety. */
const char* lh_medias_create(const char* a, ...);
bool        lh_medias_create_into(char* out, size_t out_len,
                                  const char* a, ...);

/* Existence check on the resolved filesystem path. */
bool lh_medias_exists(const char* logical);

#endif
