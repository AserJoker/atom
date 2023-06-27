#include "parser/ast.h"
#include "parser/expression.h"
#include "util/buffer.h"

AstNode AstNode_create() {
  AstNode node = (AstNode)Buffer_alloc(sizeof(struct s_AstNode));
  node->type = NT_Unknown;
  node->position.begin = 0;
  node->position.end = 0;
  return node;
}
void AstNode_dispose(AstNode node) { Buffer_free(node); }