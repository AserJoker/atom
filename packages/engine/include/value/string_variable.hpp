#pragma once
#include "simple_variable.hpp"
namespace atom::engine {
class string_variable
    : public simple_variable<std::string,
                             base_variable::variable_type::VT_STRING> {
public:
  static bool set_field(context *ctx, variable *obj, const std::string &name,
                        variable *value);
  static variable *get_field(context *ctx, variable *obj,
                             const std::string &name);
};
} // namespace atom::engine