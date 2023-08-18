#pragma once
#include "runtime/include/base.hpp"
#include "runtime/include/framework/frame.hpp"
#include "runtime/include/value/scope.hpp"
namespace atom::runtime {
class context : public base {
private:
  std::list<scope *> _scopes;
  std::list<frame *> _call_stacks;
  frame *_call_frame;
  scope *_scope;

public:
  scope *get_scope();
  void push_scope();
  void pop_scope();
  context();
  ~context() override;
  void push_call_stack(const std::string &filename, const std::string &funcname,
                       uint32_t line, uint32_t column);
  void pop_call_stack();
  std::vector<std::string> get_call_stack(uint32_t line, uint32_t column);
};
} // namespace atom::runtime
