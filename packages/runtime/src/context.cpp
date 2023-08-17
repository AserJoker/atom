#include "runtime/include/context.hpp"
using namespace atom::runtime;
context::context() { _scope = new scope(); }
context::~context() { delete _scope; }
scope *context::get_scope() { return _scope; }
void context::push_scope() {
  auto *sp = _scope;
  _scopes.push_front(_scope);
  _scope = new scope(sp);
}
void context::pop_scope() {
  delete _scope;
  _scope = *_scopes.begin();
  _scopes.pop_front();
}
