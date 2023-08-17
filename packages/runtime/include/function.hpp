#pragma once
#include "context.hpp"
#include "object.hpp"
#include <functional>
namespace atom::runtime {
class function : public object {
public:
private:
  std::string _name;
  int32_t _length;
  cfunction _callee;

public:
  function(handle *hfunction, cfunction callee, int32_t len,
           const std::string &name);
  value *call(context *ctx, value *self, std::vector<value *> args = {});
  value *bind(context *ctx, value *self);
};
} // namespace atom::runtime
