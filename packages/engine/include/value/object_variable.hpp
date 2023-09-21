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
public:
  struct property {
    node *value;
    node *getter;
    node *setter;
    bool configurable;
    bool writable;
    bool enumable;
  };

private:
  std::map<std::string, property> _properties;

protected:
  node *_proto;
  node *_constructor;
  object_variable();

public:
  ~object_variable() override;
  static variable *create(context *ctx, variable *proto = nullptr);
  static variable *construct(context *ctx, variable *constructor,
                             const std::vector<variable *> &args = {});

  static variable *get_prototype_of(context *ctx, variable *obj);
  static std::vector<std::string> keys(context *ctx, variable *obj);

  static property *get_own_property(context *ctx, variable *obj,
                                    const std::string &name);
  static property *get_property(context *ctx, variable *obj,
                                const std::string &name);
  static bool define_property(context *ctx, variable *obj,
                              const std::string &name, const property &value);
  static bool set_property(context *ctx, variable *obj, const std::string &name,
                           const property &value);

  static bool set_field(context *ctx, variable *obj, const std::string &name,
                        variable *value);
  static variable *get_field(context *ctx, variable *obj,
                             const std::string &name);
};
} // namespace atom::engine
