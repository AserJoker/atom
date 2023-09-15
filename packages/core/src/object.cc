#include "core/include/object.hpp"
#include <fmt/core.h>
#include <stdexcept>
using namespace atom::core;
static size_t memory_size = 0;
uint32_t object::add_ref() { return ++_ref; }
uint32_t object::sub_ref() { return --_ref; }
object::object() : _ref(0) {}
object::~object() {}
void *object::operator new(size_t size) {
  void *buf = ::operator new(size + sizeof(size_t));
  size_t *sbuf = (size_t *)buf;
  *sbuf = size;
  memory_size += size;
  return &sbuf[1];
}
void object::operator delete(void *buf) {
  size_t *sbuf = (size_t *)buf - 1;
  size_t size = sbuf[0];
  ::operator delete(sbuf);
  memory_size -= size;
}
void object::memory_leak_check() {
  if (memory_size) {
    throw std::runtime_error(fmt::format("memory leak: {}\n", memory_size));
  }
}
