#include "ast.h"
Expression NewExpression_create() {
  Expression expr = Expression_create();
  expr->node->type = NT_NewExpression;
  expr->level = -1;
  return expr;
}
Expression readNewExpression(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Keyword, "new")) {
    Token_dispose(token);
    ErrorStack_push(
        Error_init("Unexcept token.", getLocation(file, source), NULL));
    return NULL;
  }
  Token_dispose(token);
  Expression new_expr = NewExpression_create();
  return new_expr;
}
void NewExpression_dispose(Expression expr) {
  if (expr->newexpr.expression) {
    Expression_dispose(expr->newexpr.expression);
  }
  AstNode_dispose(expr->node);
  Buffer_free(expr);
}