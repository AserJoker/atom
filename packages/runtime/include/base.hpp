#pragma once
#include <cstddef>
namespace atom::runtime {
class base {
public:
  static size_t count;

public:
  virtual ~base();
  base();
  void *operator new(size_t size);
  void operator delete(void *obj);
};
} // namespace atom::runtime
