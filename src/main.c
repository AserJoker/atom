#include "compiler/compiler.h"
#include "engine/atom.h"
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
  JS_AtomGroup group = JS_AtomGroup_create(NULL);
  // JS_AtomGroup scope2 = JS_AtomGroup_create(NULL);
  JS_Atom o1 = JS_Atom_create(group);
  JS_Atom o2 = JS_Atom_create(group);
  List_insert_tail(o2->refs, o1);
  // Buffer_dispose(scope2);
  Buffer_dispose(group);
  // Error error = Error_get();
  // if (error) {
  //   Error_print(error);
  //   Buffer_dispose(error);
  // }
  return 0;
}