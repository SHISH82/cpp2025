#include <cmath>
#include "plugin_api.h"

namespace {
    constexpr double kPi = 3.14159265358979323846;
    double rad2deg(double r) {
        return r * 180.0 / kPi;
    }
}

CALC_PLUGIN_EXPORT bool calc_register(calc::plugin::FunctionDescriptor& out) {
    out.abi_version = calc::plugin::kAbiVersion;
    out.name = "deg";
    out.invoke = &rad2deg;
    return true;
}
