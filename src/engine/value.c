#include "engine/value.h"
#include <string.h>

static void JS_Value_tryDispose(JS_Value value) {
  List queue = List_create(False);
  List cache = List_create(False);
  List_insert_tail(queue, value);
  Bool isValueLived = False;
  while (!List_empty(queue)) {
    List_Node it = List_head(queue);
    JS_Value val = List_get(it);
    if (!List_contains(cache, val)) {
      if (!List_empty(val->scopes)) {
        isValueLived = True;
        break;
      }
      LIST_LOOP(val->refs) {
        JS_Value val = List_get(it);
        List_insert_tail(queue, val);
      }
      List_insert_tail(cache, val);
    }
    List_remove(queue, it);
  }
  Buffer_dispose(cache);
  Buffer_dispose(queue);
  if (!isValueLived) {
    Buffer_dispose(value);
  }
}

static void JS_Scope_dispose(JS_Scope scope) {
  Buffer_dispose(scope->children);
  if (scope->parent) {
    LIST_LOOP(scope->parent->children) {
      if (List_get(it) == scope) {
        List_remove(scope->parent->children, it);
        scope->parent = NULL;
        break;
      }
    }
  }
  List gc_list = List_create(False);
  LIST_LOOP(scope->values) {
    JS_Value value = (JS_Value)List_get(it);
    LIST_LOOP(value->scopes) {
      if (List_get(it) == scope) {
        List_remove(value->scopes, it);
        break;
      }
    }
    if (List_empty(value->scopes)) {
      List_insert_tail(gc_list, value);
    }
  }
  LIST_LOOP(gc_list) {
    JS_Value val = (JS_Value)List_get(it);
    JS_Value_tryDispose(val);
  }
  Buffer_dispose(gc_list);
  Buffer_dispose(scope->values);
}

JS_Scope JS_Scope_create(JS_Scope parent) {
  JS_Scope scope =
      (JS_Scope)Buffer_alloc(sizeof(struct s_JS_Scope), JS_Scope_dispose);
  scope->children = List_create(True);
  scope->values = List_create(False);
  scope->parent = parent;
  if (parent) {
    List_insert_tail(parent->children, scope);
  }
  return scope;
}

void JS_Scope_remove(JS_Scope scope, JS_Value value) {
  LIST_LOOP(value->scopes) {
    if (List_get(it) == scope) {
      List_remove(value->scopes, it);
      break;
    }
  }
  LIST_LOOP(scope->values) {
    if (List_get(it) == value) {
      List_remove(value->scopes, it);
      break;
    }
  }
  if (List_empty(value->scopes)) {
    JS_Value_tryDispose(value);
  }
}
void JS_Scope_add(JS_Scope scope, JS_Value value) {
  LIST_LOOP(value->scopes) {
    if (List_get(it) == scope) {
      return;
    }
  }
  List_insert_tail(value->scopes, scope);
  List_insert_tail(scope->values, value);
}

static void JS_Value_dispose(JS_Value value) {
  if (value->type == JS_OBJECT) {
    while (!List_empty(value->data.object->properties)) {
      List_Node it = List_head(value->data.object->properties);
      JS_ObjectProperty prop = (JS_ObjectProperty)List_get(it);
      JS_Object_deleteField(value, prop->key);
    }
    Buffer_dispose(value->data.object);
  }
  Buffer_dispose(value->scopes);
  Buffer_dispose(value->refs);
}

JS_Value JS_Value_create(JS_Scope scope) {
  JS_Value value =
      (JS_Value)Buffer_alloc(sizeof(struct s_JS_Value), JS_Value_dispose);
  value->refs = List_create(False);
  value->scopes = List_create(False);
  value->type = JS_UNDEFINED;
  List_insert_tail(value->scopes, scope);
  List_insert_tail(scope->values, value);
  return value;
}

void JS_Object_dispose(JS_Object obj) { Buffer_dispose(obj->properties); }

JS_Object JS_Object_create() {
  JS_Object obj =
      (JS_Object)Buffer_alloc(sizeof(struct s_JS_Object), JS_Object_dispose);
  obj->properties = List_create(True);
  return obj;
}

JS_Value JS_Value_createObject(JS_Scope scope) {
  JS_Value obj = JS_Value_create(scope);
  obj->type = JS_OBJECT;
  obj->data.object = JS_Object_create();
  return obj;
}

JS_Value JS_Object_getField(JS_Value obj, cstring key) {
  LIST_LOOP(obj->data.object->properties) {
    JS_ObjectProperty prop = List_get(it);
    if (strcmp(prop->key, key) == 0) {
      return prop->field;
    }
  }
  return NULL;
}
void JS_Object_deleteField(JS_Value obj, cstring key) {
  LIST_LOOP(obj->data.object->properties) {
    JS_ObjectProperty prop = List_get(it);
    if (strcmp(prop->key, key) == 0) {
      LIST_LOOP(prop->field->refs) {
        if (List_get(it) == obj) {
          List_remove(prop->field->refs, it);
          break;
        }
      }
      JS_Value_tryDispose(prop->field);
      List_remove(obj->data.object->properties, it);
      break;
    }
  }
}

void JS_ObjectProperty_dispose(JS_ObjectProperty prop) {
  Buffer_dispose(prop->key);
}
JS_ObjectProperty JS_ObjectProperty_create() {
  JS_ObjectProperty prop = (JS_ObjectProperty)Buffer_alloc(
      sizeof(struct s_JS_ObjectProperty), JS_ObjectProperty_dispose);
  prop->field = NULL;
  prop->key = NULL;
  return prop;
}
void JS_Object_setField(JS_Value obj, cstring key, JS_Value field) {
  JS_Value old = JS_Object_getField(obj, key);
  if (old == field) {
    return;
  }
  JS_Object_deleteField(obj, key);
  List_insert_tail(field->refs, obj);
  JS_ObjectProperty property = JS_ObjectProperty_create();
  property->field = field;
  property->key = cstring_toBuffer(key);
  List_insert_tail(obj->data.object->properties, property);
}