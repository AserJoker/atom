#include "ast.h"
void BracketExpression_dispose(Expression expression) {
  if (expression->bracket.expression) {
    Expression_dispose(expression->bracket.expression);
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}

Expression readBracketExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, "(")) {
    Token_dispose(token);
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  AstContext current = pushAstContext();
  Expression expression = readExpression(file, selector);
  popAstContext(current);
  if (!expression) {
    return NULL;
  }
  selector = expression->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    Expression_dispose(expression);
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, ")")) {
    Token_dispose(token);
    Expression_dispose(expression);
    ErrorStack_push(Error_init("Except ')' after expression",
                               getLocation(file, selector), NULL));
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  Expression bracket_expr = Expression_create();
  bracket_expr->level = -2;
  bracket_expr->node->type = NT_BracketExpression;
  bracket_expr->node->position.begin = source;
  bracket_expr->node->position.end = selector;
  bracket_expr->bracket.expression = expression;
  return bracket_expr;
}