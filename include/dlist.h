#pragma once
#include "buffer.h"
#include <stdint.h>

struct s_DList_Option {
  int auto_free_data;
  Buffer_Free dispose;
};
typedef struct s_DList_Option DList_Option;

struct s_DList_Node;
struct s_DList;
typedef struct s_DList *DList;
typedef struct s_DList_Node *DList_Node;

typedef int (*Finder_t)(Buffer data);

DList DList_create(DList_Option option);
void DList_dispose(DList list);
DList_Node DList_head(DList list);
DList_Node DList_tail(DList list);
void DList_insert(DList list, DList_Node pos, Buffer data);
void DList_insert_tail(DList list, Buffer data);
void DList_insert_head(DList list, Buffer data);
int DList_remove(DList list, DList_Node node);
size_t DList_size(DList list);
DList_Node DList_find(DList list, Finder_t finder);
int DList_empty(DList list);

DList_Node DList_next(DList_Node current);
DList_Node DList_last(DList_Node current);
Buffer DList_get(DList_Node node);
Buffer DList_set(DList_Node node, Buffer data);