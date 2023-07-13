#pragma once
#include "atom.h"
typedef struct s_JS_Value *JS_Value;
typedef struct s_JS_Scope *JS_Scope;
typedef void (*JS_CFunction)(JS_Scope scope);
typedef enum e_JS_ValueType {
  JS_UNDEFINED,
  JS_NULL,
  JS_STRING,
  JS_NUMBER,
  JS_INTEGER,
  JS_BOOLEAN,
  JS_OBJECT,
  JS_ARRAY,
  JS_FUNCTION,
  JS_CLASS
} JS_ValueType;

struct s_JS_Scope {
  JS_Scope parent;
  JS_AtomGroup group;
  List children;
  List values;
};
struct s_JS_Value {
  JS_ValueType type;
};
typedef struct s_JS_String {
  struct s_JS_Value super;
  cstring data;
} *JS_String;
typedef struct s_JS_Number {
  struct s_JS_Value super;
  double data;
} *JS_Number;
typedef struct s_JS_Integer {
  struct s_JS_Value super;
  int32_t data;
} *JS_Integer;
typedef struct s_JS_Boolean {
  struct s_JS_Value super;
  Bool data;
} *JS_Boolean;

typedef struct s_JS_Object {
  struct s_JS_Value super;
  Bool isSeal;
  Bool isFrozen;
  List properties;
} *JS_Object, *JS_Array;

typedef struct s_JS_Function {
  struct s_JS_Object super;
  JS_CFunction callee;
  JS_Scope closure;
} *JS_Function;

typedef struct s_JS_Class {
  struct s_JS_Object super;
  List properties;
} *JS_Class;

typedef struct s_JS_ObjectProperty {
  JS_Value value;
  Bool writable;
  Bool enumerable;
  Bool configurable;
  JS_Function get;
  JS_Function set;
} *JS_ObjectProperty;

JS_Scope JS_Scope_create(JS_Scope parent);

JS_Atom JS_Scope_define(JS_Scope scope, cstring name, JS_Value value);
Bool JS_Scope_add(JS_Scope scope, cstring name, JS_Atom value);
Bool JS_Scope_remove(JS_Scope scope, cstring name);
JS_Atom JS_Scope_get(JS_Scope scope, cstring name);
JS_Atom JS_Scope_checkout(JS_Scope scope, cstring name);

JS_String JS_createString(cstring data);
void JS_setString(JS_Atom atom, cstring data);
Bool JS_getString(JS_Atom atom, cstring *source);
Bool JS_isString(JS_Atom atom);