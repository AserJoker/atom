#include "runtime/include/stack.hpp"
#include "runtime/include/handle.hpp"
#include <algorithm>
using namespace atom::runtime;
stack::stack(stack *parent) : _parent(parent) {
  if (parent) {
    parent->_children.push_back(this);
  }
}
stack::~stack() {
  for (auto &child : _children) {
    child->_parent = nullptr;
    delete child;
  }
  while (_handles.size()) {
    auto &handle = *_handles.begin();
    remove_handle(handle);
  }
  if (_parent) {
    std::erase(_parent->_children, this);
  }
}
void stack::add_handle(handle *handle) {
  _handles.push_back(handle);
  handle->get_stacks().push_back(this);
}
void stack::remove_handle(handle *handle) {
  std::erase(_handles, handle);
  auto &stacks = handle->get_stacks();
  std::erase(stacks, this);
  if (!stacks.size()) {
    handle->auto_dispose();
  }
}
