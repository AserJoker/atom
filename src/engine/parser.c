#include "parser.h"
#include "expression.h"
#include "statement.h"
Statement parse(SourceFile file) {
  ExpressionContext ectx = pushExpressionContext();
  Statement statement = readStatement(file, file->_source);
  popExpressionContext(ectx);
  return statement;
}