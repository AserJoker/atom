#pragma once
#include "variable.hpp"
#include <cstdint>
namespace atom::engine {
template <class T, variable_type t> class variable_number : public variable {
private:
  T _data;

public:
  variable_number(context *ctx, const T &data = 0)
      : variable(ctx, t), _data(data) {}
  T &data() { return _data; }
  const T &data() const { return _data; }
  const std::string to_string() const override {
    return fmt::format("{}", _data);
  }
};
using variable_int8 = variable_number<int8_t, VT_INT8>;
using variable_int16 = variable_number<int16_t, VT_INT16>;
using variable_int32 = variable_number<int32_t, VT_INT32>;
using variable_int64 = variable_number<int64_t, VT_INT64>;
using variable_uint8 = variable_number<uint8_t, VT_UINT8>;
using variable_uint16 = variable_number<uint16_t, VT_UINT16>;
using variable_uint32 = variable_number<uint32_t, VT_UINT32>;
using variable_uint64 = variable_number<uint64_t, VT_UINT64>;
using variable_float32 = variable_number<float_t, VT_FLOAT32>;
using variable_float64 = variable_number<double_t, VT_FLOAT64>;
} // namespace atom::engine