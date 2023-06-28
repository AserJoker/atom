#include "util/error.h"
#include "util/source.h"

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

  SourceFile sf = SourceFile_read("./demo.js");
  Error error = Error_get();
  if (error) {
    Error_print(error);
    Buffer_dispose(error);
  }
  Buffer_dispose(sf);
  return 0;
}