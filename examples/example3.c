#include "../include/dlist.h"
#include "../include/dlist_dump.h"

#include <stdlib.h>

#define LOGDIR   "./logs"

int cmp(const void *first, const void *second) {
    if (((Node *)first)->value > ((Node *)second)->value)
        return 1;
    else
        return -1;
}

int main() {
    ON_DEBUG(
    char _dbg_log_dir[128] = {};
    gen_dbg_dirname(LOGDIR, _dbg_log_dir, 128); 
    )
    DList dlist = {};
    DListStatus status = DLIST_CREATE_DBG(&dlist, 5, _dbg_log_dir);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    status = dlist_insert_head(&dlist, 12, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    status = dlist_insert_tail(&dlist, 5, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    ssize_t third_ind = 0;
    status = dlist_insert_after(&dlist, NULL, 3, &third_ind);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    // ON PURPOSE doing this silly qsort (imitating a dump user) to test verification and dump systems
    qsort(dlist.data, 6, sizeof(Node), &cmp);

    dlist_destroy(&dlist);
}
