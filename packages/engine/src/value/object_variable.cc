#include "engine/include/value/object_variable.hpp"
#include "engine/include/value/boolean_variable.hpp"
#include "engine/include/value/function_variable.hpp"
#include "engine/include/value/integer_variable.hpp"
#include "engine/include/value/number_variable.hpp"
#include "engine/include/value/simple_variable.hpp"
#include "engine/include/value/string_variable.hpp"
using namespace atom::engine;
variable *object_variable::create(context *ctx, variable *proto) {
  if (!proto) {
    proto = ctx->object_prototype();
  }
  object_variable *obj = new object_variable();
  obj->_proto = proto->get_node();
  variable *val = ctx->get_scope()->create_variable(obj);
  val->get_node()->add_node(obj->_proto);
  auto object_constructor = ctx->object_constructor();
  if (object_constructor) {
    obj->_constructor = object_constructor->get_node();
    if (obj->_constructor) {
      val->get_node()->add_node(obj->_constructor);
    }
  }
  return val;
}
variable *object_variable::construct(context *ctx, variable *constructor,
                                     const std::vector<variable *> &args) {
  auto prototype = object_variable::get(ctx, constructor, "prototype");
  auto obj = object_variable::create(ctx, prototype);
  base_variable::set(ctx, obj, "constructor", constructor);
  delete function_variable::call(ctx, constructor, obj, args);
  return obj;
}
object_variable::object_variable()
    : base_variable(variable_type::VT_OBJECT), _constructor(nullptr) {}
object_variable::~object_variable() {}

variable *object_variable::get_prototype_of(context *ctx, variable *obj) {
  auto *o = dynamic_cast<object_variable *>(obj->get_data());
  return ctx->create(o->_proto);
}

object_variable::property *
object_variable::get_own_property(context *ctx, variable *obj,
                                  const std::string &name) {
  auto o = dynamic_cast<object_variable *>(obj->get_data());
  if (o->_properties.contains(name)) {
    return &o->_properties.at(name);
  }
  return nullptr;
};
std::vector<std::string> object_variable::keys(context *ctx, variable *obj) {
  auto val = ctx->assigment(obj);
  std::vector<std::string> keys;
  while (val && type_of(val) >= variable_type::VT_OBJECT) {
    auto o = dynamic_cast<object_variable *>(val);
    for (auto &[k, prop] : o->_properties) {
      if (prop.enumable) {
        keys.push_back(k);
      }
    }
    auto old = val;
    val = get_prototype_of(ctx, val);
    delete old;
  }
  delete val;
  return keys;
}
object_variable::property *
object_variable::get_property(context *ctx, variable *obj,
                              const std::string &name) {
  auto o = ctx->assigment(obj);
  property *field = nullptr;
  while (type_of(o) >= variable_type::VT_OBJECT) {
    field = object_variable::get_own_property(ctx, o, name);
    if (field) {
      break;
    }
    auto old = o;
    o = get_prototype_of(ctx, o);
    delete old;
  }
  delete o;
  return field;
}
bool object_variable::define_property(context *ctx, variable *obj,
                                      const std::string &name,
                                      const object_variable::property &prop) {
  auto *o = dynamic_cast<object_variable *>(obj->get_data());
  if (o->_properties.contains(name)) {
    return false;
  }
  o->_properties[name] = prop;
  if (prop.value) {
    obj->get_node()->add_node(prop.value);
  }
  if (prop.getter) {
    obj->get_node()->add_node(prop.getter);
  }
  if (prop.setter) {
    obj->get_node()->add_node(prop.setter);
  }
  return true;
}
bool object_variable::set_property(context *ctx, variable *obj,
                                   const std::string &name,
                                   const object_variable::property &value) {
  auto prop = object_variable::get_property(ctx, obj, name);
  if (prop) {
    if (!prop->configurable || !prop->writable) {
      return false;
    }
  } else {
    auto o = dynamic_cast<object_variable *>(obj->get_data());
    prop = &o->_properties[name];
    prop->value = nullptr;
    prop->getter = nullptr;
    prop->setter = nullptr;
    prop->configurable = value.configurable;
    prop->writable = value.writable;
    prop->enumable = value.enumable;
  }
  if (prop->value) {
    obj->get_node()->remove_node(prop->value);
  }
  if (prop->getter) {
    obj->get_node()->remove_node(prop->getter);
  }
  if (prop->setter) {
    obj->get_node()->remove_node(prop->setter);
  }
  prop->value = value.value;
  prop->setter = value.setter;
  prop->getter = value.getter;
  if (prop->value) {
    obj->get_node()->add_node(prop->value);
  }
  if (prop->getter) {
    obj->get_node()->add_node(prop->getter);
  }
  if (prop->setter) {
    obj->get_node()->add_node(prop->setter);
  }
  return true;
}
bool object_variable::set_field(context *ctx, variable *obj,
                                const std::string &name, variable *value) {
  if (name == "constructor") {
    auto o = dynamic_cast<object_variable *>(obj->get_data());
    obj->get_node()->add_node(value->get_node());
    if (o->_constructor) {
      obj->get_node()->remove_node(o->_constructor);
    }
    o->_constructor = value->get_node();
    return true;
  }

  auto prop = get_property(ctx, obj, name);
  if (prop) {
    if (!prop->writable) {
      return false;
    }
    if (prop->value) {
      obj->get_node()->add_node(value->get_node());
      obj->get_node()->remove_node(prop->value);
      prop->value = value->get_node();
    } else {
      if (!prop->setter) {
        return false;
      }
      auto setter = ctx->create(prop->setter);
      auto res = function_variable::call(ctx, setter, obj, {value});
      delete setter;
      auto blres = boolean_variable::value_of(res);
      return blres;
    }
  } else {
    return define_property(ctx, obj, name,
                           {.value = value->get_node(),
                            .getter = nullptr,
                            .setter = nullptr,
                            .configurable = true,
                            .writable = true,
                            .enumable = true});
  }
  return true;
}
variable *object_variable::get_field(context *ctx, variable *obj,
                                     const std::string &name) {
  if (name == "constructor") {
    auto o = dynamic_cast<object_variable *>(obj->get_data());
    return ctx->create(o->_constructor);
  }
  auto prop = get_property(ctx, obj, name);
  if (!prop) {
    return ctx->undefined();
  }
  if (prop->value) {
    return ctx->create(prop->value, obj);
  } else {
    auto getter = ctx->create(prop->getter);
    auto res = function_variable::call(ctx, getter, obj);
    delete getter;
    return res;
  }
};
bool object_variable::set_field(context *ctx, variable *obj,
                                const uint32_t &index, variable *field) {
  return object_variable::set_field(ctx, obj, std::to_string(index), field);
}
variable *object_variable::get_field(context *ctx, variable *obj,
                                     const uint32_t &index) {
  return object_variable::get_field(ctx, obj, std::to_string(index));
}