#include "runtime/include/scope.hpp"
#include "runtime/include/object.hpp"
#include "runtime/include/value.hpp"
#include <algorithm>
using namespace atom::runtime;
scope::scope(scope *parent) {
  _stack = new stack(parent ? parent->_stack : nullptr);
}
scope::~scope() {
  while (_values.size()) {
    auto value = *_values.begin();
    remove(value);
  };
  delete _stack;
}
stack *scope::get_stack() { return _stack; }
void scope::remove(value *value) {
  auto it = std::find(_values.begin(), _values.end(), value);
  if (it != _values.end()) {
    _values.erase(it);
    if (value->_scope) {
      delete value;
    }
  }
}
void scope::connect_value(value *value) {
  value->_scope = this;
  _values.push_back(value);
}
value *scope::create(value *source) {
  switch (source->get_type()) {
  case VT_UNDEFINED:
    return create_undefined();
  case VT_NULL:
    return create_null();
  case VT_STRING:
    return create(source->get_string());
  case VT_NUMBER:
    return create(source->get_number());
  case VT_INTEGER:
    return create(source->get_integer());
  case VT_BOOLEAN:
    return create(source->get_boolean());
  default: {
    value *val = new value();
    connect_value(val);
    val->_handle = source->_handle;
    _stack->add_handle(source->_handle);
    return val;
  }
  }
}
value *scope::create(const std::string &data) {
  value *val = new value();
  connect_value(val);
  val->_handle = new handle(
      _stack, new value::type_base(VT_STRING, new value::string(data)));
  return val;
}
value *scope::create(const char *data) {
  value *val = new value();
  connect_value(val);
  val->_handle = new handle(
      _stack, new value::type_base(VT_STRING, new value::string(data)));
  return val;
}
value *scope::create(const double &data) {
  value *val = new value();
  connect_value(val);
  val->_handle = new handle(
      _stack, new value::type_base(VT_NUMBER, new value::number(data)));
  return val;
}
value *scope::create(const int32_t &data) {
  value *val = new value();
  connect_value(val);
  val->_handle = new handle(
      _stack, new value::type_base(VT_INTEGER, new value::integer(data)));
  return val;
}
value *scope::create(const bool &data) {
  value *val = new value();
  connect_value(val);
  val->_handle = new handle(
      _stack, new value::type_base(VT_BOOLEAN, new value::boolean(data)));
  return val;
}
value *scope::create_undefined() {
  value *val = new value();
  connect_value(val);
  val->_handle =
      new handle(_stack, new value::type_base(VT_UNDEFINED, nullptr));
  return val;
}
value *scope::create_null() {
  value *val = new value();
  connect_value(val);
  val->_handle = new handle(_stack, new value::type_base(VT_NULL, nullptr));
  return val;
}
value *scope::create(handle *handle) {
  value *val = new value;
  connect_value(val);
  val->_handle = handle;
  _stack->add_handle(handle);
  return val;
}
value *scope::create_object() {
  value *val = new value();
  connect_value(val);
  handle *hobject = new handle(_stack, nullptr);
  hobject->set_object(new value::type_base(VT_OBJECT, new object(hobject)));
  val->_handle = hobject;
  return val;
}
