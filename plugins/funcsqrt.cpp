#include <cmath>
#include <stdexcept>
#include "plugin_api.h"

namespace {
    double mysqrt(double x) {
        if (x < 0.0) throw std::domain_error("sqrt: x < 0");
        return std::sqrt(x);
    }
}

CALC_PLUGIN_EXPORT bool calc_register(calc::plugin::FunctionDescriptor& out) {
    out.abi_version = calc::plugin::kAbiVersion;
    out.name = "sqrt";
    out.invoke = &mysqrt;
    return true;
}
