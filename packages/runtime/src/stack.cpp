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
    auto it =
        std::find(_parent->_children.begin(), _parent->_children.end(), this);
    if (it != _parent->_children.end()) {
      _parent->_children.erase(it);
    }
  }
}
void stack::add_handle(handle *handle) {
  _handles.push_back(handle);
  handle->get_stacks().push_back(this);
}
void stack::remove_handle(handle *handle) {
  auto hit = std::find(_handles.begin(), _handles.end(), handle);
  if (hit != _handles.end()) {
    _handles.erase(hit);
  }
  auto &stacks = handle->get_stacks();
  auto sit = std::find(stacks.begin(), stacks.end(), this);
  if (sit != stacks.end()) {
    stacks.erase(sit);
  }
  if (!stacks.size()) {
    handle->auto_dispose();
  }
}
