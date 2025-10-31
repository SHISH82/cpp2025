#include <cmath>
#include "plugin_api.h"

namespace {
    constexpr double kPi = 3.14159265358979323846;
    double cos_deg(double d) { return std::cos(d * kPi / 180.0); }
}

CALC_PLUGIN_EXPORT bool calc_register(calc::plugin::FunctionDescriptor& out) {
    out.abi_version = calc::plugin::kAbiVersion;
    out.name = "cos";
    out.invoke = &cos_deg;
    return true;
}
