#include "statement.h"
int isAssignmentStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "const") ||
         checkToken(token, TT_Keyword, "let") ||
         checkToken(token, TT_Keyword, "var");
}
Statement readAssignmentStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  Statement statement = Statement_create();
  statement->type = ST_Assignment;
  statement->assignement.type = AT_Unknown;
  if (checkToken(token, TT_Keyword, "const")) {
    statement->assignement.type = AT_Const;
  } else if (checkToken(token, TT_Keyword, "let")) {
    statement->assignement.type = AT_Let;
  } else if (checkToken(token, TT_Keyword, "var")) {
    statement->assignement.type = AT_Var;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ExpressionContext ectx = pushExpressionContext();
  statement->assignement.expression = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!statement->assignement.expression) {
    Buffer_free(statement);
  } else {
    selector = statement->assignement.expression->node->position.end;
    statement->node->position.begin = source;
    statement->node->position.end = selector;
  }
  return statement;
}