#pragma once
#include "core/include/object.hpp"
#include "engine/include/memory/node.hpp"
#include "engine/include/value/base_variable.hpp"
namespace atom::engine {
class scope;
class variable : public core::object {
private:
  friend class scope;

private:
  node *_node;
  variable(node *);
  scope *_scope;

public:
  base_variable *get_data();
  ~variable();
  node *get_node();
};
} // namespace atom::engin
