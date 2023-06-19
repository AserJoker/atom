#include "error.h"
#include "expression.h"
#include "source.h"
#include "strings.h"
#include "tokenizer.h"
int isComputeExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, "[");
}
Expression readComputeExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Expression expr = Expression_create();
  expr->type = ET_Compute;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, "[")) {
    pushError("Unexcept token.missing token '['",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ExpressionContext ectx = pushExpressionContext();
  expr->compute.key = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!expr->compute.key) {
    goto failed;
  }
  selector = expr->compute.key->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, "]")) {
    pushError("Unexcept token.missing token ']'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  expr->node->position.begin = source;
  expr->node->position.end = selector;
  return expr;
failed:
  Expression_dispose(expr);
  return NULL;
}