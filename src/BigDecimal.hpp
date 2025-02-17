#ifndef LONG_ARITHMETIC_BIGDECIMAL_HPP
#define LONG_ARITHMETIC_BIGDECIMAL_HPP

#include <cstdlib>
#include <deque>
#include <string>
#include <compare>
#include <cstdint>

static constexpr const auto chunkSize = 32;

class BigDecimal {
    std::deque<uint32_t> _chunks;
    size_t _floatingPointPosition;
    signed char _sign;

    size_t numberOfBits();

    void flipSign();

    void setNewSize(BigDecimal &number, BigDecimal &lhs, BigDecimal &rhs);

public:
    // Constructors
    BigDecimal();

    explicit BigDecimal(const std::string &s);

    BigDecimal(BigDecimal &other) = default;

    // Math operators
    BigDecimal operator+(BigDecimal &other);

    BigDecimal operator-(BigDecimal &other);

    BigDecimal operator*(BigDecimal &other);

    BigDecimal operator/(BigDecimal &other);

    BigDecimal &operator+=(BigDecimal &other);

    BigDecimal &operator-=(BigDecimal &other);

    BigDecimal &operator*=(BigDecimal &other);

    BigDecimal &operator/=(BigDecimal &other);

    // Assignment
    BigDecimal &operator=(BigDecimal &other);

    // Comparison operators
    std::strong_ordering operator<=>(BigDecimal &other);

    bool operator<=(BigDecimal &other);

    bool operator>=(BigDecimal &other);

    bool operator<(BigDecimal &other);

    bool operator>(BigDecimal &other);

    bool operator==(BigDecimal &other);

    // Bits manipulation
    [[nodiscard]] uint32_t getBit(size_t index);

    BigDecimal& operator<<=(int shift);

    void setBit(size_t index, uint32_t value);

    void pushBitFront(uint32_t bitValue);

    // Methods
    void setPrecision(size_t newPrecision);

    void trim();

    // Accessors
    [[nodiscard]] signed char sign() const;

    [[nodiscard]] size_t size();

    [[nodiscard]] size_t floatingPointPosition() const;

    [[nodiscard]] size_t intPartLen();

    [[nodiscard]] static std::string toBinary(long double d);

    [[nodiscard]] const std::deque<uint32_t>& chunks();

    ~BigDecimal() = default;
};

BigDecimal operator ""_longnum(long double number);

#endif //LONG_ARITHMETIC_BIGDECIMAL_HPP
