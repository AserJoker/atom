#include "engine/include/value/array_variable.hpp"
#include "engine/include/value/integer_variable.hpp"
using namespace atom::engine;
array_variable::~array_variable() {}
array_variable::array_variable() : object_variable() {
  _type = base_variable::variable_type::VT_ARRAY;
}
variable *array_variable::create(context *ctx,
                                 const std::vector<variable *> &items) {
  array_variable *arr = new array_variable();
  arr->_proto = ctx->array_prototype()->get_node();

  variable *val = ctx->create(arr);
  val->get_node()->add_node(arr->_proto);
  for (auto &item : items) {
    arr->_items.push_back(item->get_node());
    val->get_node()->add_node(item->get_node());
  }
  return val;
}

bool array_variable::set_field(context *ctx, variable *val,
                               const std::string &name, variable *field) {
  auto arr = dynamic_cast<array_variable *>(val->get_data());
  if (name == "length") {
    auto length = integer_variable::value_of(field);
    if (length < (int64_t)arr->_items.size()) {
      for (auto index = length; index < (int64_t)arr->_items.size(); index++) {
        val->get_node()->remove_node(arr->_items[index]);
      }
      arr->_items.resize(length);
    } else if (length > (int64_t)arr->_items.size()) {
      while (arr->_items.size() != length) {
        arr->_items.push_back(ctx->null()->get_node());
        val->get_node()->add_node(ctx->null()->get_node());
      }
    }
    return true;
  }
  return object_variable::set_field(ctx, val, name, field);
}
variable *array_variable::get_field(context *ctx, variable *val,
                                    const std::string &name) {
  auto arr = dynamic_cast<array_variable *>(val->get_data());
  if (name == "length") {
    return integer_variable::create(ctx, arr->_items.size());
  }
  return object_variable::get_field(ctx, val, name);
}

bool array_variable::set_field(context *ctx, variable *val,
                               const uint32_t &index, variable *item) {
  auto arr = dynamic_cast<array_variable *>(val->get_data());
  if (index >= arr->_items.size()) {
    while (arr->_items.size() < index) {
      arr->_items.push_back(ctx->null()->get_node());
      val->get_node()->add_node(ctx->null()->get_node());
    }
  }
  val->get_node()->add_node(item->get_node());
  val->get_node()->remove_node(arr->_items[index]);
  arr->_items[index] = item->get_node();
  return true;
}
variable *array_variable::get_field(context *ctx, variable *val,
                                    const uint32_t &index) {
  auto arr = dynamic_cast<array_variable *>(val->get_data());
  if (index <= arr->_items.size()) {
    return ctx->create(arr->_items[index]);
  }
  return ctx->null();
}