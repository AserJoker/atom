#include "expression.h"
#include "ast.h"
#include "tokenizer.h"
Expression Expression_create() {
  Expression expression = (Expression)Buffer_alloc(sizeof(struct s_Expression));
  expression->node = AstNode_create();
  expression->node->type = NT_Expression;
  expression->type = ET_Unknown;
  expression->binary.left = NULL;
  expression->binary.right = NULL;
  expression->binary.operator= NULL;
  return expression;
}
Expression readExpression(SourceFile file, cstring source) { return NULL; }
void Expression_dispose(Expression expression) {
  AstNode_dispose(expression->node);
  Buffer_free(expression);
}