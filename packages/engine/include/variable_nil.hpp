#pragma once
#include "variable.hpp"
namespace atom::engine {
class variable_nil : public variable {
public:
  variable_nil(context *);
};
} // namespace atom::engine