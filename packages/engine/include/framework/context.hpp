#pragma once
#include "core/include/object.hpp"
#include "engine/include/framework/runtime.hpp"
#include "engine/include/value/scope.hpp"
#include "engine/include/value/variable.hpp"
namespace atom::engine {
class context : public core::object {
public:
  struct call_frame {
    std::string _filename;
    std::string _funcname;
    uint32_t _line;
    uint32_t _column;
  };

private:
  scope *_scope;
  core::auto_release<runtime> _runtime;

  variable *_object_prototype;
  variable *_function_prototype;
  variable *_array_prototype;

  variable *_array_constructor;
  variable *_object_constructor;
  variable *_function_constructor;

  variable *_undefined;
  variable *_null;

  variable *_error_constructor;
  variable *_type_error_constructor;
  variable *_internel_error_constructor;

  std::vector<call_frame> _call_stack;

  inline static context *current = nullptr;

public:
  scope *get_scope();
  scope *push_scope();
  void pop_scope(scope *);

  void push_call_stack(const call_frame &frame);
  void pop_call_stack();
  std::vector<std::string> get_call_stack(uint32_t line, uint32_t column);

  context(const core::auto_release<runtime> &);
  ~context();
  variable *undefined();
  variable *null();

  variable *array_prototype();
  variable *object_prototype();
  variable *function_prototype();

  variable *array_constructor();
  variable *object_constructor();
  variable *function_constructor();

  variable *create(base_variable *);
  variable *create(node *, variable * = nullptr);
  variable *assigment(variable *);
  const core::auto_release<runtime> &get_runtime();
};
} // namespace atom::engine
