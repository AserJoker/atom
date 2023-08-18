#include "runtime/include/handle.hpp"
#include <algorithm>
#include <set>
using namespace atom::runtime;
handle::handle(stack *stack, base *object) : _object(object) {
  stack->add_handle(this);
}
handle::handle(handle *parent, base *object) : _object(object) {
  parent->add_ref(this);
}
handle::~handle() {
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
  auto cit = std::find(_children.begin(), _children.end(), child);
  if (cit != _children.end()) {
    _children.erase(cit);
  }
  auto pit = std::find(child->_parents.begin(), child->_parents.end(), this);
  if (pit != child->_parents.end()) {
    child->_parents.erase(pit);
  }
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
        auto cit =
            std::find(child->_parents.begin(), child->_parents.end(), handle);
        if (cit != child->_parents.end()) {
          child->_parents.erase(cit);
        }
        queue.push_back(child);
      }
      handle->_children.clear();
      for (auto &parent : handle->_parents) {
        auto pit =
            std::find(handle->_parents.begin(), handle->_parents.end(), handle);
        if (pit != handle->_parents.end()) {
          parent->_children.erase(pit);
        }
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
