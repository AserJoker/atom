#include "ast.h"
#include "dlist.h"
#include "json.h"
#include "tree.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef _DEBUG
#include "debug.h"
#endif

int main(int argc, char **argv) {
#ifdef _DEBUG
  // Debug_init();
#endif
  SourceFile sf = SourceFile_read("./demo.js");
  parse(sf);
  SourceFile_dispose(sf);
  return 0;
}