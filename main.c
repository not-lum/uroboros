#include "include/dlist.h"
#include "include/dlist_dump.h"


#define DOT_FILE "/tmp/dot_test.dot"
#define OUT_FILE "./logs/img/img.png"

int main() {
    DList dlist = {};
    DListStatus status = dlist_create(&dlist, 4
                                      ON_DEBUG(, "./logs"));

    // TODO: rename to Uroboros
    status = dlist_insert_after(&dlist, 0, 67);
    fprintf(stderr, "dlist added first element\n");
    // dlist_dump(&dlist);
    
    status = dlist_insert_after(&dlist, 0, 53);
    fprintf(stderr, "dlist added second element\n");
    // dlist_dump(&dlist);

    status = dlist_insert_after(&dlist, 1, 189);
    fprintf(stderr, "dlist added third element\n");
    // dlist_dump(&dlist);

    status = dlist_insert_after(&dlist, 1, 29);
    fprintf(stderr, "dlist added element between second and third\n");
    // dlist_dump(&dlist);

    status = dlist_delete(&dlist, 2);
    fprintf(stderr, "dlist deleted third element\n");
    // dlist_dump(&dlist);

    // dlist_to_dot(&dlist, DOT_FILE, OUT_FILE);

    dlist_destroy(&dlist);
    return 0;
}