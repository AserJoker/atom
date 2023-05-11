#include "json.h"
#include <string.h>
cstring JSON_fromList(List list, ToString toString) {
  List_Option opt = {1, Buffer_free};
  List parts = List_create(opt);
  size_t len = 0;
  List_insert_tail(parts, Buffer_from(2, "["));
  len += 1;
  for (List_Node it = List_head(list); it != List_tail(list);
       it = List_next(it)) {
    if (len != 1) {
      List_insert_tail(parts, Buffer_from(2, ","));
      len += 1;
    }
    cstring str = toString(List_get(it));
    List_insert_tail(parts, str);
    len += strlen(str);
  }
  List_insert_tail(parts, Buffer_from(2, "]"));
  len += 1;
  cstring result = (cstring)Buffer_alloc(len + 1);
  result[len] = '\0';
  cstring selector = result;
  for (List_Node it = List_head(parts); it != List_tail(parts);
       it = List_next(it)) {
    cstring part = (cstring)List_get(it);
    while (*part) {
      *selector++ = *part++;
    }
  }
  List_dispose(parts);
  return result;
}

static void JSON_fromTreeNode(Tree_Node node, ToString toString, size_t *len,
                              List parts) {
  List_insert_tail(parts, Buffer_from(2, "{"));
  *len += 1;
  if (Tree_get(node)) {
    List_insert_tail(parts, Buffer_from(6, "data:"));
    *len += 5;
    cstring val = toString(Tree_get(node));
    List_insert_tail(parts, val);
    *len += strlen(val);
  }
  List children = Tree_getChildren(node);
  if (!List_empty(children)) {
    List_insert_tail(parts, Buffer_from(11, "children:["));
    *len += 10;
    int c_len = 0;
    for (List_Node it = List_head(children); it != List_tail(children);
         it = List_next(it)) {
      if (c_len == 0) {
        c_len = 1;
      } else {
        List_insert_tail(parts, Buffer_from(2, ","));
        *len += 1;
      }
      JSON_fromTreeNode((Tree_Node)List_get(it), toString, len, parts);
    }
    List_insert_tail(parts, Buffer_from(2, "]"));
    *len += 1;
  }
  List_insert_tail(parts, Buffer_from(2, "}"));
  *len += 1;
}

cstring JSON_fromTree(Tree tree, ToString toString) {
  List_Option opt = {1, Buffer_free};
  List parts = List_create(opt);
  size_t len = 0;
  JSON_fromTreeNode(Tree_root(tree), toString, &len, parts);
  cstring result = (cstring)Buffer_alloc(len + 1);
  result[len] = '\0';
  cstring selector = result;
  for (List_Node it = List_head(parts); it != List_tail(parts);
       it = List_next(it)) {
    cstring part = (cstring)List_get(it);
    while (*part) {
      *selector++ = *part++;
    }
  }
  List_dispose(parts);
  return result;
}