#pragma once
#include <cstdint>

namespace calc::plugin {

#if defined(_WIN32)
#  define CALC_PLUGIN_EXPORT extern "C" __declspec(dllexport)
#else
#  define CALC_PLUGIN_EXPORT extern "C"
#endif

    struct FunctionDescriptor {
        uint32_t abi_version;
        const char* name;
        double (*invoke)(double);
    };

    using RegisterFunction = bool(*)(FunctionDescriptor&);

    inline constexpr uint32_t kAbiVersion = 1;
    inline constexpr const char* kRegistrationSymbol = "calc_register";

}
