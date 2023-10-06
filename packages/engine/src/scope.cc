#include "../include/scope.hpp"
#include "../include/variable.hpp"
#include <fmt/core.h>

using namespace atom::engine;
scope::scope(scope *parent) {
  _parent = parent;
  if (_parent != nullptr) {
    _parent->_children.push_back(this);
  }
}
scope::~scope() {
  for (auto &c : _children) {
    c->_parent = nullptr;
    delete c;
  }
  _children.clear();
  for (auto &n : _variables) {
    std::erase(n->get_groups(), this);
    n->auto_release();
  }
  _variables.clear();
  if (_parent != nullptr) {
    std::erase(_parent->_children, this);
  }
}
void scope::add_variable(variable *n) {
  _variables.push_back(n);
  n->get_groups().push_back(this);
}
void scope::remove_variable(variable *n) {
  for (auto it = _variables.begin(); it != _variables.end(); it++) {
    if (*it == n) {
      _variables.erase(it);
      break;
    }
  }
  auto &scopes = n->get_groups();
  for (auto it = scopes.begin(); it != scopes.end(); it++) {
    if (*it == this) {
      scopes.erase(it);
      break;
    }
  }
  n->auto_release();
}
