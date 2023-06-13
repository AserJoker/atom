#include "statement.h"
Statement Statement_create() {
  Statement statement = (Statement)Buffer_alloc(sizeof(struct s_Statement));
  statement->node = AstNode_create();
  statement->type = ST_Unknown;
  statement->node->type = NT_Statement;
  statement->block.body = NULL;
  return statement;
}
Statement readStatement(SourceFile file, cstring source) {
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    return NULL;
  }
  Statement statement = NULL;
  if (checkToken(token, TT_Symbol, ";")) {
    Token_dispose(token);
    statement = readEmptyStatement(file, source);
  } else if (checkToken(token, TT_Symbol, "{")) {
    Token_dispose(token);
    statement = readBlockStatement(file, source);
  } else {
    Token_dispose(token);
    statement = readExpressionStatement(file, source);
  }
  if (statement) {
    selector = statement->node->position.end;
    token = readTokenSkipNewline(file, selector);
    if (checkToken(token, TT_Symbol, ";")) {
      selector = token->raw.end;
    }
    Token_dispose(token);
    statement->node->position.end = selector;
  }
  return statement;
}
void Statement_dispose(Statement statement) {
  switch (statement->type) {

  case ST_Block:
    List_dispose(statement->block.body);
    break;
  case ST_Expression:
    Expression_dispose(statement->expression);
    break;
  default:
    break;
  }
  AstNode_dispose(statement->node);
  Buffer_free(statement);
}