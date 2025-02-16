#include <format>
#include <bitset>

#include "TestResult.h"
#include "../src/BigDecimal.hpp"

#define fail return {false, description};
#define ok return {true, ""};
#define description(dsc, exp, act) auto description = std::format(R"(dsc + ": expected: "{}", actual: "{}")", exp, act)
#define faild(dsc, exp, act) description(dsc, exp, act); fail
#define faildea(dsc, val) faild(dsc, expected##val, actual##val)
#define check(title, exp, act, dsc) \
    const auto expected##title = exp; \
    const auto actual##title = act;   \
    if (expected##title != actual##title) { \
        faildea(dsc, title)         \
    }

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

    const auto expectedNumberOfChunks = 2;
    const auto actualNumberOfChunks = chunks.size();
    if (actualNumberOfChunks != expectedNumberOfChunks) {
        faildea("String constructor produces incorrect number of chunks", NumberOfChunks)
    }

    const auto expectedSign = -1;
    const auto actualSign = bd.sign();
    if (actualSign != expectedSign) {
        faildea("String constructor produces incorrect sign", Sign)
    }

    const auto expectedFirstChunk = 2;
    const auto actualFirstChunk = chunks[0];
    if (actualFirstChunk != expectedFirstChunk) {
        faildea("First chunk doesn't match", FirstChunk)
    }

    const auto expectedSecondChunk = 3;
    const auto actualSecondChunk = chunks[1];
    if (expectedSecondChunk != actualSecondChunk) {
        faildea("Second chunk doesn't match", SecondChunk)
    }

    ok
}

TestResult fromDouble() {
    auto bd = 2.1_longnum;

    check(Sign, 1, bd.sign(), "Sign doesn't match")
    check(FirstChunk, 1, bd.chunks()[0], "First chunk doesn't match")
    check(SecondChunk, 2, bd.chunks()[1], "Second chunk doesn't match")

    ok
}
