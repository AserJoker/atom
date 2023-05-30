#include "ast.h"
void IdentifierExpression_dispose(Expression expression) {
  Identifier_dispose(expression->identifier);
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
void Identifier_dispose(Identifier identifier) {
  Token_dispose(identifier->raw);
  AstNode_dispose(identifier->node);
  Buffer_free(identifier);
}
Identifier Identifier_create() {
  Identifier identifier = (Identifier)Buffer_alloc(sizeof(struct s_Identifier));
  identifier->raw = NULL;
  identifier->node = AstNode_create();
  identifier->node->type = NT_Identifier;
  return identifier;
}

Identifier readIdentifier(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (token->type != TT_Identifier) {
    Token_dispose(token);
    return NULL;
  }
  Identifier identifier = Identifier_create();
  identifier->raw = token;
  identifier->node->position = token->raw;
  return identifier;
}


Expression readIdentifierExpression(SourceFile file, cstring source) {
  Identifier identifier = readIdentifier(file, source);
  if (!identifier) {
    return NULL;
  }
  Expression expr = Expression_create();
  expr->node->type = NT_IdentifierExpression;
  expr->node->position = identifier->node->position;
  expr->identifier = identifier;
  return expr;
}