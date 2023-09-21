#include "engine/include/value/base_variable.hpp"
#include "engine/include/value/boolean_variable.hpp"
#include "engine/include/value/function_variable.hpp"
#include "engine/include/value/integer_variable.hpp"
#include "engine/include/value/number_variable.hpp"
#include "engine/include/value/object_variable.hpp"
#include "engine/include/value/simple_variable.hpp"
#include "engine/include/value/string_variable.hpp"
#include "engine/include/value/variable.hpp"
using namespace atom::engine;
base_variable::base_variable(const variable_type &vt) : _type(vt) {}
base_variable::~base_variable() {}
const base_variable::variable_type &base_variable::get_type() const {
  return _type;
}
const base_variable::variable_type &base_variable::type_of(variable *v) {
  auto *vdata = v->get_data();
  return vdata->get_type();
}
bool base_variable::set(context *ctx, variable *obj, const std::string &name,
                        variable *value) {
  auto type = obj->get_data()->get_type();
  switch (type) {
  case base_variable::variable_type::VT_NUMBER:
    return number_variable::set_field(ctx, obj, name, value);
  case base_variable::variable_type::VT_INTEGER:
    return integer_variable::set_field(ctx, obj, name, value);
  case base_variable::variable_type::VT_STRING:
    return string_variable::set_field(ctx, obj, name, value);
  case base_variable::variable_type::VT_BOOLEAN:
    return boolean_variable::set_field(ctx, obj, name, value);
  case base_variable::variable_type::VT_OBJECT:
    return object_variable::set_field(ctx, obj, name, value);
  case base_variable::variable_type::VT_ARRAY:
  case base_variable::variable_type::VT_FUNCTION:
    return function_variable::set_field(ctx, obj, name, value);
  default:
    return false;
  }
}
variable *base_variable::get(context *ctx, variable *obj,
                             const std::string &name) {
  auto type = obj->get_data()->get_type();
  switch (type) {
  case base_variable::variable_type::VT_NUMBER:
    return number_variable::get_field(ctx, obj, name);
  case base_variable::variable_type::VT_INTEGER:
    return integer_variable::get_field(ctx, obj, name);
  case base_variable::variable_type::VT_STRING:
    return string_variable::get_field(ctx, obj, name);
  case base_variable::variable_type::VT_BOOLEAN:
    return boolean_variable::get_field(ctx, obj, name);
  case base_variable::variable_type::VT_OBJECT:
    return object_variable::get_field(ctx, obj, name);
  case base_variable::variable_type::VT_ARRAY:
  case base_variable::variable_type::VT_FUNCTION:
    return function_variable::get_field(ctx, obj, name);
  default:
    return ctx->undefined();
  }
}