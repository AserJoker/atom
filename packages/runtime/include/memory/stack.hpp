#pragma once
#include "runtime/include/base.hpp"
#include <list>
namespace atom::runtime {
class handle;
class stack:public base {
private:
  std::list<handle *> _handles;
  std::list<stack *> _children;
  stack *_parent;

public:
  stack(stack *parent);
  ~stack();
  void add_handle(handle *handle);
  void remove_handle(handle *handle);
};
} // namespace atom::runtime
