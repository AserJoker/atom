#pragma once
#include "core/include/object.hpp"
#include "engine/include/framework/runtime.hpp"
#include "engine/include/value/scope.hpp"
#include "engine/include/value/variable.hpp"
namespace atom::engine {
class context : public core::object {
private:
  scope *_scope;
  core::auto_release<runtime> _runtime;

public:
  scope *get_scope();
  scope *push_scope();
  void pop_scope(scope *);
  context(const core::auto_release<runtime> &rt);
  ~context();
  variable *undefined();
  variable *null();
  const core::auto_release<runtime> &get_runtime();
};
} // namespace atom::runtime
