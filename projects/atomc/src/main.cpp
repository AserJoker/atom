#include "runtime/include/context.hpp"
#include "runtime/include/eventloop.hpp"
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
  value *obj = ctx->get_scope()->create_object();
  ctx->push_scope();
  value *str = ctx->get_scope()->create("hello world");
  object *o = (object *)obj->get_data();
  o->define("data", str);
  ctx->pop_scope();
  str = o->get(ctx->get_scope(), "data");
  std::cout << str->get_string() << std::endl;
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
