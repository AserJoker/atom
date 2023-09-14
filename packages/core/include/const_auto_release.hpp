#pragma once
#include "object.hpp"
namespace atom::core {
template <class T> class const_auto_release {
private:
  T *_obj;

public:
  const_auto_release(const T *obj = nullptr) : _obj(obj) { _obj->add_ref(); }
  const_auto_release(const const_auto_release<T> &another)
      : _obj(another._obj) {
    _obj->add_ref();
  }
  const T *operator->() const { return _obj; }
  const T &operator*() const { return *_obj; }
  const T *get() const { return _obj; }

};
} // namespace atom::core
