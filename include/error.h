#pragma once
#include "source.h"
#include "strings.h"
struct s_Error;
typedef struct s_Error *Error;
struct s_Error {
  cstring message;
  Location location;
  Error cause;
};

Error Error_create();
void Error_dispose(Error error);
Error Error_init(cstring message, Location location, Error cause);
void Error_print(Error error);

void ErrorStack_init();
void ErrorStack_print();
void ErrorStack_push(Error error);
int ErrorStack_empty();