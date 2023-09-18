#pragma once
#include "core/include/object.hpp"
#include "engine/include/memory/node.hpp"
#include "engine/include/value/scope.hpp"
namespace atom::engine {
class runtime : public core::object {
private:
  scope *_scope;
  node *_undefined;
  node *_null;

public:
  runtime();
  ~runtime();
};
} // namespace atom::engine
