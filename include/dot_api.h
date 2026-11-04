#include "dlist.h"
#include <stddef.h>

#include "dot_theme.h"

typedef struct {
    const char *style;
    size_t weight;
    const char *dir;
    const char *color;
} ArrowStyle;

typedef struct {
    const char *name;
    const char *label;
    const char *fillcolor;
} MetaNode;

void init_node(FILE *dot_file, DList *, ssize_t);
void highlight_head_and_tail(FILE *dot_file, ssize_t head, ssize_t tail);
void highlight_free_head(FILE *, ssize_t);
void connect_nodes(FILE *dot_file, ssize_t first, ssize_t second, ArrowStyle style);
void glue_nodes(FILE *dot_file, ssize_t first, ssize_t second);
void biconnect_nodes(FILE *dot_file, ssize_t first, ssize_t second);
void connect_nodes_error(FILE *dot_file, ssize_t first, ssize_t second);
void set_default_style(FILE *dot_file);
void intertwine_nodes(FILE *dot_file, DList *dlist);
void start_digraph(FILE *dot_file);
void end_digraph(FILE *dot_file);

