#include "runtime/include/framework/context.hpp"
#include <fmt/format.h>
using namespace atom::runtime;
context::context() {
  _scope = new scope();
  _call_frame = new frame("<internel>", "run_main");
}
context::~context() {
  delete _call_frame;
  delete _scope;
}
scope *context::get_scope() { return _scope; }
void context::push_scope() {
  auto *sp = _scope;
  _scopes.push_front(_scope);
  _scope = new scope(sp);
}
void context::pop_scope() {
  delete _scope;
  _scope = *_scopes.begin();
  _scopes.pop_front();
}
void context::push_call_stack(const std::string &filename,
                              const std::string &funcname, uint32_t line,
                              uint32_t column) {
  _call_stacks.push_front(_call_frame);
  _call_frame->set_position(line, column);
  _call_frame = new frame(filename, funcname);
}
void context::pop_call_stack() {
  delete _call_frame;
  _call_frame = *_call_stacks.begin();
  _call_stacks.erase(_call_stacks.begin());
}
std::vector<std::string> context::get_call_stack(uint32_t line,
                                                 uint32_t column) {
  _call_frame->set_position(line, column);
  std::vector<std::string> result;
  result.push_back(fmt::format("  at {}", _call_frame->format()));
  for (auto *frame : _call_stacks) {
    result.push_back(fmt::format("  at {}", frame->format()));
  }
  return result;
}
