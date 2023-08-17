#pragma once
#include "base.hpp"
#include "stack.hpp"
#include <list>
namespace atom::runtime {
class handle {
private:
  base *_object;
  std::list<stack *> _stacks;
  std::list<handle *> _parents;
  std::list<handle *> _children;

private:
  bool check();

public:
  void add_ref(handle *child);
  void remove_ref(handle *child);
  handle(stack *stack, base *data = nullptr);
  handle(handle *parent, base *data = nullptr);
  std::list<stack *> &get_stacks();
  base *get_object();
  void set_object(base *obj);
  ~handle();
  void auto_dispose();
};
} // namespace atom::runtime
