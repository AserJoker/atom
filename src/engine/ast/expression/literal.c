#include "ast.h"
void Literal_dispose(Literal literal) {
  Token_dispose(literal->raw);
  AstNode_dispose(literal->node);
  Buffer_free(literal);
}
Literal Literal_create() {
  Literal literal = (Literal)Buffer_alloc(sizeof(struct s_Literal));
  literal->node = AstNode_create();
  literal->node->type = NT_Literal;
  literal->node->position.begin = 0;
  literal->node->position.end = 0;
  literal->raw = NULL;
  return literal;
}
void LiteralExpression_dispose(Expression expression) {
  Literal_dispose(expression->literal);
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}

int isLiteralToken(Token token) {
  if (token->type == TT_String || token->type == TT_Number ||
      token->type == TT_BigInt || token->type == TT_Regex) {
    return 1;
  }
  if (Token_check(token, TT_Keyword, "true") ||
      Token_check(token, TT_Keyword, "false") ||
      Token_check(token, TT_Keyword, "null") ||
      Token_check(token, TT_Keyword, "undefined")) {
    return 1;
  }
  return 0;
}
Literal readLiteral(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (!isLiteralToken(token)) {
    Token_dispose(token);
    return NULL;
  }
  Literal literal = Literal_create();
  literal->raw = token;
  literal->node->type = NT_Literal;
  literal->node->position = token->raw;
  return literal;
}

Expression readLiteralExpression(SourceFile file, cstring source) {
  Literal literal = readLiteral(file, source);
  if (!literal) {
    return NULL;
  }
  Expression literal_expr = Expression_create();
  literal_expr->level = -2;
  literal_expr->literal = literal;
  literal_expr->node->type = NT_LiteralExpression;
  literal_expr->node->position = literal->node->position;
  return literal_expr;
}