#include "runtime/include/object.hpp"
#include <iostream>
using namespace atom::runtime;
object::object(handle *handle)
    : _isSealed(false), _isFrozen(false), _handle(handle) {
  std::cout << "object" << std::endl;
};
object::~object() { std::cout << "~object" << std::endl; }
void object::seal() { _isSealed = true; }
void object::freeze() { _isFrozen = true; }
bool object::isSealed() { return _isSealed; }
bool object::isFrozen() { return _isFrozen; }
bool object::define(const std::string &field, value *value) {
  property prop;
  prop._enumable = true;
  prop._writable = true;
  prop._configurable = true;
  prop._value = value->get_handle();
  prop._getter = nullptr;
  prop._setter = nullptr;
  _handle->add_ref(prop._value);
  key k = field;
  _properties.insert({{k, prop}});
  return true;
}
bool object::define(const std::string &field, value *getter, value *setter) {
  property prop;
  prop._enumable = true;
  prop._writable = true;
  prop._configurable = true;
  prop._value = nullptr;
  prop._getter = getter->get_handle();
  prop._setter = setter ? setter->get_handle() : nullptr;
  _handle->add_ref(prop._getter);
  if (prop._setter) {
    _handle->add_ref(prop._setter);
  }
  key k = field;
  _properties.insert({{k, prop}});
  return true;
}
bool object::set(const std::string &field, value *val) {
  auto *prop = getProperty(field);
  if (!prop) {
    return false;
  }
  if (!prop->_configurable || !prop->_writable) {
    return false;
  }
  handle *h = val->get_handle();
  _handle->add_ref(h);
  if (prop->_value) {
    _handle->remove_ref(prop->_value);
    prop->_value = h;
  } else {
    // TODO: setter
  }
  return true;
}
value *object::get(scope *scope, const std::string &field) {
  auto *prop = getProperty(field);
  if (!prop) {
    return nullptr;
  }
  if (prop->_value) {
    return scope->create(prop->_value);
  } else {
    // getter
  }
  return nullptr;
}
bool object::set(value *s, value *val) {
  auto *prop = getProperty(s);
  if (!prop) {
    return false;
  }
  if (!prop->_configurable || !prop->_writable) {
    return false;
  }
  handle *h = val->get_handle();
  _handle->add_ref(h);
  if (prop->_value) {
    _handle->remove_ref(prop->_value);
    prop->_value = h;
  } else {
    // TODO: setter
  }
  return true;
}
value *object::get(scope *sp, value *s) {
  auto *prop = getProperty(s);
  if (!prop) {
    return nullptr;
  }
  if (prop->_value) {
    return sp->create(prop->_value);
  } else {
    // getter
  }
  return nullptr;
}
object::property *object::getProperty(const std::string &field) {
  key k = field;
  if (_properties.contains(k)) {
    return &_properties.at(k);
  }
  return nullptr;
}
object::property *object::getProperty(value *field) {
  key k = field->get_handle();
  if (_properties.contains(k)) {
    return &_properties.at(k);
  }
  return nullptr;
}
