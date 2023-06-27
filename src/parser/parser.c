#include "parser/parser.h"
#include "parser/expression.h"
#include "parser/statement.h"
Statement parse(SourceFile file) {
  ExpressionContext ectx = pushExpressionContext();
  Statement statement = readStatement(file, file->_source);
  popExpressionContext(ectx);
  return statement;
}