#include "ast.h"
Statement ExpressionStatement_create() {
  Statement statement = Statement_create();
  statement->node->type = NT_ExpressionStatement;
  statement->expression.expression = NULL;
  return statement;
}
void ExpressionStatement_dispose(Statement statement) {
  if (statement->expression.expression) {
    Expression_dispose(statement->expression.expression);
  }
  AstNode_dispose(statement->node);
  Buffer_free(statement);
}
Statement readExpressionStatement(SourceFile file, cstring source) {
  Expression expression = readExpression(file, source);
  if (!expression) {
    return NULL;
  }
  cstring selector = expression->node->position.end;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    Expression_dispose(expression);
  }
  if (Token_check(token, TT_Symbol, ";")) {
    selector = token->raw.end;
  }
  Token_dispose(token);
  Statement statement = ExpressionStatement_create();
  statement->expression.expression = expression;
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
}