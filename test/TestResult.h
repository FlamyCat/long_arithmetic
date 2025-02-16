#ifndef LONG_ARITHMETIC_TESTRESULT_H
#define LONG_ARITHMETIC_TESTRESULT_H

#include <string>
#include <utility>

struct TestResult {
    std::string description;
    bool success;

    TestResult(bool success, std::string description): success(success), description(std::move(description)) {}
};
#endif //LONG_ARITHMETIC_TESTRESULT_H
