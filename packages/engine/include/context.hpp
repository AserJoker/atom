#pragma once
#include "core/include/object.hpp"
#include "runtime.hpp"
#include "scope.hpp"
#include <functional>
#include <stack>
#include <stdint.h>
#include <string>

namespace atom::engine {
class variable;
class context : public core::object {
public:
  struct call_frame {
    std::string filename;
    std::string funcname;
    uint32_t line;
    uint32_t column;
  };

private:
  scope *_current_scope;
  runtime *_rt;

  variable *_nil;
  std::list<call_frame> _call_stack;

public:
  scope *push_scope();
  void pop_scope(scope *);
  scope *get_scope();
  void with_scope(const std::function<void(scope *)> &);
  context(runtime *);
  ~context() override;
  variable *nil();
  void push_call_stack(const std::string &, const std::string &,
                       const uint32_t &,const uint32_t&);
  void pop_call_stack();
  const std::list<call_frame> call_stack(const std::string&,const uint32_t&,const uint32_t&) const;
};
} // namespace atom::engine