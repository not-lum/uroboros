#include "dlist.h"


#define DOT_FILE "dot_test.dot"

int main() {
    DList dlist = {};
    DListStatus status = dlist_create(&dlist, 10);

    // todo: rename to uroboros
    status = dlist_insert_after(&dlist, 0, 67);
    fprintf(stderr, "dlist added first element\n");
    dlist_dump(&dlist);
    
    status = dlist_insert_after(&dlist, 0, 53);
    fprintf(stderr, "dlist added second element\n");
    dlist_dump(&dlist);

    status = dlist_insert_after(&dlist, 1, 189);
    fprintf(stderr, "dlist added third element\n");
    dlist_dump(&dlist);

    status = dlist_insert_after(&dlist, 1, 29);
    fprintf(stderr, "dlist added element between second and third\n");
    dlist_dump(&dlist);

    status = dlist_delete(&dlist, 2);
    fprintf(stderr, "dlist deleted third element\n");
    dlist_dump(&dlist);

    dlist_to_dot(&dlist, DOT_FILE);

    dlist_destroy(&dlist);
    return 0;
}