/*
 * sc68 public C API - minimal subset used by the LionheartMD C/SDL2 port.
 *
 * Derived from:
 *   - exported symbols of the prebuilt libsc68.so vendored at
 *     c-port/lib/libsc68.so (nm -D),
 *   - the upstream b3dgs JNA binding at
 *     lionengine/java/lionengine-audio-sc68/src/main/java/
 *     com/b3dgs/lionengine/audio/sc68/Sc68Binding.java,
 *   - disassembly of the sc68Play wrapper in the same .so (which calls
 *     sc68_init / sc68_create / sc68_load_uri / sc68_play / sc68_process
 *     / sc68_stop with NULL config args).
 *
 * Only the entry points needed for offline 16-bit stereo decoding into
 * a host-driven mixer (no libao / PortAudio output) are declared. The
 * sc68 instance is treated as an opaque pointer; structure layouts are
 * NOT replicated here — the C/SDL2 port resolves these symbols via
 * dlopen + dlsym and never dereferences the handle directly.
 *
 * sc68 itself is LGPL-2.1-or-later. The original sc68 project is
 * Copyright (C) Benjamin Gerard. The header below is a minimal API
 * description authored for binding purposes; preserve the LGPL banner
 * if redistributing.
 *
 * ---------------------------------------------------------------------
 *  sc68 - Atari ST and Amiga SC68 chiptune player
 *  Copyright (C) 1998-2016 Benjamin Gerard <https://sourceforge.net/projects/sc68/>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301 USA.
 * ---------------------------------------------------------------------
 */
#ifndef LH_VENDOR_SC68_H
#define LH_VENDOR_SC68_H

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque sc68 instance handle. Internally the first 4 bytes are the
 * little-endian magic 'sc68' (0x73633638) — sc68_process / sc68_cntl
 * use this to validate the pointer. */
typedef struct sc68_s sc68_t;

/* sc68_process return-code flags (bitmask).
 *
 * Reverse-engineered from the disassembly of sc68_process; only the
 * values actually consulted by the LionheartMD port are declared. The
 * loop pattern from sc68Play is essentially:
 *
 *     int n = nb_frames;
 *     int r = sc68_process(sc, buf, &n);
 *     if (r == SC68_ERROR)        bail;
 *     if (r & SC68_END)           track finished;
 *
 * On success n is updated to the number of stereo frames produced.
 */
#define SC68_IDLE       (1 << 0) /* no track loaded / not playing      */
#define SC68_CHANGE     (1 << 1) /* track changed (e.g. multi-tunes)   */
#define SC68_LOOP       (1 << 2) /* loop point reached                 */
#define SC68_END        (1 << 3) /* end of track                       */
#define SC68_SEEK       (1 << 4) /* seek in progress                   */
#define SC68_ERROR      (-1)     /* fatal error                        */

/* Initialise the sc68 library. Pass NULL for default configuration —
 * libsc68 internally fabricates a zero-filled sc68_init_t on the
 * stack in that case. Returns 0 on success, non-zero on failure. */
int   sc68_init(void* init_args);

/* Tear down. Matches sc68_init. */
void  sc68_shutdown(void);

/* Create an sc68 player instance. NULL ⇒ default configuration. */
sc68_t* sc68_create(void* create_args);

/* Destroy an sc68 player instance. Safe with NULL. */
void  sc68_destroy(sc68_t* sc68);

/* Load an sc68 file from a URI (plain filesystem path is accepted).
 * Returns 0 on success. */
int   sc68_load_uri(sc68_t* sc68, const char* uri);

/* Load from a memory buffer. */
int   sc68_load_mem(sc68_t* sc68, const void* buffer, int len);

/* Start a track. track==-1 selects the disk's default track,
 * loop ==-1 selects the disk's default loop count (typically 1).
 * Returns 0 on success. */
int   sc68_play(sc68_t* sc68, int track, int loop);

/* Stop the current track. Returns 0 on success. */
int   sc68_stop(sc68_t* sc68);

/* Decode stereo S16 frames into `buffer`. On entry *frames is the
 * requested number of stereo frames; on return it is updated to the
 * number actually produced. Returns a SC68_* flag bitmask, or
 * SC68_ERROR on failure. */
int   sc68_process(sc68_t* sc68, void* buffer, int* frames);

/* Variadic control / configuration. Used by the JNA wrapper for
 * volume; we mirror that path. */
int   sc68_cntl(sc68_t* sc68, int cmd, ...);

/* Best-effort error string for the last failure on `sc68`. May be NULL
 * (e.g. when sc68 is itself NULL). */
const char* sc68_error(sc68_t* sc68);

#ifdef __cplusplus
}
#endif

#endif /* LH_VENDOR_SC68_H */
