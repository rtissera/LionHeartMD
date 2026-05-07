/*
 * Lionheart Remake - C/SDL2 port. GPL-3.0.
 *
 * Port of com.b3dgs.lionengine.Xml + XmlReader (subset Lionheart needs).
 *
 * Minimal DOM-style XML reader. Handles element tags, attributes, namespace
 * prefixes (stored verbatim in names), self-closing, comments, XML
 * declaration, basic entity escapes (&amp; &lt; &gt; &quot; &apos;).
 * No CDATA, no DTD, no namespaces semantically. UTF-8 byte stream.
 *
 * Memory: a single arena owns all element + attribute strings; lh_xml_free
 * releases everything in O(1).
 */
#ifndef LH_XML_H
#define LH_XML_H

#include <stdbool.h>
#include <stddef.h>

typedef struct lh_xml_node lh_xml_node;
typedef struct lh_xml_attr lh_xml_attr;

struct lh_xml_attr {
    const char*  name;
    const char*  value;
    lh_xml_attr* next;
};

struct lh_xml_node {
    const char*  name;
    const char*  text;        /* trimmed text content; nullable */
    lh_xml_attr* attrs;
    lh_xml_node* parent;      /* nullable for root */
    lh_xml_node* first_child; /* nullable */
    lh_xml_node* next_sibling;
};

typedef struct lh_xml_doc lh_xml_doc;

/* Load + parse a logical media path through lh_medias_resolve. Returns NULL
 * on parse error; in that case lh_xml_last_error returns a static string. */
lh_xml_doc* lh_xml_load(const char* logical);

/* Parse from in-memory buffer. */
lh_xml_doc* lh_xml_parse(const char* buf, size_t len);

void        lh_xml_free(lh_xml_doc* doc);

/* Access. */
lh_xml_node* lh_xml_root(const lh_xml_doc* doc);
const char*  lh_xml_last_error(void);

/* Node API mirrors Java Xml/XmlReader. */
const char*  lh_xml_node_name (const lh_xml_node* n);
const char*  lh_xml_node_text (const lh_xml_node* n);
bool         lh_xml_node_has_node(const lh_xml_node* n, const char* child_name);
lh_xml_node* lh_xml_node_first_child(const lh_xml_node* n, const char* child_name);
lh_xml_node* lh_xml_node_next_sibling(const lh_xml_node* n, const char* sibling_name);

bool         lh_xml_attr_has   (const lh_xml_node* n, const char* attr);
const char*  lh_xml_attr_string(const lh_xml_node* n, const char* attr, const char* def);
int          lh_xml_attr_int   (const lh_xml_node* n, const char* attr, int def);
double       lh_xml_attr_double(const lh_xml_node* n, const char* attr, double def);
bool         lh_xml_attr_bool  (const lh_xml_node* n, const char* attr, bool def);

#endif
