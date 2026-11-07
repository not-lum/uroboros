#include "../include/dlist.h"
#include "../include/dlist_dump.h"


#define LOGDIR "./logs"

int main() {
    ON_DEBUG(
    char _dbg_log_dir[128] = {};
    gen_dbg_dirname(LOGDIR, _dbg_log_dir, 128); 
    )

    DList dlist = {};
    DListStatus status = DLIST_CREATE_DBG(&dlist, 4, _dbg_log_dir);

    status = dlist_insert_tail(&dlist, 67, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    status = dlist_insert_tail(&dlist, 53, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    status = dlist_insert_tail(&dlist, 189, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    status = dlist_insert_tail(&dlist, 29, NULL);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    // spoiling data ON PURPOSE to test verification and dump systems
    dlist.data[3].prev = -121;

    status = dlist_delete(&dlist, 2);
    if (status != DLIST_OK)
        fprintf(stderr, "DList error: %s\n", dlist_error_str(status));

    dlist_destroy(&dlist);
    return 0;
}