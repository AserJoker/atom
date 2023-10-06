#include "../include/variable_error.hpp"
using namespace atom::engine;
variable_error::variable_error(context *ctx, const std::string &msg,
                               const std::string &filename,
                               const uint32_t &line)
    : variable(ctx, VT_ERROR), _msg(msg) {}