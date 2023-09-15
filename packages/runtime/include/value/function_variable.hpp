#pragma once
#include "framework/context.hpp"
#include "value/base_variable.hpp"
#include <functional>
#include <vector>
namespace atom::runtime {
class function_variable : public base_variable {
private:
  std::function<variable *(context *, variable *,
                           const std::vector<variable *> &)>
      _callee;

public:
  static variable *call(context *, variable *, variable *,
                        const std::vector<variable *> & = {});
};
} // namespace atom::runtime
