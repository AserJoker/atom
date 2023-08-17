#include "runtime/include/context.hpp"
#include "runtime/include/eventloop.hpp"
#include "runtime/include/function.hpp"
#include "runtime/include/handle.hpp"
#include "runtime/include/object.hpp"
#include "runtime/include/stack.hpp"
#include "runtime/include/value.hpp"
#include <fmt/format.h>
#include <iostream>
#include <stdio.h>
using namespace atom::runtime;
void run_main() {
  context *ctx = new context();
  value *fn = ctx->get_scope()->create_function(
      [](context *ctx, value *self, std::vector<value *> &args) -> value * {
        std::cout << "hello world" << std::endl;
        return ctx->get_scope()->create_undefined();
      },
      0, "fn");
  function *func = (function *)fn->get_data();
  func->call(ctx, nullptr );
  delete ctx;
}
int main(int argc, char *argv[]) {
  eventloop *eloop = eventloop::instance();
  eloop->createTask(run_main);
  eloop->run();
  if (base::count != 0) {
    throw std::runtime_error("memory leak");
  }
  return 0;
}
