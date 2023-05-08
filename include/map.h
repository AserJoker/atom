#pragma once
#include "buffer.h"
#include "strings.h"
struct s_Map_Option {
  int auto_free_data;
  Buffer_Free dispose;
};
struct s_Map;
struct s_Map_Node;
typedef struct s_Map_Option Map_Option;
typedef struct s_Map *Map;
typedef struct s_Map_Node *Map_Node;

Map Map_create(Map_Option opt);
void Map_dispose(Map map);
Buffer Map_set(Map map, const cstring key, Buffer data);
Buffer Map_get(Map map, const cstring key);
Buffer Map_setValue(Map_Node node, Buffer data);
Buffer Map_getValue(Map_Node node);
size_t Map_size(Map map);
int Map_empty(Map map);

Map_Node Map_head(Map map);
Map_Node Map_tail(Map map);
const cstring Map_getNodeKey(Map_Node node);
Map_Node Map_next(Map_Node node);
Map_Node Map_last(Map_Node node);