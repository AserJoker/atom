#include "engine/include/framework/context.hpp"
#include "engine/include/value/object_variable.hpp"
using namespace atom::engine;
using namespace atom;
context::context(const core::auto_release<runtime> &rt) : _runtime(rt) {
  _scope = new scope(rt->get_scope());
  _object_prototype = object_variable::create(this, null());
  _function_prototype = object_variable::create(this,_object_prototype);
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
variable *context::null() {
  return get_scope()->create_variable(_runtime->null());
}
const core::auto_release<class runtime> &context::get_runtime() {
  return _runtime;
}
variable *context::object_prototype() { return _object_prototype; }
variable *context::function_prototype() { return _function_prototype; }
