#ifndef LONG_ARITHMETIC_TESTRESULT_H
#define LONG_ARITHMETIC_TESTRESULT_H

#include <string>

struct TestResult {
    std::string description;
    bool success;

    TestResult(bool success, std::string &description): success(success), description(description) {}
};
#endif //LONG_ARITHMETIC_TESTRESULT_H
