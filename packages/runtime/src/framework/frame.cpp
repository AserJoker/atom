#include "runtime/include/framework/frame.hpp"
#include <fmt/format.h>
using namespace atom::runtime;
frame::frame(const std::string &filename, const std::string &funcname) {
  _filename = filename;
  _funcname = funcname;
}
void frame::set_position(uint32_t line, uint32_t column) {
  _line = line;
  _column = column;
}
frame::~frame() {}
uint32_t frame::get_line() { return _line; }
uint32_t frame::get_column() { return _column; }
std::string frame::get_filename() { return _filename; }
std::string frame::get_funcname() { return _funcname; }
std::string frame::format() {
  if (_filename == "<internel>") {
    return fmt::format("{} ({})", _funcname, _filename);
}
return fmt::format("{} ({}:{},{})", _funcname, _filename, _line, _column);
}
