#include "engine/include/value/scope.hpp"
#include "engine/include/value/variable.hpp"
#include <iostream>
#include <optional>
using namespace atom::engine;
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
  _chunk->add_node(n);
  return v;
}
scope::~scope() {
  while (_variables.size()) {
    auto *val = *_variables.begin();
    delete val;
  }
  delete _chunk;
}
scope::scope(scope *s) { _chunk = new chunk(s ? s->_chunk : nullptr); }
void scope::remove_variable(variable *v) {
  v->_scope = nullptr;
  _chunk->remove_node(v->_node);
  for (auto it = _variables.begin(); it != _variables.end(); it++) {
    if (*it == v) {
      _variables.erase(it);
      break;
    }
  }
}
chunk *scope::get_chunk() { return _chunk; }
