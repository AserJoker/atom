#include "runtime/include/eventloop.hpp"
using namespace atom::runtime;
void eventloop::run() {
  while (_micro_tasks.size() || _tasks.size()) {
    while (_micro_tasks.size()) {
      auto [id, task] = *_micro_tasks.begin();
      _micro_tasks.erase(id);
      task();
    }
    auto [id, task] = *_tasks.begin();
    _tasks.erase(id);
    task();
  }
}
uint32_t eventloop::createNativeTask(std::function<void()> task) {
  static uint32_t index = 0;
  _tasks.insert({++index, task});
  return index;
}
uint32_t eventloop::createNativeMicroTask(std::function<void()> task) {
  static uint32_t index = 0;
  _micro_tasks.insert({++index, task});
  return index;
}
eventloop *eventloop::instance() {static eventloop eloop;return &eloop;}
