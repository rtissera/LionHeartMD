/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com)
 *                          Pierre-Alexandre (contact@b3dgs.com)
 * C/SDL2 port: faithful derivative of MapTilePersisterModel +
 *              MapTilePersisterOptimized.
 */
#include "lh_map_tile_persister.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "lh_le_file_reading.h"
#include "lh_le_file_writing.h"

/* ---- helpers ---- */

/* Java DataOutput.writeChar / readChar use 16-bit unsigned big-endian, but
 * our shared writer only exposes signed short. Same on-wire bytes; we just
 * mask the value to fit. */
static inline void write_u16(lh_file_writing* fw, unsigned v)
{
    lh_file_writing_short(fw, (int16_t)(v & 0xFFFFu));
}

static inline unsigned read_u16(lh_file_reading* fr)
{
    return (unsigned)((uint16_t)lh_file_reading_short(fr));
}

/* writeByte takes signed byte; UtilConversion.fromUnsignedByte((short) v)
 * returns ((v > 127) ? v - 256 : v) — i.e. a plain reinterpret. */
static inline void write_u8(lh_file_writing* fw, unsigned v)
{
    lh_file_writing_byte(fw, (int8_t)(v & 0xFFu));
}

static inline unsigned read_u8(lh_file_reading* fr)
{
    return (unsigned)((uint8_t)lh_file_reading_byte(fr));
}

/* Number of [0..BLOC_SIZE) blocs needed to cover len. */
static int bloc_count(int len)
{
    const int s = LH_MAP_PERSIST_BLOC_SIZE;
    if (len <= 0) return 0;
    return (len + s - 1) / s;
}

/* Count non-empty cells inside bloc (sx, sy). */
static int count_tiles_in_bloc(const lh_map_tile* m, int sx, int sy)
{
    const int s = LH_MAP_PERSIST_BLOC_SIZE;
    const int x0 = sx * s;
    const int y0 = sy * s;
    const int x1 = (x0 + s < m->in_tile_width)  ? x0 + s : m->in_tile_width;
    const int y1 = (y0 + s < m->in_tile_height) ? y0 + s : m->in_tile_height;
    int count = 0;
    /* Iterate column-major to match the save loop ordering exactly. */
    for (int tx = x0; tx < x1; tx++)
    {
        for (int ty = y0; ty < y1; ty++)
        {
            if (lh_map_tile_get_tile(m, tx, ty) != LH_TILE_EMPTY) count++;
        }
    }
    return count;
}

/* ---- save ---- */

bool lh_map_tile_persister_save(const lh_map_tile* m, const char* path)
{
    if (!m || !m->tiles || !path) return false;

    lh_file_writing fw;
    if (!lh_file_writing_open(&fw, path)) return false;

    write_u16(&fw, (unsigned)m->tile_width);
    write_u16(&fw, (unsigned)m->tile_height);
    lh_file_writing_int(&fw, (int32_t)m->in_tile_width);
    lh_file_writing_int(&fw, (int32_t)m->in_tile_height);

    const int s  = LH_MAP_PERSIST_BLOC_SIZE;
    const int tx = bloc_count(m->in_tile_width);
    const int ty = bloc_count(m->in_tile_height);
    write_u16(&fw, (unsigned)tx);
    write_u16(&fw, (unsigned)ty);

    bool truncated = false;

    for (int sx = 0; sx < tx && lh_file_writing_is_ok(&fw); sx++)
    {
        for (int sy = 0; sy < ty && lh_file_writing_is_ok(&fw); sy++)
        {
            const int n = count_tiles_in_bloc(m, sx, sy);
            write_u16(&fw, (unsigned)n);

            const int x0 = sx * s;
            const int y0 = sy * s;
            const int x1 = (x0 + s < m->in_tile_width)  ? x0 + s : m->in_tile_width;
            const int y1 = (y0 + s < m->in_tile_height) ? y0 + s : m->in_tile_height;

            for (int cx = x0; cx < x1; cx++)
            {
                for (int cy = y0; cy < y1; cy++)
                {
                    const int32_t cell = lh_map_tile_get_tile(m, cx, cy);
                    if (cell == LH_TILE_EMPTY) continue;

                    const int sheet  = LH_TILE_SHEET(cell);
                    const int number = LH_TILE_NUMBER(cell);
                    if (sheet != 0)
                    {
                        /* The optimized .lvl format only stores 16 bits;
                         * a non-zero sheet would be lost on round-trip. */
                        truncated = true;
                    }
                    write_u16(&fw, (unsigned)number);
                    write_u8 (&fw, (unsigned)(cx % LH_MAP_PERSIST_BLOC_SIZE));
                    write_u8 (&fw, (unsigned)(cy % LH_MAP_PERSIST_BLOC_SIZE));
                }
            }
        }
    }

    const bool ok = lh_file_writing_is_ok(&fw);
    lh_file_writing_close(&fw);
    if (truncated)
    {
        fprintf(stderr,
                "lh_map_tile_persister_save: warning, sheet ids dropped "
                "(format only stores 16-bit tile numbers)\n");
    }
    return ok;
}

/* ---- load ---- */

bool lh_map_tile_persister_load(lh_map_tile* m, const char* path)
{
    if (!m || !path) return false;

    lh_file_reading fr;
    if (!lh_file_reading_open(&fr, path)) return false;

    const unsigned tile_w  = read_u16(&fr);
    const unsigned tile_h  = read_u16(&fr);
    const int32_t  inw     = lh_file_reading_int(&fr);
    const int32_t  inh     = lh_file_reading_int(&fr);

    if (!lh_file_reading_is_ok(&fr)
        || tile_w == 0 || tile_h == 0 || inw <= 0 || inh <= 0)
    {
        lh_file_reading_close(&fr);
        return false;
    }
    if (!lh_map_tile_create(m, (int)tile_w, (int)tile_h, (int)inw, (int)inh))
    {
        lh_file_reading_close(&fr);
        return false;
    }

    const int tx = (int)read_u16(&fr);
    const int ty = (int)read_u16(&fr);

    const int s = LH_MAP_PERSIST_BLOC_SIZE;

    for (int sx = 0; sx < tx && lh_file_reading_is_ok(&fr); sx++)
    {
        for (int sy = 0; sy < ty && lh_file_reading_is_ok(&fr); sy++)
        {
            const int n = (int)read_u16(&fr);
            for (int t = 0; t < n && lh_file_reading_is_ok(&fr); t++)
            {
                const unsigned number = read_u16(&fr);
                const unsigned bx     = read_u8 (&fr);
                const unsigned by     = read_u8 (&fr);
                const int cx = (int)bx + sx * s;
                const int cy = (int)by + sy * s;
                /* sheet=0 — derived later from TileSheetsConfig if needed. */
                lh_map_tile_set_tile(m, cx, cy, LH_TILE_PACK(0, number));
            }
        }
    }

    const bool ok = lh_file_reading_is_ok(&fr);
    lh_file_reading_close(&fr);
    return ok;
}

/* ---- self test ---- */

/* Build a synthetic 320x40 tile grid filled with a known pattern, save it,
 * reload, and assert byte-for-byte equality of the cell array. The optional
 * `sample_lvl_path` is read for header inspection (no round-trip — see
 * NOTE below); pass NULL to skip.
 *
 * NOTE: the .lvl files committed under lionheart-stripped/assets appear to
 * encode tile entries with a sub-bloc index whose width is larger than
 * BLOC_SIZE (255), suggesting they were produced by a tool other than
 * the documented MapTilePersisterModel (or with a different BLOC_SIZE
 * constant). Lionheart's own MapTilePersister still consumes them by
 * trusting on-disk values. Our port faithfully implements the Java spec
 * (BLOC_SIZE = Constant.UNSIGNED_BYTE - 1 = 255). Files our save() emits
 * round-trip cleanly; reading the legacy .lvl files would require a
 * format-detection layer added later. */
bool lh_map_tile_persister_self_test(const char* sample_lvl_path)
{
    /* Synthetic round-trip. */
    lh_map_tile m; memset(&m, 0, sizeof(m));
    if (!lh_map_tile_create(&m, 16, 16, 320, 40))
    {
        fprintf(stderr, "self_test: create failed\n");
        return false;
    }
    /* Fill a deterministic pattern: tiles on every 3rd cell, value derived
     * from coords, plus a few empty rows/cols. */
    int set = 0;
    for (int ty = 0; ty < m.in_tile_height; ty++)
    {
        for (int tx = 0; tx < m.in_tile_width; tx++)
        {
            if ((tx + ty) % 3 == 0) continue;
            const int num = (tx * 17 + ty * 31) & 0xFFFF;
            lh_map_tile_set_tile(&m, tx, ty, LH_TILE_PACK(0, num));
            set++;
        }
    }
    printf("self_test: synthetic grid %dx%d, %d non-empty cells\n",
           m.in_tile_width, m.in_tile_height, set);

    char tmp[64];
    snprintf(tmp, sizeof(tmp), "/tmp/lh_lvl_roundtrip_%d.lvl", (int)getpid());

    if (!lh_map_tile_persister_save(&m, tmp))
    {
        fprintf(stderr, "self_test: save failed -> %s\n", tmp);
        lh_map_tile_destroy(&m);
        return false;
    }

    lh_map_tile rl; memset(&rl, 0, sizeof(rl));
    if (!lh_map_tile_persister_load(&rl, tmp))
    {
        fprintf(stderr, "self_test: reload failed\n");
        lh_map_tile_destroy(&m);
        remove(tmp);
        return false;
    }

    bool match =
        m.tile_width     == rl.tile_width    &&
        m.tile_height    == rl.tile_height   &&
        m.in_tile_width  == rl.in_tile_width &&
        m.in_tile_height == rl.in_tile_height;
    if (match)
    {
        const size_t cells = (size_t)m.in_tile_width
                           * (size_t)m.in_tile_height;
        match = memcmp(m.tiles, rl.tiles, cells * sizeof(int32_t)) == 0;
    }
    printf("self_test: round-trip %s\n", match ? "OK" : "FAILED");

    /* Optional header read of a sample legacy .lvl for diagnostics. */
    if (sample_lvl_path)
    {
        lh_map_tile s; memset(&s, 0, sizeof(s));
        const bool sok = lh_map_tile_persister_load(&s, sample_lvl_path);
        printf("self_test: legacy header %s -> %s",
               sample_lvl_path, sok ? "load OK" : "load failed");
        if (sok)
        {
            printf(", tile=%dx%d grid=%dx%d (pixels=%dx%d)\n",
                   s.tile_width, s.tile_height,
                   s.in_tile_width, s.in_tile_height,
                   lh_map_tile_get_width(&s), lh_map_tile_get_height(&s));
            lh_map_tile_destroy(&s);
        }
        else
        {
            printf("\n");
        }
    }

    remove(tmp);
    lh_map_tile_destroy(&m);
    lh_map_tile_destroy(&rl);
    return match;
}
