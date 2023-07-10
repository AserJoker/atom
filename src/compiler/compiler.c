#include "compiler/compiler.h"
AstNode compile(SourceFile file) {
  AstNode node = AstNode_read(file, file->_source);
  return node;
}