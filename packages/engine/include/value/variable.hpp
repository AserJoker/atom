#pragma once
#include "core/include/object.hpp"
#include "engine/include/memory/node.hpp"
#include "engine/include/value/base_variable.hpp"
#include <string>
namespace atom::engine {
class scope;
class variable : public core::object {
private:
  friend class scope;

private:
  node *_node;
  variable(node *, variable * = nullptr);
  scope *_scope;
  variable *_owner;

public:
  base_variable *get_data();
  ~variable();
  node *get_node();
  variable *get_owner();
};
} // namespace atom::engine
