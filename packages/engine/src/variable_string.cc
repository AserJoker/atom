#include "../include/variable_string.hpp"
#include "../include/variable_number.hpp"
using namespace atom::engine;
variable_string::variable_string(context *ctx, const std::string &data)
    : variable(ctx, VT_STRING), _data(data) {}
variable *variable_string::get(context *ctx, const std::string &name) const {
  if (name == "length") {
    return new variable_uint64(ctx, _data.length());
  }
  return variable::get(ctx, name);
}
const std::string variable_string::to_string() const { return _data; }