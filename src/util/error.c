#include "error.h"
#include <stdlib.h>
typedef struct s_ErrorStack {
  List errors;
} *ErrorStack;

static ErrorStack g_stack;

Error Error_create() {
  Error error = (Error)Buffer_alloc(sizeof(struct s_Error));
  error->message = NULL;
  error->location._filename = NULL;
  error->cause = NULL;
  return error;
}
void Error_dispose(Error error) {
  if (error->cause) {
    Error_dispose(error->cause);
  }
  Buffer_free(error);
}
Error Error_init(cstring message, Location location, Error cause) {
  Error error = Error_create();
  error->message = message;
  error->location = location;
  error->cause = cause;
  return error;
}
void Error_print(Error error) {
  fprintf(stderr, "%s at\n  %s:%d:%d\n", error->message,
          error->location._filename, error->location._position._line,
          error->location._position._column);
  if (error->cause) {
    fprintf(stderr, "caused by\n");
    Error_print(error);
  }
}

void ErrorStack_dispose(void) {
  List_dispose(g_stack->errors);
  Buffer_free(g_stack);
  g_stack = NULL;
}

void ErrorStack_init() {
  atexit(ErrorStack_dispose);
  g_stack = (ErrorStack)Buffer_alloc(sizeof(struct s_ErrorStack));
  List_Option opt = {1, (Buffer_Free)Error_dispose};
  g_stack->errors = List_create(opt);
}
void ErrorStack_push(Error error) { List_insert_tail(g_stack->errors, error); }
int ErrorStack_empty() { return List_size(g_stack->errors) == 0; }
void ErrorStack_print() {
  for (List_Node node = List_head(g_stack->errors);
       node != List_tail(g_stack->errors); node = List_next(node)) {
    Error error = (Error)List_get(node);
    Error_print(error);
  }
}