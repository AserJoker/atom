#include "ast.h"
Statement Statement_create() {
  Statement statement = (Statement)Buffer_alloc(sizeof(struct s_Statement));
  statement->node = AstNode_create();
  statement->node->position.begin = 0;
  statement->node->position.end = 0;

  statement->block.body = NULL;

  return statement;
}

void Statement_dispose(Statement statement) {
  if (statement->node->type == NT_BlockStatement) {
    return BlockStatement_dispose(statement);
  }
  if (statement->node->type == NT_EmptyStatement) {
    return EmptyStatement_dispose(statement);
  }
  if (statement->node->type == NT_ExpressionStatement) {
    return ExpressionStatement_dispose(statement);
  }
}

Statement readStatement(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (token->type == TT_Eof) {
    Token_dispose(token);
    return NULL;
  }
  if (Token_check(token, TT_Symbol, ";")) {
    Token_dispose(token);
    return readEmptyStatement(file, source);
  } else if (Token_check(token, TT_Symbol, "{")) {
    Token_dispose(token);
    return readBlockStatement(file, source);
  } else {
    Token_dispose(token);
    return readExpressionStatement(file, source);
  }
}