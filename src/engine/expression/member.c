#include "expression.h"
#include "source.h"
#include "strings.h"
#include "tokenizer.h"
int isMemberOperator(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, ".");
}
Expression readMemberExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  Expression expr = Expression_create();
  expr->binary.operator= token;
  expr->binary.bind = BT_Both;
  expr->type = ET_Calculate;
  expr->node->position = token->raw;
  return expr;
}