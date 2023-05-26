#include "ast.h"
Program Program_create() {
  Program program = (Program)Buffer_alloc(sizeof(struct s_Program));
  List_Option opt = {1, (Buffer_Free)Statement_dispose};
  program->body = List_create(opt);
  program->node = AstNode_create();
  program->node->type = NT_Program;
  return program;
}

void Program_dispose(Program program) {
  AstNode_dispose(program->node);
  List_dispose(program->body);
  Buffer_free(program);
}

Program readProgram(SourceFile file, cstring source) {
  Program program = Program_create();
  cstring selector = source;
  Statement statement = readStatement(file, selector);
  if (!statement) {
    goto failed;
  }
  for (;;) {
    List_insert_tail(program->body, statement);
    selector = statement->node->position.end;
    Token token = readTokenSkipNewline(file, selector);
    if (!token) {
      goto failed;
    }
    if (token->type == TT_Eof) {
      Token_dispose(token);
      break;
    }
    Token_dispose(token);
    statement = readStatement(file, selector);
    if (!statement) {
      goto failed;
    }
  }
  return program;
failed:
  Program_dispose(program);
  return NULL;
}