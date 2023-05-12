#include "tree.h"
struct s_Tree_Node {
  Tree_Node _root;
  Buffer _data;
  List _children;
  Tree _tree;
};
struct s_Tree {
  Tree_Node _root;
  uint32_t _size;
  Tree_Option _option;
};

static void Tree_Node_dispose(Tree_Node node) {
  if (node->_tree->_option.auto_free_data && node->_data) {
    node->_tree->_option.dispose(node->_data);
  }
  if (node->_children) {
    List_dispose(node->_children);
  }
  Buffer_free(node);
}

static Tree_Node Tree_Node_create() {
  List_Option opt;
  opt.auto_free_data = 1;
  opt.dispose = (Buffer_Free)Tree_Node_dispose;
  Tree_Node node = (Tree_Node)Buffer_alloc(sizeof(struct s_Tree_Node));
  node->_children = List_create(opt);
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
List Tree_getChildren(Tree_Node node) { return node->_children; }

Tree_Node Tree_getRoot(Tree_Node node) { return node->_root; }

Tree_Node Tree_insert(Tree tree, Tree_Node pos, Buffer data) {
  if (pos->_root) {
    Tree_Node node = Tree_Node_create();
    node->_data = data;
    node->_root = pos->_root;
    node->_tree = tree;
    Tree_Node root = pos->_root;
    for (List_Node it = List_head(root->_children);
         it != List_tail(root->_children); it = List_next(it)) {
      if (List_get(it) == pos) {
        List_insert(root->_children, it, node);
        return node;
      }
    }
  }
  return NULL;
}

int Tree_remove(Tree tree, Tree_Node node) {
  if (node->_root) {
    Tree_Node root = node->_root;
    List children = root->_children;
    for (List_Node it = List_head(children); it != List_tail(children);
         it = List_next(it)) {
      Tree_Node c = (Tree_Node)List_get(it);
      if (c == node) {
        List_remove(children, it);
        return 1;
      }
    }
  }
  return 0;
}
Tree_Node Tree_find(Tree tree, Finder_t finder) {
  List_Option opt = {0, NULL};
  List list = List_create(opt);
  List_insert_tail(list, tree->_root);
  while (!List_empty(list)) {
    List_Node it = List_head(list);
    Tree_Node node = (Tree_Node)List_get(it);
    if (finder(node->_data)) {
      return node;
    }
    for (List_Node cit = List_head(node->_children);
         cit != List_tail(node->_children); cit = List_next(cit)) {
      List_insert_tail(list, cit);
    }
  }
  return NULL;
}
Tree_Node Tree_insertChildren(Tree tree, Tree_Node root, Buffer data) {
  Tree_Node node = Tree_Node_create();
  node->_data = data;
  node->_root = root;
  node->_tree = tree;
  List_insert_tail(root->_children, node);
  return node;
}
uint32_t Tree_size(Tree tree) { return tree->_size; }
int Tree_empty(Tree tree) { return tree->_size == 0; }