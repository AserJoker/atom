#include "memory/node.hpp"
#include <fmt/core.h>
#include <set>
using namespace atom::runtime;
using namespace atom;
node::node(chunk *c, base_variable *d) : _data(d) { c->add_node(this); }
node::node(node *p, base_variable *d) : _data(d) { p->add_node(this); }
node::~node() {
  if (_data) {
    delete _data;
    _data = nullptr;
  }
}
void node::add_node(node *n) {
  _children.push_back(n);
  n->_parents.push_back(this);
}
void node::remove_node(node *n) {
  for (auto it = n->_parents.begin(); it != n->_parents.end(); it++) {
    if (*it == this) {
      n->_parents.erase(it);
      break;
    }
  }
  for (auto it = _children.begin(); it != _children.end(); it++) {
    if (*it == n) {
      _children.erase(it);
      break;
    }
  }
  n->auto_release();
}
std::list<chunk *> &node::get_chunks() { return _chunks; }
void node::auto_release() {
  if (!_chunks.empty() || !_parents.empty()) {
    return;
  }
  static uint64_t gc_time = 0;
  std::set<core::auto_release<node>> release_list;
  std::list<core::auto_release<node>> worker_queue = {this};
  _gc_time = ++gc_time;
  while (!worker_queue.empty()) {
    auto n = *worker_queue.begin();
    worker_queue.erase(worker_queue.begin());
    if (n->check_alone(gc_time)) {
      for (auto &c : n->_children) {
        if (c->_gc_time != gc_time) {
          c->_gc_time = gc_time;
        }
        worker_queue.push_back(c);
      }
      release_list.insert(n);
    }
  }
  while (!release_list.empty()) {
    release_list.erase(release_list.begin());
  }
}
bool node::check_alone(uint64_t check_time) {
  std::list<node *> worker_queue = {this};
  this->_check_time = check_time;
  while (!worker_queue.empty()) {
    auto n = *worker_queue.begin();
    worker_queue.erase(worker_queue.begin());
    if (!n->_chunks.empty()) {
      return false;
    }
    for (auto &p : n->_parents) {
      if (p->_check_time != check_time) {
        p->_check_time = check_time;
        worker_queue.push_back(p);
      }
    }
  }
  return true;
}
base_variable *node::get_data() { return _data; }
