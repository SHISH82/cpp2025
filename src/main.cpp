#include <iostream>
#include <string>
#include "subject.h"
#include "wrapper.h"
#include "engine.h"

void print_test(const std::string& name, double result, double expected)
{
    std::cout << name << ": результат = " << result
              << ", ожидается = " << expected << std::endl;
}


//переопределение arg1
void test_override()
{
    subject subj;

    wrapper<subject, double(subject::*)(double, double), double> wrapper(
        &subj,
        &subject::multiply,
        { {"arg1", 2.0}, {"arg2", 1.5} }
    );

    engine engine;
    engine.register_cmd<subject, double(subject::*)(double, double), double>(&wrapper, "mul");

    double r = engine.execute<subject, double(subject::*)(double, double), double>(
        "mul",
        { {"arg1", 3.0} }
    );

    print_test("test_override", r, 4.5);
}

//все лишнее игнорируем
void test_mul_ignore()
{
    subject subj;

    wrapper<subject, double(subject::*)(double, double), double> wrapper(
        &subj,
        &subject::multiply,
        { {"arg1", 2.0}, {"arg2", 1.5} }
    );

    engine engine;
    engine.register_cmd<subject, double(subject::*)(double, double), double>(&wrapper, "mul_ignore");

    double r = engine.execute<subject, double(subject::*)(double, double), double>(
        "mul_ignore",
        {
            {"argX", 999.0},
            {"arg1", 3.0}
        }
    );


    print_test("test_mul_ignore", r, 4.5);
}

//пустой список
void test_sum_defaults()
{
    subject subj;

    wrapper<subject, int(subject::*)(int, int), int> wrapper(
        &subj,
        &subject::sum,
        { {"arg1", 0}, {"arg2", 0} }
    );

    engine engine;
    engine.register_cmd<subject, int(subject::*)(int, int), int>(&wrapper, "sum_defaults");

    int r = engine.execute<subject, int(subject::*)(int, int), int>(
        "sum_defaults",
        {}
    );

    print_test("ttest_sum_defaults", r, 0);
}

//суммирование где арументы явные
void test_original()
{
    subject subj;

    wrapper<subject, int(subject::*)(int, int), int> wrapper(
        &subj,
        &subject::sum,
        { {"arg1", 0}, {"arg2", 0} }
    );

    engine engine;
    engine.register_cmd<subject, int(subject::*)(int, int), int>(&wrapper, "sum");

    int r = engine.execute<subject, int(subject::*)(int, int), int>(
        "sum",
        { {"arg1", 4}, {"arg2", 5} }
    );

    print_test("test_original", r, 9);
}


int main()
{
    test_override();
    test_mul_ignore();
    test_sum_defaults();
    test_original();

    return 0;
}
