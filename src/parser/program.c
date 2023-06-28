#include "parser/program.h"
#include "parser/statement.h"
Program readProgram(SourceFile file, cstring source) {
  cstring selector = source;
  Program program = (Program)Buffer_alloc(sizeof(struct s_Program));
  program->node = AstNode_create();
  program->node->type = NT_Program;
  List_Option opt = {1, (Buffer_Free)Statement_dispose};
  program->body = List_create(opt);
  for (;;) {
    Statement st = readStatement(file, selector);
    if (!st) {
      goto failed;
    }
    List_insert_tail(program->body, st);
    selector = st->node->position.end;
    Token token = readTokenSkipNewline(file, selector);
    if (token->type == TT_Eof) {
      Token_dispose(token);
      break;
    }
    Token_dispose(token);
  }
  program->node->position.begin = source;
  program->node->position.end = selector;
  return program;
failed:
  Program_dispose(program);
  return NULL;
}
void Program_dispose(Program program) {
  List_dispose(program->body);
  Buffer_free(program->node);
  Buffer_free(program);
}