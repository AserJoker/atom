#include "runtime/include/value/function.hpp"
using namespace atom::runtime;
function::function(handle *hfunction, handle *proto, cfunction callee,
                   int32_t length, const std::string &name,
                   const std::string &filename)
    : object(hfunction, proto) {
  _filename = filename;
  _callee = callee;
  _length = length;
  _name = name;
}
value *function::call(uint32_t column, uint32_t line, context *ctx, value *self,
                      std::vector<value *> args) {
  scope *current = ctx->get_scope();
  ctx->push_scope();
  ctx->push_call_stack(_filename, _name, line, column);
  value *local_result = _callee(ctx, self, args);
  value *result = current->create(local_result->get_handle());
  ctx->pop_call_stack();
  ctx->pop_scope();
  return result;
}
