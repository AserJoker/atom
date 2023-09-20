#pragma once
#include "core/include/object.hpp"
namespace atom::engine {
class variable;
class base_variable : public core::object {
public:
  enum class variable_type {
    VT_NUMBER,
    VT_INTEGER,
    VT_STRING,
    VT_BOOLEAN,
    VT_UNDEFINED,
    VT_NULL,
    VT_OBJECT,
    VT_ARRAY,
    VT_FUNCTION,
  };

protected:
  variable_type _type;

public:
  explicit base_variable(const variable_type &);
  virtual ~base_variable();
  const variable_type &get_type() const;
  static const variable_type &type_of(variable *);
  static bool is_undefined(variable *val) {
    return type_of(val) == variable_type::VT_UNDEFINED;
  };
  static bool is_null(variable *val) {
    return type_of(val) == variable_type::VT_NULL;
  }
};
} // namespace atom::engine
