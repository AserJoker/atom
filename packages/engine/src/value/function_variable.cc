#include "engine/include/value/function_variable.hpp"
#include <algorithm>
#include <iostream>
using namespace atom::engine;
variable *function_variable::call(context *ctx, variable *func, variable *self,
                                  const std::vector<variable *> &args) {
  auto *fn = (function_variable *)func->get_data();
  auto &callee = fn->_callee;
  auto *scope = ctx->push_scope();
  variable *res = callee(ctx, self, args);
  variable *result = scope->create_variable(res->get_node());
  ctx->pop_scope(scope);
  return result;
}
function_variable::~function_variable() {}
function_variable::function_variable(
    const function_variable::function_handle &handle)
    : object_variable(), _callee(std::move(handle)) {
  _type = variable_type::VT_FUNCTION;
}
variable *function_variable::create(context *ctx, const std::string &name,
                                    int32_t length,
                                    const function_handle &handle,
                                    variable *prototype) {
  auto *fn = new function_variable(handle);
  fn->_name = name;
  fn->_length = length;
  fn->_proto = ctx->function_prototype()->get_node();
  auto *val = ctx->get_scope()->create_variable(fn);
  val->get_node()->add_node(fn->_proto);
  if (prototype) {
    object_variable::define(val, "prototype", prototype);
  } else {
    auto proto = object_variable::create(ctx);
    object_variable::define(proto, "constructor", val);
    object_variable::define(val, "prototype", proto);
  }
  return val;
}
