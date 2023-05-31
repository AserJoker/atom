#include "ast.h"
Expression AwaitExpression_create() {
  Expression expr = Expression_create();
  expr->node->type = NT_AwaitExpression;
  expr->level = -1;
  return expr;
}
void AwaitExpression_dispose(Expression expression) {
  if (expression->await.expression) {
    Expression_dispose(expression->await.expression);
  }
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}
Expression readAwaitExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Keyword, "await")) {
    Token_dispose(token);
    ErrorStack_push(
        Error_init("Unexcept token.", getLocation(file, source), NULL));
    return NULL;
  }
  Token_dispose(token);
  Expression await_expr = AwaitExpression_create();
  return await_expr;
}