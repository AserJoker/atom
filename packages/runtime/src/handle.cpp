#include "runtime/include/handle.hpp"
#include <algorithm>
#include <iostream>
#include <set>
using namespace atom::runtime;
handle::handle(stack *stack, base *object) : _object(object) {
  stack->add_handle(this);
  std::cout << "handle" << std::endl;
}
handle::handle(handle *parent, base *object) : _object(object) {
  parent->add_ref(this);
  std::cout << "handle" << std::endl;
}
handle::~handle() {
  std::cout << "~handle" << std::endl;
  if (_object) {
    delete _object;
    _object = nullptr;
  }
}
base *handle::get_object() { return _object; }
void handle::set_object(base *obj) { this->_object = obj; }
void handle::add_ref(handle *child) {
  _children.push_back(child);
  child->_parents.push_back(this);
}
void handle::remove_ref(handle *child) {
  std::erase(_children, child);
  std::erase(child->_parents, this);
  if (!_stacks.size()) {
    auto_dispose();
  }
}
bool handle::check() {
  std::list<handle *> queue = {this};
  std::set<handle *> cache;
  while (queue.size()) {
    auto &handle = *queue.begin();
    queue.erase(queue.begin());
    if (handle->_stacks.size()) {
      return true;
    }
    for (auto &item : handle->_parents) {
      if (!cache.contains(item)) {
        queue.push_back(item);
        cache.insert(item);
      }
    }
  }
  return false;
}

void handle::auto_dispose() {
  std::list<handle *> queue = {this};
  std::set<handle *> cache;
  std::set<handle *> destories;
  while (queue.size()) {
    auto handle = *queue.begin();
    queue.erase(queue.begin());
    if (cache.contains(handle)) {
      continue;
    }
    cache.insert(handle);
    if (!handle->check()) {
      for (auto &child : handle->_children) {
        std::erase(child->_parents, handle);
        queue.push_back(child);
      }
      handle->_children.clear();
      for (auto &parent : handle->_parents) {
        std::erase(parent->_children, handle);
      }
      handle->_parents.clear();
      destories.insert(handle);
    }
  }
  for (auto it = destories.rbegin(); it != destories.rend(); it++) {
    auto &item = *it;
    delete item;
  }
}
std::list<stack *> &handle::get_stacks() { return _stacks; }
