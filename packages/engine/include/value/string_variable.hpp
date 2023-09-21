#pragma once
#include "simple_variable.hpp"
namespace atom::engine {
class string_variable
    : public simple_variable<std::string,
                             base_variable::variable_type::VT_STRING> {
public:
  static bool set_field(context *, variable *, const uint32_t &, variable *);
  static variable *get_field(context *, variable *, const uint32_t &);
  static bool set_field(context *, variable *, const std::string &, variable *);
  static variable *get_field(context *, variable *, const std::string &);
};
} // namespace atom::engine