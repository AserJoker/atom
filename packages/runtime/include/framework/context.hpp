#pragma once
#include "runtime/include/value/scope.hpp"
namespace atom::runtime {
class context {
private:
  scope *_scope;

public:
  scope *get_scope();
  scope *push_scope();
  void pop_scope(scope *);
  context();
  ~context();
};
} // namespace atom::runtime
