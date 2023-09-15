#pragma once
#include "core/include/object.hpp"
#include "runtime/include/framework/runtime.hpp"
#include "runtime/include/value/scope.hpp"
#include "runtime/include/value/variable.hpp"
namespace atom::runtime {
class context:public core::object {
private:
  scope *_scope;
  runtime *_runtime;

public:
  scope *get_scope();
  scope *push_scope();
  void pop_scope(scope *);
  context(runtime *rt);
  ~context();
  variable *undefined();
  variable *null();
};
} // namespace atom::runtime
