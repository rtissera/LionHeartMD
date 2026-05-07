/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_medias.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static char s_root[LH_MEDIA_PATH_MAX] = "assets";
static char s_buf [LH_MEDIA_PATH_MAX];

void lh_medias_set_root(const char* root)
{
    if (!root) return;
    snprintf(s_root, sizeof(s_root), "%s", root);
}

const char* lh_medias_get_root(void)
{
    return s_root;
}

bool lh_medias_resolve(const char* logical, char* out, size_t out_len)
{
    if (!logical || !out || out_len == 0) return false;
    const int n = snprintf(out, out_len, "%s/%s", s_root, logical);
    return n > 0 && (size_t)n < out_len;
}

static bool join_va(char* out, size_t out_len, const char* a, va_list ap)
{
    if (!out || out_len == 0 || !a) return false;
    size_t pos = 0;
    out[0] = '\0';
    const char* seg = a;
    while (seg)
    {
        const size_t need = strlen(seg) + (pos ? 1 : 0);
        if (pos + need + 1 > out_len) return false;
        if (pos) out[pos++] = '/';
        memcpy(out + pos, seg, strlen(seg));
        pos += strlen(seg);
        out[pos] = '\0';
        seg = va_arg(ap, const char*);
    }
    return true;
}

const char* lh_medias_create(const char* a, ...)
{
    va_list ap;
    va_start(ap, a);
    const bool ok = join_va(s_buf, sizeof(s_buf), a, ap);
    va_end(ap);
    return ok ? s_buf : "";
}

bool lh_medias_create_into(char* out, size_t out_len, const char* a, ...)
{
    va_list ap;
    va_start(ap, a);
    const bool ok = join_va(out, out_len, a, ap);
    va_end(ap);
    return ok;
}

bool lh_medias_exists(const char* logical)
{
    char path[LH_MEDIA_PATH_MAX];
    if (!lh_medias_resolve(logical, path, sizeof(path))) return false;
    struct stat st;
    return stat(path, &st) == 0;
}
