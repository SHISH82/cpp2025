#include "calc/parser.hpp"
#include <stdexcept>
#include <cmath>
#include <optional>

namespace calc {

OperatorTable::OperatorTable() {
    tbl.emplace("+", OperatorInfo{10, false, 2, [](double a,double b){return a+b;}, {}});
    tbl.emplace("-", OperatorInfo{10, false, 2, [](double a,double b){return a-b;}, {}});
    tbl.emplace("*", OperatorInfo{20, false, 2, [](double a,double b){return a*b;}, {}});
    tbl.emplace("/", OperatorInfo{20, false, 2, [](double a,double b){ if (b==0.0) throw std::runtime_error("Деление на ноль"); return a/b; }, {}});
    tbl.emplace("^", OperatorInfo{30, true , 2, [](double a,double b){return std::pow(a,b);}, {}});
    tbl.emplace("u-",OperatorInfo{40, true , 1, {}, [](double x){return -x;}});
    tbl.emplace("u+",OperatorInfo{40, true , 1, {}, [](double x){return x;}});
}

const OperatorInfo& OperatorTable::get(const std::string& k) const {
    auto it = tbl.find(k);
    if (it == tbl.end()) throw std::runtime_error("Неизвестный оператор: " + k);
    return it->second;
}
bool OperatorTable::contains(const std::string& k) const { return tbl.count(k) != 0; }

std::vector<Token> RpnCompiler::compile(const std::vector<Token>& tokens) {
    std::vector<Token> out;
    std::vector<Token> ops;
    const OperatorTable opt;
    std::optional<Token> prev;
    for (std::size_t i = 0; i < tokens.size(); ++i) {
        Token tok = tokens[i];
        switch (tok.type) {
            case TokenType::Number:
                out.push_back(tok); prev = tok; break;
            case TokenType::Function:
                ops.push_back(tok); prev = tok; break;
            case TokenType::Operator: {
                std::string key = tok.text;
                if (is_unary(prev, tok)) { key = tok.text == "-" ? "u-" : "u+"; tok.text = key; }
                const auto& cur = opt.get(key);
                while (!ops.empty()) {
                    const Token& top = ops.back();
                    if (top.type == TokenType::Function) { out.push_back(top); ops.pop_back(); continue; }
                    if (top.type == TokenType::Operator) {
                        const auto& ti = opt.get(top.text);
                        bool higher = (!cur.right_assoc && ti.precedence >= cur.precedence) ||
                                      ( cur.right_assoc && ti.precedence >  cur.precedence);
                        if (higher) { out.push_back(top); ops.pop_back(); continue; }
                    }
                    break;
                }
                ops.push_back(tok); prev = tok; break;
            }
            case TokenType::LeftParen:
                ops.push_back(tok); prev = tok; break;
            case TokenType::RightParen: {
                bool found = false;
                while (!ops.empty()) {
                    Token top = ops.back(); ops.pop_back();
                    if (top.type == TokenType::LeftParen) { found = true; break; }
                    out.push_back(top);
                }
                if (!found) throw std::runtime_error("Скобки не совпадают");
                if (!ops.empty() && ops.back().type == TokenType::Function) {
                    out.push_back(ops.back()); ops.pop_back();
                }
                prev = tok; break;
            }
        }
    }
    while (!ops.empty()) {
        Token top = ops.back(); ops.pop_back();
        if (top.type == TokenType::LeftParen || top.type == TokenType::RightParen) throw std::runtime_error("Скобки не совпадают");
        out.push_back(top);
    }
    return out;
}

bool RpnCompiler::is_unary(const std::optional<Token>& prev, const Token& tok) {
    if (tok.text != "-" && tok.text != "+") return false;
    if (!prev.has_value()) return true;
    switch (prev->type) {
        case TokenType::Operator:
        case TokenType::LeftParen:
            return true;
        default:
            return false;
    }
}

} // namespace calc
