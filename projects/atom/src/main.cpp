#include "runtime/include/framework/context.hpp"
#include "runtime/include/framework/runtime.hpp"
#include "runtime/include/memory/chunk.hpp"
#include "runtime/include/value/object_variable.hpp"
#include "runtime/include/value/scope.hpp"
#include "runtime/include/value/simple_variable.hpp"
#include "runtime/include/value/variable.hpp"
#include <fmt/format.h>
#include <iostream>
#include <ostream>
#include <stdio.h>
#include <vector>
using namespace atom;
runtime::variable *print(runtime::context *ctx, runtime::variable *self,
                         const std::vector<runtime::variable *> &args) {
  runtime::variable *data =
      runtime::object_variable::getProperty(ctx, self, "data");
  auto *ndata = dynamic_cast<runtime::number_variable *>(data->get_data());
  std::cout << ndata->get_value() << std::endl;
  return ctx->undefined();
}
int main(int argc, char *argv[]) {
  try {
    runtime::runtime *rt = new runtime::runtime();
    runtime::context *ctx = new runtime::context(rt);
    auto proto = runtime::object_variable::create(ctx, ctx->null());
    auto obj = runtime::object_variable::create(ctx, proto);
    runtime::object_variable::define(
        proto, "data",
        ctx->get_scope()->create_variable(new runtime::number_variable(123)));
    print(ctx, obj, {});
    delete ctx;
    delete rt;
    core::object::memory_leak_check();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
