#include "dlist.h"
#include <stdio.h>
#include <stdlib.h>

struct s_DList_Node {
  void *_data;
  DList_Node _next;
  DList_Node _last;
  DList _list;
};

struct s_DList {
  size_t _size;
  DList_Node _head;
  DList_Node _tail;
  DList_Option _option;
};

static DList_Node DList_Node_create() {
  DList_Node node = (DList_Node)Buffer_alloc(sizeof(struct s_DList_Node));
  node->_data = NULL;
  node->_last = NULL;
  node->_next = NULL;
  node->_list = NULL;
  return node;
}

static void DList_Node_dispose(DList_Node node) {
  if (node->_list->_option.auto_free_data && node->_data) {
    node->_list->_option.dispose(node->_data);
  }
  Buffer_free(node);
}

DList DList_create(DList_Option option) {
  DList list = (DList)Buffer_alloc(sizeof(struct s_DList));
  list->_size = 0;
  list->_head = DList_Node_create();
  list->_head->_list = list;
  list->_tail = list->_head;
  list->_option = option;
  return list;
}
void DList_dispose(DList list) {
  while (list->_head) {
    DList_Node node = list->_head;
    list->_head = list->_head->_next;
    DList_Node_dispose(node);
  }
  Buffer_free(list);
}
DList_Node DList_head(DList list) { return list->_head; }
DList_Node DList_tail(DList list) { return list->_tail; }
void DList_insert(DList list, DList_Node pos, void *data) {
  DList_Node node = DList_Node_create();
  node->_data = data;
  node->_next = pos;
  node->_last = pos->_last;
  node->_list = list;
  if (pos != list->_head) {
    pos->_last->_next = node;
  } else {
    list->_head = node;
  }
  pos->_last = node;
  list->_size++;
}
void DList_insert_tail(DList list, void *data) {
  if (list->_head != list->_tail) {
    DList_insert(list, list->_tail, data);
  } else {
    DList_insert_head(list, data);
  }
}
void DList_insert_head(DList list, void *data) {
  DList_Node node = DList_Node_create();
  node->_data = data;
  node->_next = list->_head;
  list->_head->_last = node;
  list->_head = node;
  node->_list = list;
  list->_size++;
}
int DList_remove(DList list, DList_Node node) {
  if (node != list->_tail) {
    if (node == list->_head) {
      node->_next->_last = NULL;
      list->_head = node->_next;
    } else {
      node->_last->_next = node->_next;
      node->_next->_last = node->_last;
    }
    DList_Node_dispose(node);
    list->_size--;
    return 1;
  }
  return 0;
}
size_t DList_size(DList list) { return list->_size; }
DList_Node DList_find(DList list, Finder_t finder) {
  for (DList_Node it = list->_head; it != list->_tail; it = it->_next) {
    if (finder(it->_data)) {
      return it;
    }
  }
  return NULL;
}
int DList_empty(DList list) { return list->_size == 0; }

DList_Node DList_next(DList_Node current) { return current->_next; }
DList_Node DList_last(DList_Node current) { return current->_last; }
void *DList_get(DList_Node node) { return node->_data; }
void *DList_set(DList_Node node, void *data) {
  void *result = node->_data;
  node->_data = data;
  return result;
}