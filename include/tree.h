#pragma once
#include "buffer.h"
#include "list.h"
struct s_Tree_Option {
  int auto_free_data;
  Buffer_Free dispose;
};

typedef struct s_Tree_Option Tree_Option;

struct s_Tree;
struct s_Tree_Node;

typedef struct s_Tree *Tree;
typedef struct s_Tree_Node *Tree_Node;

Tree Tree_create(Tree_Option opt);
void Tree_dispose(Tree tree);

Tree_Node Tree_root(Tree tree);

Buffer Tree_get(Tree_Node node);
Buffer Tree_set(Tree_Node node, Buffer data);

List Tree_getChildren(Tree_Node node);

Tree_Node Tree_getRoot(Tree_Node node);
Tree_Node Tree_insert(Tree tree, Tree_Node pos, Buffer data);
Tree_Node Tree_insertChildren(Tree tree, Tree_Node root, Buffer data);
int Tree_remove(Tree tree, Tree_Node node);
Tree_Node Tree_find(Tree tree, Finder_t finder);

uint32_t Tree_size(Tree tree);
int Tree_empty(Tree tree);