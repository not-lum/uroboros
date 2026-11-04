#include "dlist.h"

typedef enum {
    DUMP_OK = 0,
    DUMP_CANNOT_STRFTIME,
    DUMP_CANNOT_GET_TIME,
    DUMP_CANNOT_WRITE_TO_BUFF,
    DUMP_CANNOT_OPEN_DUMPFILE,
    DUMP_CANNOT_MAKE_DIR,
    DUMP_CANNOT_OPEN_LOG_FILE,
    DUMP_PATH_IS_TOO_LONG
} DumpStatus;

#define DLIST_DUMP_TO_HTML(dlist, status, msg) dlist_dump_to_html((dlist), (status), __func__, (msg))

DumpStatus gen_dbg_dirname(const char *dirname, char *out_buff, size_t max_size);
DumpStatus init_logs(DList *dlist);
DumpStatus dlist_to_img(DList *dlist, const char *, const char *);
DumpStatus dlist_dump_to_html(DList *dlist, DListStatus status, const char *, const char *);