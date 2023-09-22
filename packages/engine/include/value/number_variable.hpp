#pragma once
#include "simple_variable.hpp"
namespace atom::engine {
class number_variable
    : public simple_variable<double, base_variable::variable_type::VT_NUMBER> {
};
} // namespace atom::engine