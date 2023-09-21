#include "engine/include/value/function_variable.hpp"
#include <algorithm>
#include <iostream>
using namespace atom::engine;
variable *function_variable::call(context *ctx, variable *func, variable *self,
                                  const std::vector<variable *> &args) {
  auto *fn = dynamic_cast<function_variable *>(func->get_data());
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
    : object_variable(), _callee(std::move(handle)), _prototype(nullptr) {
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
  if (!prototype) {
    prototype = object_variable::create(ctx);
  }
  fn->_prototype = prototype->get_node();
  auto *val = ctx->get_scope()->create_variable(fn);
  val->get_node()->add_node(fn->_proto);
  val->get_node()->add_node(fn->_prototype);
  return val;
}
bool function_variable::set_field(context *ctx, variable *obj,
                                  const std::string &name, variable *value) {
  if (name == "prototype") {
    obj->get_node()->add_node(value->get_node());
    auto fn = dynamic_cast<function_variable *>(obj);
    if (fn->_prototype) {
      obj->get_node()->remove_node(fn->_prototype);
    }
    fn->_prototype = value->get_node();
    return true;
  }
  return object_variable::set_field(ctx, obj, name, value);
}
variable *function_variable::get_field(context *ctx, variable *obj,
                                       const std::string &name) {
  if (name == "prototype") {
    auto fn = dynamic_cast<function_variable *>(obj->get_data());
    return ctx->create(fn->_prototype);
  }
  return object_variable::get_field(ctx, obj, name);
}