#include "expression.h"

int isIdentifierToken(Token token) { return token->type == TT_Identifier; }
Expression readIdentifierExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (token->type != TT_Identifier) {
    Token_dispose(token);
    pushError("Unexcept token.", getExpressionLocation(file, selector));
    return NULL;
  }
  Expression expression = Expression_create();
  expression->level = -2;
  expression->Identifier = token;
  expression->type = ET_Identifier;
  expression->node->position = token->raw;
  return expression;
failed:
  Expression_dispose(expression);
  return NULL;
}