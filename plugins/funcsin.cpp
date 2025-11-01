#include "calc/plugin_api.h"
#include <cmath>
using namespace calc::plugin;


static double CALC_CALL f(double x_deg) {
    constexpr long double PI = 3.141592653589793238462643383279502884L;
    long double rad = (long double)x_deg * (PI / 180.0L);
    return std::sin((double)rad);
}

CALC_API bool CALC_CALL calc_register(FunctionDescriptor& d) {
    d.abi_version = kAbiVersion;
    d.name   = "sin";
    d.invoke = &f;
    return true;
}
