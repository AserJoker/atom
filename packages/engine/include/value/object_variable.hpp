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
  static variable *create(context *, variable * = nullptr);
  static variable *construct(context *, variable *,
                             const std::vector<variable *> & = {});

  static variable *get_prototype_of(context *, variable *);
  static std::vector<std::string> keys(context *, variable *);

  static property *get_own_property(context *, variable *, const std::string &);
  static property *get_property(context *, variable *, const std::string &);
  static bool define_property(context *, variable *, const std::string &,
                              const property &);
  static bool set_property(context *, variable *, const std::string &,
                           const property &);

  static bool set_field(context *, variable *, const uint32_t &, variable *);
  static variable *get_field(context *, variable *, const uint32_t &);

  static bool set_field(context *, variable *, const std::string &, variable *);
  static variable *get_field(context *, variable *, const std::string &);
};
} // namespace atom::engine
