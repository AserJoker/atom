#pragma once
#include "core/include/auto_release.hpp"
#include <list>
namespace atom::runtime {
class node;
class chunk : public core::object {
private:
  std::list<chunk *> _children;
  std::list<node *> _nodes;
  chunk *_parent;

public:
  chunk(chunk * = nullptr);
  ~chunk();
  void add_node(node *);
  void remove_node(node *);
};
} // namespace atom::runtime
