#include "calc/plugin_api.h"
#include <cmath>
#include <stdexcept>
using namespace calc::plugin;

static double CALC_CALL f(double x) {
    if (x <= 0) throw std::domain_error("ln: x <= 0");
    return std::log(x);
}

CALC_API bool CALC_CALL calc_register(FunctionDescriptor& d) {
    d.abi_version = kAbiVersion;
    d.name   = "ln";
    d.invoke = &f;
    return true;
}
