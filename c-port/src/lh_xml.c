/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Tiny single-file XML reader. Recursive-descent parser, arena-allocated.
 */
#include "lh_xml.h"
#include "lh_medias.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LH_XML_ARENA_BLOCK (64 * 1024)

typedef struct lh_xml_chunk {
    struct lh_xml_chunk* next;
    size_t   used;
    size_t   cap;
    char     data[];
} lh_xml_chunk;

struct lh_xml_doc {
    lh_xml_chunk* arena;
    lh_xml_node*  root;
};

static char s_err[256] = "";

const char* lh_xml_last_error(void) { return s_err; }

static void* arena_alloc(lh_xml_doc* d, size_t n)
{
    if (!d->arena || d->arena->used + n > d->arena->cap)
    {
        const size_t cap = (n > LH_XML_ARENA_BLOCK) ? n : LH_XML_ARENA_BLOCK;
        lh_xml_chunk* c = (lh_xml_chunk*)calloc(1, sizeof(*c) + cap);
        if (!c) return NULL;
        c->cap  = cap;
        c->next = d->arena;
        d->arena = c;
    }
    void* p = d->arena->data + d->arena->used;
    d->arena->used += n;
    return p;
}

static char* arena_strndup(lh_xml_doc* d, const char* s, size_t n)
{
    char* out = (char*)arena_alloc(d, n + 1);
    if (!out) return NULL;
    memcpy(out, s, n);
    out[n] = '\0';
    return out;
}

/* Parser state. */
typedef struct {
    const char*  p;
    const char*  end;
    lh_xml_doc*  doc;
} P;

static void skip_ws(P* p)
{
    while (p->p < p->end && isspace((unsigned char)*p->p)) p->p++;
}

static bool starts_with(P* p, const char* s)
{
    const size_t n = strlen(s);
    return (size_t)(p->end - p->p) >= n && memcmp(p->p, s, n) == 0;
}

static bool skip_comment_or_decl(P* p)
{
    if (starts_with(p, "<!--"))
    {
        const char* q = strstr(p->p, "-->");
        if (!q || q >= p->end) { snprintf(s_err, sizeof(s_err), "unterminated comment"); return false; }
        p->p = q + 3;
        return true;
    }
    if (starts_with(p, "<?"))
    {
        const char* q = strstr(p->p, "?>");
        if (!q || q >= p->end) { snprintf(s_err, sizeof(s_err), "unterminated processing instruction"); return false; }
        p->p = q + 2;
        return true;
    }
    if (starts_with(p, "<!"))
    {
        /* Skip <!DOCTYPE ...> non-recursively. */
        while (p->p < p->end && *p->p != '>') p->p++;
        if (p->p < p->end) p->p++;
        return true;
    }
    return true;
}

static bool is_name_start(int c)
{
    return isalpha(c) || c == '_';
}

static bool is_name_char(int c)
{
    return isalnum(c) || c == '_' || c == '-' || c == '.' || c == ':';
}

static const char* parse_name(P* p, size_t* out_len)
{
    const char* start = p->p;
    if (p->p >= p->end || !is_name_start((unsigned char)*p->p)) return NULL;
    p->p++;
    while (p->p < p->end && is_name_char((unsigned char)*p->p)) p->p++;
    *out_len = (size_t)(p->p - start);
    return start;
}

/* Decode &amp; &lt; &gt; &quot; &apos; in-place into dst. */
static char* decode_entities(lh_xml_doc* d, const char* src, size_t n)
{
    char* out = (char*)arena_alloc(d, n + 1);
    if (!out) return NULL;
    size_t o = 0;
    for (size_t i = 0; i < n; i++)
    {
        if (src[i] == '&')
        {
            if (i + 4 < n && memcmp(src + i, "&amp;", 5) == 0)  { out[o++] = '&'; i += 4; continue; }
            if (i + 3 < n && memcmp(src + i, "&lt;",  4) == 0)  { out[o++] = '<'; i += 3; continue; }
            if (i + 3 < n && memcmp(src + i, "&gt;",  4) == 0)  { out[o++] = '>'; i += 3; continue; }
            if (i + 5 < n && memcmp(src + i, "&quot;",6) == 0)  { out[o++] = '"'; i += 5; continue; }
            if (i + 5 < n && memcmp(src + i, "&apos;",6) == 0)  { out[o++] = '\''; i += 5; continue; }
        }
        out[o++] = src[i];
    }
    out[o] = '\0';
    return out;
}

/* Parse a quoted attribute value (delimited by " or '). */
static const char* parse_quoted(P* p, size_t* out_len)
{
    if (p->p >= p->end) return NULL;
    const char q = *p->p;
    if (q != '"' && q != '\'') return NULL;
    p->p++;
    const char* start = p->p;
    while (p->p < p->end && *p->p != q) p->p++;
    if (p->p >= p->end) return NULL;
    *out_len = (size_t)(p->p - start);
    p->p++;
    return start;
}

static lh_xml_node* parse_element(P* p);

static bool parse_attrs(P* p, lh_xml_node* n)
{
    lh_xml_attr** tail = &n->attrs;
    while (1)
    {
        skip_ws(p);
        if (p->p >= p->end) return false;
        if (*p->p == '/' || *p->p == '>') return true;

        size_t nlen = 0;
        const char* name = parse_name(p, &nlen);
        if (!name) { snprintf(s_err, sizeof(s_err), "expected attr name"); return false; }
        skip_ws(p);
        if (p->p >= p->end || *p->p != '=') { snprintf(s_err, sizeof(s_err), "expected '='"); return false; }
        p->p++;
        skip_ws(p);

        size_t vlen = 0;
        const char* val = parse_quoted(p, &vlen);
        if (!val) { snprintf(s_err, sizeof(s_err), "expected quoted attr value"); return false; }

        lh_xml_attr* a = (lh_xml_attr*)arena_alloc(p->doc, sizeof(*a));
        if (!a) return false;
        a->name  = arena_strndup(p->doc, name, nlen);
        a->value = decode_entities(p->doc, val, vlen);
        a->next  = NULL;
        *tail = a;
        tail = &a->next;
    }
}

static lh_xml_node* parse_element(P* p)
{
    skip_ws(p);
    while (starts_with(p, "<!--") || starts_with(p, "<?") || starts_with(p, "<!"))
    {
        if (!skip_comment_or_decl(p)) return NULL;
        skip_ws(p);
    }

    if (p->p >= p->end || *p->p != '<') { snprintf(s_err, sizeof(s_err), "expected '<'"); return NULL; }
    p->p++;

    size_t nlen = 0;
    const char* name = parse_name(p, &nlen);
    if (!name) { snprintf(s_err, sizeof(s_err), "expected element name"); return NULL; }

    lh_xml_node* node = (lh_xml_node*)arena_alloc(p->doc, sizeof(*node));
    if (!node) return NULL;
    memset(node, 0, sizeof(*node));
    node->name = arena_strndup(p->doc, name, nlen);

    if (!parse_attrs(p, node)) return NULL;

    if (p->p < p->end && *p->p == '/')
    {
        p->p++;
        if (p->p >= p->end || *p->p != '>') { snprintf(s_err, sizeof(s_err), "expected '>'"); return NULL; }
        p->p++;
        return node;       /* self-closing */
    }
    if (p->p >= p->end || *p->p != '>') { snprintf(s_err, sizeof(s_err), "expected '>'"); return NULL; }
    p->p++;

    /* children + text */
    const char* text_start = p->p;
    lh_xml_node** child_tail = &node->first_child;
    while (1)
    {
        if (p->p >= p->end) { snprintf(s_err, sizeof(s_err), "unexpected eof"); return NULL; }

        if (starts_with(p, "<!--") || starts_with(p, "<?") || starts_with(p, "<!"))
        {
            if (!skip_comment_or_decl(p)) return NULL;
            continue;
        }

        if (*p->p == '<' && p->p + 1 < p->end && p->p[1] == '/')
        {
            /* End tag. */
            if (!node->text)
            {
                /* Capture text content if any. */
                size_t tlen = (size_t)(p->p - text_start);
                while (tlen > 0 && isspace((unsigned char)text_start[tlen - 1])) tlen--;
                while (tlen > 0 && isspace((unsigned char)*text_start)) { text_start++; tlen--; }
                if (tlen > 0) node->text = decode_entities(p->doc, text_start, tlen);
            }
            p->p += 2;
            size_t elen = 0;
            const char* ename = parse_name(p, &elen);
            (void)ename; (void)elen;
            skip_ws(p);
            if (p->p >= p->end || *p->p != '>') { snprintf(s_err, sizeof(s_err), "expected '>' end tag"); return NULL; }
            p->p++;
            return node;
        }
        if (*p->p == '<')
        {
            lh_xml_node* child = parse_element(p);
            if (!child) return NULL;
            child->parent = node;
            *child_tail = child;
            child_tail = &child->next_sibling;
            text_start = p->p;
        }
        else
        {
            p->p++;
        }
    }
}

lh_xml_doc* lh_xml_parse(const char* buf, size_t len)
{
    if (!buf) { snprintf(s_err, sizeof(s_err), "null buffer"); return NULL; }
    lh_xml_doc* d = (lh_xml_doc*)calloc(1, sizeof(*d));
    if (!d) return NULL;

    P p = { buf, buf + len, d };
    skip_ws(&p);
    while (starts_with(&p, "<!--") || starts_with(&p, "<?") || starts_with(&p, "<!"))
    {
        if (!skip_comment_or_decl(&p)) { lh_xml_free(d); return NULL; }
        skip_ws(&p);
    }
    d->root = parse_element(&p);
    if (!d->root) { lh_xml_free(d); return NULL; }
    return d;
}

lh_xml_doc* lh_xml_load(const char* logical)
{
    char path[LH_MEDIA_PATH_MAX];
    if (!lh_medias_resolve(logical, path, sizeof(path)))
    {
        snprintf(s_err, sizeof(s_err), "media path overflow");
        return NULL;
    }
    FILE* fp = fopen(path, "rb");
    if (!fp) { snprintf(s_err, sizeof(s_err), "open '%s' failed", path); return NULL; }
    fseek(fp, 0, SEEK_END);
    const long n = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (n <= 0) { fclose(fp); snprintf(s_err, sizeof(s_err), "empty file"); return NULL; }
    char* buf = (char*)malloc((size_t)n);
    if (!buf) { fclose(fp); return NULL; }
    if (fread(buf, 1, (size_t)n, fp) != (size_t)n) { free(buf); fclose(fp); snprintf(s_err, sizeof(s_err), "read fail"); return NULL; }
    fclose(fp);
    lh_xml_doc* d = lh_xml_parse(buf, (size_t)n);
    free(buf);
    return d;
}

void lh_xml_free(lh_xml_doc* d)
{
    if (!d) return;
    lh_xml_chunk* c = d->arena;
    while (c) { lh_xml_chunk* n = c->next; free(c); c = n; }
    free(d);
}

lh_xml_node* lh_xml_root(const lh_xml_doc* d) { return d ? d->root : NULL; }

const char* lh_xml_node_name(const lh_xml_node* n) { return n ? n->name : NULL; }
const char* lh_xml_node_text(const lh_xml_node* n) { return n ? n->text : NULL; }

bool lh_xml_node_has_node(const lh_xml_node* n, const char* child)
{
    return lh_xml_node_first_child(n, child) != NULL;
}

lh_xml_node* lh_xml_node_first_child(const lh_xml_node* n, const char* child)
{
    if (!n) return NULL;
    for (lh_xml_node* c = n->first_child; c; c = c->next_sibling)
        if (!child || strcmp(c->name, child) == 0) return c;
    return NULL;
}

lh_xml_node* lh_xml_node_next_sibling(const lh_xml_node* n, const char* sibling)
{
    if (!n) return NULL;
    for (lh_xml_node* c = n->next_sibling; c; c = c->next_sibling)
        if (!sibling || strcmp(c->name, sibling) == 0) return c;
    return NULL;
}

bool lh_xml_attr_has(const lh_xml_node* n, const char* attr)
{
    if (!n || !attr) return false;
    for (lh_xml_attr* a = n->attrs; a; a = a->next)
        if (strcmp(a->name, attr) == 0) return true;
    return false;
}

const char* lh_xml_attr_string(const lh_xml_node* n, const char* attr, const char* def)
{
    if (!n || !attr) return def;
    for (lh_xml_attr* a = n->attrs; a; a = a->next)
        if (strcmp(a->name, attr) == 0) return a->value;
    return def;
}

int    lh_xml_attr_int   (const lh_xml_node* n, const char* a, int def)    { const char* v = lh_xml_attr_string(n, a, NULL); return v ? atoi(v) : def; }
double lh_xml_attr_double(const lh_xml_node* n, const char* a, double def) { const char* v = lh_xml_attr_string(n, a, NULL); return v ? atof(v) : def; }
bool   lh_xml_attr_bool  (const lh_xml_node* n, const char* a, bool def)   {
    const char* v = lh_xml_attr_string(n, a, NULL);
    if (!v) return def;
    return strcmp(v, "true") == 0 || strcmp(v, "1") == 0;
}
