#include "statement.h"
int isWhileStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "while");
}
Statement readWhileStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Statement statement = Statement_create();
  statement->type = ST_While;
  statement->whileStatement.body = NULL;
  statement->whileStatement.condition = NULL;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, "(")) {
    pushError("Unexcept token.missing token '('",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  ExpressionContext ectx = pushExpressionContext();
  statement->whileStatement.condition = readExpression(file, selector);
  popExpressionContext(ectx);
  if (!statement->whileStatement.condition) {
    goto failed;
  }
  selector = statement->whileStatement.condition->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, ")")) {
    pushError("Unexcept token.missing token ')'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  statement->whileStatement.body = readStatement(file, selector);
  if (!statement->whileStatement.body) {
    goto failed;
  }
  selector = statement->whileStatement.body->node->position.end;
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}