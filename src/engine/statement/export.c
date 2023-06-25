#include "statement.h"

int isExportStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Keyword, "export");
}
Statement readExportStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  selector = token->raw.end;
  Token_dispose(token);
  Statement statement = Statement_create();
  statement->type = ST_Export;
  statement->export.isDefault = 0;
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (checkToken(token, TT_Keyword, "default")) {
    statement->export.isDefault = 1;
    selector = token->raw.end;
  }
  Token_dispose(token);
  statement->export.statement = readStatement(file, selector);
  if (!statement->export.statement) {
    goto failed;
  }
  selector = statement->export.statement->node->position.end;
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}