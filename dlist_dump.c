#include "dlist.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LEN 50

#define DOT_RANKDIR                   "LR"
#define DOT_NODE_SHAPE                "Mrecord"
#define DOT_NODE_STYLE                "filled"
#define DOT_NODE_FILLCOLOR            "#C0C0FF"
#define DOT_UNUSED_NODE_FILLCOLOR     "#C0C0C0"
#define DOT_NODE_COLOR                "#000000"
#define DOT_FONTNAME                  "monospace"
#define DOT_SPLINES                   "ortho"
#define DOT_NODESEP                    "0.8"

#define DOT_HEAD_FILLCOLOR            "#FFFFC0"
#define DOT_TAIL_FILLCOLOR            "#FFFFC0"

#define GLUE_WEIGHT 100
#define DEFAULT_WEIGHT 5


static void form_cmd(char cmd[MAX_CMD_LEN], const char *filename) {
    assert(cmd != NULL);
    assert(filename != NULL);

    strncat(cmd, "dot ", MAX_CMD_LEN);
    strncat(cmd, filename, MAX_CMD_LEN); // TODO: this is command injection actually!
    strncat(cmd, " -Tpng -o ./a.png", MAX_CMD_LEN);
}

static void init_node(FILE *dot_file, Node *node, size_t ind) {
    assert(dot_file != NULL);
    assert(node != NULL);

    const char *fillcolor = DOT_NODE_FILLCOLOR;
    if (!node->used)
        fillcolor = DOT_UNUSED_NODE_FILLCOLOR;

    fprintf(dot_file, "node%zu[label=\"ind: %zu | prev: %zu | value: %d | next: %zu\", " 
            "fillcolor=\"%s\"];\n",
            ind, ind, node->prev, node->value, node->next, fillcolor);
}


static void highlight_head_and_tail(FILE *dot_file, size_t head, size_t tail) {
    assert(dot_file != NULL);

    fprintf(dot_file, "{ rank=same; head; node%zu; }\n"
                      "{ rank=same; tail; node%zu; }\n", head, tail);
    fputs("head[fillcolor=\"" DOT_HEAD_FILLCOLOR "\"];\n"
          "tail[fillcolor=\"" DOT_TAIL_FILLCOLOR "\"];\n", dot_file);
    fprintf(dot_file, "head->node%zu;\n"
                      "tail->node%zu;\n", head, tail);
}

static void connect_nodes(FILE *dot_file,
                          size_t first, size_t second,
                          ArrowStyle *style) {
    assert(dot_file != NULL);
    assert(style != NULL);

    fprintf(dot_file, "node%zu->node%zu[style=%s, weight=%zu, dir=%s, color=%s];\n",
            first, second, style->style, style->weight, style->dir, style->color);
}

static void glue_nodes(FILE *dot_file, size_t first, size_t second) {
    assert(dot_file != NULL);

    ArrowStyle style = {
        .style = "invis",
        .weight = GLUE_WEIGHT,
        .dir = "forward",
        .color = "white" // TODO: if field is null don't write it
    };
    connect_nodes(dot_file, first, second, &style);
}

static void biconnect_nodes(FILE *dot_file, size_t first, size_t second) {
    assert(dot_file != NULL);

    ArrowStyle style = {
        .style = "filled",
        .weight = DEFAULT_WEIGHT,
        .dir = "both",
        .color = "black",
    };
    connect_nodes(dot_file, first, second, &style);   
}

static void connect_nodes_error(FILE *dot_file, size_t first, size_t second) {
    assert(dot_file != NULL);

    ArrowStyle style = {
        .style = "bold",
        .weight = DEFAULT_WEIGHT,
        .dir = "both",
        .color = "#FFC0C0"
    };
    connect_nodes(dot_file, first, second, &style);   
}

static void set_default_style(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("rankdir="     DOT_RANKDIR        ";\n"
          "splines="     DOT_SPLINES        ";\n"
          "nodesep="     DOT_NODESEP        ";\n"
          "node[shape="  DOT_NODE_SHAPE     ", "
          "style=\""     DOT_NODE_STYLE     "\", "
          "fillcolor=\"" DOT_NODE_FILLCOLOR "\", "
          "color=\""     DOT_NODE_COLOR     "\", "
          "fontname=\""  DOT_FONTNAME       "\"];\n"
          , dot_file);
}


static void intertwine_nodes(FILE *dot_file, DList *dlist) {
    assert(dot_file != NULL);
    assert(dlist != NULL);

    for (size_t i = 0; i < dlist->capacity; i++) {
        size_t node_prev = dlist->data[i].prev;
        size_t node_next = dlist->data[i].next;

        init_node(dot_file, &dlist->data[i], i);

        if (i != dlist->capacity - 1)
            glue_nodes(dot_file, i, i + 1);

        if (node_prev != DLIST_BAD_ELEM) {
            if (dlist->data[node_prev].next == i)
                biconnect_nodes(dot_file, i, node_prev);
            else
                connect_nodes_error(dot_file, i, node_prev);
        }

        // if (node_next != DLIST_BAD_ELEM)
        //     connect_nodes(dot_file, i, node_next, "bold", 5);
    }
}

static void start_digraph(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("digraph {\n", dot_file);
}

static void end_digraph(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("}\n", dot_file);
}

void dlist_to_dot(DList *dlist, const char *filename) {
    assert(dlist != NULL);
    assert(filename != NULL);

    FILE *dot_file = fopen(filename, "w");

    start_digraph(dot_file);
    set_default_style(dot_file);
    intertwine_nodes(dot_file, dlist);
    highlight_head_and_tail(dot_file, dlist->head, dlist->tail);
    end_digraph(dot_file);

    fclose(dot_file);

    char cmd[MAX_CMD_LEN] = {};
    form_cmd(cmd, filename);

    puts(cmd);
    system(cmd);
}
