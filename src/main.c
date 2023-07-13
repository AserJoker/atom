#include "compiler/compiler.h"
#include "engine/atom.h"
#include "engine/value.h"
#include "util/error.h"
#include "util/source.h"
#include "util/strings.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _DEBUG
#include "debug/debug.h"
#endif

void Dispose_int(int *val) { printf("%d", *val); }

int main(int argc, char **argv) {

#ifdef _DEBUG
  Debug_init();
#endif

#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, NULL, _IOFBF, 1000);
#endif

  //   SourceFile sf = SourceFile_read("./demo.js");
  //   AstNode node = JS_compile(sf);
  // #ifdef _DEBUG
  //   if (node) {
  //     JSON_Value value = JSON_fromAstNode(node);
  //     cstring str = JSON_stringlify(value);
  //     Buffer_dispose(value);
  //     printf("%s\n", str);
  //     Buffer_dispose(str);
  //   }
  // #endif
  //   Buffer_dispose(node);
  //   Buffer_dispose(sf);
  JS_Scope scope = JS_Scope_create(NULL);
  JS_Scope scope2 = JS_Scope_create(scope);
  
  JS_Atom str =
      JS_Scope_define(scope, "str", (JS_Value)JS_createString("hello world"));
  
  JS_Atom str2 = JS_Scope_checkout(scope2, "str");
  cstring s = NULL;
  if (JS_getString(str2, &s)) {
    printf("%s\n",s);
  }
  Buffer_dispose(scope);
  // Error error = Error_get();
  // if (error) {
  //   Error_print(error);
  //   Buffer_dispose(error);
  // }
  return 0;
}