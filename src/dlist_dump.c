#include "../include/dlist_dump.h"
#include "../include/dlist.h"
#include "../include/dot_api.h"
#include <assert.h>
#include <endian.h>
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
#define LOG_IMGDIR "img"

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

DumpStatus unite_paths(char *buff,
                       const char *path1, const char *path2,
                       size_t max_size) {
    assert(buff != NULL);
    assert(path1 != NULL);
    assert(path2 != NULL);
    
    int len = snprintf(buff, max_size, "%s/%s",
                       path1, path2);
    if (len >= (int)max_size || len < 0)
        return DUMP_PATH_IS_TOO_LONG;

    return DUMP_OK;
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

    int len = snprintf(buff, max_size, "%s/img/img_%zu.png",
                       dlist->_dbg_logdir, dlist->_dbg_log_counter);
    if (len >= max_size || len < 0)
        return DUMP_PATH_IS_TOO_LONG;

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
        fprintf(stderr, ".value: %zi\n", dlist->data[i].value);
        fprintf(stderr, ".next: %zu\n", dlist->data[i].next);
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
    highlight_head_and_tail(dot_file, dlist_head(dlist), dlist_tail(dlist));
    highlight_free_head(dot_file, dlist_free_head(dlist));
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

    return unite_paths(out_buff, dirname, time_str, max_size);

    )
}

DumpStatus make_logdir(const char *dirname) {
    assert(dirname != NULL);

    int status = mkdir(dirname, LOGDIR_MODE);
    if (status < 0)
        return DUMP_CANNOT_MAKE_DIR;

    return DUMP_OK;
}

DumpStatus init_logs(DList *dlist) {
    ON_DEBUG(
    assert(dlist != NULL);

    DumpStatus status = make_logdir(dlist->_dbg_logdir);
    if (status != DUMP_OK)
        return status;
    
    char img_logdir[135] = {};
    status = unite_paths(img_logdir, dlist->_dbg_logdir, LOG_IMGDIR, 135);
    if (status != DUMP_OK)
        return status;

    status = make_logdir(img_logdir);
    if (status != DUMP_OK)
        return status;

    char full_log_filename[140] = {}; // TODO: is 140 worth putting in #define?
    status = unite_paths(full_log_filename, dlist->_dbg_logdir, LOG_FILENAME, 140);
    if (status != DUMP_OK)
        return status;

    dlist->_dbg_logfile = fopen(full_log_filename, "w");
    if (dlist->_dbg_logfile == NULL)
        return DUMP_CANNOT_OPEN_LOG_FILE;

    setvbuf(dlist->_dbg_logfile, NULL, _IONBF, 0);
    
    fputs("<pre>\n", dlist->_dbg_logfile);
    
    return DUMP_OK;
    )
}

static DumpStatus dlist_dump_html_header(DList *dlist, const char *funcname, const char *msg) {
    ON_DEBUG(
    assert(dlist != NULL);

    char time_str[64] = {};
    DumpStatus stat = gen_time_str(time_str, "%Y.%m.%d %H:%M:%S", 64);
    if (stat != DUMP_OK)
        return stat;

    fprintf(dlist->_dbg_logfile,
            "<h1>%s | Called from <span style=\"color: blue;\">'%s'</span></h1>\n"
            "<hr>\n"
            "<h2>Message: <i>\"%s\"</i></h2>\n", time_str, funcname, msg);
    
    return DUMP_OK;
    )
}

static void dlist_dump_html_status(DList *dlist, DListStatus status) {
    ON_DEBUG(

    assert(dlist != NULL);

    fprintf(dlist->_dbg_logfile, "<h2>DList status: ");

    const char *color = "green";
    if (status != DLIST_OK)
        color = "red";

    fprintf(dlist->_dbg_logfile,
            "<span style=\"color: %s;\">\"%s\"</span></h2>\n",
            color, dlist_error_str(status));

    )
}

static void dlist_dump_html_fields(DList *dlist, DListStatus status) {
    ON_DEBUG(

    assert(dlist != NULL);

    const char *error_style = " style=\"color: red;\"";

    const char *capacity_style = "";
    const char *size_style = "";
    const char *free_head_style = "";

    if (status == DLIST_SIZE_LESS_THAN_ZERO ||
        status == DLIST_SIZE_BIGGER_THAN_CAPACITY)
        size_style = error_style;
    if (status == DLIST_CAPACITY_LESS_THAN_ZERO)
        capacity_style = error_style;
    if (status == DLIST_FREE_HEAD_LESS_THAN_ZERO)
        free_head_style = error_style;

    fprintf(dlist->_dbg_logfile,
            "<h2>DList fields dump:</h2>\n"
            "<table border=\"1\" cellpadding=\"5\" style=\"border-collapse: collapse;\">\n"
            "<tr><th>SIZE</th><th%s>%zi</th></tr>\n"
            "<tr><th>CAPACITY</th><th%s>%zi</th></tr>\n"
            "<tr><th>FREE_HEAD</th><th%s>%zi</th></tr>\n"
            "</table>\n"
            "<br>\n",
            size_style, dlist->size,
            capacity_style, dlist->capacity,
            free_head_style, dlist->free_head);
    )
}

DumpStatus dlist_dump_to_html(DList *dlist, DListStatus status,
                              const char *funcname, const char *msg) {
    ON_DEBUG(

    assert(dlist != NULL);
    assert(funcname != NULL);
    assert(msg != NULL);
    
    DumpStatus stat = dlist_dump_html_header(dlist, funcname, msg);
    if (stat != DUMP_OK)
        return stat;

    dlist_dump_html_status(dlist, status);

    dlist_dump_html_fields(dlist, status);

    char img_filename[128] = {};
    stat = gen_img_filename(dlist, img_filename, 128);
    if (stat != DUMP_OK)
        return stat;

    if (status == DLIST_DATA_PTR_IS_NULL)
        return DUMP_OK;

    stat = dlist_to_img(dlist, DLIST_DOTFILE, img_filename);
    if (stat != DUMP_OK)
        return stat;

    // getc(stdin);

    fprintf(dlist->_dbg_logfile,
            "<img src=\"../../%s\">\n"
            "<br>\n"
            "<hr>\n\n",
            img_filename);

    return DUMP_OK;
    )
}
