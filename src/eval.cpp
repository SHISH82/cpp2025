#include "calc/eval.hpp"
#include "calc/tokenizer.hpp"

namespace calc {

PushLiteralCommand::PushLiteralCommand(double v) : v_(v) {}
void PushLiteralCommand::execute(std::vector<double>& stack) const { stack.push_back(v_); }

UnaryCommand::UnaryCommand(std::function<double(double)> f) : f_(std::move(f)) {}
void UnaryCommand::execute(std::vector<double>& stack) const {
    if (stack.empty()) throw std::runtime_error("Не хватает операндов");
    double a = stack.back(); stack.pop_back();
    stack.push_back(f_(a));
}

BinaryCommand::BinaryCommand(std::function<double(double,double)> f) : f_(std::move(f)) {}
void BinaryCommand::execute(std::vector<double>& stack) const {
    if (stack.size() < 2) throw std::runtime_error("Не хватает операндов");
    double b = stack.back(); stack.pop_back();
    double a = stack.back(); stack.pop_back();
    stack.push_back(f_(a,b));
}

CommandBuilder::CommandBuilder(const FunctionRegistry& r, const OperatorTable& o) : reg_(r), ops_(o) {}

std::vector<std::unique_ptr<ICommand>> CommandBuilder::build(const std::vector<Token>& rpn) const {
    std::vector<std::unique_ptr<ICommand>> cmds;
    cmds.reserve(rpn.size());
    for (const auto& t : rpn) {
        switch (t.type) {
            case TokenType::Number:
                cmds.push_back(std::make_unique<PushLiteralCommand>(t.number));
                break;
            case TokenType::Operator: {
                const auto& op = ops_.get(t.text);
                if (op.arity == 1) cmds.push_back(std::make_unique<UnaryCommand>(op.unary));
                else if (op.arity == 2) cmds.push_back(std::make_unique<BinaryCommand>(op.binary));
                else throw std::runtime_error("Неподдерживаемая арность оператора");
                break;
            }
            case TokenType::Function: {
                auto fn = reg_.get_function(t.text);
                cmds.push_back(std::make_unique<UnaryCommand>(std::move(fn)));
                break;
            }
            default:
                throw std::runtime_error("Неожиданный токен");
        }
    }
    return cmds;
}

Calculator::Calculator(FunctionRegistry r) : reg_(std::move(r)) {}

double Calculator::evaluate(const std::string& expr) const {
    Tokenizer tz(expr);
    auto tokens = tz.tokenize();
    OperatorTable ops;
    RpnCompiler cp;
    auto rpn = cp.compile(tokens);
    CommandBuilder bld(reg_, ops);
    auto cmds = bld.build(rpn);
    std::vector<double> st;
    st.reserve(cmds.size());
    for (auto& c : cmds) c->execute(st);
    if (st.size() != 1) throw std::runtime_error("Неверное выражение");
    return st.front();
}

void Calculator::print_functions(std::ostream& os) const {
    if (reg_.empty()) { os << "  Плагиновые функции не загружены\n"; return; }
    os << "  Доступные функции: ";
    auto n = reg_.names();
    for (std::size_t i = 0; i < n.size(); ++i) { if (i) os << ", "; os << n[i]; }
    os << '\n';
}

} // namespace calc
