
#include "ast.h"
Expression ComputeExpression_create() {
  Expression expr = Expression_create();
  expr->level = -2;
  expr->node->type = NT_ComputeExpression;
  return expr;
}
void ComputeExpression_dispose(Expression expr) {
  if (expr->compute.left) {
    Expression_dispose(expr->compute.left);
  }
  if (expr->compute.right) {
    Expression_dispose(expr->compute.right);
  }
  AstNode_dispose(expr->node);
  Buffer_free(expr);
}

Expression readComputeExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, "[")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("failed to read compute expression",
                               getLocation(file, source), NULL));
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  Expression value = readExpression(file, selector);
  if (!value) {
    return NULL;
  }
  selector = value->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Symbol, "]")) {
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  Expression compute_expr = ComputeExpression_create();
  compute_expr->compute.right = value;
  compute_expr->node->position.begin = source;
  compute_expr->node->position.end = selector;
  return compute_expr;
failed:
  Expression_dispose(value);
  return NULL;
}