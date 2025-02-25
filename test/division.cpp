#include "division.hpp"

#include <bitset>

#include "../src/BigDecimal.hpp"
#include "macros.hpp"

static std::string makeChunk(int digit) {
    return std::bitset<32>(digit).to_string();
}

testGroup(Division, {
    skip(testMethod(EmptyByInt, {
            BigDecimal lhs;
            BigDecimal rhs("10");

            lhs /= rhs;

            lhsRhsCheck(Size, 0, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign")

            ok
    }))

    testMethod(IntByIntToInt, {
            BigDecimal lhs(makeChunk(10));
            BigDecimal rhs(makeChunk(5));

            std::cout << "Lhs:\n\t";
            lhs.binaryDisplay();
            std::cout << std::endl;

            std::cout << "Rhs:\n\t";
            rhs.binaryDisplay();
            std::cout << std::endl;

            lhs /= rhs;

            std::cout << "\nResult (lhs):\n\t";
            lhs.binaryDisplay();
            std::cout << "\n";

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 2, 5, chunks()[0], "Incorrect chunk")

            lhsRhsCheck(Precision, 0, 0, floatingPointPosition(), "Incorrect FPP")

            ok
    })

    testMethod(IntByIntToFloatDoesNotChangePrecision, {
            BigDecimal lhs(makeChunk(5));
            BigDecimal rhs(makeChunk(2));

            lhs /= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 2, 2, chunks()[0], "Incorrect chunk")

            lhsRhsCheck(Precision, 0, 0, floatingPointPosition(), "Incorrect FPP")

            ok
    })

    testMethod(IntByFloatProperlyChangesPrecision, {
            BigDecimal lhs("1");
            BigDecimal rhs("0." + makeChunk(1));

            lhs /= rhs;

            lhsRhsCheck(Size, 2, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(FirstChunk, 0, 1, chunks()[0], "Incorrect first chunk")
            lhsRhsCheck(SecondChunk, 1, 0, chunks()[1], "Incorrect second chunk")

            ok
    })

    testMethod(posLhsNegRhs, {
            BigDecimal lhs("1");
            BigDecimal rhs("-1");

            lhs /= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, -1, -1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 1, 1, chunks()[0], "Incorrect chunk")

            ok
    })

    testMethod(negLhsPosRhs, {
            BigDecimal lhs("-1");
            BigDecimal rhs("1");

            lhs /= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, -1, 1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 1, 1, chunks()[0], "Incorrect chunk")

            ok
    })

    testMethod(bothNeg, {
            BigDecimal lhs("-1");
            BigDecimal rhs("-1");

            lhs /= rhs;

            lhsRhsCheck(Size, 1, 1, size(), "Incorrect size")
            lhsRhsCheck(Sign, 1, -1, sign(), "Incorrect sign")

            lhsRhsCheck(Chunk, 1, 1, chunks()[0], "Incorrect chunk")

            ok
    })
})