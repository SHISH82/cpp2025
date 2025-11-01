#pragma once
#include "calc/tokenizer.hpp"
#include <functional>
#include <optional>
#include <map>
#include <string>
#include <vector>

namespace calc {

    struct OperatorInfo {
        int  precedence;
        bool right_assoc;
        int  arity;
        std::function<double(double,double)> binary;
        std::function<double(double)>        unary;
    };

    class OperatorTable {
    public:
        OperatorTable();
        const OperatorInfo& get(const std::string& k) const;
        bool contains(const std::string& k) const;
    private:
        std::map<std::string, OperatorInfo> tbl;
    };

    class RpnCompiler {
    public:
        std::vector<Token> compile(const std::vector<Token>& tokens);
    private:
        static bool is_unary(const std::optional<Token>& prev, const Token& tok);
    };

} // namespace calc
