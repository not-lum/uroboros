#include "dlist.h"

typedef enum {
    DUMP_OK = 0,
    DUMP_CANNOT_STRFTIME,
    DUMP_CANNOT_GET_TIME,
    DUMP_DIRNAME_IS_TOO_LONG,
    DUMP_CANNOT_WRITE_TO_BUFF,
    DUMP_CANNOT_OPEN_DUMPFILE
} DumpStatus;

DumpStatus gen_log_filename(DList *dlist, const char *ext,
                            char *buff, size_t max_size);
DumpStatus dlist_to_img(DList *dlist, const char *, const char *);
DumpStatus dlist_dump_to_html(DList *dlist, DListStatus status, const char *msg);