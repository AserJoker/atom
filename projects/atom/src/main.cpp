
#include "engine/include/context.hpp"
#include "engine/include/runtime.hpp"
#include "engine/include/scope.hpp"
#include "engine/include/variable_number.hpp"
#include "engine/include/variable_string.hpp"
#include <iostream>

using namespace atom;
int main(int argc, char *argv[]) {
  engine::runtime *rt = new engine::runtime();
  engine::context *ctx = new engine::context(rt);
  auto val = new engine::variable_int32(ctx, 123);
  std::cout << val->data() << std::endl;
  delete ctx;
  delete rt;
  core::object::memory_leak_check();
  return 0;
}
