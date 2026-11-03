#include "include/dlist_dump.h"
#include "include/dlist.h"
#include "include/dot_api.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MAX_CMD_LEN 100
#define MAX_IMG_FILENAME_LEN 50
#define DLIST_DOTFILE "/tmp/dlist_dump.dot"

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

DumpStatus gen_log_filename(DList *dlist, const char *ext,
                            char *buff, size_t max_size) {
    ON_DEBUG(
    assert(buff != NULL);
    assert(max_size != 0);

    char time_str[64] = {};
    DumpStatus status = gen_time_str(time_str, "%Y_%m_%d_%H_%M_%S", max_size);
    if (status != DUMP_OK)
        return status;

    int len = snprintf(buff, max_size, "%s/%s_%zu.%s",
                       dlist->_dbg_logdir, time_str,
                       dlist->_dbg_log_counter, ext);
    if (len > (int)max_size)
        return DUMP_DIRNAME_IS_TOO_LONG;
    else if (len < 0)
        return DUMP_CANNOT_WRITE_TO_BUFF;

    dlist->_dbg_log_counter++;

    return DUMP_OK;
    )
}


DListStatus dlist_text_dump(DList *dlist) {
    assert(dlist != NULL);

    for (size_t i = 0; i < dlist->capacity; i++) {
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

DumpStatus dlist_dump_to_html(DList *dlist, DListStatus status, const char *msg) {
    ON_DEBUG(

    assert(dlist != NULL);

    if (ftell(dlist->_dbg_logfile) == 0) 
        fputs("<pre>\n", dlist->_dbg_logfile);
    
    char time_str[64] = {};
    DumpStatus stat = gen_time_str(time_str, "%Y:%m:%d %H:%M:%S", 64);
    fprintf(dlist->_dbg_logfile,
            "<h1>%s | %s</h1>\n"
            "<h2>DList status: \"%s\"</h2>\n",
             time_str, msg, dlist_error_str(status));

    fprintf(dlist->_dbg_logfile,
            "<h3>SIZE: %zu\n"
            "CAPACITY: %zu</h3>\n\n",
            dlist->size, dlist->capacity);

    char img_filename[MAX_IMG_FILENAME_LEN] = {};
    stat = gen_log_filename(dlist, "png",
                                       img_filename, MAX_IMG_FILENAME_LEN);
    if (stat != DUMP_OK)
        return stat;

    stat = dlist_to_img(dlist, DLIST_DOTFILE, img_filename);
    if (stat != DUMP_OK)
        return stat;

    fprintf(dlist->_dbg_logfile,
            "<img src=\"../%s\">\n", img_filename);
    
    return DUMP_OK;
    )
}
