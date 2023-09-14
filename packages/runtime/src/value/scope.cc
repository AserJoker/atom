#include "runtime/include/value/scope.hpp"
#include "runtime/include/value/variable.hpp"
#include <iostream>
#include <optional>
using namespace atom::runtime;
using namespace atom;
variable *scope::create_variable(base_variable *data) {
  variable *v = new variable(new node(_chunk, data));
  _variables.push_back(v);
  v->_scope = this;
  return v;
}
variable *scope::create_variable(node *n) {
  variable *v = new variable(n);
  _variables.push_back(v);
  v->_scope = this;
  return v;
}
scope::~scope() { delete _chunk; }
scope::scope(scope *s) { _chunk = new chunk(s ? s->_chunk : nullptr); }
void scope::remove_variable(variable *v) {
  v->_scope = nullptr;
  _chunk->remove_node(v->_node);
  std::erase(_variables, v);
}
