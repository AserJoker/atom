#include "engine/include/framework/runtime.hpp"
#include "engine/include/memory/node.hpp"
#include "engine/include/value/simple_variable.hpp"
using namespace atom::engine;
runtime::runtime() {}
runtime::~runtime() {}
void runtime::run() {
  for (;;) {
    while (_micro_jobs.size()) {
      auto it = _micro_jobs.begin();
      auto &job = *_micro_jobs.begin();
      job.handle();
      if (it == _micro_jobs.begin()) {
        _micro_jobs.erase(it);
      }
    }
    if (_macro_jobs.empty()) {
      break;
    }
    auto it = _macro_jobs.begin();
    auto &job = *_macro_jobs.begin();
    job.handle();
    if (it == _macro_jobs.begin()) {
      _macro_jobs.erase(it);
    }
  }
}
uint32_t runtime::create_macro_job(const event_handle &handle,
                                   uint64_t timeout) {
  auto index = ++_index;
  _macro_jobs.emplace_back(event{handle, timeout, index});
  return index;
}
uint32_t runtime::create_micro_job(const event_handle &handle,
                                   uint64_t timeout) {
  auto index = ++_index;
  _micro_jobs.emplace_back(event{handle, timeout, index});
  return index;
}
