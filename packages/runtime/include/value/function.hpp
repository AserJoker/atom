#pragma once
#include "object.hpp"
#include "runtime/include/framework/context.hpp"
#include <functional>
namespace atom::runtime {
class function : public object {
public:
private:
  std::string _name;
  int32_t _length;
  cfunction _callee;
  std::string _filename;

public:
  function(handle *hfunction, handle *proto, cfunction callee, int32_t len,
           const std::string &name,const std::string &filename="<internel>");
  value *call(uint32_t line, uint32_t column, context *ctx, value *self,
              std::vector<value *> args = {});
  value *bind(context *ctx, value *self);
};
} // namespace atom::runtime
