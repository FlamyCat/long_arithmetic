#include "TestResult.h"
#include "../src/BigDecimal.hpp"

TestResult emptyAddEmpty() {
    auto emptyLhs = BigDecimal();
    auto emptyRhs = BigDecimal();

    emptyLhs += emptyRhs;

    if (emptyLhs.size() != 0) {
        return {false, "Empty + empty != empty"};
    }

    if (emptyLhs.sign() != 1) {
        return {false, "Empty + empty resulted in sign change"};
    }

    if (emptyRhs.size() != 0) {
        return {false, "Empty + empty resulted in rhs being not empty"};
    }

    if (emptyRhs.sign() != 1) {
        return {false, "Empty + empty resulted in rhs sign change"};
    }

    return {true, ""};
}