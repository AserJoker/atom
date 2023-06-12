#include "expression.h"
int isLiteralToken(Token token) {
  if (token->type == TT_String || token->type == TT_Number ||
      token->type == TT_Regex || token->type == TT_BigInt) {
    return 1;
  }
  if (Token_check(token, TT_Keyword, "true") ||
      Token_check(token, TT_Keyword, "false") ||
      Token_check(token, TT_Keyword, "undefined") ||
      Token_check(token, TT_Keyword, "null")) {
    return 1;
  }
  return 0;
}

Expression readLiteralExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!isLiteralToken(token)) {
    Token_dispose(token);
    pushError("Unexcept token.", getExpressionLocation(file, source));
    goto failed;
  }
  Expression expr = Expression_create();
  expr->type = ET_Literal;
  expr->literal = token;
  expr->node->position.begin = token->raw.begin;
  expr->node->position.end = token->raw.end;
  return expr;
failed:
  Expression_dispose(expr);
  return NULL;
}