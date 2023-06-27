#include "parser/statement.h"
int isDoWhileStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "do");
}
Statement readDoWhileStatement(SourceFile file, cstring source) {
  cstring selector = source;

  Statement statement = readStatement(file, selector);
  statement->type = ST_DoWhile;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  statement->whileStatement.body = readBlockStatement(file, selector);
  if (!statement->whileStatement.body) {
    goto failed;
  }
  selector = statement->whileStatement.body->node->position.end;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Keyword, "while")) {
    pushError("Unexcept token.missing token 'while'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipNewline(file, selector);
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
  if (checkToken(token, TT_Symbol, ")")) {
    pushError("Unexcept token.missing token ')'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}