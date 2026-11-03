#include "../include/dlist.h"
#include "../include/dlist_dump.h"

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

ON_DEBUG(
#define ASSERT_OK(dlist) do { \
    DListStatus status = dlist_verify((dlist)); \
    dlist_dump_to_html((dlist), status, __func__, "Internal call"); \
    assert(status == DLIST_OK);  \
} while(0)
)

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
        default:
            return "unknown dlist error";

    }
}

DListStatus dlist_create(DList *dlist, ssize_t capacity
                         ON_DEBUG(, const char *_dbg_logdir)) {
    assert(dlist != NULL);

    dlist->data = calloc(capacity, sizeof(Node));
    dlist->head = DLIST_BAD_ELEM;
    dlist->tail = DLIST_BAD_ELEM;
    dlist->size = 0;
    dlist->capacity = capacity;
    
    for (ssize_t i = 0; i < capacity; i++) {
        dlist->data[i].prev = DLIST_BAD_ELEM;
        dlist->data[i].next = DLIST_BAD_ELEM;
    }

    ON_DEBUG(
    dlist->_dbg_logdir = _dbg_logdir;

    if (init_logs(dlist) != DUMP_OK)
        return DLIST_CANNOT_INIT_LOGS;
    )

    ASSERT_OK(dlist);

    return DLIST_OK;
}

static DListStatus dlist_verify_linking(DList *dlist) {
    for (ssize_t i = 0; i < dlist->capacity; i++) {
        if (!dlist->data[i].used)
            continue;

        ssize_t node_prev = dlist->data[i].prev;
        ssize_t node_next = dlist->data[i].next;
    
        if (node_prev != DLIST_BAD_ELEM) {
            if (node_prev >= dlist->capacity)
                return DLIST_PREV_BIGGER_THAN_CAPACITY;

            if (dlist->data[node_prev].next != i)
                return DLIST_PREV_ELEMENT_DOESNT_LINK_BACK; 
        }

        if (node_next != DLIST_BAD_ELEM) {
            if (node_next >= dlist->capacity)
                return DLIST_NEXT_BIGGER_THAN_CAPACITY;

            if (dlist->data[node_next].prev != i)
                return DLIST_NEXT_ELEMENT_DOESNT_LINK_BACK; 
        }
    }

    return DLIST_OK;
}

DListStatus dlist_verify(DList *dlist) {
    // TODO: add more checks
    if (dlist == NULL)
        return DLIST_PTR_IS_NULL;

    if (dlist->data == NULL)
        return DLIST_DATA_PTR_IS_NULL;

    if (dlist->size > dlist->capacity)
        return DLIST_SIZE_BIGGER_THAN_CAPACITY;

    DListStatus status = dlist_verify_linking(dlist);
    if (status != DLIST_OK)
        return status;

    return DLIST_OK;
}


DListStatus dlist_destroy(DList *dlist) {
    assert(dlist != NULL);

    ASSERT_OK(dlist);

    dlist->head = 0;
    free(dlist->data);
    dlist->tail = 0;

    return DLIST_OK;
}

static ssize_t dlist_find_free(DList *dlist) {
    assert(dlist != NULL);

    for (ssize_t i = 0; i < dlist->capacity; i++) {
        if (!dlist->data[i].used) {
            return i;
        }
    }

    return DLIST_BAD_ELEM;
}


DListStatus dlist_insert_after(DList *dlist, ssize_t ind, dlist_elem_t value,
                               ssize_t *real_ind) {
    assert(dlist != NULL);
    assert(ind < dlist->capacity);

    ASSERT_OK(dlist);

    if (dlist->size == 0 && ind != 0)
        return DLIST_INDEX_ERROR;

    ssize_t new_ind = dlist_find_free(dlist);
    if (ind == DLIST_BAD_ELEM)
        return DLIST_IS_FULL;

    if (dlist->size == 0) {
        dlist->data[new_ind] = (Node){
            .prev = DLIST_BAD_ELEM,
            .next = DLIST_BAD_ELEM,
            .value = value,
            .used = true
        };
        dlist->head = new_ind;
        dlist->tail = new_ind;

        dlist->size++; // TODO: add dlist_resize

        if (real_ind != NULL)
            *real_ind = new_ind;

        ASSERT_OK(dlist);

        return DLIST_OK;
    }

    // size_t real_ind = dlist_node_at(dlist, ind);
    ssize_t right = dlist->data[ind].next;
    if (right != DLIST_BAD_ELEM)
        dlist->data[right].prev = new_ind;

    dlist->data[new_ind] = (Node){
        .prev = ind,
        .value = value,
        .next = right,
        .used = true
    };

    dlist->data[ind].next = new_ind;

    if (ind == dlist->tail)
        dlist->tail = new_ind;

    dlist->size++;

    if (real_ind != NULL)
        *real_ind = new_ind;

    ASSERT_OK(dlist);

    return DLIST_OK;
}

DListStatus dlist_delete(DList *dlist, ssize_t ind) {
    assert(dlist != NULL);

    ASSERT_OK(dlist);

    if (!dlist->data[ind].used)
        return DLIST_CANNOT_DELETE_UNUSED;

    // size_t real_ind = dlist_node_at(dlist, ind);

    ssize_t left = dlist->data[ind].prev;
    ssize_t right = dlist->data[ind].next;

    if (left != DLIST_BAD_ELEM)
        dlist->data[left].next = right;
    if (right != DLIST_BAD_ELEM)
        dlist->data[right].prev = left;

    if (ind == dlist->head)
        dlist->head = right;
    else if (ind == dlist->tail)
        dlist->tail = left;

    dlist->data[ind].used = false;
    dlist->data[ind].value = 0;
    dlist->data[ind].prev = DLIST_BAD_ELEM; // TODO: rename to something like DLIST_POISON or DLIST_NONE
    dlist->data[ind].next = DLIST_BAD_ELEM;

    dlist->size--;

    ASSERT_OK(dlist);

    return DLIST_OK;
}

