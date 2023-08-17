#pragma once
namespace atom::runtime {
enum value_type {
  VT_UNDEFINED = 0,
  VT_NULL,
  VT_STRING,
  VT_NUMBER,
  VT_INTEGER,
  VT_BOOLEAN,
  VT_OBJECT,
  VT_ARRAY,
  VT_FUNCTION
};
} // namespace atom::runtime
