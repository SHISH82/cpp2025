#pragma once

#include <string>
#include <vector>
#include "wrapper.h"

class engine
{

public:
    template<typename Obj, typename Method, typename ArgType>
    void register_cmd(wrapper<Obj, Method, ArgType>* cmd, const std::string& text)
    {
        cmd->set_name(text);
        list.push_back((void*)cmd);
    };

    template<typename Obj, typename Method, typename ArgType>
    ArgType execute(const std::string& text,
        const std::vector<std::pair<std::string, ArgType>>& args)
    {
        for (auto* ptr : list)
        {
            auto* cmd = static_cast<wrapper<Obj, Method, ArgType>*>(ptr);

            if (cmd->get_name() == text)
                return cmd->call(args);
        }

        return ArgType{};
    }


private:
    std::vector<void*> list;
};
