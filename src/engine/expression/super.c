#include "expression.h"
int isSuperExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "super");
}
Expression readSuperExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  Expression expr = Expression_create();
  expr->type = ET_Super;
  expr->node->position = token->raw;
  Token_dispose(token);
  return expr;
}