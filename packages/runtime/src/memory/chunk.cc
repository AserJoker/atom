#include "memory/chunk.hpp"
#include "memory/node.hpp"
#include <fmt/core.h>
using namespace atom::runtime;
chunk::chunk(chunk *parent) {
  _parent = parent;
  if (_parent != nullptr) {
    _parent->_children.push_back(this);
  }
}
chunk::~chunk() {
  for (auto &c : _children) {
    c->_parent = nullptr;
  }
  _children.clear();
  for (auto &n : _nodes) {
    std::erase(n->get_chunks(), this);
    n->auto_release();
  }
  _nodes.clear();
  if (_parent != nullptr) {
    std::erase(_parent->_children, this);
  }
}
void chunk::add_node(node *n) {
  _nodes.push_back(n);
  n->get_chunks().push_back(this);
}
void chunk::remove_node(node *n) {
  for (auto it = _nodes.begin(); it != _nodes.end(); it++) {
    if (*it == n) {
      _nodes.erase(it);
      break;
    }
  }
  auto &chunks = n->get_chunks();
  for (auto it = chunks.begin(); it != chunks.end(); it++) {
    if (*it == this) {
      chunks.erase(it);
      break;
    }
  }
  n->auto_release();
}
