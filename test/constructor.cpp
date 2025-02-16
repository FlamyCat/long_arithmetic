#include <format>
#include <bitset>

#include "TestResult.h"
#include "../src/BigDecimal.hpp"
#include "macros.hpp"

TestResult integerFromSingleChunk() {
    BigDecimal bd{"10"};

    auto chunks = bd.chunks();

    const auto expectedNumberOfDigits = 1;
    if (bd.size() != expectedNumberOfDigits) {
        faild("BigDecimal string constructor constructed unexpected number of arguments", expectedNumberOfDigits,
              bd.size())
    }

    auto firstDigit = bd.chunks()[0];

    const int expectedDigit = 2;
    if (firstDigit != expectedDigit) {
        faild("String constructor of BigDecimal produces incorrect chunks", expectedDigit, firstDigit)
    }

    ok
}

TestResult integerFromMultipleChunks() {
    auto str = std::bitset<33>(257).to_string();

    BigDecimal bd{str};

    auto chunks = bd.chunks();

    const auto expectedNumberOfChunks = 2;
    const auto actualNumberOfChunks = chunks.size();
    if (actualNumberOfChunks != expectedNumberOfChunks) {
        faildea("String constructor produces incorrect number of chunks", NumberOfChunks);
    }

    const auto expectedFirstChunk = 0;
    const auto actualFirstChunk = chunks[0];
    if (actualFirstChunk != expectedFirstChunk) {
        faildea("First chunk is incorrect", FirstChunk)
    }

    const auto expectedSecondChunk = 2;
    const auto actualSecondChunk = chunks[1];
    if (actualSecondChunk != expectedSecondChunk) {
        faildea("Second chunk is incorrect", SecondChunk)
    }

    ok
}

TestResult negativeInteger() {
    auto str = '-' + std::bitset<33>(257).to_string();

    BigDecimal bd{str};

    auto chunks = bd.chunks();

    const auto expectedNumberOfChunks = 2;

    const auto actualNumberOfChunks = chunks.size();
    if (actualNumberOfChunks != expectedNumberOfChunks) {
        faildea("String constructor produces incorrect number of chunks", NumberOfChunks)
    }

    const auto expectedFirstChunk = 0;
    const auto actualFirstChunk = chunks[0];
    if (actualFirstChunk != expectedFirstChunk) {
        faildea("First chunk is incorrect", FirstChunk)
    }

    const auto expectedSecondChunk = 2;
    const auto actualSecondChunk = chunks[1];
    if (actualSecondChunk != expectedSecondChunk) {
        faildea("Second chunk is incorrect", SecondChunk)
    }

    ok
}

TestResult floatingPointNumber() {
    const auto intPart = "-11.";
    auto floatPart = std::bitset<32>(2).to_string();

    // Let's check the padding
    floatPart.pop_back();

    // Expected to equal -2.2
    BigDecimal bd{intPart + floatPart};

    auto chunks = bd.chunks();

    check(NumberOfChunks, 2, chunks.size(), "Incorrect number of chunks")

    check(Sign, -1, bd.sign(), "Incorrect sign")

    check(FirstChunk, 2, chunks[0], "First chunk doesn't match")
    check(SecondChunk, 3, chunks[1], "Second chunk doesn't match")

    ok
}

TestResult fromDouble() {
    auto bd = 2.1_longnum;

    check(Sign, 1, bd.sign(), "Sign doesn't match")
    check(FirstChunk, 1, bd.chunks()[0], "First chunk doesn't match")
    check(SecondChunk, 2, bd.chunks()[1], "Second chunk doesn't match")

    ok
}
