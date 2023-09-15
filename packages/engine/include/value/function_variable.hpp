#pragma once
#include "engine/include/framework/context.hpp"
#include "engine/include/value/base_variable.hpp"
#include "engine/include/value/object_variable.hpp"
#include <functional>
#include <vector>
namespace atom::engine {
class function_variable : public object_variable {
public:
  using function_handle = std::function<variable *(
      context *, variable *, const std::vector<variable *> &)>;

private:
  function_handle _callee;
  int32_t _length;
  std::string _name;
  function_variable(const function_handle &handle);

public:
  ~function_variable() override;
  function_variable *create(context *ctx, const std::string &name,
                            int32_t length, const function_handle &handle);
  static variable *call(context *, variable *, variable *,
                        const std::vector<variable *> & = {});
};
} // namespace atom::engine
