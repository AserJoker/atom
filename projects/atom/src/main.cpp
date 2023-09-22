#include "engine/include/framework/context.hpp"
#include "engine/include/framework/runtime.hpp"
#include "engine/include/value/class_variable.hpp"
#include "engine/include/value/function_variable.hpp"
#include "engine/include/value/number_variable.hpp"
#include "engine/include/value/object_variable.hpp"
#include "engine/include/value/string_variable.hpp"
#include "engine/include/value/userdata_variable.hpp"
#include "engine/include/value/variable.hpp"
#include <fmt/format.h>
#include <iostream>
#include <ostream>
#include <source_location>
#include <stdio.h>
#include <vector>
using namespace atom;
engine::variable *print(engine::context *ctx, engine::variable *self,
                        const std::vector<engine::variable *> &args) {
  auto data = engine::object_variable::get(ctx, self, "data");
  std::cout << engine::number_variable::value_of(data) << std::endl;
  auto stack = ctx->get_call_stack(__LINE__, 0);
  for (auto &frame : stack) {
    std::cout << "at " << frame << std::endl;
  }
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
class DemoClass : public core::object {
private:
  int data;

public:
  DemoClass(engine::variable *self, const std::vector<engine::variable *> &) {
    std::cout << "DemoClass" << std::endl;
    data = 123;
  }
  ~DemoClass() { std::cout << "~DemoClass" << std::endl; }
  engine::variable *print(engine::context *ctx,
                          const std::vector<engine::variable *> &) {
    std::cout << data << std::endl;
    return ctx->undefined();
  }
};
int main(int argc, char *argv[]) {
  atexit(on_exit);
  core::auto_release runtime = new engine::runtime;
  runtime->create_macro_job([runtime] {
    core::auto_release ctx = new engine::context(runtime);
    auto clazz = engine::class_variable::create<DemoClass>(
        ctx.get(), "DemoClass", {{"print", &DemoClass::print}});
    auto obj = engine::object_variable::construct(ctx.get(), clazz);
    auto print = engine::base_variable::get(ctx.get(), obj, "print");
    engine::function_variable::call(ctx.get(), print, obj);
  });
  runtime->run();
  return 0;
}
