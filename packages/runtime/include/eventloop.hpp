#pragma once
#include <functional>
#include <map>
namespace atom::runtime {
class eventloop {
private:
  uint32_t createNativeTask(std::function<void()> task);
  uint32_t createNativeMicroTask(std::function<void()> task);

public:
  void run();
  template <class F> uint32_t createTask(F task) {
    std::function<void()> fn = [=]() -> void { task(); };
    return createNativeTask(fn);
  }
  template <class F> uint32_t createMicroTask(F task) {
    std::function<void()> fn = [=]() -> void { task(); };
    return createNativeMicroTask(fn);
  }
  static eventloop *instance();

private:
  std::map<uint32_t, std::function<void()>> _tasks;
  std::map<uint32_t, std::function<void()>> _micro_tasks;
  eventloop() = default;
};
} // namespace atom::runtime
