#pragma once
#include "core/include/object.hpp"
#include "runtime/include/memory/node.hpp"
#include "runtime/include/value/scope.hpp"
namespace atom::runtime {
class runtime:public core::object {
private:
  scope *_scope;
  node *_undefined;
  node *_null;

public:
  runtime();
  ~runtime();
  scope *get_scope();
  node *undefined();
  node *null();
};
} // namespace atom::runtime
