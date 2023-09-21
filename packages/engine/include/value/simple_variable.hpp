#pragma once
#include "engine/include/framework/context.hpp"
#include "engine/include/value/base_variable.hpp"
#include "engine/include/value/variable.hpp"
#include <cstdint>
#include <string>
namespace atom::engine {
template <class T, base_variable::variable_type vt>
class simple_variable : public base_variable {
private:
  T _value;

public:
  simple_variable(const T &val = {}) : base_variable(vt), _value(val) {}
  const T &get_value() const { return _value; }
  T &get_value() { return _value; }
  static variable *create(context *ctx, const T &val = {}) {
    return ctx->get_scope()->create_variable(new simple_variable<T, vt>(val));
  }
  static T &value_of(variable *val) {
    auto *v = (simple_variable<T, vt> *)val->get_data();
    return v->get_value();
  }
};
using null_variable =
    simple_variable<void *, base_variable::variable_type::VT_NULL>;
using undefined_variable =
    simple_variable<void *, base_variable::variable_type::VT_UNDEFINED>;

} // namespace atom::engine
