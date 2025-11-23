#include "calc/plugin_api.h"
#include <cmath>
#include <stdexcept>
using namespace calc::plugin;

static double CALC_CALL f(double x) {
    if (x < 0) throw std::domain_error("sqrt: x < 0");
    return std::sqrt(x);
}

CALC_API bool CALC_CALL calc_register(FunctionDescriptor& d) {
    d.abi_version = kAbiVersion;
    d.name   = "sqrt";
    d.invoke = &f;
    return true;
}
