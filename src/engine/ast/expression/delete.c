#include "ast.h"
Expression DeleteExpression_create() {
  Expression expr = Expression_create();
  expr->node->type = NT_DeleteExpression;
  expr->level = -1;
  return expr;
}
Expression readDeleteExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Keyword, "delete")) {
    Token_dispose(token);
    ErrorStack_push(
        Error_init("Unexcept token.", getLocation(file, source), NULL));
    return NULL;
  }
  Token_dispose(token);
  Expression delete_expr = DeleteExpression_create();
  return delete_expr;
}
void DeleteExpression_dispose(Expression expr) {
  if (expr->deleteexpr.expression) {
    Expression_dispose(expr->deleteexpr.expression);
  }
  AstNode_dispose(expr->node);
  Buffer_free(expr);
}