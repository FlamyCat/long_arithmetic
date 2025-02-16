//
// Created by Aleksandr on 16.02.25.
//

#ifndef LONG_ARITHMETIC_MACROS_HPP
#define LONG_ARITHMETIC_MACROS_HPP

#define fail return {false, description};
#define ok return {true, ""};
#define description(dsc, exp, act) auto description = std::format(R"(dsc + ": expected: "{}", actual: "{}")", exp, act)
#define faild(dsc, exp, act) description(dsc, exp, act); fail
#define faildea(dsc, val) faild(dsc, expected##val, actual##val)
#define check(title, exp, act, dsc) \
    const auto expected##title = exp; \
    const auto actual##title = act;   \
    if (expected##title != actual##title) { \
        faildea(dsc, title)         \
    }

#endif //LONG_ARITHMETIC_MACROS_HPP
