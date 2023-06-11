#include "error.h"
#include "source.h"

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
  Error error = getError();
  if (error) {
    printError(error);
    Error_dispose(error);
  }
  SourceFile_dispose(sf);
  return 0;
}