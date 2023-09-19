#include "engine/include/value/base_variable.hpp"
#include "engine/include/value/variable.hpp"
using namespace atom::engine;
base_variable::base_variable(const variable_type &vt) : _type(vt) {}
base_variable::~base_variable() {}
const base_variable::variable_type &base_variable::get_type() const {
  return _type;
}
const base_variable::variable_type &base_variable::type_of(variable *v) {
  auto *vdata = (base_variable *)v->get_data();
  return vdata->get_type();
}