#include "framework/context.hpp"
using namespace atom::runtime;
context::context(runtime *rt) : _runtime(rt) {
  _scope = new scope(rt->get_scope());
}
context::~context() { delete _scope; }
scope *context::get_scope() { return _scope; }
scope *context::push_scope() {
  scope *old = _scope;
  _scope = new scope(old);
  return old;
}
void context::pop_scope(scope *s) {
  delete _scope;
  _scope = s;
}
variable *context::undefined() {
  return get_scope()->create_variable(_runtime->undefined());
}
variable *context::null(){return get_scope()->create_variable(_runtime->null());}
