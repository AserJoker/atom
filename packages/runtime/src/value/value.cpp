#include "runtime/include/value/value.hpp"
#include "runtime/include/value/function.hpp"
#include <iostream>
using namespace atom::runtime;
value::type_base::type_base(value_type type, base *data) {
  _type = type;
  _data = data;

}
value::type_base::~type_base() {
  if (_data) {
    delete _data;
  }
}
value_type value::type_base::get_type() { return _type; }
base *value::type_base::get_data() { return _data; }
value_type value::get_type() {
  type_base *tbase = (type_base *)_handle->get_object();
  return tbase->get_type();
}
base *value::get_data() {
  type_base *tbase = (type_base *)_handle->get_object();
  return tbase->get_data();
}
value::~value() {
  auto scope = _scope;
  _scope = nullptr;
  scope->remove(this);
  if (_handle) {
    scope->get_stack()->remove_handle(_handle);
  }
}
handle *value::get_handle() { return _handle; }
std::string &value::get_string() {
  string *vdata = (string *)get_data();
  return vdata->data;
}
double &value::get_number() {
  number *vdata = (number *)get_data();
  return vdata->data;
}
int32_t &value::get_integer() {
  integer *vdata = (integer *)get_data();
  return vdata->data;
}
bool &value::get_boolean() {
  boolean *vdata = (boolean *)get_data();
  return vdata->data;
}
object *value::get_object() { return (object *)get_data(); }

function *value::get_function() { return (function *)get_data(); }
