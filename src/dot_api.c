#include "../include/dot_api.h"
#include "../include/dlist.h"

#include <stdio.h>
#include <assert.h>


void init_node(FILE *dot_file, DList *dlist, ssize_t ind) {
    assert(dot_file != NULL);
    assert(dlist != NULL);

    Node node = dlist->data[ind];

    const char *fillcolor = DOT_NODE_FILLCOLOR;

    if (dlist_verify_prev(dlist, ind) != DLIST_OK ||
        dlist_verify_next(dlist, ind) != DLIST_OK)
        fillcolor = DOT_ERROR_NODE_FILLCOLOR;
    else if (dlist_check_ind_reachability(dlist, ind, NULL) != DLIST_OK)
        fillcolor = DOT_UNREACHABLE_NODE_FILLCOLOR;
    else if (ind == 0)
        fillcolor = DOT_SERVICE_NODE_FILLCOLOR;
    else if (node.prev == DLIST_POISON)
        fillcolor = DOT_FREE_NODE_FILLCOLOR;

    fprintf(dot_file, "{ rank=same; node%zi; ind_node%zi; }\n", ind, ind);
    fprintf(dot_file,
            "node%zi[label=\"prev: %zi | value: %zi | next: %zi\", " 
            "fillcolor=\"%s\"];\n",
            ind, node.prev, node.value, node.next, fillcolor);
    fprintf(dot_file, "ind_node%zi[label=\"[%zi]\", fillcolor=\"%s\", fontname=\"monospace bold\"];\n",
            ind, ind, fillcolor);
    
    fprintf(dot_file, "node%zi->ind_node%zi[dir=none];\n", ind, ind); 
}


void change_node_fillcolor(FILE *dot_file, ssize_t ind, const char *fillcolor) {
    assert(dot_file != NULL);
    assert(fillcolor != NULL);

    fprintf(dot_file, "node%zi[fillcolor=\"%s\"];\n", ind, fillcolor);
}

void add_meta_node(FILE *dot_file,
                   ssize_t ind, MetaNode meta_node) {
    assert(dot_file != NULL);

    fprintf(dot_file,
            "{ rank=same; node%zi; %s; }\n"
            "%s[label=\"%s\", "
            "fillcolor=\"%s\", "
            "];\n"
            "%s->node%zi[dir=none];\n",
            ind, meta_node.name, meta_node.name,
            meta_node.label, meta_node.fillcolor,
            meta_node.name, ind);
}

void highlight_head_and_tail(FILE *dot_file, ssize_t head, ssize_t tail) {
    assert(dot_file != NULL);

    if (head == tail) {
        MetaNode meta_node = {
            .name      = "head_tail",
            .label     = "HEAD | TAIL",
            .fillcolor = DOT_HEAD_FILLCOLOR
        };
        add_meta_node(dot_file, head, meta_node);
    } else {
        MetaNode meta_head = {
            .name      = "head",
            .label     = "HEAD",
            .fillcolor = DOT_HEAD_FILLCOLOR
        };
        add_meta_node(dot_file, head, meta_head);

        MetaNode meta_tail = {
            .name      = "tail",
            .label     = "TAIL",
            .fillcolor = DOT_TAIL_FILLCOLOR
        };
        add_meta_node(dot_file, tail, meta_tail);
    }
}

void highlight_free_head(FILE *dot_file, ssize_t free_ind) {
    assert(dot_file != NULL);

    if (free_ind <= 0)
        return;

    MetaNode meta_free = {
        .name = "free_head",
        .label = "FREE",
        .fillcolor = DOT_FREE_HEAD_FILLCOLOR
    };
    add_meta_node(dot_file, free_ind, meta_free);
}

void connect_nodes(FILE *dot_file,
                   ssize_t first, ssize_t second,
                   ArrowStyle style) {
    assert(dot_file != NULL);

    // fprintf(dot_file, "node%zi->node%zi[style=%s, weight=%zi, dir=%s, color=\"%s\"];\n",
    //         first, second, style.style, style.weight, style.dir, style.color);
    
    fprintf(dot_file, "node%zi->node%zi[", first, second);
    if (style.weight > 0)    fprintf(dot_file, "weight=%zu, ", style.weight);
    if (style.style != NULL) fprintf(dot_file, "style=%s, ", style.style);
    if (style.dir != NULL)   fprintf(dot_file, "dir=%s, ", style.dir);
    if (style.color != NULL) fprintf(dot_file, "color=\"%s\", ", style.color);

    fprintf(dot_file, "];\n");
}

void glue_nodes(FILE *dot_file, ssize_t first, ssize_t second) {
    assert(dot_file != NULL);

    ArrowStyle style = {
        .style  = "invis",
        .weight = GLUE_ARROW_WEIGHT,
    };
    connect_nodes(dot_file, first, second, style);
}

void biconnect_nodes(FILE *dot_file, ssize_t first, ssize_t second) {
    assert(dot_file != NULL);

    ArrowStyle style = {
        .style  = BICONNECT_ARROW_STYLE,
        .dir    = "both",
        .color  = BICONNECT_ARROW_COLOR,
    };
    connect_nodes(dot_file, first, second, style);   
}

void connect_nodes_error(FILE *dot_file, ssize_t first, ssize_t second) {
    assert(dot_file != NULL);

    ArrowStyle style = {
        .style  = ERROR_ARROW_STYLE,
        .color  = ERROR_ARROW_COLOR
    };
    connect_nodes(dot_file, first, second, style);   
}

void set_default_style(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("rankdir="     DOT_RANKDIR          ";\n"
          "splines="     DOT_SPLINES          ";\n"
          "nodesep="     DOT_NODESEP          ";\n"
          "edge["
          "weight="      DEFAULT_ARROW_WEIGHT ", "
          "];\n"
          "node["
          "shape="       DOT_NODE_SHAPE       ", "
          "style=\""     DOT_NODE_STYLE       "\", "
          "fillcolor=\"" DOT_NODE_FILLCOLOR   "\", "
          "color=\""     DOT_NODE_COLOR       "\", "
          "fontname=\""  DOT_FONTNAME         "\"];\n"
          , dot_file);
}


void intertwine_nodes(FILE *dot_file, DList *dlist) {
    assert(dot_file != NULL);
    assert(dlist != NULL);

    for (ssize_t i = 0; i < dlist->capacity; i++) {
        ssize_t node_prev = dlist->data[i].prev;
        ssize_t node_next = dlist->data[i].next;

        init_node(dot_file, dlist, i);

        if (i != dlist->capacity - 1)
            glue_nodes(dot_file, i, i + 1);
    
        if (node_prev >= 0 && node_prev < dlist->capacity) {
            if (dlist->data[node_prev].next == i)
                biconnect_nodes(dot_file, i, node_prev);
            else {
                connect_nodes_error(dot_file, i, node_prev);
            }
        }

        if (node_next >= 0 && node_prev != DLIST_POISON && node_next < dlist->capacity)
            if (dlist->data[node_next].prev != i) {
                connect_nodes_error(dot_file, i, node_next);
            }
    }
}

void start_digraph(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("digraph {\n", dot_file);
}

void end_digraph(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("}\n", dot_file);
}
