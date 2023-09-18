#include "engine/include/framework/context.hpp"
#include "engine/include/value/object_variable.hpp"
#include "engine/include/value/simple_variable.hpp"
using namespace atom::engine;
using namespace atom;
context::context(const core::auto_release<runtime> &rt) : _runtime(rt) {
  _scope = new scope();
  _undefined = _scope->create_variable(new undefined_variable());
  _null = _scope->create_variable(new null_variable());
  _object_prototype = object_variable::create(this, null());
  _function_prototype = object_variable::create(this, _object_prototype);
}
context::~context() { pop_scope(nullptr); }
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
  return _scope->create_variable(_undefined->get_node());
}
variable *context::null() { return _scope->create_variable(_null->get_node()); }
const core::auto_release<class runtime> &context::get_runtime() {
  return _runtime;
}
variable *context::object_prototype() { return _object_prototype; }
variable *context::function_prototype() { return _function_prototype; }
