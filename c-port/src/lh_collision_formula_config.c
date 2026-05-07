/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 */
#include "lh_collision_formula_config.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>

static lh_coll_axis parse_axis(const char* s)
{
    if (!s) return LH_COLL_AXIS_Y;
    return (s[0] == 'X' || s[0] == 'x') ? LH_COLL_AXIS_X : LH_COLL_AXIS_Y;
}

static lh_coll_function_type parse_fn_type(const char* s)
{
    if (!s) return LH_COLL_FN_NONE;
    if (strcasecmp(s, "LINEAR") == 0) return LH_COLL_FN_LINEAR;
    return LH_COLL_FN_NONE;
}

static void parse_range(lh_coll_range* r, const lh_xml_node* node)
{
    memset(r, 0, sizeof(*r));
    r->axis  = parse_axis(lh_xml_attr_string(node, "output", "Y"));
    r->min_x = lh_xml_attr_int(node, "minX", 0);
    r->max_x = lh_xml_attr_int(node, "maxX", 0);
    r->min_y = lh_xml_attr_int(node, "minY", 0);
    r->max_y = lh_xml_attr_int(node, "maxY", 0);
}

static void parse_function(lh_coll_function* f, const lh_xml_node* node)
{
    memset(f, 0, sizeof(*f));
    f->type = parse_fn_type(lh_xml_attr_string(node, "type", "NONE"));
    f->a    = lh_xml_attr_double(node, "a", 0.0);
    f->b    = lh_xml_attr_double(node, "b", 0.0);
}

void lh_collision_formula_config_init(lh_collision_formula_config* cfg)
{
    if (!cfg) return;
    memset(cfg, 0, sizeof(*cfg));
}

bool lh_collision_formula_config_load(lh_collision_formula_config* cfg,
                                      const char* media)
{
    if (!cfg || !media) return false;
    lh_collision_formula_config_init(cfg);

    lh_xml_doc* doc = lh_xml_load(media);
    if (!doc) return false;

    const lh_xml_node* root = lh_xml_root(doc);
    if (!root || strcmp(lh_xml_node_name(root), LH_COLL_FORMULA_NODE_ROOT) != 0)
    {
        lh_xml_free(doc);
        return false;
    }

    for (lh_xml_node* f = lh_xml_node_first_child((lh_xml_node*)root,
                                                  LH_COLL_FORMULA_NODE_FORMULA);
         f;
         f = lh_xml_node_next_sibling(f, LH_COLL_FORMULA_NODE_FORMULA))
    {
        if (cfg->count >= LH_COLL_MAX_FORMULAS) break;
        lh_coll_formula* e = &cfg->entries[cfg->count];
        memset(e, 0, sizeof(*e));

        const char* name = lh_xml_attr_string(f, "name", "");
        snprintf(e->name, sizeof(e->name), "%s", name);

        lh_xml_node* range = lh_xml_node_first_child(f, LH_COLL_FORMULA_NODE_RANGE);
        if (range) parse_range(&e->range, range);

        lh_xml_node* fn = lh_xml_node_first_child(f, LH_COLL_FORMULA_NODE_FUNCTION);
        if (fn) parse_function(&e->function, fn);

        cfg->count++;
    }
    lh_xml_free(doc);
    return cfg->count > 0;
}

const lh_coll_formula* lh_collision_formula_config_get(
        const lh_collision_formula_config* cfg, const char* name)
{
    if (!cfg || !name) return NULL;
    for (int i = 0; i < cfg->count; i++)
    {
        if (strcmp(cfg->entries[i].name, name) == 0) return &cfg->entries[i];
    }
    return NULL;
}
