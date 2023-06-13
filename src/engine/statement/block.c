#include "statement.h"
int isBlockStatement(SourceFile file, Token token) {
  return checkToken(token, TT_Symbol, "{");
}
Statement readBlockStatement(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!checkToken(token, TT_Symbol, "{")) {
    pushError("Unexcept token.missing token '{'",
              getLocation(file, token->raw.begin));
    Token_dispose(token);
    return NULL;
  }
  cstring selector = token->raw.end;
  Token_dispose(token);
  Statement statement = Statement_create();
  statement->type = ST_Block;
  List_Option opt = {1, (Buffer_Free)Statement_dispose};
  statement->block.body = List_create(opt);
  token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!checkToken(token, TT_Symbol, "}")) {
    Token_dispose(token);
    Statement item = readStatement(file, selector);
    if (!item) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(statement->block.body, item);
      selector = item->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (checkToken(token, TT_Symbol, "}")) {
        break;
      } else if (token->type == TT_Eof) {
        Token_dispose(token);
        goto failed;
      }
      Token_dispose(token);
      item = readStatement(file, selector);
      if (!item) {
        goto failed;
      }
    }
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