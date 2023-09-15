#include "engine/include/value/function_variable.hpp"
using namespace atom::engine;
variable *function_variable::call(context *ctx, variable *func, variable *self,
                                  const std::vector<variable *> &args) {
  auto *fn = (function_variable *)func->get_data();
  auto &callee = fn->_callee;
  auto *scope = ctx->push_scope();
  variable *res = callee(ctx, self, args);
  variable *result = scope->create_variable(res->get_node());
  ctx->pop_scope(scope);
  return result;
}
