#include "TestResult.h"
#include "../src/BigDecimal.hpp"
#include "macros.hpp"
#include <bitset>

std::string makeChunk(int digit) {
    return std::bitset<32>(digit).to_string();
}

testGroup(Addition, {
    testMethod(emptyAddEmpty, {
            BigDecimal lhs;
            BigDecimal rhs;

            lhs += rhs;

            check(LhsSign, 1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 0, lhs.size(), "Lhs size changed")
            check(RhsSize, 0, rhs.size(), "Rhs size changed")

            ok
    })

    testMethod(intAddEmpty, {
            BigDecimal lhs("1");
            BigDecimal rhs;

            lhs += rhs;

            check(LhsSign, 1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 1, lhs.size(), "Lhs size changed")
            check(RhsSize, 0, rhs.size(), "Rhs size changed")

            check(LhsFirstChunk, 1, lhs.chunks()[0], "Lhs first chunk changed")

            ok
    })

    testMethod(negativeIntAddEmpty, {
            BigDecimal lhs("-1");
            BigDecimal rhs;

            lhs += rhs;

            check(LhsSign, -1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 1, lhs.size(), "Lhs size changed")
            check(RhsSize, 0, rhs.size(), "Rhs size changed")

            check(LhsFirstChunk, 1, lhs.chunks()[0], "Lhs first chunk changed")

            ok
    })

    testMethod(floatAddEmpty, {
            BigDecimal lhs(makeChunk(1) + "." + makeChunk(2));
            BigDecimal rhs;

            lhs += rhs;

            check(LhsSign, 1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 2, lhs.size(), "Lhs size changed")
            check(RhsSize, 0, rhs.size(), "Rhs size changed")

            check(LhsFirstChunk, 2, lhs.chunks()[0], "Lhs first chunk changed")
            check(LhsSecondChunk, 1, lhs.chunks()[1], "Lhs second chunk changed")

            ok
    })

    testMethod(floatAddFloatSamePrecision, {
            BigDecimal lhs("1." + makeChunk(2));
            BigDecimal rhs(makeChunk(3) + "." + makeChunk(1));

            lhs += rhs;

            check(LhsSign, 1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 2, lhs.size(), "Lhs size changed")
            check(RhsSize, 2, rhs.size(), "Rhs size changed")

            check(LhsIntPart, 4, lhs.chunks()[1], "Lhs: incorrect int part")
            check(LhsFloatPart, 3, lhs.chunks()[0], "Lhs: incorrect float part")

            check(RhsIntPart, 3, rhs.chunks()[1], "Rhs: incorrect int part")
            check(RhsFloatPart, 1, rhs.chunks()[0], "Rhs: incorrect float part")

            ok
    })

    testMethod(floatAddLhsMorePrecise, {
            BigDecimal lhs("1." + makeChunk(2) + makeChunk(1));
            BigDecimal rhs("1." + makeChunk(1));

            lhs += rhs;

            lhsRhsCheck(Sign, 1, 1, sign(), "sign changed")

            lhsRhsCheck(FirstChunk, 1, 1, chunks()[0], "incorrect first chunk")
            lhsRhsCheck(SecondChunk, 3, 1, chunks()[1], "incorrect second chunk")
            check(LhsThirdChunk, 2, lhs.chunks()[2], "Lhs: Incorrect third chunk")

            ok
    })

    testMethod(floatAddRhsMorePrecise, {
            BigDecimal lhs("1." + makeChunk(1));
            BigDecimal rhs("1." + makeChunk(2) + makeChunk(1));

            lhs += rhs;

            lhsRhsCheck(Sign, 1, 1, sign(), "sign changed")

            lhsRhsCheck(FirstChunk, 1, 1, chunks()[0], "incorrect first chunk")
            lhsRhsCheck(SecondChunk, 3, 2, chunks()[1], "incorrect second chunk")
            lhsRhsCheck(ThirdChunk, 2, 1, chunks()[2], "incorrect third chunk")

            ok
    })

    testMethod(differentSignAdditionBecomesSubtraction, {
            BigDecimal lhs("1." + makeChunk(3));
            auto rhs = BigDecimal("-0." + makeChunk(1));

            lhs += rhs;

            lhsRhsCheck(Sign, 1, -1, sign(), "incorrect sign")

            lhsRhsCheck(FirstChunk, 2, 1, chunks()[0], "incorrect first chunk")
            lhsRhsCheck(SecondChunk, 1, 0, chunks()[1], "incorrect second chunk")

            ok
    })

    testMethod(overflowWorksCorrectly, {
            auto lhsVal = std::to_string(UINT32_MAX - 1);

            auto lhs = BigDecimal(lhsVal);
            auto rhs = 1.0_longnum;

            lhs += rhs;

            lhsRhsCheck(Size, 2, 1, size(), "incorrect size")

            lhsRhsCheck(Sign, 1, 1, sign(), "incorrect sign")

            lhsRhsCheck(FirstChunk, 0, 1, chunks()[0], "incorrect first chunk")
            check(LhsSecondChunk, 1, lhs.chunks()[1], "Lhs: incorrect second chunk")

            ok
    })
})