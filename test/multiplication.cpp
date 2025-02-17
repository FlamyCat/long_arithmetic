#include "multiplication.hpp"

#include <bitset>

#include "../src/BigDecimal.hpp"
#include "macros.hpp"

static std::string makeChunk(int digit) {
    return std::bitset<32>(digit).to_string();
}

testGroup(Multiplication, {
    testMethod(EmptyMulEmpty, {
            BigDecimal lhs;
            BigDecimal rhs;

            lhs *= rhs;

            lhsRhsCheck(Size, 0, 0, size(), "Size changed");
            lhsRhsCheck(Sign, 1, 1, sign(), "Sign changed");

            ok
    })

    testMethod(IntMulEmpty, {
            BigDecimal lhs("1");
            BigDecimal rhs;

            lhs *= rhs;

            lhsRhsCheck(Size, 0, 0, size(), "Incorrect size");
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign");

            ok
    })

    testMethod(EmptyMulInt, {
            BigDecimal lhs;
            BigDecimal rhs("1");

            lhs *= rhs;

            lhsRhsCheck(Size, 0, 1, size(), "Incorrect size");
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign");

            ok
    })

    testMethod(IntMulInt, {
            BigDecimal lhs("10");
            BigDecimal rhs("11");

            lhs *= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 6, 3, chunks()[0], "Incorrect first chunk")

            ok
    })

    testMethod(negLhsPosRhs, {
            BigDecimal lhs("-10");
            BigDecimal rhs("11");

            lhs *= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, -1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 6, 3, chunks()[0], "Incorrect first chunk")

            ok
    })

    testMethod(posLhsNegRhs, {
            BigDecimal lhs("10");
            BigDecimal rhs("-11");

            lhs *= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, -1, -1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 6, 3, chunks()[0], "Incorrect first chunk")

            ok
    })

    testMethod(bothNegative, {
            BigDecimal lhs("-10");
            BigDecimal rhs("-11");

            lhs *= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, -1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 6, 3, chunks()[0], "Incorrect first chunk")

            ok
    })

    testMethod(floatMulFloat, {
            BigDecimal lhs("10." + makeChunk(2));
            BigDecimal rhs("11." + makeChunk(3));

            lhs *= rhs;

            lhsRhsCheck(Size, 3, 2, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(FirstChunk, 6, 3, chunks()[0], "Incorrect first chunk")
            lhsRhsCheck(SecondChunk, 12, 3, chunks()[1], "Incorrect second chunk")
            check(LhsThirdChunk, 6, lhs.chunks()[2], "Lhs: Incorrect third chunk")

            check(LhsFpp, 2, lhs.floatingPointPosition(), "Lhs: incorrect FPP")

            ok
    })
})
