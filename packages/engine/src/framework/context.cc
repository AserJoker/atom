#include "engine/include/framework/context.hpp"
#include "engine/include/lib/object_helper.hpp"
#include "engine/include/value/function_variable.hpp"
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
  _object_constructor = function_variable::create(
      this, "Object", 1,
      [](context *ctx, variable *self, const std::vector<variable *> &args)
          -> variable * { return ctx->undefined(); },
      _object_prototype);
  _function_constructor = function_variable::create(
      this, "Function", 1,
      [](context *ctx, variable *self, const std::vector<variable *> &args)
          -> variable * { return ctx->undefined(); },
      _function_prototype);

  object_helper::init_object(this);
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
variable *context::object_constructor() { return _object_constructor; }
variable *context::function_constructor() { return _function_constructor; }
variable *context::create(auto... args) {
  return get_scope()->create_variable(args...);
}
variable *context::assigment(variable *val) {
  switch (base_variable::type_of(val)) {
  case base_variable::variable_type::VT_NUMBER:
    return number_variable::create(this, number_variable::value_of(val));
  case base_variable::variable_type::VT_INTEGER:
    return integer_variable::create(this, integer_variable::value_of(val));
  case base_variable::variable_type::VT_STRING:
    return string_variable::create(this, string_variable::value_of(val));
  case base_variable::variable_type::VT_BOOLEAN:
    return boolean_variable::create(this, boolean_variable::value_of(val));
  case base_variable::variable_type::VT_UNDEFINED:
    return undefined();
  case base_variable::variable_type::VT_NULL:
    return null();
  default:
    return create(val->get_node());
  }
}