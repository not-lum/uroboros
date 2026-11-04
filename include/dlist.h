#pragma once

#include <stdio.h>
#include <stdbool.h>

#define DLIST_DEBUG

#ifdef DLIST_DEBUG
#   define ON_DEBUG(...) __VA_ARGS__
#   define DLIST_CREATE_DBG(dlist, capacity, logdir) dlist_create((dlist), (capacity), (logdir));
#else
#   define ON_DEBUG(...)
#   define DLIST_CREATE_DBG(dlist, capacity, logdir) dlist_create((dlist), (capacity));
#endif

#define DLIST_BAD_ELEM (ssize_t)-1
typedef ssize_t dlist_elem_t;

typedef struct {
    ssize_t prev;
    dlist_elem_t value;
    ssize_t next;
} Node;

typedef struct {
    Node *data;
    ssize_t size;
    ssize_t capacity;
    ssize_t free_head;

    ON_DEBUG(
    const char *_dbg_logdir;
    FILE *_dbg_logfile;
    size_t _dbg_log_counter;
    )
} DList;

typedef enum {
    DLIST_OK = 0,
    DLIST_IS_FULL,
    DLIST_INDEX_ERROR,
    DLIST_PREV_BIGGER_THAN_CAPACITY,
    DLIST_NEXT_BIGGER_THAN_CAPACITY,
    DLIST_PTR_IS_NULL,
    DLIST_DATA_PTR_IS_NULL,
    DLIST_SIZE_BIGGER_THAN_CAPACITY,
    DLIST_PREV_ELEMENT_DOESNT_LINK_BACK,
    DLIST_NEXT_ELEMENT_DOESNT_LINK_BACK,
    DLIST_CANNOT_OPEN_DBG_LOG_FILE,
    DLIST_CANNOT_DELETE_UNUSED,
    DLIST_CANNOT_INIT_LOGS,
    DLIST_ATTEMPTED_TO_DELETE_SERVICE_ELEM,
    DLIST_SIZE_LESS_THAN_ZERO,
    DLIST_CAPACITY_LESS_THAN_ZERO,
    DLIST_PREV_BAD_VALUE,
    DLIST_NEXT_BAD_VALUE,
    DLIST_FREE_HEAD_LESS_THAN_ZERO,
    DLIST_ALLOC_FAIL,
    DLIST_INSERT_AFTER_FREE_ELEM_ATTEMPT,
    DLIST_UNREACHABLE_ELEM_DETECTED,
    DLIST_INVALID_CONNECTIVITY
} DListStatus;

typedef enum {
    NEXT,
    PREV,
} DListField;

typedef struct {
    DListStatus status;
    ssize_t node;
    ssize_t related;
    DListField field;
} DListErrorInfo;

const char *dlist_error_str(DListStatus status);
ssize_t dlist_free_head(DList *);
ssize_t dlist_head(DList *);
ssize_t dlist_tail(DList *);
DListStatus dlist_dump(DList *dlist);
DListStatus dlist_create(DList *dlist, ssize_t capacity
                         ON_DEBUG(, const char *));
DListStatus dlist_destroy(DList *dlist);

DListStatus dlist_get(DList *, ssize_t, ssize_t *);
DListStatus dlist_insert_tail(DList *, ssize_t, ssize_t *);
DListStatus dlist_insert_head(DList *, ssize_t, ssize_t *);
DListStatus dlist_insert_after(DList *dlist, ssize_t ind, dlist_elem_t value, ssize_t *);
DListStatus dlist_delete(DList *dlist, ssize_t ind);

DListStatus dlist_verify(DList *);
DListStatus dlist_verify_reachability(DList *);
DListStatus dlist_check_ind_reachability(DList *, ssize_t, ssize_t *);
DListStatus dlist_verify_fields(DList *);
DListStatus dlist_verify_prev(DList *, ssize_t);
DListStatus dlist_verify_next(DList *, ssize_t);
