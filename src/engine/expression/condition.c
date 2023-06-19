#include "expression.h"

int isConditionExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, "?");
}
Expression readConditionExpression(SourceFile file, cstring source) {
  Expression expr = Expression_create();
  expr->type = ET_Condition;
  expr->condition.condition = NULL;
  expr->condition.consequent = NULL;
  expr->condition.alternate = NULL;
  expr->level = 10;
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ExpressionContext ectx = pushExpressionContext();
  setMaxOperatorLevel(12);
  expr->condition.consequent = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!expr->condition.consequent) {
    goto failed;
  }
  selector = expr->condition.consequent->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ":")) {
    pushError("Unexcept token,missing token ':'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ectx = pushExpressionContext();
  setMaxOperatorLevel(12);
  expr->condition.alternate = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!expr->condition.alternate) {
    goto failed;
  }
  selector = expr->condition.alternate->node->position.end;
  expr->node->position.end = selector;
  return expr;
failed:
  Expression_dispose(expr);
  return NULL;
}