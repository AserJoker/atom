#include "runtime/include/function.hpp"
using namespace atom::runtime;
function::function(handle *hfunction,cfunction callee,int32_t length,const std::string& name):object(hfunction){_callee = callee;_length = length;_name = name;}
