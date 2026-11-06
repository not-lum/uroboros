#include "../include/dlist.h"
#include "../include/dlist_dump.h"

#include <stdlib.h>

#define LOGDIR   "./logs"

int main() {
    ON_DEBUG(
    char _dbg_log_dir[128] = {};
    gen_dbg_dirname(LOGDIR, _dbg_log_dir, 128); 
    )

    DList dlist = {};
    DListStatus status = DLIST_CREATE_DBG(&dlist, 2, _dbg_log_dir);

    for (ssize_t i = 0; i < 5; i++) {
        status = dlist_insert_tail(&dlist, i, NULL);
        if (status != DLIST_OK)
            fprintf(stderr, "Error: %s\n", dlist_error_str(status));
    }
    
    for (ssize_t i = 0; i < 5; i++) {
        status = dlist_delete_tail(&dlist);
        if (status != DLIST_OK)
           fprintf(stderr, "Error: %s\n", dlist_error_str(status));
    }

    dlist_destroy(&dlist);
}
