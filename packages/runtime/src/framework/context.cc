#include "framework/context.hpp"
using namespace atom::runtime;
context::context() { _scope = new scope(); }
context::~context() { delete _scope; }
scope *context::get_scope() { return _scope; }
scope *context::push_scope() {
  scope *old = _scope;
  _scope = new scope(old);
  return old;
}
void context::pop_scope(scope *s){delete _scope;_scope = s;}
