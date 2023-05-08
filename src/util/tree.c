#include "tree.h"
struct s_Tree_Node {
  Tree_Node _root;
  Buffer _data;
  DList _children;
  Tree _tree;
};
struct s_Tree {
  Tree_Node _root;
  size_t _size;
  Tree_Option _option;
};

static void Tree_Node_dispose(Tree_Node node) {
  if (node->_tree->_option.auto_free_data && node->_data) {
    node->_tree->_option.dispose(node->_data);
  }
  if (node->_children) {
    DList_dispose(node->_children);
  }
  Buffer_free(node);
}

static Tree_Node Tree_Node_create() {
  DList_Option opt;
  opt.auto_free_data = 1;
  opt.dispose = (Buffer_Free)Tree_Node_dispose;
  Tree_Node node = (Tree_Node)Buffer_alloc(sizeof(struct s_Tree_Node));
  node->_children = DList_create(opt);
  node->_data = NULL;
  node->_root = NULL;
  return node;
}

Tree Tree_create(Tree_Option opt) {
  Tree tree = (Tree)Buffer_alloc(sizeof(struct s_Tree));
  tree->_root = Tree_Node_create();
  tree->_size = 0;
  tree->_option = opt;
  tree->_root->_tree = tree;
  return tree;
}

void Tree_dispose(Tree tree) {
  Tree_Node_dispose(tree->_root);
  Buffer_free(tree);
}

Tree_Node Tree_root(Tree tree) { return tree->_root; }

Buffer Tree_get(Tree_Node node) { return node->_data; }
Buffer Tree_set(Tree_Node node, Buffer data) {
  Buffer old = node->_data;
  node->_data = data;
  return old;
}
DList Tree_getChildren(Tree_Node node) { return node->_children; }

Tree_Node Tree_getRoot(Tree_Node node) { return node->_root; }

Tree_Node Tree_insert(Tree tree, Tree_Node pos, Buffer data) {
  if (pos->_root) {
    Tree_Node node = Tree_Node_create();
    node->_data = data;
    node->_root = pos->_root;
    node->_tree = tree;
    Tree_Node root = pos->_root;
    for (DList_Node it = DList_head(root->_children);
         it != DList_tail(root->_children); it = DList_next(it)) {
      if (DList_get(it) == pos) {
        DList_insert(root->_children, it, node);
        return node;
      }
    }
  }
  return NULL;
}

int Tree_remove(Tree tree, Tree_Node node) {
  if (node->_root) {
    Tree_Node root = node->_root;
    DList children = root->_children;
    for (DList_Node it = DList_head(children); it != DList_tail(children);
         it = DList_next(it)) {
      Tree_Node c = (Tree_Node)DList_get(it);
      if (c == node) {
        DList_remove(children, it);
        return 1;
      }
    }
  }
  return 0;
}
Tree_Node Tree_find(Tree tree, Finder_t finder) {
  DList_Option opt = {0, NULL};
  DList list = DList_create(opt);
  DList_insert_tail(list, tree->_root);
  while (!DList_empty(list)) {
    DList_Node it = DList_head(list);
    Tree_Node node = (Tree_Node)DList_get(it);
    if (finder(node->_data)) {
      return node;
    }
    for (DList_Node cit = DList_head(node->_children);
         cit != DList_tail(node->_children); cit = DList_next(cit)) {
      DList_insert_tail(list, cit);
    }
  }
  return NULL;
}
Tree_Node Tree_insertChildren(Tree tree, Tree_Node root, Buffer data) {
  Tree_Node node = Tree_Node_create();
  node->_data = data;
  node->_root = root;
  node->_tree = tree;
  DList_insert_tail(root->_children, node);
  return node;
}
size_t Tree_size(Tree tree) { return tree->_size; }
int Tree_empty(Tree tree) { return tree->_size == 0; }