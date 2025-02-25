#include <format>
#include <bitset>

#include "TestResult.h"
#include "../src/BigDecimal.hpp"
#include "macros.hpp"

testGroup(Constructor, {
    testMethod(integerFromSingleChunk, {
            BigDecimal bd{ "10" };

            auto chunks = bd.chunks();

            check(NumberOfDigits, 1, bd.size(), "Incorrect number of chunks")

            check(Chunk, 2, bd.chunks()[0], "Incorrect chunk")

            ok
    })

    testMethod(integerFromMultipleChunks, {
            auto bitset = std::bitset<33>(1);
            bitset <<= 32;

            auto str = bitset.to_string();

            BigDecimal bd{ str };

            auto chunks = bd.chunks();

            check(NumberOfChunks, 2, chunks.size(), "Incorrect number of chunks")

            check(FirstChunk, 0, chunks[0], "Incorrect first chunk")
            check(SecondChunk, 1, chunks[1], "Incorrect second chunk")

            ok
    })

    testMethod(negativeInteger, {
            auto str = '-' + (std::bitset<33>(1) << 32).to_string();

            BigDecimal bd{ str };

            auto chunks = bd.chunks();

            check(NumberOfChunks, 2, chunks.size(), "Incorrect number of chunks")

            check(FirstChunk, 0, chunks[0], "Incorrect first chunk")
            check(SecondChunk, 1, chunks[1], "Incorrect second chunk")

            ok
    })

    testMethod(floatingPointNumber, {
            const auto intPart = "-11.";
            auto floatPart = std::bitset<32>(2).to_string();

            // Let's check the padding
            floatPart.pop_back();

            // Expected to equal -3.2
            BigDecimal bd{ intPart + floatPart };

            auto chunks = bd.chunks();

            check(NumberOfChunks, 2, chunks.size(), "Incorrect number of chunks")

            check(Sign, -1, bd.sign(), "Incorrect sign")

            check(FirstChunk, 2, chunks[0], "First chunk doesn't match")
            check(SecondChunk, 3, chunks[1], "Second chunk doesn't match")

            ok
    })
})
