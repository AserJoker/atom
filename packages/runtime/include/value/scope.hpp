#pragma once
#include "core/include/object.hpp"
#include "runtime/include/memory/chunk.hpp"
#include "variable.hpp"
#include <list>
namespace atom::runtime {
class scope : public core::object {
private:
  std::list<variable *> _variables;
  chunk *_chunk;

public:
  scope(scope * = nullptr);
  ~scope();
  variable * create_variable(base_variable * = nullptr);
  variable * create_variable(node *);
  void remove_variable(variable *);
};
} // namespace atom::runtime
