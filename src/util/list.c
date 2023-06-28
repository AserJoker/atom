#include "util/list.h"
#include <stdio.h>
#include <stdlib.h>

struct s_List_Node {
  void *_data;
  List_Node _next;
  List_Node _last;
  List _list;
};

struct s_List {
  uint32_t _size;
  List_Node _head;
  List_Node _tail;
  List_Option _option;
};

static void List_Node_dispose(List_Node node) {
  if (node->_list->_option.auto_free_data && node->_data) {
    node->_list->_option.dispose(node->_data);
  }
}

static List_Node List_Node_create() {
  List_Node node =
      (List_Node)Buffer_alloc(sizeof(struct s_List_Node), List_Node_dispose);
  node->_data = NULL;
  node->_last = NULL;
  node->_next = NULL;
  node->_list = NULL;
  return node;
}

void List_dispose(List list) {
  while (list->_head) {
    List_Node node = list->_head;
    list->_head = list->_head->_next;
    Buffer_dispose(node);
  }
}
List List_create(List_Option option) {
  List list = (List)Buffer_alloc(sizeof(struct s_List), List_dispose);
  list->_size = 0;
  list->_head = List_Node_create();
  list->_head->_list = list;
  list->_tail = list->_head;
  list->_option = option;
  return list;
}
List_Node List_head(List list) { return list->_head; }
List_Node List_tail(List list) { return list->_tail; }
void List_insert(List list, List_Node pos, void *data) {
  List_Node node = List_Node_create();
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
void List_insert_tail(List list, void *data) {
  if (list->_head != list->_tail) {
    List_insert(list, list->_tail, data);
  } else {
    List_insert_head(list, data);
  }
}
void List_insert_head(List list, void *data) {
  List_Node node = List_Node_create();
  node->_data = data;
  node->_next = list->_head;
  list->_head->_last = node;
  list->_head = node;
  node->_list = list;
  list->_size++;
}
Bool List_remove(List list, List_Node node) {
  if (node != list->_tail) {
    if (node == list->_head) {
      node->_next->_last = NULL;
      list->_head = node->_next;
    } else {
      node->_last->_next = node->_next;
      node->_next->_last = node->_last;
    }
    List_Node_dispose(node);
    list->_size--;
    return True;
  }
  return False;
}
uint32_t List_size(List list) { return list->_size; }
List_Node List_find(List list, Finder_t finder) {
  for (List_Node it = list->_head; it != list->_tail; it = it->_next) {
    if (finder(it->_data)) {
      return it;
    }
  }
  return NULL;
}
Bool List_empty(List list) { return list->_size == 0; }

List_Node List_next(List_Node current) { return current->_next; }
List_Node List_last(List_Node current) { return current->_last; }
void *List_get(List_Node node) { return node->_data; }
void *List_set(List_Node node, void *data) {
  void *result = node->_data;
  node->_data = data;
  return result;
}