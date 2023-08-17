#include "runtime/include/base.hpp"
#include <iostream>
#include <typeinfo>
using namespace atom::runtime;
size_t base::count = 0;
base::base() {}
base::~base() {}
void *base::operator new(size_t size) {
  count++;
  return ::operator new(size);
}
void base::operator delete(void *obj) {--count; ::operator delete(obj); }
