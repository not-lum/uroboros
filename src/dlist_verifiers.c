#include "../include/dlist.h"
#include <malloc.h>


DListStatus dlist_verify_prev(DList *dlist, ssize_t ind) {
    ssize_t node_prev = dlist->data[ind].prev;

    if (node_prev >= dlist->capacity)
        return DLIST_PREV_BIGGER_THAN_CAPACITY;
    if (node_prev < 0 && node_prev != DLIST_BAD_ELEM)
        return DLIST_PREV_BAD_VALUE;

    return DLIST_OK;
}

DListStatus dlist_verify_next(DList *dlist, ssize_t ind) {
    ssize_t node_next = dlist->data[ind].next;

    if (node_next >= dlist->capacity)
        return DLIST_NEXT_BIGGER_THAN_CAPACITY;
    if (node_next < 0 && node_next != DLIST_BAD_ELEM)
        return DLIST_NEXT_BAD_VALUE;

    return DLIST_OK;
}

DListStatus dlist_verify_fields(DList *dlist) {
    for (ssize_t i = 0; i < dlist->capacity; i++) {
        DListStatus status = dlist_verify_prev(dlist, i);
        if (status != DLIST_OK)
            return status;

        status = dlist_verify_next(dlist, i);
        if (status != DLIST_OK)
            return status;
    }

    return DLIST_OK;
}

DListStatus dlist_verify_linking(DList *dlist) {
    for (ssize_t i = 0; i < dlist->capacity; i++) {
        if (dlist->data[i].prev == DLIST_BAD_ELEM)
            continue;

        ssize_t node_prev = dlist->data[i].prev;
        ssize_t node_next = dlist->data[i].next;
    
        if (dlist->data[node_prev].next != i) {
            return DLIST_PREV_ELEMENT_DOESNT_LINK_BACK;
        }

        if (node_next != DLIST_BAD_ELEM) {
            if (dlist->data[node_next].prev != i) {
                return DLIST_NEXT_ELEMENT_DOESNT_LINK_BACK;
            }
        }
    }

    return DLIST_OK;
}

DListStatus dlist_check_ind_reachability(DList *dlist, ssize_t ind, ssize_t *out_cnt) {
    // TODO: we can identify which nodes are unreachable in only one loop
    if (dlist->data[ind].prev == DLIST_BAD_ELEM)
        return DLIST_OK;

    ssize_t counter = 0;

    for (ssize_t i = dlist_head(dlist); i != ind; i = dlist->data[i].next) {
        if (counter > dlist->size) {
            if (out_cnt != NULL) *out_cnt = counter;
            return DLIST_UNREACHABLE_ELEM_DETECTED;
        }

        counter++;
    }

    if (out_cnt != NULL) *out_cnt = counter;
    return DLIST_OK;
}

DListStatus dlist_verify_reachability(DList *dlist) {
    ssize_t counter = 0;
    DListStatus status = dlist_check_ind_reachability(dlist, 0, &counter);

    if (status == DLIST_OK && counter < dlist->size)
        return DLIST_INVALID_CONNECTIVITY;
    
    return status;
}

DListStatus dlist_verify(DList *dlist) {
    if (dlist == NULL)
        return DLIST_PTR_IS_NULL;

    if (dlist->data == NULL)
        return DLIST_DATA_PTR_IS_NULL;

    if (dlist->size > dlist->capacity)
        return DLIST_SIZE_BIGGER_THAN_CAPACITY;

    if (dlist->size < 0)
        return DLIST_SIZE_LESS_THAN_ZERO;

    if (dlist->capacity < 0)
        return DLIST_CAPACITY_LESS_THAN_ZERO;

    if (dlist->free_head < 0)
        return DLIST_FREE_HEAD_LESS_THAN_ZERO;

    if (dlist->capacity * sizeof(Node) > malloc_usable_size(dlist->data))
        return DLIST_CAPACITY_BIGGER_THAN_MUSABLE;

    DListStatus status = dlist_verify_fields(dlist);
    if (status != DLIST_OK)
        return status;

    status = dlist_verify_linking(dlist);
    if (status != DLIST_OK)
        return status;

    status = dlist_verify_reachability(dlist);
    if (status != DLIST_OK)
        return status;

    return DLIST_OK;
}
