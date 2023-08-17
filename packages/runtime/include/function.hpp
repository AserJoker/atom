#pragma once
#include "object.hpp"
#include <functional>
namespace atom::runtime {
class function : public object {
public:
  using cfunction = std::function<value *(value *, int32_t argc, value **argv)>;

private:
  std::string _name;
  int32_t _length;
  cfunction _callee;

public:
  function(handle *hfunction, cfunction callee, int32_t len,
           const std::string &name);
};
} // namespace atom::runtime
