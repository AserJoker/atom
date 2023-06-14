#include "statement.h"
int isEmptyStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, ";") || token->type == TT_Eof;
}
Statement readEmptyStatement(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (token->type != TT_Eof) {
    if (!checkToken(token, TT_Symbol, ";")) {
      pushError("Unexcept token.missing token ';'",
                getLocation(file, token->raw.begin));
      Token_dispose(token);
      return NULL;
    }
  }
  Token_dispose(token);
  cstring selector = token->raw.end;
  Statement statement = Statement_create();
  statement->type = ST_Empty;
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
}