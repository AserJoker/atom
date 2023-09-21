#include "engine/include/framework/context.hpp"
#include "engine/include/framework/runtime.hpp"
#include "engine/include/value/function_variable.hpp"
#include "engine/include/value/number_variable.hpp"
#include "engine/include/value/object_variable.hpp"
#include "engine/include/value/string_variable.hpp"
#include "engine/include/value/variable.hpp"
#include <fmt/format.h>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <vector>
using namespace atom;
engine::variable *print(engine::context *ctx, engine::variable *self,
                        const std::vector<engine::variable *> &args) {
  auto data = engine::object_variable::get(ctx, self, "data");
  std::cout << engine::number_variable::value_of(data) << std::endl;
  return ctx->undefined();
}
engine::variable *DemoConstructor(engine::context *ctx, engine::variable *self,
                                  const std::vector<engine::variable *> &args) {
  engine::base_variable::set(ctx, self, "data", args[0]);
  return ctx->undefined();
}
void on_exit() {
  try {
    core::object::memory_leak_check();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}
int main(int argc, char *argv[]) {
  atexit(on_exit);
  core::auto_release runtime = new engine::runtime;
  runtime->create_macro_job([runtime] {
    core::auto_release ctx = new engine::context(runtime);
    auto proto = engine::object_variable::create(ctx.get());
    engine::base_variable::set(
        ctx.get(), proto, "print",
        engine::function_variable::create(ctx.get(), "print", 0, print));
    auto Demo = engine::function_variable::create(ctx.get(), "Demo", 1,
                                                  DemoConstructor, proto);
    auto obj = engine::object_variable::construct(
        ctx.get(), Demo, {engine::number_variable::create(ctx.get(), 123)});
    auto printFn = engine::object_variable::get(ctx.get(), obj, "print");
    engine::function_variable::call(ctx.get(), printFn, obj);
  });
  runtime->run();
  return 0;
}
