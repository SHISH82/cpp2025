#include "calc/tokenizer.hpp"
#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace calc {

Tokenizer::Tokenizer(std::string s) : s_(std::move(s)) {}

std::vector<Token> Tokenizer::tokenize() const {
    std::vector<Token> t;
    std::size_t i = 0;
    while (i < s_.size()) {
        char ch = s_[i];

        if (std::isspace((unsigned char)ch)) { ++i; continue; }

        if (std::isdigit((unsigned char)ch) || ch == '.') {
            auto [num, next] = parse_number(i);
            t.push_back(Token{TokenType::Number, "", num});
            i = next; continue;
        }

        if (std::isalpha((unsigned char)ch)) {
            auto [id, next] = parse_id(i);
            std::string name = id;
            std::transform(name.begin(), name.end(), name.begin(),
                           [](unsigned char c){ return char(std::tolower(c)); });

            std::size_t j = next;
            while (j < s_.size() && std::isspace((unsigned char)s_[j])) ++j;
            if (j < s_.size() && s_[j] == '(') {
                t.push_back(Token{TokenType::Function, name, 0.0});
            } else {
                throw std::runtime_error("Ожидался вызов функции: " + name + "(...)");
            }
            i = next; continue;
        }

        switch (ch) {
            case '+': case '-': case '*': case '/': case '^':
                t.push_back(Token{TokenType::Operator, std::string(1, ch), 0.0}); ++i; break;
            case '(':
                t.push_back(Token{TokenType::LeftParen, "(", 0.0}); ++i; break;
            case ')':
                t.push_back(Token{TokenType::RightParen, ")", 0.0}); ++i; break;
            default:
                throw std::runtime_error(std::string("Неожиданный символ: ") + ch);
        }
    }
    return t;
}

std::pair<double, std::size_t> Tokenizer::parse_number(std::size_t start) const {
    std::size_t pos = start; bool dot = false;
    while (pos < s_.size()) {
        char ch = s_[pos];
        if (std::isdigit((unsigned char)ch)) { ++pos; continue; }
        if (ch == '.' && !dot) { dot = true; ++pos; continue; }
        break;
    }
    std::string v = s_.substr(start, pos - start);
    if (v.empty() || v == ".") throw std::runtime_error("Некорректное число");
    return {std::stod(v), pos};
}

std::pair<std::string, std::size_t> Tokenizer::parse_id(std::size_t start) const {
    std::size_t pos = start;
    while (pos < s_.size()) {
        char ch = s_[pos];
        if (std::isalnum((unsigned char)ch) || ch == '_') { ++pos; continue; }
        break;
    }
    return {s_.substr(start, pos - start), pos};
}

} // namespace calc
