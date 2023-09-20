#pragma once
#include "core/include/object.hpp"
#include "engine/include/framework/context.hpp"
#include "engine/include/memory/node.hpp"
#include "engine/include/value/base_variable.hpp"
#include "engine/include/value/variable.hpp"
#include <map>
#include <string>
#include <vector>
namespace atom::engine {
class object_variable : public base_variable {
private:
  struct property {
    node *value;
    node *getter;
    node *setter;
    bool configurable;
    bool writable;
    bool enumable;
  };
  std::map<std::string, property> _properties;

protected:
  node *_proto;
  object_variable();

public:
  ~object_variable() override;
  static variable *create(context *ctx, variable *proto = nullptr);
  static variable *construct(context *ctx, variable *constructor,
                             const std::vector<variable *> &args = {});
  static std::vector<std::string> keys(context *ctx, variable *obj);
  static variable *get_own_property(context *ctx, variable *value,
                                    const std::string &name);
  static variable *get_prototype_of(context *ctx, variable *value);

  static bool set_property(variable *obj, const std::string &name,
                           variable *value, variable *getter, variable *setter,
                           bool configurable = true, bool writable = false,
                           bool enumable = true);
  static bool define(variable *obj, const std::string &name, variable *field,
                     bool configurable = true, bool writable = true,
                     bool enumable = true);
  static bool define(variable *obj, const std::string &name, variable *getter,
                     variable *setter, bool configurable = true,
                     bool writable = true, bool enumable = true);
  static bool remove(variable *obj, const std::string &name);
  static bool set(context *ctx, variable *value, const std::string &name,
                  variable *field);

  static variable *get(context *ctx, variable *value, const std::string &name);
};
} // namespace atom::engine
