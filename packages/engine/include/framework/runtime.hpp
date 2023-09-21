#pragma once
#include "core/include/object.hpp"
#include "engine/include/memory/node.hpp"
#include "engine/include/value/scope.hpp"
#include <functional>
namespace atom::engine {
class runtime : public core::object {
public:
  using event_handle = std::function<void()>;

private:
  struct event {
    event_handle handle;
    uint64_t timeout;
    uint32_t index;
  };
  std::vector<event> _macro_jobs;
  std::vector<event> _micro_jobs;
  static inline uint32_t _index = 0;

public:
  runtime();
  ~runtime();
  void run();
  uint32_t create_macro_job(const event_handle &, uint64_t = 4);
  uint32_t create_micro_job(const event_handle &, uint64_t = 0);
};
} // namespace atom::engine
