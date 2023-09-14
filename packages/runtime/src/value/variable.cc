#include "value/variable.hpp"
#include "value/scope.hpp"
using namespace atom::runtime;
base_variable *variable::get_data() { return _node->get_data(); }
variable::~variable() {
  if (_scope) {
    _scope->remove_variable(this);
  }
}
variable::variable(node *n) { _node = n; }
node *variable::get_node() { return _node; }
