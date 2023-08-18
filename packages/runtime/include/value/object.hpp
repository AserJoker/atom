#pragma once
#include "runtime/include/framework/context.hpp"
#include "runtime/include/value/scope.hpp"
#include "value.hpp"
#include <map>
namespace atom::runtime {
class function;
class object : public base {
public:
  struct property {
    bool _enumable;
    bool _writable;
    bool _configurable;
    handle *_value;
    handle *_getter;
    handle *_setter;
  };
  struct key {
    std::string str;
    handle *symbol;
    key(const std::string &s) {
      str = s;
      symbol = nullptr;
    }
    key(handle *s) { symbol = s; }
    bool operator==(key &another) const {
      return another.symbol ? another.symbol == symbol : another.str == str;
    }
    bool operator<(const key &another) const {
      if (symbol) {
        return (uint64_t)symbol < (uint64_t)another.symbol;
      } else {
        return str < another.str;
      }
    }
  };

private:
  bool _isSealed;
  bool _isFrozen;
  std::map<key, property> _properties;
  handle *_proto_;

protected:
  handle *_handle;

public:
  object(handle *hobject, handle *_proto_);
  ~object() override;
  void seal();
  bool isSealed();
  void freeze();
  bool isFrozen();
  bool define(const std::string &key, value *value);
  bool define(const std::string &key, value *getter, value *setter);
  bool set(context *ctx, const std::string &key, value *value);
  value *get(context *ctx, const std::string &key);
  bool set(context *ctx, value *symbol, value *value);
  value *get(context *ctx, value *symbol);
  property *getProperty(const std::string &key);
  property *getProperty(value *symbol);
  value *getOwnPrototype(context *ctx);
};
} // namespace atom::runtime
