#pragma once
#include "core/include/object.hpp"
#include "runtime/include/memory/node.hpp"
#include "runtime/include/value/base_variable.hpp"
namespace atom::runtime {
class scope;
class variable  {
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
} // namespace atom::runtime
