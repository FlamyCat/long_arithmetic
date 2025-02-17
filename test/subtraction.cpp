#include <bitset>

#include "subtraction.hpp"

#include "TestResult.h"
#include "macros.hpp"

#include "../src/BigDecimal.hpp"

testGroup(Subtraction, {
    testMethod(emptySubEmpty, {
            BigDecimal lhs;
            BigDecimal rhs;

            lhs -= rhs;

            lhsRhsCheck(Size, 0, 0, size(), "Size changed");
            lhsRhsCheck(Sign, 1, 1, sign(), "Sign changed");

            ok
    })

    testMethod(intSubEmpty, {
            BigDecimal lhs("1");
            BigDecimal rhs;

            lhs -= rhs;

            lhsRhsCheck(Size, 1, 0, size(), "Size changed")
            lhsRhsCheck(Sign, 1, 1, sign(), "Sign changed")
            check(FirstChunk, 1, lhs.chunks()[0], "First chunk changed")

            ok
    })

    testMethod(intSubIntNoBorrow, {
            BigDecimal lhs("11"); // = 3
            BigDecimal rhs("10"); // = 2

            lhs -= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Size changed")
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign")
            lhsRhsCheck(FirstChunk, 1, 2, chunks()[0], "Incorrect first chunk")

            ok
    })

    testMethod(intSubIntWithBorrow, {
            BigDecimal lhs(std::bitset<33>(1ULL << 32).to_string());
            BigDecimal rhs("1");

            lhs -= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(FirstChunk, UINT32_MAX, 1, chunks()[0], "Incorrect first chunk")

            ok
    })

    testMethod(FloatSubFloatEqualPrecision, {
            BigDecimal lhs("10.0"); // = 2.0
            BigDecimal rhs("1.0");

            lhs -= rhs;

            lhsRhsCheck(Size, 2, 2, size(), "Incorrect size")
            lhsRhsCheck(FirstChunk, 0, 0, chunks()[0], "Incorrect first chunk")
            lhsRhsCheck(SecondChunk, 1, 1, chunks()[1], "Incorrect second chunk")

            ok
    })

    testMethod(FloatSubFloatLhsPrecise, {
            auto one = std::bitset<32>(1).to_string();
            auto two = std::bitset<32>(2).to_string();
            auto three = std::bitset<32>(3).to_string();

            // 3.21 1.1
            BigDecimal lhs(three + "." + two + one);
            BigDecimal rhs(one + "." + one);

            lhs -= rhs;

            lhsRhsCheck(Size, 3, 2, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(FirstChunk, 1, 1, chunks()[0], "Incorrect first chunk")
            lhsRhsCheck(SecondChunk, 1, 1, chunks()[1], "Incorrect second chunk")
            check(ThirdChunk, 2, lhs.chunks()[2], "Lhs: Incorrect third chunk")

            ok
    })

    testMethod(EmptySubInt, {
            BigDecimal lhs;
            BigDecimal rhs("1");

            lhs -= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, -1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 1, 1, chunks()[0], "Incorrect chunk")

            ok
    })

    testMethod(SubWithDifferentSignsBecomesAddition, {
            BigDecimal lhs("1");
            BigDecimal rhs("-1");

            lhs -= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, -1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 2, 1, chunks()[0], "Incorrect chunk")

            ok
    })
})
