#include "json.h"
#include <string.h>
cstring JSON_fromDList(DList list, ToString toString) {
  DList_Option opt = {1, Buffer_free};
  DList parts = DList_create(opt);
  size_t len = 0;
  DList_insert_tail(parts, Buffer_from(2, "["));
  len += 1;
  for (DList_Node it = DList_head(list); it != DList_tail(list);
       it = DList_next(it)) {
    if (len != 1) {
      DList_insert_tail(parts, Buffer_from(2, ","));
      len += 1;
    }
    cstring str = toString(DList_get(it));
    DList_insert_tail(parts, str);
    len += strlen(str);
  }
  DList_insert_tail(parts, Buffer_from(2, "]"));
  len += 1;
  cstring result = (cstring)Buffer_alloc(len + 1);
  result[len] = '\0';
  cstring selector = result;
  for (DList_Node it = DList_head(parts); it != DList_tail(parts);
       it = DList_next(it)) {
    cstring part = (cstring)DList_get(it);
    while (*part) {
      *selector++ = *part++;
    }
  }
  DList_dispose(parts);
  return result;
}

static void JSON_fromTreeNode(Tree_Node node, ToString toString, size_t *len,
                              DList parts) {
  DList_insert_tail(parts, Buffer_from(2, "{"));
  *len += 1;
  if (Tree_get(node)) {
    DList_insert_tail(parts, Buffer_from(6, "data:"));
    *len += 5;
    cstring val = toString(Tree_get(node));
    DList_insert_tail(parts, val);
    *len += strlen(val);
  }
  DList children = Tree_getChildren(node);
  if (!DList_empty(children)) {
    DList_insert_tail(parts, Buffer_from(11, "children:["));
    *len += 10;
    int c_len = 0;
    for (DList_Node it = DList_head(children); it != DList_tail(children);
         it = DList_next(it)) {
      if (c_len == 0) {
        c_len = 1;
      } else {
        DList_insert_tail(parts, Buffer_from(2, ","));
        *len += 1;
      }
      JSON_fromTreeNode((Tree_Node)DList_get(it), toString, len, parts);
    }
    DList_insert_tail(parts, Buffer_from(2, "]"));
    *len += 1;
  }
  DList_insert_tail(parts, Buffer_from(2, "}"));
  *len += 1;
}

cstring JSON_fromTree(Tree tree, ToString toString) {
  DList_Option opt = {1, Buffer_free};
  DList parts = DList_create(opt);
  size_t len = 0;
  JSON_fromTreeNode(Tree_root(tree), toString, &len, parts);
  cstring result = (cstring)Buffer_alloc(len + 1);
  result[len] = '\0';
  cstring selector = result;
  for (DList_Node it = DList_head(parts); it != DList_tail(parts);
       it = DList_next(it)) {
    cstring part = (cstring)DList_get(it);
    while (*part) {
      *selector++ = *part++;
    }
  }
  DList_dispose(parts);
  return result;
}