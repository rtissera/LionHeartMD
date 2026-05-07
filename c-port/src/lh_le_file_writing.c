/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_le_file_writing.h"

#include <string.h>

bool lh_file_writing_open(lh_file_writing* fw, const char* path)
{
    if (!fw || !path) return false;
    fw->fp = fopen(path, "wb");
    fw->ok = fw->fp != NULL;
    return fw->ok;
}

void lh_file_writing_close(lh_file_writing* fw)
{
    if (!fw) return;
    if (fw->fp) fclose(fw->fp);
    fw->fp = NULL;
    fw->ok = false;
}

static void write_n(lh_file_writing* fw, const void* buf, size_t n)
{
    if (!fw || !fw->ok || !fw->fp) return;
    if (fwrite(buf, 1, n, fw->fp) != n) fw->ok = false;
}

void lh_file_writing_bool (lh_file_writing* fw, bool v)
{
    uint8_t b = v ? 1 : 0;
    write_n(fw, &b, 1);
}

void lh_file_writing_byte (lh_file_writing* fw, int8_t v)
{
    write_n(fw, &v, 1);
}

void lh_file_writing_short(lh_file_writing* fw, int16_t v)
{
    uint8_t b[2] = { (uint8_t)(v >> 8), (uint8_t)v };
    write_n(fw, b, 2);
}

void lh_file_writing_int(lh_file_writing* fw, int32_t v)
{
    uint8_t b[4] = {
        (uint8_t)(v >> 24), (uint8_t)(v >> 16),
        (uint8_t)(v >> 8),  (uint8_t)v
    };
    write_n(fw, b, 4);
}

void lh_file_writing_long(lh_file_writing* fw, int64_t v)
{
    uint8_t b[8];
    for (int i = 0; i < 8; i++) b[i] = (uint8_t)(v >> ((7 - i) * 8));
    write_n(fw, b, 8);
}

void lh_file_writing_float(lh_file_writing* fw, float v)
{
    uint32_t u;
    memcpy(&u, &v, sizeof(u));
    lh_file_writing_int(fw, (int32_t)u);
}

void lh_file_writing_double(lh_file_writing* fw, double v)
{
    uint64_t u;
    memcpy(&u, &v, sizeof(u));
    lh_file_writing_long(fw, (int64_t)u);
}

void lh_file_writing_string(lh_file_writing* fw, const char* s)
{
    if (!s) s = "";
    const size_t n = strlen(s);
    const uint16_t len = (uint16_t)(n > 0xFFFF ? 0xFFFF : n);
    uint8_t lb[2] = { (uint8_t)(len >> 8), (uint8_t)len };
    write_n(fw, lb, 2);
    write_n(fw, s, len);
}

bool lh_file_writing_is_ok(const lh_file_writing* fw)
{
    return fw && fw->ok;
}
