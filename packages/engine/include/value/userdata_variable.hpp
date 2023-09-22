#pragma once
#include "base_variable.hpp"
#include "core/include/auto_release.hpp"
#include "engine/include/framework/context.hpp"
#include "variable.hpp"
namespace atom::engine {
class userdata_variable : public base_variable {
private:
  core::auto_release<core::object> _userdata;
  userdata_variable(const core::auto_release<core::object> &udata)
      : _userdata(udata), base_variable(variable_type::VT_USERDATA) {}
  ~userdata_variable() {}

public:
  static core::auto_release<core::object> get_userdata(variable *val) {
    return dynamic_cast<userdata_variable *>(val->get_data())->_userdata;
  }
  static variable *create(context *ctx,
                          const core::auto_release<core::object> &udata) {
    return ctx->create(new userdata_variable(udata));
  }
};
} // namespace atom::engine