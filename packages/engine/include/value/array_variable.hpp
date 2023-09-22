#pragma once
#include "object_variable.hpp"
namespace atom::engine {
class array_variable : public object_variable {
private:
  array_variable();
  std::vector<node *> _items;

public:
  ~array_variable();
  static variable *create(context *, const std::vector<variable *> & = {});

  static bool set_field(context *, variable *, const std::string &, variable *);
  static variable *get_field(context *, variable *, const std::string &);

  static bool set_field(context *, variable *, const uint32_t &, variable *);
  static variable *get_field(context *, variable *, const uint32_t &);
};
} // namespace atom::engine