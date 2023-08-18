#include "runtime/runtime.hpp"
#include <fmt/format.h>
#include <iostream>
#include <stdio.h>
using namespace atom::runtime;
void run_main() {
  context *ctx = new context();
  value *fn = ctx->get_scope()->create_function(
      [](context *ctx, value *self, std::vector<value *> &args) -> value * {
        static int32_t data = 0;
        auto stacks = ctx->get_call_stack(__LINE__, 1);
        for (auto &stack : stacks) {
          std::cout << stack << std::endl;
        }
        return ctx->get_scope()->create(data++);
      },
      0, "fn", __FILE__);
  value *obj = ctx->get_scope()->create_object();
  value *obj2 = ctx->get_scope()->create_object(obj);
  obj->get_object()->define("data", fn, nullptr);
  std::cout << obj->get_object()->get(ctx, "data")->get_integer() << std::endl;
  std::cout << obj2->get_object()->get(ctx, "data")->get_integer() << std::endl;
  std::cout << obj->get_object()->get(ctx, "data")->get_integer() << std::endl;
  delete ctx;
}
int main(int argc, char *argv[]) {
  eventloop *eloop = eventloop::instance();
  eloop->createTask(run_main);
  eloop->run();
  return 0;
}
