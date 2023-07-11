#pragma once
#include "util/bool.h"
#include "util/list.h"
#include "util/strings.h"
typedef enum e_JS_ValueType {
  JS_BOOLEAN,
  JS_NUMBER,
  JS_STRING,
  JS_UNDEFINED,
  JS_OBJECT,
} JS_ValueType;

typedef struct s_JS_Value *JS_Value;
typedef struct s_JS_Scope *JS_Scope;

typedef struct s_JS_Scope {
  JS_Scope parent;
  List children;
  List values;
} *JS_Scope;

typedef struct s_JS_ObjectProperty {
  JS_Value field;
  cstring key;
} *JS_ObjectProperty;

typedef struct s_JS_Object {
  List properties;
} *JS_Object;

struct s_JS_Value {
  JS_ValueType type;
  List refs;
  List scopes;
  union {
    double number;
    cstring string;
    Bool boolean;
    JS_Object object;
  } data;
};

JS_Scope JS_Scope_create(JS_Scope parent);
void JS_Scope_remove(JS_Scope scope, JS_Value value);
void JS_Scope_add(JS_Scope scope, JS_Value value);

JS_Value JS_Value_create(JS_Scope scope);
JS_Value JS_Value_createObject(JS_Scope scope);
void JS_Object_setField(JS_Value obj, cstring key, JS_Value field);
void JS_Object_deleteField(JS_Value obj, cstring key);
JS_Value JS_Object_getField(JS_Value obj, cstring key);