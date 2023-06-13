#include "statement.h"
Statement readExpressionStatement(SourceFile file, cstring source) {
  ExpressionContext ectx = pushExpressionContext();
  Expression expr = readExpression(file, source);
  popExpressionContext(ectx);
  if (!expr) {
    return NULL;
  }
  Statement state = Statement_create();
  state->type = ST_Expression;
  state->node->position.begin = source;
  state->node->position.end = expr->node->position.end;
  state->expression = expr;
  return state;
}