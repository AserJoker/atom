#include "parser/parser.h"
#include "parser/expression.h"
#include "parser/statement.h"
Program parse(SourceFile file) {
  ExpressionContext ectx = pushExpressionContext();
  Program program = readProgram(file, file->_source);
  popExpressionContext(ectx);
  return program;
}