#include "runtime/include/framework/context.hpp"
#include "runtime/include/memory/chunk.hpp"
#include "runtime/include/value/scope.hpp"
#include "runtime/include/value/simple_variable.hpp"
#include "runtime/include/value/variable.hpp"
#include <fmt/format.h>
#include <iostream>
#include <stdio.h>
using namespace atom::runtime;
using namespace atom;
int main(int argc, char *argv[]) {
  core::auto_release sc = new scope;
  auto str = sc->create_variable(new string_variable("hello world"));
  std::cout << dynamic_cast<string_variable *>(str->get_data())->get_value()
            << std::endl;
  return 0;
}
