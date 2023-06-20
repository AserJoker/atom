#include "statement.h"
int isLabelStatement(SourceFile file, Token token) {
  if (token->type == TT_Identifier) {
    Token next = readTokenSkipNewline(file, token->raw.end);
    if (!next) {
      return 0;
    }
    if (checkToken(next, TT_Symbol, ":")) {
      Token_dispose(next);
      return 1;
    }
    Token_dispose(next);
  }
  return 0;
}
Statement readLabelStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Statement statement = Statement_create();
  statement->type = ST_Label;
  statement->labelStatement.label = NULL;
  statement->labelStatement.body = NULL;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  statement->labelStatement.label = token;
  selector = token->raw.end;
  token = readTokenSkipNewline(file, selector);
  if (!checkToken(token, TT_Symbol, ":")) {
    pushError("Unexcept token.missing token ':'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    goto failed;
  }
  selector = token->raw.end;
  Token_dispose(token);
  statement->labelStatement.body = readStatement(file, selector);
  if (!statement->labelStatement.body) {
    goto failed;
  }
  selector = statement->labelStatement.body->node->position.end;
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}