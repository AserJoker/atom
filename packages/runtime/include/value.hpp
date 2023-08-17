#pragma once
#include "base.hpp"
#include "handle.hpp"
#include "scope.hpp"
#include "value_type.hpp"
#include <cstdint>
#include <string>
namespace atom::runtime {
class value : public base {
public:
  class string : public base {
  public:
    std::string data;
    string(const std::string &data) : data(data) {}
  };
  class number : public base {
  public:
    double data;
    number(const double &data) : data(data) {}
  };
  class integer : public base {
  public:
    int32_t data;
    integer(const int32_t &data) : data(data) {}
  };
  class boolean : public base {
  public:
    bool data;
    boolean(const bool &data) : data(data) {}
  };

private:
  class type_base : public base {
  private:
    value_type _type;
    base *_data;

  public:
    type_base(value_type t = VT_UNDEFINED, base *data = nullptr);
    ~type_base() override;
    value_type get_type();
    base *get_data();
  };

private:
  handle *_handle;
  scope *_scope;
  value() = default;
  friend class scope;

public:
  ~value() override;
  value_type get_type();
  base *get_data();
  handle* get_handle();
  std::string &get_string();
  double &get_number();
  int32_t &get_integer();
  bool &get_boolean();
};
} // namespace atom::runtime
