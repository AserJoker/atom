#pragma once
#include "chunk.hpp"
#include "core/include/auto_release.hpp"
#include "engine/include/value/base_variable.hpp"
#include <cstdint>
#include <list>
#include <vector>
namespace atom::engine {
class node : public core::object {
private:
  std::list<node *> _parents;
  std::list<node *> _children;
  std::list<chunk *> _chunks;

  base_variable *_data;

  uint64_t _gc_time;
  uint64_t _check_time;

private:
  bool check_alone(uint64_t);

  inline static uint64_t gc_time = 0;

public:
  node(chunk *, base_variable * = nullptr);
  node(node *, base_variable * = nullptr);
  ~node();
  void add_node(node *);
  void remove_node(node *);
  std::list<chunk *> &get_chunks();
  void auto_release();
  base_variable *get_data();
  static inline std::vector<node *> nodes;
};
} // namespace atom::engine
