#pragma once
#include <string>
#include <vector>
#include <utility>

namespace calc {

    enum class TokenType {
        Number,
        Operator,
        Function,
        LeftParen,
        RightParen
    };

    struct Token {
        TokenType   type;
        std::string text;
        double      number{};
    };

    class Tokenizer {
    public:
        explicit Tokenizer(std::string s);
        std::vector<Token> tokenize() const;

    private:
        std::pair<double, std::size_t> parse_number(std::size_t start) const;
        std::pair<std::string, std::size_t> parse_id(std::size_t start) const;

        std::string s_;
    };

} // namespace calc
