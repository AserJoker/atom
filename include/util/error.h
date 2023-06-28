#pragma once
#include "Strings.h"
#include "source.h"

struct s_Error;
typedef struct s_Error *Error;
struct s_Error {
  cstring message;
  Location location;
  Error cause;
};

Error Error_create(cstring message, Location location, Error cause);

void Error_set(Error error);
void Error_push(cstring message, Location location);
Error Error_get();
void Error_print(Error error);