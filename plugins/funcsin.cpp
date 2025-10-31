#include <cmath>
#include "plugin_api.h"

namespace {
    constexpr double kPi = 3.14159265358979323846;
    double sin_deg(double d) { return std::sin(d * kPi / 180.0); }
}

CALC_PLUGIN_EXPORT bool calc_register(calc::plugin::FunctionDescriptor& out) {
    out.abi_version = calc::plugin::kAbiVersion;
    out.name = "sin";
    out.invoke = &sin_deg;
    return true;
}
