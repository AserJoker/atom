#include "ast.h"

void EmptyStatement_dispose(Statement statement) {
  AstNode_dispose(statement->_node);
  Buffer_free(statement);
}

Statement EmptyStatement_create() {
  Statement statement = (Statement)Buffer_alloc(sizeof(struct s_Statement));
  statement->_node = AstNode_create();
  statement->_node->_type = NT_EmptyStatement;
  return statement;
}
Statement readEmptyStatement(SourceFile file, cstring source) {
  Statement empty_s = EmptyStatement_create();
  empty_s->_node->_position.begin = source;
  Token token = readTokenSkipNewline(file, source);
  empty_s->_node->_position.end = token->_raw.end;
  Token_dispose(token);
  return empty_s;
}