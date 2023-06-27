#include "parser/statement.h"
int isContinueStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "continue");
}
Statement readContinueStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  token = readTokenSkipComment(file, selector);
  if (!token) {
    return NULL;
  }
  Statement statement = Statement_create();
  statement->type = ST_Continue;
  if (token->type == TT_Identifier) {
    statement->label = token;
    if (!statement->label) {
      Buffer_free(statement);
      return NULL;
    }
    selector = token->raw.end;
  } else {
    Token_dispose(token);
  }
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
}