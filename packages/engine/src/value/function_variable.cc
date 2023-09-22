#include "engine/include/value/function_variable.hpp"
#include "engine/include/value/string_variable.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <iostream>
using namespace atom::engine;
variable *function_variable::call(context *ctx, variable *func, variable *self,
                                  const std::vector<variable *> &args,
                                  const context::call_frame &frame) {

  auto *fn = dynamic_cast<function_variable *>(func->get_data());
  if (fn->_bind) {
    self = ctx->create(fn->_bind);
  }
  auto &callee = fn->_callee;
  auto *scope = ctx->push_scope();
  auto *owner = self == nullptr ? func->get_owner() : self;
  std::string call_func_name = frame._funcname;
  if (owner) {
    auto constructor = base_variable::get(ctx, owner, "constructor");
    auto name = base_variable::get(ctx, constructor, "name");
    auto funcname = base_variable::get(ctx, func, "name");
    call_func_name = fmt::format("{}.{}", string_variable::value_of(name),
                                 string_variable::value_of(funcname));
    delete funcname;
    delete name;
    delete constructor;
  } else {
    auto funcname = base_variable::get(ctx, func, "name");
    call_func_name = fmt::format("{}", string_variable::value_of(funcname));
    delete funcname;
  }
  ctx->push_call_stack({._filename = frame._filename,
                        ._funcname = call_func_name,
                        ._line = frame._line,
                        ._column = frame._column});
  variable *res = callee(ctx, self == nullptr ? owner : self, args);
  variable *result = scope->create_variable(res->get_node());
  ctx->pop_call_stack();
  ctx->pop_scope(scope);
  return result;
}
function_variable::~function_variable() {}
function_variable::function_variable(
    const function_variable::function_handle &handle)
    : object_variable(), _callee(std::move(handle)), _prototype(nullptr),
      _bind(nullptr) {
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
  if (name == "name") {
    return false;
  }
  return object_variable::set_field(ctx, obj, name, value);
}
variable *function_variable::get_field(context *ctx, variable *obj,
                                       const std::string &name) {
  if (name == "prototype") {
    auto fn = dynamic_cast<function_variable *>(obj->get_data());
    return ctx->create(fn->_prototype);
  }
  if (name == "name") {
    auto fn = dynamic_cast<function_variable *>(obj->get_data());
    return string_variable::create(ctx, fn->_name);
  }
  return object_variable::get_field(ctx, obj, name);
}
variable *function_variable::bind(context *ctx, variable *func, variable *obj) {
  auto result = ctx->assigment(func);
  auto fn = dynamic_cast<function_variable *>(result->get_data());
  if (!fn->_bind) {
    fn->_bind = obj->get_node();
    result->get_node()->add_node(fn->_bind);
  }
  return result;
}

bool function_variable::set_field(context *ctx, variable *obj,
                                  const uint32_t &index, variable *field) {
  return object_variable::set_field(ctx, obj, index, field);
};
variable *function_variable::get_field(context *ctx, variable *obj,
                                       const uint32_t &index) {
  return object_variable::get_field(ctx, obj, index);
}