#include "../include/dlist.h"
#include "../include/dlist_dump.h"


#define LOGDIR   "./logs"

int main() {
    ON_DEBUG(
    char _dbg_log_dir[128] = {};
    gen_dbg_dirname(LOGDIR, _dbg_log_dir, 128); 
    )

    DList dlist = {};
    DListStatus status = DLIST_CREATE_DBG(&dlist, 4, _dbg_log_dir);

    status = dlist_insert_head(&dlist, 88, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    status = dlist_insert_head(&dlist, 77, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    status = dlist_insert_head(&dlist, 66, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    status = dlist_insert_head(&dlist, 55, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    // ON PURPOSE creating two cycles to test verificator
    dlist.data[0] = (Node){.next = 1, .prev = 2, .value = 67};
    dlist.data[1] = (Node){.next = 2, .prev = 0, .value = 67};
    dlist.data[2] = (Node){.next = 0, .prev = 1, .value = 67};
    dlist.data[3] = (Node){.next = 4, .prev = 4, .value = 76};
    dlist.data[4] = (Node){.next = 3, .prev = 3, .value = 76};

    dlist_destroy(&dlist);
    return 0;
}