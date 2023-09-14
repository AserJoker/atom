#include "runtime/include/value/object_variable.hpp"
#include "value/function_variable.hpp"
#include "value/simple_variable.hpp"
using namespace atom::runtime;
object_variable::object_variable() : base_variable(variable_type::VT_OBJECT) {}
object_variable::~object_variable() {}
bool object_variable::define(variable *value, const std::string &name,
                             variable *field, bool configurable, bool writable,
                             bool enumable) {
  object_variable *obj = (object_variable *)value->get_data();
  for (auto &[k, v] : obj->_properties) {
    if (k.field == name) {
      return false;
    }
  }
  key k = {.field = name, .symbol = nullptr};
  property prop = {.value = field->get_node(),
                   .getter = nullptr,
                   .setter = nullptr,
                   .configurable = configurable,
                   .writable = writable,
                   .enumable = enumable};
  value->get_node()->add_node(field->get_node());
  obj->_properties[k] = prop;
  return true;
}
bool object_variable::define(variable *value, const std::string &name,
                             variable *getter, variable *setter,
                             bool configurable, bool writable, bool enumable) {
  object_variable *obj = (object_variable *)value->get_data();
  for (auto &[k, v] : obj->_properties) {
    if (k.field == name) {
      return false;
    }
  }
  key k = {.field = name, .symbol = nullptr};
  property prop = {.value = nullptr,
                   .getter = getter->get_node(),
                   .setter = setter ? setter->get_node() : nullptr,
                   .configurable = configurable,
                   .writable = writable,
                   .enumable = enumable};
  value->get_node()->add_node(getter->get_node());
  if (setter) {
    value->get_node()->add_node(setter->get_node());
  }
  obj->_properties[k] = prop;
  return true;
}
bool object_variable::set(context *ctx, variable *value,
                          const std::string &name, variable *field,
                          bool configurable, bool writable, bool enumable) {
  object_variable *obj = (object_variable *)value->get_data();
  for (auto &[k, v] : obj->_properties) {
    if (k.field == name) {
      if (v.value) {
        value->get_node()->add_node(field->get_node());
        value->get_node()->remove_node(v.value);
      } else {
        if (v.setter) {
          auto *setter = ctx->get_scope()->create_variable(v.setter);
          auto *result = function_variable::call(ctx, setter, value, {field});
          delete setter;
          bool blresult =
              dynamic_cast<boolean_variable *>(result->get_data())->get_value();
          delete result;
          return blresult;
        }
        return false;
      }
      v.value = field->get_node();
      v.enumable = enumable;
      v.configurable = configurable;
      v.writable = writable;
      return true;
    }
  }
  return false;
}
bool object_variable::remove(variable *value, const std::string &name) {
  object_variable *obj = (object_variable *)value->get_data();
  for (auto it = obj->_properties.begin(); it != obj->_properties.end(); it++) {
    if (it->first.field == name) {
      if (it->second.value) {
        value->get_node()->remove_node(it->second.value);
      }
      if (it->second.getter) {
        value->get_node()->remove_node(it->second.getter);
      }
      if (it->second.setter) {
        value->get_node()->remove_node(it->second.setter);
      }
      obj->_properties.erase(it);
      return true;
    }
  }
  return false;
}
variable *object_variable::get(context *ctx, variable *value,
                               const std::string &name) {
  object_variable *obj = (object_variable *)value->get_data();
  for (auto &[k, v] : obj->_properties) {
    if (k.field == name) {
      if (v.value) {
        return ctx->get_scope()->create_variable(v.value);
      } else {
        auto *getter = ctx->get_scope()->create_variable(v.getter);
        auto *result = function_variable::call(ctx, getter, value);
        delete getter;
        return result;
      }
    }
  }
  return ctx->get_scope()->create_variable(new undefined_variable());
}
std::vector<std::string> object_variable::keys(variable *value) {
  object_variable *obj = (object_variable *)value->get_data();
  std::vector<std::string> result;
  for (auto &[k, v] : obj->_properties) {
    if (!k.symbol && v.enumable) {
      result.push_back(k.field);
    }
  }
  return result;
}
