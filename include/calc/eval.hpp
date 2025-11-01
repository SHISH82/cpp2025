#pragma once
#include "calc/tokenizer.hpp"
#include "calc/parser.hpp"
#include "calc/plugins.hpp"
#include <functional>
#include <memory>
#include <vector>
#include <ostream>

namespace calc {

    struct ICommand {
        virtual ~ICommand() = default;
        virtual void execute(std::vector<double>& stack) const = 0;
    };

    class PushLiteralCommand : public ICommand {
    public:
        explicit PushLiteralCommand(double v);
        void execute(std::vector<double>& stack) const override;
    private:
        double v_;
    };

    class UnaryCommand : public ICommand {
    public:
        explicit UnaryCommand(std::function<double(double)> f);
        void execute(std::vector<double>& stack) const override;
    private:
        std::function<double(double)> f_;
    };

    class BinaryCommand : public ICommand {
    public:
        explicit BinaryCommand(std::function<double(double,double)> f);
        void execute(std::vector<double>& stack) const override;
    private:
        std::function<double(double,double)> f_;
    };

    class CommandBuilder {
    public:
        CommandBuilder(const FunctionRegistry& r, const OperatorTable& o);
        std::vector<std::unique_ptr<ICommand>> build(const std::vector<Token>& rpn) const;
    private:
        const FunctionRegistry& reg_;
        const OperatorTable&    ops_;
    };

    class Calculator {
    public:
        explicit Calculator(FunctionRegistry r);
        double evaluate(const std::string& expr) const;
        void print_functions(std::ostream& os) const;

    private:
        FunctionRegistry reg_;
    };

} // namespace calc
