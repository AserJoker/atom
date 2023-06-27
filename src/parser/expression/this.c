#include "parser/expression.h"
int isThisExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "this");
}
Expression readThisExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  Expression expr = Expression_create();
  expr->type = ET_This;
  expr->bind = BT_None;
  expr->node->position = token->raw;
  Token_dispose(token);
  return expr;
}