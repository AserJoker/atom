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
  struct key {
    std::string field;
    node *symbol;
    bool operator<(const key &k) const {
      if (symbol && k.symbol) {
        return symbol < k.symbol;
      }
      if (!symbol && !k.symbol) {
        return field < k.field;
      }
      return false;
    }
  };
  struct property {
    node *value;
    node *getter;
    node *setter;
    bool configurable;
    bool writable;
    bool enumable;
  };
  std::map<key, property> _properties;

protected:
  node *_proto;
  object_variable();

public:
  ~object_variable() override;
  static variable *create(context *ctx, variable *proto = nullptr);
  static variable *construct(context *ctx, variable *constructor,
                             const std::vector<variable *> &args = {});
  static std::vector<std::string> keys(variable *obj);
  static variable *getOwnProperty(context *ctx, variable *value,
                                  const std::string &name);
  static variable *getProperty(context *ctx, variable *value,
                               const std::string &name);
  static variable *getPrototypeOf(context *ctx, variable *value);
  static bool setProperty(context *ctx, variable *value,
                          const std::string &name, variable *field);
  static bool define(variable *obj, const std::string &name, variable *field,
                     bool configurable = true, bool writable = true,
                     bool enumable = true);
  static bool define(variable *obj, const std::string &name, variable *getter,
                     variable *setter, bool configurable = true,
                     bool writable = true, bool enumable = true);
  static bool remove(variable *obj, const std::string &name);
};
} // namespace atom::engine
