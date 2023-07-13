#include "util/list.h"
#include <stdio.h>
#include <stdlib.h>

struct s_List_Node {
  void *data;
  List_Node next;
  List_Node last;
  List list;
};

struct s_List {
  uint32_t size;
  List_Node head;
  List_Node tail;
  Bool autoRelease;
};

static void List_Node_dispose(List_Node node) {
  if (node->list->autoRelease && node->data) {
    Buffer_dispose(node->data);
  }
}

static List_Node List_Node_create() {
  List_Node node =
      (List_Node)Buffer_alloc(sizeof(struct s_List_Node), List_Node_dispose);
  node->data = NULL;
  node->last = NULL;
  node->next = NULL;
  node->list = NULL;
  return node;
}

void List_dispose(List list) {
  while (list->head) {
    List_Node node = list->head;
    list->head = list->head->next;
    Buffer_dispose(node);
  }
}
List List_create(Bool autoRelease) {
  List list = (List)Buffer_alloc(sizeof(struct s_List), List_dispose);
  list->size = 0;
  list->head = List_Node_create();
  list->head->list = list;
  list->tail = list->head;
  list->autoRelease = autoRelease;
  return list;
}
List_Node List_head(List list) { return list->head; }
List_Node List_tail(List list) { return list->tail; }
void List_insert(List list, List_Node pos, void *data) {
  List_Node node = List_Node_create();
  node->data = data;
  node->next = pos;
  node->last = pos->last;
  node->list = list;
  if (pos != list->head) {
    pos->last->next = node;
  } else {
    list->head = node;
  }
  pos->last = node;
  list->size++;
}
void List_insert_tail(List list, void *data) {
  if (list->head != list->tail) {
    List_insert(list, list->tail, data);
  } else {
    List_insert_head(list, data);
  }
}
void List_insert_head(List list, void *data) {
  List_Node node = List_Node_create();
  node->data = data;
  node->next = list->head;
  list->head->last = node;
  list->head = node;
  node->list = list;
  list->size++;
}
Bool List_remove(List list, List_Node node) {
  if (node != list->tail) {
    if (node == list->head) {
      node->next->last = NULL;
      list->head = node->next;
    } else {
      node->last->next = node->next;
      node->next->last = node->last;
    }
    Buffer_dispose(node);
    list->size--;
    return True;
  }
  return False;
}

Bool List_erase(List list, void *data) {
  LIST_LOOP(list) {
    if (List_get(it) == data) {
      List_remove(list, it);
      return True;
    }
  }
  return False;
}
uint32_t List_size(List list) { return list->size; }
List_Node List_find(List list, Finder_t finder) {
  for (List_Node it = list->head; it != list->tail; it = it->next) {
    if (finder(it->data)) {
      return it;
    }
  }
  return NULL;
}
Bool List_empty(List list) { return list->size == 0; }

List_Node List_next(List_Node current) { return current->next; }
List_Node List_last(List_Node current) { return current->last; }
void *List_get(List_Node node) { return node->data; }
void *List_set(List_Node node, void *data) {
  void *result = node->data;
  node->data = data;
  return result;
}

Bool List_contains(List list, Buffer data) {
  LIST_LOOP(list) {
    if (it->data == data) {
      return True;
    }
  }
  return False;
}