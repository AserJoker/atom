#include "runtime/include/function.hpp"
using namespace atom::runtime;
function::function(handle *hfunction, handle *proto, cfunction callee,
                   int32_t length, const std::string &name)
    : object(hfunction, proto) {
  _callee = callee;
  _length = length;
  _name = name;
}
value *function::call(context *ctx, value *self, std::vector<value *> args) {
  scope *current = ctx->get_scope();
  ctx->push_scope();
  value *local_result = _callee(ctx, self, args);
  value *result = current->create(local_result->get_handle());
  ctx->pop_scope();
  return result;
}
