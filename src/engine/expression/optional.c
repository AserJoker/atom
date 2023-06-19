#include "expression.h"
int isOptionalExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, "?.");
}
Expression readOptionalExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Expression expr = NULL;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Token next = readTokenSkipNewline(file, selector);
  if (!next) {
    Token_dispose(token);
    goto failed;
  }
  if (isComputeExpression(file, next)) {
    Token_dispose(token);
    Token_dispose(next);
    expr = readComputeExpression(file, selector);
    if (!expr) {
      goto failed;
    }
    expr->type = ET_OptionalCompute;
    selector = expr->node->position.end;
  } else if (isCallExpression(file, next)) {
    Token_dispose(token);
    Token_dispose(next);
    expr = readCallExpression(file, selector);
    if (!expr) {
      goto failed;
    }
    expr->type = ET_OptionalCall;
    selector = expr->node->position.end;
  } else {
    Token_dispose(next);
    expr = Expression_create();
    expr->type = ET_Calculate;
    expr->binary.operator= token;
  }
  expr->node->position.begin = source;
  expr->node->position.end = selector;
  return expr;
failed:
  if (expr) {
    Expression_dispose(expr);
  }
  return NULL;
}