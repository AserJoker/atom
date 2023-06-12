#include "error.h"
#include "strings.h"
static Error globalError = NULL;
Error Error_create(cstring message, Location location, Error cause) {
  Error error = (Error)Buffer_alloc(sizeof(struct s_Error));
  error->message = (cstring)cstring_toBuffer(message);
  error->location = location;
  error->cause = cause;
  return error;
}
void Error_dispose(Error error) {
  if (error->cause) {
    Error_dispose(error->cause);
  }
  Buffer_free(error->message);
  Buffer_free(error);
}
void setError(Error error) { globalError = error; }
Error getError() { return globalError; }
void printError(Error error) {
  printf("%s at\n    %s:%d:%d\n", error->message, error->location._filename,
         error->location._position._line, error->location._position._column);
  if (error->cause) {
    printf("caused by:\n");
    printError(error->cause);
  }
}

void pushError(cstring message, Location location) {
  setError(Error_create(message, location, globalError));
}