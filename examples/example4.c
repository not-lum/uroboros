#include "../include/dlist.h"
#include "../include/dlist_dump.h"

#include <stdlib.h>

#define LOGDIR   "./logs"

int main() {
    DList dlist = {};
    ON_DEBUG(
    char _dbg_log_dir[128] = {};
    gen_dbg_dirname(LOGDIR, _dbg_log_dir, 128); 
    )

    DListStatus status = DLIST_CREATE_DBG(&dlist, 1, _dbg_log_dir);
    status = dlist_insert_after(&dlist, 0, 5, NULL);    
    status = dlist_insert_after(&dlist, 1, 6, NULL);    
    status = dlist_insert_after(&dlist, 1, 7, NULL);    
    status = dlist_insert_after(&dlist, 1, 8, NULL);    
    status = dlist_insert_after(&dlist, 1, 9, NULL);
    status = dlist_insert_before(&dlist, 1, 9, NULL);
    status = dlist_insert_before(&dlist, 3, 9, NULL);
    status = dlist_delete_head(&dlist);
    status = dlist_delete_head(&dlist);
    status = dlist_delete_head(&dlist);
    status = dlist_delete_head(&dlist);
    status = dlist_delete_head(&dlist);
    status = dlist_delete_head(&dlist);
    status = dlist_delete_head(&dlist);
    status = dlist_delete_head(&dlist);
    status = dlist_delete_head(&dlist);
    status = dlist_delete_head(&dlist);

    dlist_destroy(&dlist);
}
