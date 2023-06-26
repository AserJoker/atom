#include "statement.h"
int isForStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "for");
}
Statement readForStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Statement statement = Statement_create();
  statement->type = ST_For;
  statement->forStatement.after = NULL;
  statement->forStatement.condition = NULL;
  statement->forStatement.init = NULL;
  statement->forStatement.body = NULL;
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

  
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}