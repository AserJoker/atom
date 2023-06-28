#include "parser/parser.h"
#include "util/error.h"
#include "util/source.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _DEBUG
#include "debug/debug.h"
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
  Program program = parse(sf);
  if (program) {
    JSON_Value val = JSON_fromProgram(program);
    Program_dispose(program);
    cstring json = JSON_stringlify(val);
    JSON_dispose(val);
    printf("%s\n", json);
    Buffer_free(json);
  }
  Error error = getError();
  if (error) {
    printError(error);
    Error_dispose(error);
  }
  SourceFile_dispose(sf);
  return 0;
}