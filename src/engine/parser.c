#include "parser.h"
#include "expression.h"
Expression parse(SourceFile file) {
  ExpressionContext ectx = pushExpressionContext();
  Expression expr = readExpression(file, file->_source);
  popExpressionContext(ectx);
  return expr;
}