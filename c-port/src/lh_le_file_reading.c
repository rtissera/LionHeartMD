/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Big-endian to host-byte-order conversions assume little-endian host
 * (x86_64 / Mega Drive 68k both follow Java big-endian over the wire).
 */
#include "lh_le_file_reading.h"

#include <string.h>

bool lh_file_reading_open(lh_file_reading* fr, const char* path)
{
    if (!fr || !path) return false;
    fr->fp = fopen(path, "rb");
    fr->ok = fr->fp != NULL;
    return fr->ok;
}

void lh_file_reading_close(lh_file_reading* fr)
{
    if (!fr) return;
    if (fr->fp) fclose(fr->fp);
    fr->fp = NULL;
    fr->ok = false;
}

static size_t read_n(lh_file_reading* fr, void* buf, size_t n)
{
    if (!fr || !fr->ok || !fr->fp) return 0;
    const size_t got = fread(buf, 1, n, fr->fp);
    if (got != n) fr->ok = false;
    return got;
}

bool lh_file_reading_bool(lh_file_reading* fr)
{
    uint8_t b = 0;
    read_n(fr, &b, 1);
    return b != 0;
}

int8_t lh_file_reading_byte(lh_file_reading* fr)
{
    int8_t b = 0;
    read_n(fr, &b, 1);
    return b;
}

int16_t lh_file_reading_short(lh_file_reading* fr)
{
    uint8_t b[2] = { 0, 0 };
    read_n(fr, b, 2);
    return (int16_t)((b[0] << 8) | b[1]);
}

int32_t lh_file_reading_int(lh_file_reading* fr)
{
    uint8_t b[4] = { 0 };
    read_n(fr, b, 4);
    return (int32_t)((uint32_t)b[0] << 24 | (uint32_t)b[1] << 16 |
                     (uint32_t)b[2] << 8  | (uint32_t)b[3]);
}

int64_t lh_file_reading_long(lh_file_reading* fr)
{
    uint8_t b[8] = { 0 };
    read_n(fr, b, 8);
    uint64_t v = 0;
    for (int i = 0; i < 8; i++) v = (v << 8) | b[i];
    return (int64_t)v;
}

float lh_file_reading_float(lh_file_reading* fr)
{
    uint32_t u = (uint32_t)lh_file_reading_int(fr);
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

double lh_file_reading_double(lh_file_reading* fr)
{
    uint64_t u = (uint64_t)lh_file_reading_long(fr);
    double d;
    memcpy(&d, &u, sizeof(d));
    return d;
}

/* Java DataInput.readUTF format: 2-byte unsigned length, then modified UTF-8.
 * For Lionheart save data the strings are ASCII media paths — we treat the
 * payload as plain bytes and null-terminate. */
size_t lh_file_reading_string(lh_file_reading* fr, char* out, size_t out_len)
{
    if (!out || out_len == 0) return 0;
    out[0] = '\0';
    uint8_t lb[2] = { 0, 0 };
    read_n(fr, lb, 2);
    const size_t n = (size_t)((lb[0] << 8) | lb[1]);
    const size_t cap = (n < out_len - 1) ? n : out_len - 1;
    if (cap) read_n(fr, out, cap);
    out[cap] = '\0';
    /* If string was longer than buffer, drain the rest. */
    if (n > cap)
    {
        char dump[64];
        size_t left = n - cap;
        while (left)
        {
            const size_t chunk = left > sizeof(dump) ? sizeof(dump) : left;
            read_n(fr, dump, chunk);
            left -= chunk;
        }
    }
    return cap;
}

bool lh_file_reading_is_ok(const lh_file_reading* fr)
{
    return fr && fr->ok;
}
