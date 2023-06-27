#include "expression.h"
int isDeleteExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "delete");
}
Expression readDeleteExpression(SourceFile file, cstring source) {
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
    ex->type = ET_Delete;
    ex->deleteExpr = expr;
    ex->bind = BT_Right;
    ex->node->position.begin = source;
    ex->node->position.end = expr->node->position.end;
    return ex;
  }
  return NULL;
}