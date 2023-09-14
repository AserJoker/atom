#pragma once
#include "core/include/object.hpp"
#include "framework/context.hpp"
#include "memory/node.hpp"
#include "value/base_variable.hpp"
#include "value/variable.hpp"
#include <map>
#include <string>
#include <vector>
namespace atom::runtime {
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

public:
  object_variable();
  ~object_variable() override;
  static bool define(variable *obj, const std::string &name, variable *field,
                     bool configurable = true, bool writable = true,
                     bool enumable = true);
  static bool define(variable *obj, const std::string &name, variable *getter,
                     variable *setter, bool configurable = true,
                     bool writable = true, bool enumable = true);
  static bool remove(variable *obj, const std::string &name);
  static bool set(context *ctx, variable *obj, const std::string &name,
                  variable *field, bool configurable = true,
                  bool writable = true, bool enumable = true);
  static variable *get(context *ctx,variable *value, const std::string &name);
  static std::vector<std::string> keys(variable *obj);
};
} // namespace atom::runtime
