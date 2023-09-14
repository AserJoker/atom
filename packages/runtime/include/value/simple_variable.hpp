#pragma once
#include "runtime/include/value/base_variable.hpp"
#include <cstdint>
#include <string>
namespace atom::runtime {
template <class T, base_variable::variable_type vt>
class simple_variable : public base_variable {
private:
  T _value;

public:
  simple_variable(const T &val = {}) : base_variable(vt), _value(val) {}
  const T &get_value() const { return _value; }
  T &get_value() { return _value; }
};
using string_variable =
    simple_variable<std::string, base_variable::variable_type::VT_STRING>;
using number_variable =
    simple_variable<double, base_variable::variable_type::VT_NUMBER>;
using integer_variable =
    simple_variable<int32_t, base_variable::variable_type::VT_INTEGER>;
using boolean_variable =
    simple_variable<bool, base_variable::variable_type::VT_BOOLEAN>;
using null_variable =
    simple_variable<void *, base_variable::variable_type::VT_NULL>;
using undefined_variable =
    simple_variable<void *, base_variable::variable_type::VT_UNDEFINED>;

} // namespace atom::runtime
