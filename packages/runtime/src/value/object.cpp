#include "runtime/include/value/object.hpp"
#include "runtime/include/value/function.hpp"
#include <iostream>
using namespace atom::runtime;
object::object(handle *hobject, handle *proto)
    : _isSealed(false), _isFrozen(false), _handle(hobject), _proto_(proto) {
  _handle->add_ref(_proto_);
};
object::~object() {}
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
bool object::set(context *ctx, const std::string &field, value *val) {
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
    ctx->push_scope();
    value *self = ctx->get_scope()->create(_handle);
    value *vsetter = ctx->get_scope()->create(prop->_setter);
    function *fn = (function *)vsetter->get_data();
    value *result = fn->call(0, 0, ctx, self, {val});
    bool res = result->get_boolean();
    ctx->pop_scope();
    return res;
  }
  return true;
}
value *object::get(context *ctx, const std::string &field) {
  property *prop = nullptr;
  auto *obj = this;
  while (!prop) {
    prop = obj->getProperty(field);
    if (prop) {
      break;
    }
    auto *vobj = (value::type_base *)_proto_->get_object();
    if (vobj->get_type() >= VT_OBJECT) {
      obj = (object *)vobj->get_data();
    } else {
      break;
    }
  }
  if (!prop) {
    return nullptr;
  }
  if (prop->_value) {
    return ctx->get_scope()->create(prop->_value);
  } else {
    value *self = ctx->get_scope()->create(_handle);
    value *vgetter = ctx->get_scope()->create(prop->_getter);
    function *fn = (function *)vgetter->get_data();
    value *result = fn->call(0, 0, ctx, self, {});
    delete vgetter;
    delete self;
    return result;
  }
  return nullptr;
}
bool object::set(context *ctx, value *s, value *val) {
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
    ctx->push_scope();
    value *self = ctx->get_scope()->create(_handle);
    value *vsetter = ctx->get_scope()->create(prop->_setter);
    function *fn = (function *)vsetter->get_data();
    value *result = fn->call(0,0,ctx, self, {val});
    bool res = result->get_boolean();
    ctx->pop_scope();
    return res;
  }
  return true;
}
value *object::get(context *ctx, value *s) {
  property *prop = nullptr;
  auto *obj = this;
  while (!prop) {
    prop = obj->getProperty(s);
    if (prop) {
      break;
    }
    auto *vobj = (value::type_base *)_proto_->get_object();
    if (vobj->get_type() >= VT_OBJECT) {
      obj = (object *)vobj->get_data();
    } else {
      break;
    }
  }
  if (!prop) {
    return nullptr;
  }
  if (prop->_value) {
    return ctx->get_scope()->create(prop->_value);
  } else {
    value *vgetter = ctx->get_scope()->create(prop->_getter);
    value *self = ctx->get_scope()->create(_handle);
    function *getter = (function *)vgetter->get_data();
    value *result = getter->call(0, 0, ctx, self, {});
    delete self;
    delete vgetter;
    return result;
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
value *object::getOwnPrototype(context *ctx) {
  return ctx->get_scope()->create(_proto_);
}
