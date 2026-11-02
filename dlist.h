#pragma once

#include <stdio.h>
#include <stdbool.h>

#define DLIST_BAD_ELEM (size_t)-1
typedef int dlist_elem_t;

typedef struct {
    size_t prev;
    dlist_elem_t value;
    size_t next;

    bool used;
} Node;

typedef struct {
    size_t head;
    size_t tail;
    Node *data;
    size_t size;
    size_t capacity;
} DList;

typedef enum {
    DLIST_OK = 0,
    DLIST_IS_FULL,
    DLIST_INDEX_ERROR
} DListStatus;

typedef struct {
    const char *style;
    size_t weight;
    const char *dir;
    const char *color;
} ArrowStyle;

DListStatus dlist_dump(DList *dlist);
DListStatus dlist_create(DList *dlist, size_t capacity);
DListStatus dlist_destroy(DList *dlist);
DListStatus dlist_insert_after(DList *dlist, size_t ind, dlist_elem_t value);
DListStatus dlist_delete(DList *dlist, size_t ind);
void dlist_to_dot(DList *dlist, const char *);
