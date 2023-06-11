#pragma once
#include "list.h"
#include "strings.h"

typedef enum JSON_TYPE {
  JSON_NULL,
  JSON_STRING,
  JSON_DOUBLE,
  JSON_BOOLEAN,
  JSON_OBJECT,
  JSON_ARRAY
} JSON_TYPE;
struct s_JSON_Value;
typedef struct s_JSON_Value *JSON_Value;
struct s_JSON_Value {
  union {
    cstring s;
    double d;
    int b;
  } d;
  cstring key;
  JSON_Value next;
  JSON_Value children;
  JSON_TYPE type;
};

JSON_Value JSON_createDouble(double val);
JSON_Value JSON_createString(cstring val);
JSON_Value JSON_createBoolean(int val);
JSON_Value JSON_createNull();
JSON_Value JSON_createObject();
JSON_Value JSON_createArray();
typedef JSON_Value (*ToJSON)(void *);
JSON_Value JSON_fromList(List list, ToJSON toJSON);
void JSON_dispose(JSON_Value val);
void JSON_setField(JSON_Value obj, cstring key, JSON_Value value);
void JSON_setIndex(JSON_Value obj, int32_t index, JSON_Value value);

cstring JSON_stringlify(JSON_Value val);