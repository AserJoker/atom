#pragma once
#include "buffer.h"
#include <stdint.h>

struct s_List_Option {
  int auto_free_data;
  Buffer_Free dispose;
};
typedef struct s_List_Option List_Option;

struct s_List_Node;
struct s_List;
typedef struct s_List *List;
typedef struct s_List_Node *List_Node;

typedef int (*Finder_t)(Buffer data);

List List_create(List_Option option);
void List_dispose(List list);
List_Node List_head(List list);
List_Node List_tail(List list);
void List_insert(List list, List_Node pos, Buffer data);
void List_insert_tail(List list, Buffer data);
void List_insert_head(List list, Buffer data);
int List_remove(List list, List_Node node);
size_t List_size(List list);
List_Node List_find(List list, Finder_t finder);
int List_empty(List list);

List_Node List_next(List_Node current);
List_Node List_last(List_Node current);
Buffer List_get(List_Node node);
Buffer List_set(List_Node node, Buffer data);