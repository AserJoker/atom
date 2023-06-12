#pragma once
#include "source.h"

typedef struct s_AstNode *AstNode;
typedef enum e_NodeType {
  NT_Unknown,
  NT_Program,
  NT_Statement,
  NT_Expression,
  NT_Pattern
} NodeType;
AstNode AstNode_create();
void AstNode_dispose(AstNode node);
struct s_AstNode {
  NodeType type;
  strings position;
};
