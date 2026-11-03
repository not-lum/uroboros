#include "include/dot_api.h"
#include "include/dlist.h"
#include "include/dot_theme.h"
#include "include/dlist.h"

#include <stdio.h>
#include <assert.h>


void init_node(FILE *dot_file, Node *node, size_t ind) {
    assert(dot_file != NULL);
    assert(node != NULL);

    const char *fillcolor = DOT_NODE_FILLCOLOR;
    if (!node->used)
        fillcolor = DOT_UNUSED_NODE_FILLCOLOR;

    fprintf(dot_file, "{ rank=same; node%zi; ind_node%zi; }", ind, ind);
    fprintf(dot_file,
            "node%zi[label=\"prev: %zi | value: %d | next: %zi\", " 
            "fillcolor=\"%s\"];\n",
            ind, node->prev, node->value, node->next, fillcolor);
    fprintf(dot_file, "ind_node%zi[label=\"[%zi]\"];\n", ind, ind);
    
    // TODO: kinda want to do this via connect_nodes
    fprintf(dot_file, "node%zi->ind_node%zi[dir=none];\n", ind, ind); 
}


void highlight_head_and_tail(FILE *dot_file, size_t head, size_t tail) {
    assert(dot_file != NULL);

    // TODO: think about copypaste here

    if (head != DLIST_BAD_ELEM) {
        // fprintf(dot_file, "{ rank=same; node%zi; head; }\n", head);
        // fputs("head[fillcolor=\"" DOT_HEAD_FILLCOLOR "\"];\n", dot_file);
        // fprintf(dot_file, "head->node%zi;\n", head);
        fprintf(dot_file,
                "ind_node%zi[label=\"[%zi]\\nHEAD\", "
                "fillcolor=\""DOT_HEAD_FILLCOLOR "\"];\n",
                head, head);
    }

    if (tail != DLIST_BAD_ELEM) {
        fprintf(dot_file,
                "ind_node%zi[label=\"[%zi]\\nTAIL\", "
                "fillcolor=\""DOT_TAIL_FILLCOLOR "\"];\n",
                tail, tail);

        // fprintf(dot_file, "{ rank=same; node%zi; tail; }\n", tail);
        // fputs("tail[fillcolor=\"" DOT_TAIL_FILLCOLOR "\"];\n", dot_file);
        // fprintf(dot_file, "tail->node%zi;\n", tail);
    }
}

void connect_nodes(FILE *dot_file,
                   size_t first, size_t second,
                   ArrowStyle *style) {
    assert(dot_file != NULL);
    assert(style != NULL);

    fprintf(dot_file, "node%zi->node%zi[style=%s, weight=%zi, dir=%s, color=%s];\n",
            first, second, style->style, style->weight, style->dir, style->color);
}

void glue_nodes(FILE *dot_file, size_t first, size_t second) {
    assert(dot_file != NULL);

    ArrowStyle style = {
        .style = "invis",
        .weight = GLUE_ARROW_WEIGHT,
        .dir = "forward",
        .color = "white" // TODO: if field is null don't write it
    };
    connect_nodes(dot_file, first, second, &style);
}

void biconnect_nodes(FILE *dot_file, size_t first, size_t second) {
    assert(dot_file != NULL);

    ArrowStyle style = {
        .style = "filled",
        .weight = DEFAULT_ARROW_WEIGHT,
        .dir = "both",
        .color = "black",
    };
    connect_nodes(dot_file, first, second, &style);   
}

void connect_nodes_error(FILE *dot_file, size_t first, size_t second) {
    assert(dot_file != NULL);

    ArrowStyle style = {
        .style = "bold",
        .weight = DEFAULT_ARROW_WEIGHT,
        .dir = "both",
        .color = "#FFC0C0"
    };
    connect_nodes(dot_file, first, second, &style);   
}

void set_default_style(FILE *dot_file) {
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


void intertwine_nodes(FILE *dot_file, DList *dlist) {
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

void start_digraph(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("digraph {\n", dot_file);
}

void end_digraph(FILE *dot_file) {
    assert(dot_file != NULL);

    fputs("}\n", dot_file);
}
