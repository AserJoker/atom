#include "../include/context.hpp"
#include "../include/variable_nil.hpp"
#include <stdint.h>
using namespace atom::engine;
context::context(runtime *rt) : _rt(rt) {
  _current_scope = new scope();
  _nil = new variable_nil(this);
}
context::~context() { delete _current_scope; }
scope *context::get_scope() { return _current_scope; }
void context::with_scope(const std::function<void(scope *)> &process) {
  auto sc = push_scope();
  process(sc);
  pop_scope(sc);
}
scope *context::push_scope() {
  scope *current = _current_scope;
  _current_scope = new scope(current);
  return current;
}
void context::pop_scope(scope *sc) {
  delete _current_scope;
  _current_scope = sc;
}
variable *context::nil() { return _nil; }

void context::push_call_stack(const std::string &filename,
                              const std::string &funcname, const uint32_t &line,
                              const uint32_t &column) {
  auto &tail = *_call_stack.rbegin();
  tail.filename = filename;
  tail.line = line;
  tail.column = column;
  _call_stack.push_back(
      call_frame{.funcname = funcname, .line = 0, .column = 0});
};
void context::pop_call_stack() { _call_stack.pop_back(); };
const std::list<context::call_frame>
context::call_stack(const std::string &filename, const uint32_t &line,
                    const uint32_t &column) const {
  auto res = _call_stack;
  auto &tail = *res.rbegin();
  tail.filename = filename;
  tail.line = line;
  tail.column = column;
  return res;
};