#include "ast.h"
void BinaryExpression_dispose(Expression expression) {
  if (expression->binary.left) {
    Expression_dispose(expression->binary.left);
  }
  if (expression->binary.right) {
    Expression_dispose(expression->binary.right);
  }
  if (expression->binary.operator) {
    Token_dispose(expression->binary.operator);
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}