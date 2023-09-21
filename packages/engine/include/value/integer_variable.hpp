#pragma once
#include "simple_variable.hpp"
namespace atom::engine {
class integer_variable
    : public simple_variable<int64_t,
                             base_variable::variable_type::VT_INTEGER> {};
} // namespace atom::engine