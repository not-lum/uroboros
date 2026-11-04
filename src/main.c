#include "../include/dlist.h"
#include "../include/dlist_dump.h"


#define DOT_FILE "/tmp/dot_test.dot"
#define OUT_FILE "./logs/img/img.png"
#define LOGDIR "./logs"

int main() {
    DList dlist = {};
    ON_DEBUG(
    char _dbg_log_dir[128] = {};
    gen_dbg_dirname(LOGDIR, _dbg_log_dir, 128); 
    )

    DListStatus status = DLIST_CREATE_DBG(&dlist, 4, _dbg_log_dir);

    // TODO: rename to Uroboros
    status = dlist_insert_after(&dlist, 0, 67, NULL);
    fprintf(stderr, "dlist added first element\n");

    status = dlist_insert_after(&dlist, 0, 53, NULL);
    fprintf(stderr, "dlist added second element\n");

    status = dlist_insert_after(&dlist, 1, 189, NULL);
    fprintf(stderr, "dlist added third element\n");

    status = dlist_insert_after(&dlist, 1, 29, NULL);
    fprintf(stderr, "dlist added element between second and third\n");

    dlist.data[3].prev = -121;

    status = dlist_delete(&dlist, 2);
    fprintf(stderr, "dlist deleted third element\n");


    dlist_destroy(&dlist);
    return 0;
}