#include "../include/dlist_dump.h"
#include "../include/dlist.h"
#include "../include/dot_api.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>


#define MAX_CMD_LEN 100
#define MAX_IMG_FILENAME_LEN 50
#define DLIST_DOTFILE "/tmp/dlist_dump.dot"
#define LOGDIR_MODE 0755
#define LOG_FILENAME "log.html"

static bool form_cmd(char cmd[MAX_CMD_LEN],
                     const char *inp_filename,
                     const char *out_filename) {
    assert(cmd != NULL);
    assert(inp_filename != NULL);
    assert(out_filename != NULL);

    int len = snprintf(cmd, MAX_CMD_LEN, "dot %s -Tpng -o %s",
                       inp_filename, out_filename);

    if (len < 0 || len >= MAX_CMD_LEN) {
        cmd[0] = '\0';

        return false;
    }

    return true;
}


DumpStatus gen_time_str(char *buff, const char *fmt, size_t max_size) {
    assert(buff != NULL);
    assert(fmt != NULL);
    assert(max_size > 0);

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    if (tm == NULL)
        return DUMP_CANNOT_GET_TIME;

    if (strftime(buff, max_size, fmt, tm) == 0)
        return DUMP_CANNOT_STRFTIME;

    return DUMP_OK;
}

DumpStatus gen_img_filename(DList *dlist, char *buff, int max_size) {
    ON_DEBUG(

    assert(dlist != NULL);
    assert(buff != NULL);
    assert(max_size > 0);

    int len = snprintf(buff, max_size, "%s/img_%zu.png",
                       dlist->_dbg_logdir, dlist->_dbg_log_counter);
    if (len > max_size || len < 0)
        return DUMP_DIRNAME_IS_TOO_LONG;

    dlist->_dbg_log_counter++;

    return DUMP_OK;

    )
} 


DListStatus dlist_text_dump(DList *dlist) {
    assert(dlist != NULL);

    for (ssize_t i = 0; i < dlist->capacity; i++) {
        fprintf(stderr, "\n-------------\n");
        fprintf(stderr, "---[%zu]---\n", i);
        fprintf(stderr, ".prev: %zu\n", dlist->data[i].prev);
        fprintf(stderr, ".value: %d\n", dlist->data[i].value);
        fprintf(stderr, ".next: %zu\n", dlist->data[i].next);
        fprintf(stderr, ".used: %d\n", dlist->data[i].used);
    }

    return DLIST_OK;
}


DumpStatus dlist_to_img(DList *dlist,
                        const char *inp_filename,
                        const char *out_filename) {
    assert(dlist != NULL);
    assert(inp_filename != NULL);
    assert(out_filename != NULL);

    FILE *dot_file = fopen(inp_filename, "w");
    if (dot_file == NULL)
        return DUMP_CANNOT_OPEN_DUMPFILE;

    start_digraph(dot_file);
    set_default_style(dot_file);
    intertwine_nodes(dot_file, dlist);
    highlight_head_and_tail(dot_file, dlist->head, dlist->tail);
    end_digraph(dot_file);

    fclose(dot_file);

    char cmd[MAX_CMD_LEN] = {};
    if (form_cmd(cmd, inp_filename, out_filename)) {
        puts(cmd);
        system(cmd);
    }

    return DUMP_OK;
}

DumpStatus gen_dbg_dirname(const char *dirname, char *out_buff, size_t max_size) {
    ON_DEBUG(
    assert(dirname != NULL);
    assert(out_buff != NULL);
    assert(max_size != 0);

    char time_str[64] = {};
    DumpStatus status = gen_time_str(time_str, "%Y_%m_%d_%H_%M_%S", 64);
    if (status != DUMP_OK)
        return status;

    int len = snprintf(out_buff, max_size, "%s/%s",
                       dirname, time_str);
    if (len > 128 || len < 0)
        return DUMP_DIRNAME_IS_TOO_LONG;

    return DUMP_OK;

    )
}

DumpStatus init_logs(DList *dlist) {
    ON_DEBUG(
    assert(dlist != NULL);

    DumpStatus status = mkdir(dlist->_dbg_logdir, LOGDIR_MODE);
    if (status < 0)
        return DUMP_CANNOT_MAKE_DIR;

    char full_log_filename[140] = {};
    int len = snprintf(full_log_filename, 140, "%s/%s",
                       dlist->_dbg_logdir, LOG_FILENAME);
    if (len > 140 || len < 0)
        return DUMP_DIRNAME_IS_TOO_LONG;

    dlist->_dbg_logfile = fopen(full_log_filename, "w");
    if (dlist->_dbg_logfile == NULL)
        return DUMP_CANNOT_OPEN_LOG_FILE;

    setvbuf(dlist->_dbg_logfile, NULL, _IONBF, 0);
    
    fputs("<pre>\n", dlist->_dbg_logfile);
    
    return DUMP_OK;
    )
}

DumpStatus dlist_dump_to_html(DList *dlist, DListStatus status,
                              const char *funcname, const char *msg) {
    ON_DEBUG(

    assert(dlist != NULL);
    assert(msg != NULL);
    
    char time_str[64] = {};
    DumpStatus stat = gen_time_str(time_str, "%Y:%m:%d %H:%M:%S", 64);
    if (stat != DUMP_OK)
        return stat;

    fprintf(dlist->_dbg_logfile,
            "<h1>%s | Called from '%s'</h1>\n"
            "<h2>%s</h2>"
            "<h2>DList status: \"%s\"</h2>\n",
             time_str, funcname, msg, dlist_error_str(status));

    fprintf(dlist->_dbg_logfile,
            "<h3>SIZE: %zu\n"
            "CAPACITY: %zu</h3>\n\n",
            dlist->size, dlist->capacity);

    char img_filename[128] = {};
    stat = gen_img_filename(dlist, img_filename, 128);
    if (stat != DUMP_OK)
        return stat;

    fprintf(stderr, "img_filename: %s\n", img_filename);

    stat = dlist_to_img(dlist, DLIST_DOTFILE, img_filename);
    if (stat != DUMP_OK)
        return stat;

    fprintf(dlist->_dbg_logfile,
            "<img src=\"../../%s\">\n",
            img_filename);
    
    return DUMP_OK;
    )
}
