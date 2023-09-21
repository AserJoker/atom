
#include "engine/include/value/string_variable.hpp"
#include "engine/include/value/number_variable.hpp"
using namespace atom::engine;
bool string_variable::set_field(context *ctx, variable *obj,
                                const std::string &name, variable *value) {
  return false;
}
variable *string_variable::get_field(context *ctx, variable *obj,
                                     const std::string &name) {
  if (name == "length") {
    return number_variable::create(ctx, (int32_t)value_of(obj).size());
  }
  return ctx->undefined();
}

bool string_variable::set_field(context *ctx, variable *obj,
                                const uint32_t &name, variable *value) {
  return false;
}
variable *string_variable::get_field(context *ctx, variable *obj,
                                     const uint32_t &name) {
  return ctx->undefined();
}