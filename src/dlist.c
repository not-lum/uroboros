#include "../include/dlist.h"
#include "../include/dlist_dump.h"

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


#define ASSERT_OK(dlist, const_msg) do { \
    DListStatus status = dlist_verify((dlist)); \
    /* TODO: add error handling of dlist_dump_to_html */ \
    DLIST_DUMP_TO_HTML((dlist), (status), "Internal call: " const_msg " func body"); \
    assert(status == DLIST_OK);  \
} while(0)

#define ASSERT_OK_BEFORE(dlist) do { ASSERT_OK(dlist, "BEFORE"); } while (0)
#define ASSERT_OK_AFTER(dlist) do { ASSERT_OK(dlist, "AFTER"); } while (0)

const char *dlist_error_str(DListStatus status) {
    switch (status) {
        case DLIST_OK:
            return "dlist ok";
        case DLIST_IS_FULL:
            return "dlist is full";
        case DLIST_INDEX_ERROR:
            return "dlist index error";
        case DLIST_PREV_BIGGER_THAN_CAPACITY:
            return "dlist 'prev' field is bigger than dlist capacity";
        case DLIST_NEXT_BIGGER_THAN_CAPACITY:
            return "dlist 'next' field is bigger than dlist capacity";
        case DLIST_PTR_IS_NULL:
            return "pointer to dlist is NULL";
        case DLIST_DATA_PTR_IS_NULL:
            return "dlist data pointer is NULL";
        case DLIST_PREV_ELEMENT_DOESNT_LINK_BACK:
            return "previous element doesn't link back";
        case DLIST_NEXT_ELEMENT_DOESNT_LINK_BACK:
            return "next element doesn't link back";
        case DLIST_SIZE_BIGGER_THAN_CAPACITY:
            return "dlist size is bigger than its capacity";
        case DLIST_CANNOT_OPEN_DBG_LOG_FILE:
            return "cannot open debug log file";
        case DLIST_CANNOT_DELETE_UNUSED:
            return "attempted to delete unused element";
        case DLIST_CANNOT_INIT_LOGS:
            return "cannot init logging system";
        case DLIST_ATTEMPTED_TO_DELETE_SERVICE_ELEM:
            return "attempted to delete service element";
        case DLIST_SIZE_LESS_THAN_ZERO:
            return "dlist size is less than zero";
        case DLIST_CAPACITY_LESS_THAN_ZERO:
            return "dlist capacity is less than zero";
        case DLIST_PREV_BAD_VALUE:
            return "element prev value is negative, but not DLIST_BAD_ELEM";
        case DLIST_NEXT_BAD_VALUE:
            return "element next value is negative, but not DLIST_BAD_ELEM";
        case DLIST_FREE_HEAD_LESS_THAN_ZERO:
            return "free_head is less than zero";
        case DLIST_ALLOC_FAIL:
            return "allocation failed";
        case DLIST_INSERT_AFTER_FREE_ELEM_ATTEMPT:
            return "attempted to insert after free element";
        case DLIST_INVALID_CONNECTIVITY:
            return "invalid nodes connectivity";
        case DLIST_UNREACHABLE_ELEM_DETECTED:
            return "unreachable element detected";
        default:
            return "unknown dlist error";

    }
}

static void dlist_connect_free_elems(DList *dlist) {
    assert(dlist != NULL);

    for (ssize_t i = 1; i < dlist->capacity; i++) {
        dlist->data[i].prev = DLIST_BAD_ELEM;
        if (i != dlist->capacity - 1)
            dlist->data[i].next = i + 1;
        else
            dlist->data[i].next = 0;
    }
}

ssize_t dlist_free_head(DList *dlist) {
    assert(dlist != NULL);

    return dlist->free_head;
}

ssize_t dlist_head(DList *dlist) {
    assert(dlist != NULL);

    return dlist->data[0].next;
}

ssize_t dlist_tail(DList *dlist) {
    assert(dlist != NULL);

    return dlist->data[0].prev;
}

DListStatus dlist_create(DList *dlist, ssize_t capacity
                         ON_DEBUG(, const char *_dbg_logdir)) {
    assert(dlist != NULL);
    assert(capacity > 0); // TODO: maybe removed in future

    ON_DEBUG(
    assert(_dbg_logdir != NULL);
    )

    dlist->data = calloc(capacity + 1, sizeof(Node));
    if (dlist->data == NULL)
        return DLIST_ALLOC_FAIL;

    dlist->capacity = capacity + 1;

    dlist->free_head = 1;
    dlist_connect_free_elems(dlist);

    ON_DEBUG(
    dlist->_dbg_logdir = _dbg_logdir;

    if (init_logs(dlist) != DUMP_OK)
        return DLIST_CANNOT_INIT_LOGS;
    )

    ASSERT_OK_AFTER(dlist);

    return DLIST_OK;
}


DListStatus dlist_destroy(DList *dlist) {
    assert(dlist != NULL);

    ASSERT_OK_BEFORE(dlist);

    ON_DEBUG(
    fclose(dlist->_dbg_logfile);
    )

    free(dlist->data);
    *dlist = (DList){};

    return DLIST_OK;
}

static ssize_t take_from_free(DList *dlist) {
    assert(dlist != NULL);

    ssize_t res = dlist->free_head;
    if (res > 0)
        dlist->free_head = dlist->data[res].next;

    return res;
}

DListStatus dlist_insert_after(DList *dlist, ssize_t ind, dlist_elem_t value,
                               ssize_t *out_ind) {
    assert(dlist != NULL);
    assert(ind < dlist->capacity);

    ASSERT_OK_BEFORE(dlist);

    if (dlist->data[ind].prev == DLIST_BAD_ELEM)
        return DLIST_INSERT_AFTER_FREE_ELEM_ATTEMPT;
    if (dlist->size == 0 && ind != 0)
        return DLIST_INDEX_ERROR;

    ssize_t new_ind = take_from_free(dlist);
    if (new_ind == 0) // TODO: add realloc
        return DLIST_IS_FULL;

    ssize_t left = dlist->data[ind].prev;
    ssize_t right = dlist->data[ind].next;

    dlist->data[right].prev = new_ind;
    dlist->data[ind].next = new_ind;

    dlist->data[new_ind] = (Node){
        .prev = ind,
        .value = value,
        .next = right
    };

    dlist->size++;

    if (out_ind != NULL)
        *out_ind = new_ind;

    ASSERT_OK_AFTER(dlist);

    return DLIST_OK;
}

DListStatus dlist_insert_before(DList *dlist,
                                ssize_t ind, ssize_t value,
                                ssize_t *out_ind) {
    assert(dlist != NULL);

    return dlist_insert_after(dlist, dlist->data[ind].prev, value, out_ind);
}


DListStatus dlist_insert_head(DList *dlist, ssize_t value, ssize_t *out_ind) {
    assert(dlist != NULL);

    return dlist_insert_after(dlist, 0, value, out_ind);
}

DListStatus dlist_insert_tail(DList *dlist, ssize_t value, ssize_t *out_ind) {
    assert(dlist != NULL);

    return dlist_insert_after(dlist, dlist_tail(dlist), value, out_ind);
}

DListStatus dlist_get(DList *dlist, ssize_t ind, ssize_t *out_value) {
    assert(dlist != NULL);
    assert(out_value != NULL);

    ASSERT_OK_BEFORE(dlist);

    *out_value = dlist->data[ind].value;

    return DLIST_OK;
}

DListStatus dlist_delete(DList *dlist, ssize_t ind) {
    assert(dlist != NULL);

    ASSERT_OK_BEFORE(dlist);

    if (ind == 0)
        return DLIST_ATTEMPTED_TO_DELETE_SERVICE_ELEM;

    ssize_t left = dlist->data[ind].prev;
    ssize_t right = dlist->data[ind].next;

    dlist->data[left].next = right;
    dlist->data[right].prev = left;

    dlist->data[ind].prev = DLIST_BAD_ELEM;
    dlist->data[ind].next = dlist->free_head;
    dlist->data[ind].value = DLIST_BAD_ELEM;

    dlist->free_head = ind;
    dlist->size--;

    ASSERT_OK_AFTER(dlist);

    return DLIST_OK;
}

