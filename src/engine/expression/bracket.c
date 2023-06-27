#include "expression.h"
int isBracketExpression(SourceFile file, Token token) {
  if (!checkToken(token, TT_Symbol, "(")) {
    return 0;
  }
  return 1;
}
Expression readBracketExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!checkToken(token, TT_Symbol, "(")) {
    pushError("Unexcept token.missing token '('",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ExpressionContext ectx = pushExpressionContext();
  Expression expr = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!expr) {
    goto failed;
  }
  selector = expr->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ")")) {
    pushError("Unexcept token.missing token ')'",
              getLocation(file, token->raw.end));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  Expression bracket_expr = Expression_create();
  bracket_expr->type = ET_Bracket;
  bracket_expr->bind = BT_None;
  bracket_expr->binary.left = expr;
  bracket_expr->node->position.begin = source;
  bracket_expr->node->position.end = selector;
  return bracket_expr;
failed:
  if (expr) {
    Expression_dispose(expr);
  }
  return NULL;
}