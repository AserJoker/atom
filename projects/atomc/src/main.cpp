#include "runtime/include/context.hpp"
#include "runtime/include/eventloop.hpp"
#include "runtime/include/memory/handle.hpp"
#include "runtime/include/memory/stack.hpp"
#include "runtime/include/value/function.hpp"
#include "runtime/include/value/object.hpp"
#include "runtime/include/value/value.hpp"
#include <fmt/format.h>
#include <iostream>
#include <stdio.h>
using namespace atom::runtime;
void run_main() {
  context *ctx = new context();
  value *fn = ctx->get_scope()->create_function(
      [](context *ctx, value *self, std::vector<value *> &args) -> value * {
        static int32_t data = 0;
        return ctx->get_scope()->create(data++);
      },
      0, "fn");
  value *vobj = ctx->get_scope()->create_object();
  object *obj = vobj->get_object();
  value *vobj2 = ctx->get_scope()->create_object(vobj);
  auto *obj2 = vobj2->get_object();
  obj->define("data", fn, nullptr);
  std::cout << obj->get(ctx, "data")->get_integer() << std::endl;
  std::cout << obj2->get(ctx, "data")->get_integer() << std::endl;
  std::cout << obj->get(ctx, "data")->get_integer() << std::endl;
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
