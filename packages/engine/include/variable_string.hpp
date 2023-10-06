#pragma once
#include "variable.hpp"
namespace atom::engine {
class variable_string : public variable {
private:
  std::string _data;

public:
  variable_string(context *, const std::string & = "");
  inline std::string &data() { return _data; };
  inline const std::string &data() const { return _data; }
  variable *get(context *, const std::string &) const override;
  const std::string to_string() const override;
};
} // namespace atom::engine