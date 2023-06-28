#include "util/error.h"
#include "util/Strings.h"
static Error globalError = NULL;
Error Error_create(cstring message, Location location, Error cause) {
  Error error = (Error)Buffer_alloc(sizeof(struct s_Error), Error_dispose);
  error->message = (cstring)cstring_toBuffer(message);
  error->location = location;
  error->cause = cause;
  return error;
}
void Error_dispose(Error error) {
  if (error->cause) {
    Error_dispose(error->cause);
  }
  Buffer_dispose(error->message);
}
void Error_set(Error error) { globalError = error; }
Error Error_get() { return globalError; }
void Error_print(Error error) {
  printf("%s at\n    %s:%d:%d\n", error->message, error->location._filename,
         error->location._position._line, error->location._position._column);
  if (error->cause) {
    printf("caused by:\n");
    Error_print(error->cause);
  }
}

void Error_push(cstring message, Location location) {
  Error_set(Error_create(message, location, globalError));
}