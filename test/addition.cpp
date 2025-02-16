#include "TestResult.h"
#include "../src/BigDecimal.hpp"
#include "macros.hpp"
#include <bitset>

testGroup(Addition, {
    test(emptyAddEmpty, {
            auto lhs = BigDecimal();
            auto rhs = BigDecimal();

            lhs += rhs;

            check(LhsSign, 1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 0, lhs.size(), "Lhs size changed")
            check(RhsSize, 0, rhs.size(), "Rhs size changed")

            ok
    })

    test(intAddEmpty, {
            auto lhs = 1.0_longnum;
            auto rhs = BigDecimal();

            lhs += rhs;

            check(LhsSign, 1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 1, lhs.size(), "Lhs size changed")
            check(RhsSize, 0, rhs.size(), "Rhs size changed")

            check(LhsFirstChunk, 1, lhs.chunks()[0], "Lhs first chunk changed")

            ok
    })

    test(negativeIntAddEmpty, {
            BigDecimal lhs{ "-1.0" };

            auto rhs = BigDecimal();

            lhs += rhs;

            check(LhsSign, -1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 1, lhs.size(), "Lhs size changed")
            check(RhsSize, 0, rhs.size(), "Rhs size changed")

            check(LhsFirstChunk, 1, lhs.chunks()[0], "Lhs first chunk changed")

            ok
    })

    test(floatAddEmpty, {
            auto lhs = 1.2_longnum;

            auto rhs = BigDecimal();

            lhs += rhs;

            check(LhsSign, 1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 2, lhs.size(), "Lhs size changed")
            check(RhsSize, 0, rhs.size(), "Rhs size changed")

            check(LhsFirstChunk, 1, lhs.chunks()[0], "Lhs first chunk changed")
            check(LhsSecondChunk, 2, lhs.chunks()[1], "Lhs second chunk changed")

            ok
    })

    test(floatAddFloatSamePrecision, {
            auto lhs = 1.2_longnum;
            auto rhs = 3.1_longnum;

            lhs += rhs;

            check(LhsSign, 1, lhs.sign(), "Lhs sign changed")
            check(RhsSign, 1, rhs.sign(), "Rhs sign changed")

            check(LhsSize, 2, lhs.size(), "Lhs size changed")
            check(RhsSize, 2, rhs.size(), "Rhs size changed")

            check(LhsIntPart, 4, lhs.chunks()[1], "Lhs: incorrect int part")
            check(LhsFloatPart, 3, lhs.chunks()[0], "Lhs: incorrect float part")

            check(RhsIntPart, 3, rhs.chunks()[1], "Rhs: incorrect int part")
            check(RhsFloatPart, 1, lhs.chunks()[0], "Rhs: incorrect float part")

            ok
    })

    test(floatAddLhsMorePrecise, {
            auto lhs = 1.21_longnum;
            auto rhs = 1.1_longnum;

            lhs += rhs;

            lhsRhsCheck(Sign, 1, 1, sign(), "sign changed")

            lhsRhsCheck(FirstChunk, 1, 1, chunks()[0], "incorrect first chunk")
            lhsRhsCheck(SecondChunk, 3, 1, chunks()[1], "incorrect second chunk")
            check(LhsThirdChunk, 2, lhs.chunks()[2], "Lhs: Incorrect third chunk")

            ok
    })

    test(floatAddRhsMorePrecise, {
            auto lhs = 1.1_longnum;
            auto rhs = 1.21_longnum;

            lhs += rhs;

            lhsRhsCheck(Sign, 1, 1, sign(), "sign changed")

            lhsRhsCheck(FirstChunk, 1, 1, chunks()[0], "incorrect first chunk")
            lhsRhsCheck(SecondChunk, 3, 2, chunks()[1], "incorrect second chunk")
            lhsRhsCheck(ThirdChunk, 2, 1, chunks()[2], "incorrect third chunk")

            ok
    })

    test(differentSignAdditionBecomesSubtraction, {
            auto lhs = 1.3_longnum;
            auto rhs = BigDecimal("-0.1");

            lhs += rhs;

            lhsRhsCheck(Sign, 1, -1, sign(), "incorrect sign")

            lhsRhsCheck(FirstChunk, 2, 1, chunks()[0], "incorrect first chunk")
            lhsRhsCheck(SecondChunk, 1, 0, chunks()[1], "incorrect second chunk")

            ok
    })

    test(overflowWorksCorrectly, {
            auto lhsVal = std::to_string(UINT32_MAX - 1);

            auto lhs = BigDecimal(lhsVal);
            auto rhs = 1.0_longnum;

            lhsRhsCheck(Size, 2, 1, size(), "incorrect size")

            lhsRhsCheck(Sign, 1, 1, sign(), "incorrect sign")

            lhsRhsCheck(FirstChunk, 0, 0, chunks()[0], "incorrect first chunk")
            lhsRhsCheck(SecondChunk, 1, 1, chunks()[1], "incorrect second chunk")

            ok
    })
})