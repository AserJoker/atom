#pragma once
#include "core/include/object.hpp"
namespace atom::runtime {
class base_variable : public core::object {
public:
  enum class variable_type {
    VT_NUMBER,
    VT_INTEGER,
    VT_STRING,
    VT_BOOLEAN,
    VT_OBJECT,
    VT_ARRAY,
    VT_FUNCTION,
    VT_UNDEFINED,
    VT_NULL
  };

private:
  variable_type _type;

public:
  explicit base_variable(const variable_type &);
  virtual ~base_variable();
  const variable_type &get_type() const;
};
} // namespace atom::runtime
