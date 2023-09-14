#include "core/include/object.hpp"
using namespace atom::core;
uint32_t object::add_ref() { return ++_ref; }
uint32_t object::sub_ref() { return --_ref; }
object::object() : _ref(0) {}
object::~object(){}
