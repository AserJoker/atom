#pragma once
#include "base.hpp"
#include "stack.hpp"
#include "type.hpp"
#include <cstdint>
#include <functional>
#include <string>
namespace atom::runtime {
class function;
class value;
class context;
using cfunction =
    std::function<value *(context *, value *, std::vector<value *> &args)>;
class scope : public base {
private:
  std::list<value *> _values;
  stack *_stack;

private:
  void connect_value(value *val);
  value *create_value(value_type type, base *data);

public:
  scope(scope *parent = nullptr);
  ~scope() override;
  void remove(value *value);
  value *create(handle *handle);
  value *create(value *source);
  value *create(const char *data);
  value *create(const std::string &data);
  value *create(const int32_t &data);
  value *create(const double &data);
  value *create(const bool &data);
  value *create_undefined();
  value *create_null();
  value *create_object();
  value *create_array();
  value *create_function(cfunction callee, int32_t length,
                         const std::string &name);
  stack *get_stack();
};
}; // namespace atom::runtime
