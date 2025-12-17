#pragma once

#include <string>
#include <vector>

template<typename Obj, typename Method, typename ArgType>
class wrapper
{
public:
    wrapper(Obj* obj,
        Method method,
        const std::vector<std::pair<std::string, ArgType>>& defaults)
        : objectRef(obj),
        methodRef(method),
        defaultArgs(defaults)
    {
    }

    void set_name(const std::string& text)
    {
        name = text;
    }
    const std::string& get_name() const
    {
        return name;
    }

    ArgType call(const std::vector<std::pair<std::string, ArgType>>& args)
    {
        ArgType a = get_value("arg1", args);
        ArgType b = get_value("arg2", args);

        return std::invoke(methodRef, objectRef, a, b);
    }

private:
    std::string name;

    Obj* objectRef;
    Method methodRef;

    std::vector<std::pair<std::string, ArgType>> defaultArgs;

private:
    ArgType get_value(const std::string& key,
        const std::vector<std::pair<std::string, ArgType>>& args)
    {
        for (const auto& p : args)
        {
            if (p.first == key)
                return p.second;
        }

        for (const auto& p : defaultArgs)
        {
            if (p.first == key)
                return p.second;
        }

        return ArgType{};
    }
};
