#include "ast.h"
#include "error.h"
#include "json.h"
#include "list.h"
#include "source.h"
#include "tokenizer.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _DEBUG
#include "debug.h"
#endif

int main(int argc, char **argv) {

#ifdef _DEBUG
  Debug_init();
#endif
  
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, NULL, _IOFBF, 1000);
#endif
  
  SourceFile sf = SourceFile_read("./demo.js");
  ErrorStack_init();
  parse(sf);
  if (!ErrorStack_empty()) {
    ErrorStack_print();
  }
  SourceFile_dispose(sf);
  return 0;
}