#pragma once
#include "bool.h"
#include "buffer.h"
#include <stdint.h>

struct s_List_Node;
struct s_List;
typedef struct s_List *List;
typedef struct s_List_Node *List_Node;

typedef int (*Finder_t)(Buffer data);

List List_create(Bool autoRelease);
List_Node List_head(List list);
List_Node List_tail(List list);
void List_insert(List list, List_Node pos, Buffer data);
void List_insert_tail(List list, Buffer data);
void List_insert_head(List list, Buffer data);
Bool List_remove(List list, List_Node node);
Bool List_erase(List list, void *data);
uint32_t List_size(List list);
List_Node List_find(List list, Finder_t finder);
Bool List_empty(List list);
Bool List_contains(List list, Buffer data);

List_Node List_next(List_Node current);
List_Node List_last(List_Node current);
Buffer List_get(List_Node node);
Buffer List_set(List_Node node, Buffer data);

#define LIST_LOOP(list)                                                        \
  for (List_Node it = List_head(list); it != List_tail(list);                  \
       it = List_next(it))
