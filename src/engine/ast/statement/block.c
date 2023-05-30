#include "ast.h"
void BlockStatement_dispose(Statement statement) {
  if (statement->block.body) {
    List_dispose(statement->block.body);
  }
  AstNode_dispose(statement->node);
  Buffer_free(statement);
}

Statement BlockStatement_create() {
  Statement statement = Statement_create();
  statement->node->type = NT_BlockStatement;
  List_Option opt = {1, (Buffer_Free)Statement_dispose};
  statement->block.body = List_create(opt);
  return statement;
}

Statement readBlockStatement(SourceFile file, cstring source) {
  Token token = readTokenSkipNewline(file, source);
  if (!token) {
    return NULL;
  }
  if (!Token_check(token, TT_Symbol, "{")) {
    Token_dispose(token);
    return NULL;
  }
  cstring selector = token->raw.end;
  Token_dispose(token);

  Statement block = BlockStatement_create();
  token = readTokenSkipNewline(file, selector);
  if (!Token_check(token, TT_Symbol, "}")) {
    Token_dispose(token);
    Statement statement = readStatement(file, selector);
    if (!statement) {
      goto failed;
    }
    for (;;) {
      List_insert_tail(block->block.body, statement);
      selector = statement->node->position.end;
      token = readTokenSkipNewline(file, selector);
      if (!token) {
        goto failed;
      }
      if (Token_check(token, TT_Symbol, "}")) {
        break;
      }
      Token_dispose(token);
      statement = readStatement(file, selector);
      if (!statement) {
        goto failed;
      }
    }
  }
  selector = token->raw.end;
  Token_dispose(token);
  block->node->position.begin = source;
  block->node->position.end = selector;
  return block;
failed:
  BlockStatement_dispose(block);
  return NULL;
}
