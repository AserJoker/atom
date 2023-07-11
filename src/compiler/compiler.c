#include "compiler/compiler.h"
AstNode JS_compile(SourceFile file) {
  AstNode node = JS_Compile_read(file, file->_source);
  return node;
}