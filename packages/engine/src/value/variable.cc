#include "engine/include/value/variable.hpp"
#include "engine/include/value/scope.hpp"
using namespace atom::engine;
base_variable *variable::get_data() { return _node->get_data(); }
variable::~variable() {
  if (_scope) {
    _scope->remove_variable(this);
  }
}
variable::variable(node *n, variable *owner) : _owner(owner) { _node = n; }
node *variable::get_node() { return _node; }
variable *variable::get_owner() { return _owner; }