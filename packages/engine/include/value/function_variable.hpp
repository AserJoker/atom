#pragma once
#include "engine/include/framework/context.hpp"
#include "engine/include/value/base_variable.hpp"
#include "engine/include/value/object_variable.hpp"
#include <functional>
#include <source_location>
#include <vector>
namespace atom::engine {
class function_variable : public object_variable {
public:
  using function_handle = std::function<variable *(
      context *, variable *, const std::vector<variable *> &)>;
  static context::call_frame loc_to_frame(const std::source_location &loc) {
    context::call_frame frame = {._filename = loc.file_name(),
                                 ._funcname = loc.function_name(),
                                 ._line = loc.line(),
                                 ._column = loc.column()};
    return frame;
  }

private:
  function_handle _callee;
  node *_bind;

protected:
  int32_t _length;
  std::string _name;
  node *_prototype;
  function_variable(const function_handle &);

public:
  ~function_variable() override;
  static variable *create(context *, const std::string &, int32_t,
                          const function_handle &, variable * = nullptr);
  static variable *call(context *, variable *, variable *,
                        const std::vector<variable *> &,
                        const context::call_frame &);
  static variable *
  call(context *ctx, variable *fn, variable *self,
       const std::vector<variable *> &args = {},
       const std::source_location &loc = std::source_location::current()) {
    return call(ctx, fn, self, args,
                {._filename = loc.file_name(),
                 ._line = loc.line(),
                 ._column = loc.column()});
  }

  static bool set_field(context *, variable *, const uint32_t &, variable *);
  static variable *get_field(context *, variable *, const uint32_t &);

  static bool set_field(context *, variable *, const std::string &, variable *);
  static variable *get_field(context *, variable *, const std::string &);

  static variable *bind(context *, variable *, variable *);
};
} // namespace atom::engine
#define native_function(name)                                                  \
  engine::variable *name(engine::context *ctx, engine::variable *self,         \
                         const std::vector<variable *> &args)
