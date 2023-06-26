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
  List_Option opt = {1, (Buffer_Free)Statement_dispose};
  statement->export.exports = List_create(opt);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (checkToken(token, TT_Symbol, "{")) {
    selector = token->raw.end;
    Token_dispose(token);
    for (;;) {
      Expression expr = readIdentifierExpression(file, selector);
      if (!expr) {
        goto failed;
      }
      Statement st = Statement_create();
      st->type = ST_Expression;
      st->node->position = expr->node->position;
      st->expression = expr;
      List_insert_tail(statement->export.exports, st);
      selector = st->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, ",")) {
        selector = token->raw.end;
        Token_dispose(token);
      } else if (checkToken(token, TT_Symbol, "}")) {
        selector = token->raw.end;
        Token_dispose(token);
        break;
      } else {
        pushError("Unexcept token.missing token '}'",
                  getLocation(file, token->raw.begin));
        Token_dispose(token);
        goto failed;
      }
    }
  } else {
    if (checkToken(token, TT_Keyword, "default")) {
      statement->export.isDefault = 1;
      selector = token->raw.end;
    }
    Token_dispose(token);
    Statement st = readStatement(file, selector);
    if (!st) {
      goto failed;
    }
    selector = st->node->position.end;
    List_insert_tail(statement->export.exports, st);
  }
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  Statement_dispose(statement);
  return NULL;
}