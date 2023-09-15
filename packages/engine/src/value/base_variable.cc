#include "engine/include/value/base_variable.hpp"
using namespace atom::engine;
base_variable::base_variable(const variable_type &vt) : _type(vt) {}
base_variable::~base_variable() {}
const base_variable::variable_type &base_variable::get_type() const {
  return _type;
}
