#pragma once
#include <cstdint>

namespace calc::plugin {

#if defined(_WIN32)
#define CALC_API   extern "C" __declspec(dllexport)
#define CALC_CALL  __cdecl
#else
#define CALC_API   extern "C"
#define CALC_CALL
#endif

    using InvokeFn = double (CALC_CALL *)(double);

    struct FunctionDescriptor {
        std::uint32_t abi_version;
        const char*   name;
        InvokeFn      invoke;
    };

    using RegisterFunction = bool (CALC_CALL *)(FunctionDescriptor&);

    inline constexpr std::uint32_t kAbiVersion = 1;
    inline constexpr const char*   kRegistrationSymbol = "calc_register";

    static_assert(sizeof(InvokeFn) == sizeof(void*), "InvokeFn must be a plain function pointer");

} // namespace calc::plugin
