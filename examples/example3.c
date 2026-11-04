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
    DList dlist = {};
    ON_DEBUG(
    char _dbg_log_dir[128] = {};
    gen_dbg_dirname(LOGDIR, _dbg_log_dir, 128); 
    )

    DListStatus status = DLIST_CREATE_DBG(&dlist, 5, _dbg_log_dir);
    ssize_t first_ind = 0;
    status = dlist_insert_head(&dlist, 12, &first_ind);

    ssize_t second_ind = 0;
    status = dlist_insert_after(&dlist, first_ind, 5, &second_ind);

    ssize_t third_ind = 0;
    status = dlist_insert_after(&dlist, second_ind, 3, &third_ind);

    qsort(dlist.data, 6, sizeof(Node), &cmp);

    dlist_destroy(&dlist);
}
