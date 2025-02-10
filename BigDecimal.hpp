#ifndef LONG_ARITHMETIC_BIGDECIMAL_HPP
#define LONG_ARITHMETIC_BIGDECIMAL_HPP

#include <cstdlib>
#include <deque>
#include <string>
#include <compare>
#include <cstdint>

class BigDecimal
{
    std::deque<uint32_t> _chunks;
    size_t _floatingPointPosition;
    signed char _sign;

public:
    // Constructors
    BigDecimal(std::string& s);

    BigDecimal(const BigDecimal& other) = default;

    // Math operators
    BigDecimal operator+(BigDecimal& other);

    BigDecimal operator-(BigDecimal& other);

    BigDecimal operator*(BigDecimal& other);

    BigDecimal operator/(BigDecimal& other);

    BigDecimal& operator+=(BigDecimal& other);

    BigDecimal& operator-=(BigDecimal& other);

    BigDecimal& operator*=(BigDecimal& other);

    BigDecimal& operator/=(BigDecimal& other);

    // Assignment
    BigDecimal& operator=(const BigDecimal& other);

    // Comparison operators
    std::strong_ordering operator<=>(const BigDecimal& other) const;

    bool operator<=(const BigDecimal& other);

    bool operator>=(const BigDecimal& other);

    bool operator<(const BigDecimal& other);

    bool operator>(const BigDecimal& other);

    bool operator==(const BigDecimal& other);

    // Methods
    void setPrecision(size_t newPrecision);

    void trim();

    // Accessors
    signed char sign() const;
    size_t size();
    size_t floatingPointPosition() const;

    size_t intPartLen();

    ~BigDecimal() = default;
};

BigDecimal& operator ""_longnum(long double number);

#endif //LONG_ARITHMETIC_BIGDECIMAL_HPP
