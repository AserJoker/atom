#include "map.h"
#include "list.h"
#include <string.h>
struct s_Map_Node {
  cstring _key;
  void *_data;
  Map _map;
};
struct s_Map {
  List _list;
  Map_Option _option;
};
static Map_Node Map_Node_create() {
  Map_Node node = (Map_Node)Buffer_alloc(sizeof(struct s_Map_Node));
  node->_key = NULL;
  node->_data = NULL;
  node->_map = NULL;
  return node;
}
static void Map_Node_dispose(Map_Node node) {
  if (node->_map->_option.auto_free_data && node->_data) {
    node->_map->_option.dispose(node->_data);
  }
  Buffer_free(node->_key);
  Buffer_free(node);
}

Map Map_create(Map_Option opt) {
  Map map = (Map)Buffer_alloc(sizeof(struct s_Map));
  map->_option = opt;
  List_Option list_opt = {1, (Buffer_Free)Map_Node_dispose};
  map->_list = List_create(list_opt);
  return map;
}
Buffer Map_set(Map map, const cstring key, Buffer data) {
  for (List_Node it = List_head(map->_list); it != List_tail(map->_list);
       it = List_next(it)) {
    Map_Node node = (Map_Node)(List_get(it));
    if (node->_key == key) {
      Buffer old = node->_data;
      node->_data = data;
      return old;
    }
  }
  Map_Node node = Map_Node_create();
  node->_data = data;
  node->_key = key;
  node->_key = (cstring)Buffer_alloc(strlen(key) + 1);
  memcpy(node->_key, key, strlen(key) + 1);
  node->_map = map;
  List_insert_tail(map->_list, node);
  return NULL;
}
Buffer Map_get(Map map, const cstring key) {
  for (List_Node it = List_head(map->_list); it != List_tail(map->_list);
       it = List_next(it)) {
    Map_Node node = (Map_Node)(List_get(it));
    if (node->_key == key) {
      return node->_data;
    }
  }
  return NULL;
}
Buffer Map_setValue(Map_Node node, Buffer data) {
  Buffer old = node->_data;
  node->_data = data;
  return old;
}
Buffer Map_getValue(Map_Node node) { return node->_data; }
size_t Map_size(Map map) { return List_size(map->_list); }
int Map_empty(Map map) { return List_empty(map->_list); }

Map_Node Map_head(Map map) { return (Map_Node)List_get(List_head(map->_list)); }
Map_Node Map_tail(Map map) { return (Map_Node)List_get(List_tail(map->_list)); }
const cstring Map_getNodeKey(Map_Node node) { return node->_key; }
Map_Node Map_next(Map_Node node) {
  for (List_Node it = List_head(node->_map->_list);
       it != List_tail(node->_map->_list); it = List_next(it)) {
    if (List_get(it) == node) {
      return List_get(List_next(it));
    }
  }
  return NULL;
}
Map_Node Map_last(Map_Node node) {
  for (List_Node it = List_head(node->_map->_list);
       it != List_tail(node->_map->_list); it = List_next(it)) {
    if (List_get(it) == node) {
      return List_get(List_last(it));
    }
  }
  return NULL;
}