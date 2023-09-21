#pragma once
#include "core/include/object.hpp"
#include "engine/include/framework/runtime.hpp"
#include "engine/include/value/scope.hpp"
#include "engine/include/value/variable.hpp"
namespace atom::engine {
class context : public core::object {
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

  inline static context *current = nullptr;

public:
  scope *get_scope();
  scope *push_scope();
  void pop_scope(scope *);
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
