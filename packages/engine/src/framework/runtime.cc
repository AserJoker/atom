#include "engine/include/framework/runtime.hpp"
#include "engine/include/memory/node.hpp"
#include "engine/include/value/simple_variable.hpp"
using namespace atom::engine;
runtime::runtime() {
  _scope = new scope();
  _undefined = new node(_scope->get_chunk(), new undefined_variable());
  _null = new node(_scope->get_chunk(), new null_variable());
}
runtime::~runtime() {
  delete _scope;
  _scope = nullptr;
  _undefined = nullptr;
  _null = nullptr;
}
scope *runtime::get_scope() { return _scope; }
node *runtime::undefined() { return _undefined; }
node *runtime::null() { return _null; }
