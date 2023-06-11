#include "ast.h"
#include "buffer.h"
AstNode AstNode_create() {
  AstNode node = (AstNode)Buffer_alloc(sizeof(struct s_AstNode));
  node->type = NT_Unknown;
  node->location._position._column = 0;
  node->location._position._line = 0;
  return node;
}
void AstNode_dispose(AstNode node) { Buffer_free(node); }