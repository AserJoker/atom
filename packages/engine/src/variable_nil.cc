#include "../include/variable_nil.hpp"
using namespace atom::engine;
variable_nil::variable_nil(context *ctx) : variable(ctx, VT_NIL) {}