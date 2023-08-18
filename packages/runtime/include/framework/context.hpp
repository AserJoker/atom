#pragma once
#include "runtime/include/base.hpp"
#include "runtime/include/value/scope.hpp"
namespace atom::runtime {
class context : public base {
private:
  std::list<scope *> _scopes;
  scope *_scope;

public:
  scope *get_scope();
  void push_scope();
  void pop_scope();
  context();
  ~context() override;
};
} // namespace atom::runtime