#pragma once
#include <cstdint>
namespace atom::core {
class object {
private:
  uint32_t _ref;

public:
  uint32_t add_ref();
  uint32_t sub_ref();
  object();
  virtual ~object();
};
} // namespace atom::core
