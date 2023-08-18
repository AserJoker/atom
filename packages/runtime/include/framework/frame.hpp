#pragma once
#include "runtime/include/base.hpp"
#include <cstdint>
#include <string>
namespace atom::runtime {
class frame : public base {
private:
  uint32_t _line;
  uint32_t _column;
  std::string _filename;
  std::string _funcname;

public:
  frame(const std::string &filename, const std::string &funcname);
  ~frame() override;
  void set_position(uint32_t line, uint32_t column); std::string format();
  uint32_t get_line();
  uint32_t get_column();
  std::string get_filename();
  std::string get_funcname();
};
} // namespace atom::runtime
