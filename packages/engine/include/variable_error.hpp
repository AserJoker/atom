#pragma once
#include "variable.hpp"
namespace atom::engine {
class variable_error : public variable {
private:
  std::string _msg;

public:
  variable_error(context *, const std::string &, const std::string &,
                 const uint32_t &);
};
} // namespace atom::engine