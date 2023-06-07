#include "ast.h"
static AstContext g_context = NULL;
AstContext pushAstContext() {
  AstContext result = g_context;
  g_context = (AstContext)Buffer_alloc(sizeof(struct s_AstContext));
  g_context->tContext = pushTokenContext();
  g_context->maxLevel = -1;
  return result;
}
void popAstContext(AstContext ctx) {
  popTokenContext(g_context->tContext);
  Buffer_free(g_context);
  g_context = ctx;
}

void enableCommaTail() { setMaxOperatorLevel(11); }
void disableCommaTail() { setMaxOperatorLevel(-1); }

void setMaxOperatorLevel(int max) { g_context->maxLevel = max; }
int getMaxOperatorLevel() { return g_context->maxLevel; }

AstNode AstNode_create() {
  AstNode node = (AstNode)Buffer_alloc(sizeof(struct s_AstNode));
  node->position.begin = 0;
  node->position.end = 0;
  return node;
}
void AstNode_dispose(AstNode node) { Buffer_free(node); }

Program parse(SourceFile file) {
  AstContext ctx = pushAstContext();
  Program program = readProgram(file, file->_source);
  if (program) {
    JSON_Value json = JSON_fromProgram(program);
    Program_dispose(program);
    cstring s_json = JSON_stringlify(json);
    JSON_dispose(json);
    printf("%s\n", s_json);
    Buffer_free(s_json);
  }
  popAstContext(ctx);
  return NULL;
}
