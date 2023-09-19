#include "engine/include/lib/object_helper.hpp"
#include "engine/include/value/function_variable.hpp"
#include "engine/include/value/simple_variable.hpp"
using namespace atom::engine;
using namespace atom;
static native_function(object_assign) {
  auto target = ctx->assigment(args[0]);
  for (auto index = 1; index < args.size(); index++) {
    auto keys = object_variable::keys(ctx, args[index]);
    for (auto &k : keys) {
      object_variable::set_property(
          ctx, target, k, object_variable::get_property(ctx, args[index], k));
    }
  }
  return target;
}
static native_function(object_create) {
  return object_variable::create(ctx, args[0]);
}
static native_function(object_define_properties) {
  auto target = ctx->assigment(args[0]);
  auto configs = ctx->assigment(args[1]);
  auto keys = object_variable::keys(ctx, configs);
  for (auto &k : keys) {
    auto config = object_variable::get_property(ctx, configs, k);
    auto config_keys = object_variable::keys(ctx, config);
    if (std::find(config_keys.begin(), config_keys.end(), "value") !=
            config_keys.end() ||
        std::find(config_keys.begin(), config_keys.end(), "writable") !=
            config_keys.end() ||
        std::find(config_keys.begin(), config_keys.end(), "get") !=
            config_keys.end() ||
        std::find(config_keys.begin(), config_keys.end(), "set") !=
            config_keys.end()) {
      auto configurable =
          object_variable::get_property(ctx, config, "configurable");
      if (base_variable::type_of(configurable) ==
          base_variable::variable_type::VT_UNDEFINED) {
        configurable = boolean_variable::create(ctx, false);
      }
      auto writable = object_variable::get_property(ctx, config, "writable");
      if (base_variable::type_of(writable) ==
          base_variable::variable_type::VT_UNDEFINED) {
        writable = boolean_variable::create(ctx, false);
      }
      auto enumerable =
          object_variable::get_property(ctx, config, "enumerable");
      if (base_variable::type_of(enumerable) ==
          base_variable::variable_type::VT_UNDEFINED) {
        enumerable = boolean_variable::create(ctx, false);
      }

      auto value = object_variable::get_property(ctx, config, "value");
      auto getter = object_variable::get_property(ctx, config, "get");
      auto setter = object_variable::get_property(ctx, config, "set");
      if (base_variable::type_of(getter) ==
          base_variable::variable_type::VT_UNDEFINED) {
        getter = nullptr;
      }
      if (base_variable::type_of(setter) ==
          base_variable::variable_type::VT_UNDEFINED) {
        setter = nullptr;
      }
      if (base_variable::type_of(value) ==
          base_variable::variable_type::VT_UNDEFINED) {
        if (getter) {
          value = nullptr;
        }
      }
      object_variable::set(target, k, value, getter, setter,
                           boolean_variable::value_of(configurable),
                           boolean_variable::value_of(writable),
                           boolean_variable::value_of(enumerable));
    } else {
      object_variable::set_property(ctx, target, k, config);
    }
  }
  return target;
}
static native_function(object_define_property) {
  auto target = ctx->assigment(args[0]);
  auto name = ctx->assigment(args[1]);
  auto descriptor = ctx->assigment(args[2]);
  auto configurable =
      object_variable::get_property(ctx, descriptor, "configurable");
  if (base_variable::type_of(configurable) ==
      base_variable::variable_type::VT_UNDEFINED) {
    configurable = boolean_variable::create(ctx, false);
  }
  auto writable = object_variable::get_property(ctx, descriptor, "writable");
  if (base_variable::type_of(writable) ==
      base_variable::variable_type::VT_UNDEFINED) {
    writable = boolean_variable::create(ctx, false);
  }
  auto enumerable =
      object_variable::get_property(ctx, descriptor, "enumerable");
  if (base_variable::type_of(enumerable) ==
      base_variable::variable_type::VT_UNDEFINED) {
    enumerable = boolean_variable::create(ctx, false);
  }
  auto value = object_variable::get_property(ctx, descriptor, "value");
  auto getter = object_variable::get_property(ctx, descriptor, "get");
  auto setter = object_variable::get_property(ctx, descriptor, "set");
  if (base_variable::type_of(getter) !=
      base_variable::variable_type::VT_UNDEFINED) {
    object_variable::define(target, string_variable::value_of(name), getter,
                            setter, boolean_variable::value_of(configurable),
                            boolean_variable::value_of(writable),
                            boolean_variable::value_of(enumerable));
  } else {
    object_variable::define(target, string_variable::value_of(name), value,
                            boolean_variable::value_of(configurable),
                            boolean_variable::value_of(writable),
                            boolean_variable::value_of(enumerable));
  }
  return target;
}
void object_helper::init_object(context *ctx) {
  auto object_constructor = ctx->object_constructor();
  object_variable::define(
      object_constructor, "assign",
      function_variable::create(ctx, "assign", 1, object_assign));
  object_variable::define(
      object_constructor, "create",
      function_variable::create(ctx, "create", 1, object_create));
  object_variable::define(object_constructor, "defineProperties",
                          function_variable::create(ctx, "defineProperties", 2,
                                                    object_define_properties));
}