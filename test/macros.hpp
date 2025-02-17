#ifndef LONG_ARITHMETIC_MACROS_HPP
#define LONG_ARITHMETIC_MACROS_HPP

#include <format>
#include <iostream>

#define fail return {false, description};
#define ok return {true, ""};

#define description(dsc, exp, act) \
    auto description = dsc + std::format(": \n\texpected: \"{}\", \n\tactual: \"{}\"", exp, act)

#define faild(dsc, exp, act) \
    description(dsc, exp, act); fail

#define faildea(dsc, val) \
    faild(dsc, expected##val, actual##val)

#define check(title, exp, act, dsc)         \
    const auto expected##title = exp;       \
    const auto actual##title = act;         \
    if (expected##title != actual##title) { \
        faildea(dsc, title)                 \
    }

#define lhsRhsCheck(title, lhsExp, rhsExp, act, dsc)                                        \
    check(Lhs##title, lhsExp, lhs.act, (std::string)"Lhs: " + (std::string)dsc)             \
    check(Rhs##title, rhsExp, rhs.act, (std::string)"Rhs: " + (std::string)dsc)             \

#define testMethod(title, body)                                            \
    {                                                                      \
        std::cout << "Running test " << #title << " : ";                   \
        TestResult _res = ([]() -> TestResult body)();                     \
        if (_res.success) {                                                \
            std::cout << "OK" << std::endl;                                \
        } else {                                                           \
            std::cout << "FAIL\n";                                         \
            std::cout << "Description: " << _res.description << std::endl; \
        }                                                                  \
        std::cout << "-----------------------------------------------\n";  \
    }

#define testGroup(title, body)                                                        \
    void test##title() {                                                              \
        const std::string _sep = "===============================================\n"; \
        std::cout << _sep;                                                            \
        std::cout << "Running \"" << #title << "\" test group\n";                     \
        std::cout << _sep;                                                            \
        body                                                                          \
    }

#endif //LONG_ARITHMETIC_MACROS_HPP
