#include "dlist.h"
#include <stddef.h>


typedef struct {
    const char *style;
    size_t weight;
    const char *dir;
    const char *color;
} ArrowStyle;

void init_node(FILE *dot_file, Node *node, size_t ind);
void highlight_head_and_tail(FILE *dot_file, size_t head, size_t tail);
void connect_nodes(FILE *dot_file, size_t first, size_t second, ArrowStyle *style);
void glue_nodes(FILE *dot_file, size_t first, size_t second);
void biconnect_nodes(FILE *dot_file, size_t first, size_t second);
void connect_nodes_error(FILE *dot_file, size_t first, size_t second);
void set_default_style(FILE *dot_file);
void intertwine_nodes(FILE *dot_file, DList *dlist);
void start_digraph(FILE *dot_file);
void end_digraph(FILE *dot_file);

