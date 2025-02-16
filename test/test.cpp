#include <iostream>
#include <functional>
#include <string>
#include <format>
#include "TestResult.h"

void runTest(const std::function<TestResult(void)> &testMethod, const std::string &testDescription) {
    std::cout << "Running test " << testDescription << std::endl;

    auto result = testMethod();

    std::cout << '\t';

    if (result.success) {
        std::cout << "OK";
    } else {
        std::cout << "FAIL: " << result.description;
    }
}
