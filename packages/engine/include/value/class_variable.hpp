#pragma once
#include "function_variable.hpp"
#include "userdata_variable.hpp"
namespace atom::engine {
class class_variable : public function_variable {
private:
  function_handle _constructor;
  engine::variable *construct_handle(engine::context *ctx,
                                     engine::variable *self,
                                     const std::vector<variable *> &args) {
    std::string construct_sign = "";
    for (auto &arg : args) {
      construct_sign += base_variable::type_name(base_variable::type_of(arg));
    }
    _constructor(ctx, self, args);
    return ctx->undefined();
  };

protected:
  class_variable()
      : function_variable(
            [this](context *ctx, variable *self,
                   const std::vector<variable *> &args) -> variable * {
              return this->construct_handle(ctx, self, args);
            }) {
    _type = base_variable::variable_type::VT_CLASS;
  }

public:
  template <class T>
  static variable *
  create(context *ctx, const std::string &name = typeid(T).name(),
         const std::map<std::string,
                        variable *(T::*)(context *ctx,
                                         const std::vector<variable *> &args)>
             &methods = {}) {
    auto *clazz = new class_variable();
    clazz->_name = name;
    clazz->_length = 0;
    clazz->_proto = ctx->function_prototype()->get_node();
    auto prototype = object_variable::create(ctx);
    clazz->_prototype = prototype->get_node();
    auto *val = ctx->get_scope()->create_variable(clazz);
    val->get_node()->add_node(clazz->_proto);
    val->get_node()->add_node(clazz->_prototype);
    clazz->_constructor = [](context *ctx, variable *self,
                             const std::vector<variable *> args) -> variable * {
      auto userdata = userdata_variable::create(ctx, new T(self, args));
      object_variable::property prop = {.value = userdata->get_node(),
                                        .getter = nullptr,
                                        .setter = nullptr,
                                        .configurable = false,
                                        .writable = false,
                                        .enumable = false};
      object_variable::define_property(ctx, self, "[[internel]]", prop);
      return ctx->undefined();
    };
    for (auto it = methods.begin(); it != methods.end(); it++) {
      auto name = it->first;
      auto method = it->second;
      auto m = [=](context *ctx, variable *self,
                   const std::vector<variable *> &args) -> variable * {
        auto udata = (T *)userdata_variable::get_userdata(
                         base_variable::get(ctx, self, "[[internel]]"))
                         .get();
        return (udata->*method)(ctx, args);
      };
      base_variable::set(ctx, prototype, name,
                         function_variable::create(ctx, name, 1, m));
    }
    return val;
  };
};
} // namespace atom::engine