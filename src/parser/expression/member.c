#include "parser/expression.h"
#include "util/source.h"
#include "util/strings.h"
#include "parser/tokenizer.h"
int isMemberOperator(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, ".");
}
Expression readMemberExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  Expression expr = Expression_create();
  expr->binary.operator= token;
  expr->bind = BT_Both;
  expr->type = ET_Calculate;
  expr->node->position = token->raw;
  return expr;
}