#pragma once
#include "simple_variable.hpp"
namespace atom::engine {
class boolean_variable
    : public simple_variable<bool, base_variable::variable_type::VT_BOOLEAN> {};
} // namespace atom::engine