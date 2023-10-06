#pragma once
#include "context.hpp"
#include "core/include/auto_release.hpp"
#include "scope.hpp"
#include <any>
#include <cstdint>
#include <fmt/format.h>
#include <list>
#include <map>
#include <string>
#include <vector>
namespace atom::engine {
enum variable_type {
  VT_ERROR = -1,
  VT_NIL,
  VT_OBJECT,
  VT_FUNCTION,
  VT_ARRAY,
  VT_STRING,
  VT_BOOLEAN,
  VT_FLOAT32,
  VT_FLOAT64,
  VT_INT8,
  VT_INT16,
  VT_INT32,
  VT_INT64,
  VT_UINT8,
  VT_UINT16,
  VT_UINT32,
  VT_UINT64,
  VT_USERDATA,
};
class variable : public core::object {
private:
  std::list<variable *> _parents;
  std::list<variable *> _children;
  std::list<scope *> _scopes;

  int _type;

  uint64_t _gc_time;
  uint64_t _check_time;

private:
  bool check_alone(uint64_t);

  inline static uint64_t gc_time = 0;

public:
  variable(context *, const int &);
  ~variable();
  void add_variable(variable *);
  void remove_variable(variable *);
  std::list<scope *> &get_groups();
  void auto_release();
  static inline std::vector<variable *> variables;

public:
  const int &type_of() const { return _type; }
  int &type_of() { return _type; }
  variable *to_string(context *);
  virtual variable *value_of() { return this; };
  virtual const variable *value_of() const { return this; };
  virtual const std::string to_string() const {
    return fmt::format("0x{:X}", (ptrdiff_t)this);
  };
  virtual variable *get(context *, const std::string &) const;
  virtual variable *set(context *, const std::string &, variable *);

  virtual variable *get(context *, const int32_t &) const;
  virtual variable *set(context *, const int32_t &, variable *);
};
} // namespace atom::engine
