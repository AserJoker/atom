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

Error Error_create(cstring message, Location location, Error cause);
void Error_dispose(Error error);
void setError(Error error);
Error getError();
void printError(Error error);