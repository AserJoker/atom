#include "expression.h"
int isAwaitExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "await");
}
Expression readAwaitExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ExpressionContext ectx = pushExpressionContext();
  setMaxOperatorLevel(-1);
  Expression expr = readExpression(file, selector);
  popExpressionContext(ectx);
  if (expr) {
    Expression ex = Expression_create();
    ex->level = -1;
    ex->type = ET_Await;
    ex->await = expr;
    ex->node->position.begin = source;
    ex->node->position.end = expr->node->position.end;
    return ex;
  }
  return NULL;
}