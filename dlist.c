#include "dlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


DListStatus dlist_dump(DList *dlist) {
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

DListStatus dlist_create(DList *dlist, size_t capacity) {
    assert(dlist != NULL);

    dlist->data = calloc(capacity, sizeof(Node));
    dlist->head = DLIST_BAD_ELEM;
    dlist->tail = DLIST_BAD_ELEM;
    dlist->size = 0;
    dlist->capacity = capacity;
    

    for (size_t i = 0; i < capacity; i++) {
        dlist->data[i].prev = DLIST_BAD_ELEM;
        dlist->data[i].next = DLIST_BAD_ELEM;
    }

    return DLIST_OK;
}


DListStatus dlist_destroy(DList *dlist) {
    assert(dlist != NULL);

    dlist->head = 0;
    free(dlist->data);
    dlist->tail = 0;

    return DLIST_OK;
}

static size_t dlist_node_at(DList *dlist, size_t ind) {
    assert(dlist != NULL);
    
    size_t curr_ind = dlist->head;

    for (size_t i = 0; i < ind; i++) {
        curr_ind = dlist->data[curr_ind].next; // TODO: handle when curr_ind is DLIST_BAD_ELEM
    }

    return curr_ind;
}


static size_t dlist_find_free(DList *dlist) {
    assert(dlist != NULL);

    for (size_t i = 0; i < dlist->capacity; i++) {
        if (!dlist->data[i].used) {
            return i;
        }
    }

    return DLIST_BAD_ELEM;
}


DListStatus dlist_insert_after(DList *dlist, size_t ind, dlist_elem_t value) {
    assert(dlist != NULL);
    assert(ind < dlist->capacity);

    if (dlist->size == 0 && ind != 0)
        return DLIST_INDEX_ERROR;

    size_t new_ind = dlist_find_free(dlist);
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

        return DLIST_OK;
    }

    // size_t real_ind = dlist_node_at(dlist, ind);
    size_t right = dlist->data[ind].next;
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

    return DLIST_OK;
}

DListStatus dlist_delete(DList *dlist, size_t ind) {
    assert(dlist != NULL);

    // size_t real_ind = dlist_node_at(dlist, ind);

    size_t left = dlist->data[ind].prev;
    size_t right = dlist->data[ind].next;

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

    return DLIST_OK;
}

