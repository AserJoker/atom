#pragma once
#include "object.hpp"
namespace atom::core {
template <class T> class auto_release {
private:
  T *_obj;
  void release() {
    if (_obj && !_obj->sub_ref()) {
      delete _obj;
    }
    _obj = nullptr;
  }

public:
  auto_release(T *obj = nullptr) {
    _obj = obj;
    if (obj) {
      obj->add_ref();
    }
  }
  auto_release(const auto_release<T> &another) {
    _obj = dynamic_cast<T *>(another._obj);
    _obj->add_ref();
  }
  ~auto_release() { release(); }
  T *operator->() { return _obj; }
  T &operator*() { return *_obj; }
  T *operator->() const { return const_cast<T *>(_obj); }
  T &operator*() const { return *const_cast<T *>(_obj); }
  auto_release<T> &operator=(const auto_release<T> &another) {
    auto *obj = const_cast<T *>(another._obj);
    if (obj) {
      obj->add_ref();
    }
    release();
    _obj = obj;
    return *this;
  }
  auto_release<T> &operator=(const T *another) {
    auto *obj = const_cast<T *>(another);
    if (obj) {
      obj->add_ref();
    }
    release();
    _obj = obj;
    return *this;
  }
  T *get() { return _obj; }
  T *get() const { return const_cast<T *>(_obj); }
  bool operator<(const auto_release<T> &another) const {
    return _obj < another._obj;
  }
  bool operator==(const auto_release<T> &another) const {
    return _obj == another._obj;
  }
  bool operator!=(const auto_release<T> &another) const {
    return _obj != another._obj;
  }
};
} // namespace atom::core
