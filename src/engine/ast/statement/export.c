#include "ast.h"
Statement ExportStatement_create() {
  Statement statement = Statement_create();
  statement->node->type = NT_ExportStatement;
  statement->export_.statement = NULL;
  return statement;
}
Statement readExportStatement(SourceFile file, cstring source) {
  Statement statement = ExportStatement_create();
  cstring selector = source;
  Token token = readTokenSkipNewline(file, selector);
  if (!token) {
    goto failed;
  }
  if (!Token_check(token, TT_Keyword, "export")) {
    Token_dispose(token);
    ErrorStack_push(Error_init("Unexcept token.missing token 'export'",
                               getLocation(file, selector), NULL));
  }
  selector = token->raw.end;
  Token_dispose(token);
  Statement body = readStatement(file, selector);
  if (!body) {
    goto failed;
  }
  selector = body->node->position.end;
  statement->export_.statement = body;
  statement->node->position.begin = source;
  statement->node->position.end = selector;
  return statement;
failed:
  ExportStatement_dispose(statement);
  return NULL;
}
void ExportStatement_dispose(Statement statement) {
  if (statement->export_.statement) {
    Statement_dispose(statement->export_.statement);
  }
  AstNode_dispose(statement->node);
  Buffer_free(statement);
}