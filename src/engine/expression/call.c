#include "expression.h"
#include "source.h"
#include "strings.h"
#include "tokenizer.h"
int isCallExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, "(");
}
Expression readCallExpression(SourceFile file, cstring source) {
  Expression expr = Expression_create();
  expr->type = ET_Call;
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  expr->node->position.begin = source;
  expr->node->position.end = selector;
  return expr;
failed:
  Expression_dispose(expr);
  return NULL;
}