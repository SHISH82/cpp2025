#include <cmath>
#include <stdexcept>
#include "plugin_api.h"

namespace {
    double myln(double x) {
        if (x <= 0.0) throw std::domain_error("ln: x <= 0");
        return std::log(x);
    }
}

CALC_PLUGIN_EXPORT bool calc_register(calc::plugin::FunctionDescriptor& out) {
    out.abi_version = calc::plugin::kAbiVersion;
    out.name = "ln";
    out.invoke = &myln;
    return true;
}
