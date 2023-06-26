#include "expression.h"
int isAssignmentExpression(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "const") ||
         checkToken(token, TT_Keyword, "let") ||
         checkToken(token, TT_Keyword, "var");
}
Expression readAssignmentExpression(SourceFile file, cstring source) {
  cstring selector = source;
  Expression expr = Expression_create();
  expr->type = ET_Assignment;
  expr->assignment.expression = NULL;
  expr->assignment.type = NULL;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  expr->assignment.type = token;
  selector = token->raw.end;
  expr->assignment.expression = readExpression(file, selector);
  if (!expr->assignment.expression) {
    goto failed;
  }
  expr->node->position.begin = source;
  expr->node->position.end = selector;
  return expr;
failed:
  Expression_dispose(expr);
  return NULL;
}