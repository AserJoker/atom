#include "util/json.h"
#include "util/Strings.h"
#include "util/buffer.h"
#include "util/list.h"
#include <stdint.h>
#include <string.h>

JSON_Value JSON_fromList(List list, ToJSON toJSON) {
  JSON_Value arr = JSON_createArray();
  int index = 0;
  for (List_Node node = List_head(list); node != List_tail(list);
       node = List_next(node)) {
    JSON_setIndex(arr, index++, toJSON(List_get(node)));
  }
  return arr;
}

static JSON_Value JSON_create() {
  JSON_Value obj =
      (JSON_Value)Buffer_alloc(sizeof(struct s_JSON_Value), JSON_dispose);
  obj->children = NULL;
  obj->next = NULL;
  obj->type = JSON_NULL;
  obj->d.s = NULL;
  obj->key = NULL;
  return obj;
}

static cstring JSON_formatString(cstring val) {
  cstring result = (cstring)Buffer_alloc(strlen(val) * 2 + 1, NULL);
  cstring selector = result;
  while (*val) {
    if (*val == '\"') {
      *selector++ = '\\';
      *selector++ = *val++;
    } else if (*val == '\r') {
      *selector++ = '\\';
      *selector++ = '\r';
      val++;
    } else if (*val == '\n') {
      *selector++ = '\\';
      *selector++ = '\n';
      val++;
    } else {
      *selector++ = *val++;
    }
  }
  *selector = '\0';
  return result;
}

void JSON_dispose(JSON_Value val) {
  switch (val->type) {
  case JSON_STRING:
    Buffer_dispose(val->d.s);
    break;
  case JSON_ARRAY:
  case JSON_OBJECT:
    if (val->children) {
      JSON_dispose(val->children);
    }
    break;
  default:
    break;
  }
  if (val->next) {
    JSON_dispose(val->next);
  }
  if (val->key) {
    Buffer_dispose(val->key);
  }
}

JSON_Value JSON_createDouble(double val) {
  JSON_Value obj = JSON_create();
  obj->type = JSON_DOUBLE;
  obj->d.d = val;
  return obj;
}
JSON_Value JSON_createString(cstring val) {
  JSON_Value obj = JSON_create();
  obj->type = JSON_STRING;
  obj->d.s = JSON_formatString(val);
  return obj;
}
JSON_Value JSON_createBoolean(int val) {
  JSON_Value obj = JSON_create();
  obj->type = JSON_BOOLEAN;
  obj->d.b = val;
  return obj;
}
JSON_Value JSON_createNull() {
  JSON_Value obj = JSON_create();
  return obj;
}
JSON_Value JSON_createObject() {
  JSON_Value obj = JSON_create();
  obj->type = JSON_OBJECT;
  return obj;
}
JSON_Value JSON_createArray() {
  JSON_Value obj = JSON_create();
  obj->type = JSON_ARRAY;
  return obj;
}

void JSON_setField(JSON_Value obj, cstring key, JSON_Value value) {
  value->key = JSON_formatString(key);
  if (!obj->children) {
    obj->children = value;
  } else {
    JSON_Value c = obj->children;
    while (c->next) {
      c = c->next;
    }
    c->next = value;
  }
}

void JSON_setIndex(JSON_Value obj, int32_t index, JSON_Value value) {
  if (index == 0) {
    if (obj->children) {
      value->next = obj->children->next;
      obj->children->next = NULL;
      JSON_dispose(obj->children);
    }
    obj->children = value;
  } else {
    JSON_Value c = obj->children;
    for (int i = 0; i < index - 1; i++) {
      if (!c->next) {
        c->next = JSON_createNull();
      }
      c = c->next;
    }
    if (c->next) {
      value->next = c->next->next;
      c->next->next = NULL;
      JSON_dispose(c->next);
    }
    c->next = value;
  }
}

uint32_t JSON_getLength(JSON_Value val) {
  JSON_Value c = val->children;
  uint32_t count = 0;
  while (c) {
    c = c->next;
    count++;
  }
  return count;
}

static cstring JSON_stringlify_string(JSON_Value val) {
  cstring str = (cstring)Buffer_alloc(strlen(val->d.s) + 3, NULL);
  str[0] = '\"';
  cstring selector = str + 1;
  cstring ss = val->d.s;
  while (*ss) {
    *selector++ = *ss++;
  }
  *selector++ = '\"';
  *selector = '\0';
  return str;
}
cstring JSON_stringlify_double(JSON_Value val) {
  cstring str = (cstring)Buffer_alloc(32, NULL);
  sprintf(str, "%lf", val->d.d);
  return str;
}

cstring JSON_stringlify_array(JSON_Value val) {
  List_Option opt = {1, Buffer_dispose};
  List parts = List_create(opt);
  uint32_t len = 0;
  List_insert_tail(parts, cstring_toBuffer("["));
  len++;
  JSON_Value c = val->children;
  while (c) {
    if (len != 1) {
      List_insert_tail(parts, cstring_toBuffer(","));
      len++;
    }
    cstring s_item = JSON_stringlify(c);
    List_insert_tail(parts, s_item);
    len += strlen(s_item);
    c = c->next;
  }
  List_insert_tail(parts, cstring_toBuffer("]"));
  len++;

  cstring result = (cstring)Buffer_alloc(len + 1, NULL);
  cstring selector = result;
  result[len] = 0;
  for (List_Node node = List_head(parts); node != List_tail(parts);
       node = List_next(node)) {
    cstring s = List_get(node);
    while (*s) {
      *selector++ = *s++;
    }
  }
  Buffer_dispose(parts);
  return result;
}

cstring JSON_stringlify_object(JSON_Value val) {
  List_Option opt = {1, Buffer_dispose};
  List parts = List_create(opt);
  uint32_t len = 0;
  List_insert_tail(parts, cstring_toBuffer("{"));
  len++;
  JSON_Value c = val->children;
  while (c) {
    if (len != 1) {
      List_insert_tail(parts, cstring_toBuffer(","));
      len++;
    }
    List_insert_tail(parts, cstring_toBuffer("\""));
    len++;
    cstring s_key = (cstring)cstring_toBuffer(c->key);
    List_insert_tail(parts, s_key);
    len += strlen(s_key);
    List_insert_tail(parts, cstring_toBuffer("\""));
    len++;
    List_insert_tail(parts, cstring_toBuffer(":"));
    len++;
    cstring s_item = JSON_stringlify(c);
    List_insert_tail(parts, s_item);
    len += strlen(s_item);
    c = c->next;
  }
  List_insert_tail(parts, cstring_toBuffer("}"));
  len++;

  cstring result = (cstring)Buffer_alloc(len + 1, NULL);
  cstring selector = result;
  result[len] = 0;
  for (List_Node node = List_head(parts); node != List_tail(parts);
       node = List_next(node)) {
    cstring s = List_get(node);
    while (*s) {
      *selector++ = *s++;
    }
  }
  Buffer_dispose(parts);
  return result;
}

cstring JSON_stringlify(JSON_Value val) {
  switch (val->type) {
  case JSON_NULL:
    return (cstring)cstring_toBuffer("null");
  case JSON_STRING:
    return JSON_stringlify_string(val);
  case JSON_DOUBLE:
    return JSON_stringlify_double(val);
  case JSON_BOOLEAN:
    return (cstring)cstring_toBuffer(val->d.b ? "true" : "false");
  case JSON_OBJECT:
    return JSON_stringlify_object(val);
  case JSON_ARRAY:
    return JSON_stringlify_array(val);
  }
  return NULL;
}