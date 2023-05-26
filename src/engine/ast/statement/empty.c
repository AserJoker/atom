#include "ast.h"
void EmptyStatement_dispose(Statement statement) {
  Statement_dispose(statement);
}
Statement EmptyStatement_create() {
  Statement statement = Statement_create();
  statement->node->type = NT_EmptyStatement;
  return statement;
}
Statement readEmptyStatement(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (Token_check(token, TT_Symbol, ";")) {
    Statement statement = EmptyStatement_create();
    statement->node->position.begin = source;
    statement->node->position.end = token->raw.end;
    Token_dispose(token);
    return statement;
  }
  Token_dispose(token);
  return NULL;
}