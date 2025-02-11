#include <vector>
#include "BigDecimal.hpp"

union u64 {
    uint64_t value;
    uint32_t chunks[2];
};

void BigDecimal::trim() {
    while (this->_chunks.back() == 0 && this->size() > 0) {
        this->_chunks.pop_back();
    }
}

signed char BigDecimal::sign() const {
    return this->_sign;
}

size_t BigDecimal::size() {
    return this->_chunks.size();
}

size_t BigDecimal::floatingPointPosition() const {
    return this->_floatingPointPosition;
}

BigDecimal &BigDecimal::operator+=(BigDecimal &other) {
    this->trim();
    other.trim();

    auto lhs = this;
    auto rhs = &other;

    if (lhs->sign() != rhs->sign()) {
        // TODO: вычитание
    }

    auto lhsRight = lhs->size() - lhs->floatingPointPosition();
    auto rhsRight = rhs->size() - rhs->floatingPointPosition();

    auto setNewSize = [=](BigDecimal *number) {
        auto right = number->size() - number->floatingPointPosition();

        auto newRight = std::max(lhsRight, rhsRight) - right;
        auto newLeft =
                std::max(lhs->floatingPointPosition(), rhs->floatingPointPosition()) - number->floatingPointPosition();

        auto rightDelta = newRight - lhsRight;
        auto leftDelta = newLeft - number->floatingPointPosition();

        for (int i = 0; i < rightDelta; i++) {
            number->_chunks.push_back(0);
        }
        for (int i = 0; i < leftDelta; i++) {
            number->_chunks.push_front(0);
        }
    };

    setNewSize(lhs);
    setNewSize(rhs);

    uint32_t overflow = 0;

    for (int i = 0; i < lhs->size(); ++i) {
        u64 t{};
        t.value = lhs->_chunks[i];
        t.value += rhs->_chunks[i] + overflow;

        overflow = t.chunks[1];
        lhs->_chunks[i] = t.chunks[0];
    }

    if (overflow != 0) {
        lhs->_chunks.emplace_back(overflow);
    }

    return *lhs;
}

BigDecimal BigDecimal::operator+(BigDecimal &other) {
    auto lhs = BigDecimal(*this);

    lhs += other;

    return lhs;
}

BigDecimal &BigDecimal::operator-=(BigDecimal &other) {
    auto lhs = this;
    auto rhs = &other;

    if (lhs->sign() == rhs->sign()) {
        *lhs += *rhs;
        return *lhs;
    }

    // Absolute values are needed
    auto lhsSign = lhs->sign();
    auto rhsSign = rhs->sign();

    lhs->_sign *= lhsSign;
    rhs->_sign += rhsSign;

    bool lrhsSwap = lhs < rhs;
    BigDecimal otherCopy{};
    if (lrhsSwap) {
        rhs = lhs;
        otherCopy = other;
        lhs = &otherCopy;
    }

    lhs->_sign *= lhsSign;
    rhs->_sign += rhsSign;

    auto lhsRight = lhs->size() - lhs->floatingPointPosition();
    auto rhsRight = rhs->size() - rhs->floatingPointPosition();

    auto setNewSize = [=](BigDecimal *number) {
        auto right = number->size() - number->floatingPointPosition();

        auto newRight = std::max(lhsRight, rhsRight) - right;
        auto newLeft =
                std::max(lhs->floatingPointPosition(), rhs->floatingPointPosition()) - number->floatingPointPosition();

        auto rightDelta = newRight - lhsRight;
        auto leftDelta = newLeft - number->floatingPointPosition();

        for (int i = 0; i < rightDelta; i++) {
            number->_chunks.push_back(0);
        }
        for (int i = 0; i < leftDelta; i++) {
            number->_chunks.push_front(0);
        }
    };

    setNewSize(lhs);
    setNewSize(rhs);

    std::vector<int64_t> borrowed(lhs->size());

    for (int i = 0; i < lhs->size(); ++i) {
        if (lhs->_chunks[i] + borrowed[i] < rhs->_chunks[i]) {
            lhs->_chunks[i] += borrowed[i] - rhs->_chunks[i];

            continue;
        }

        size_t nonZeroIndex;
        for (int j = 0; j < lhs->size(); ++j) {
            if (lhs->_chunks[j] + borrowed[j] > 0) {
                nonZeroIndex = j;
            }
        }

        borrowed[nonZeroIndex] -= 1;

        for (int j = i + 1; j < nonZeroIndex; ++j) {
            borrowed[j] += UINT32_MAX;
        }

        borrowed[i] += (uint64_t) UINT32_MAX + 1;

        lhs->_chunks[i] += borrowed[i] - rhs->_chunks[i];
    }

    if (lrhsSwap) {
        *this = otherCopy;
    }

    return *this;
}

BigDecimal BigDecimal::operator-(BigDecimal &other) {
    auto lhs = BigDecimal(*this);

    lhs -= other;

    return lhs;
}

std::strong_ordering BigDecimal::operator<=>(BigDecimal &other) {
    auto lhs = this;
    auto rhs = &other;

    lhs->trim();
    rhs->trim();

    //region Zero size related comparison
    if (lhs->size() == 0 && rhs->size() != 0) {
        return rhs->sign() > 0 ? std::strong_ordering::less : std::strong_ordering::greater;
    }

    if (lhs->size() != 0 && rhs->size() == 0) {
        return lhs->sign() < 0 ? std::strong_ordering::less : std::strong_ordering::greater;
    }

    if (lhs->size() == 0 && rhs->size() == 0) {
        return std::strong_ordering::equal;
    }
    //endregion

    //region Sign related comparison
    if (lhs->sign() > 0 && rhs->sign() < 0) {
        return std::strong_ordering::greater;
    }

    if (lhs->sign() < 0 && rhs->sign() > 0) {
        return std::strong_ordering::less;
    }
    //endregion

    //region Different abs value len comparison
    if (lhs->intPartLen() > rhs->intPartLen()) {
        return lhs->sign() <=> 0;
    }

    if (lhs->intPartLen() < rhs->intPartLen()) {
        return rhs->sign() <=> 0;
    }
    //endregion

    //region Floating point position comparison
    if (lhs->floatingPointPosition() < rhs->floatingPointPosition()) {
        return std::strong_ordering::greater;
    }

    if (lhs->floatingPointPosition() > rhs->floatingPointPosition()) {
        return std::strong_ordering::less;
    }
    //endregion

    for (auto i = lhs->size(); i > 0; i++) {
        if (lhs->_chunks[i] != rhs->_chunks[i]) {
            return lhs->_chunks[i] <=> rhs->_chunks[i];
        }
    }

    return std::strong_ordering::equal;
}

size_t BigDecimal::intPartLen() {
    return this->size() - floatingPointPosition();
}

bool BigDecimal::operator<=(BigDecimal &other) {
    auto ordering = *this <=> other;
    return (ordering == std::strong_ordering::equal) || (ordering == std::strong_ordering::less);
}

bool BigDecimal::operator>=(BigDecimal &other) {
    auto ordering = *this <=> other;
    return (ordering == std::strong_ordering::equal) || (ordering == std::strong_ordering::greater);
}

bool BigDecimal::operator<(BigDecimal &other) {
    auto ordering = *this <=> other;
    return ordering == std::strong_ordering::less;
}

bool BigDecimal::operator>(BigDecimal &other) {
    auto ordering = *this <=> other;
    return ordering == std::strong_ordering::greater;
}

bool BigDecimal::operator==(BigDecimal &other) {
    auto ordering = *this <=> other;
    return ordering == std::strong_ordering::equal;
}

BigDecimal::BigDecimal() {
    this->_chunks = std::deque<uint32_t>();
    this->_sign = 1;
    this->_floatingPointPosition = 0;
}

void BigDecimal::setPrecision(size_t newPrecision) {
    this->_floatingPointPosition = newPrecision;
}

BigDecimal::BigDecimal(std::string &s) {
    std::deque<char> sd = reinterpret_cast<const std::deque<char> &>(s);

    if (sd.front() == '-') {
        this->_sign = -1;
        sd.pop_front();
    }

    auto floatingPointPosition = s.find('.', 0);
    sd.erase(sd.begin() + floatingPointPosition);

    if (floatingPointPosition != std::string::npos) {
        auto left = floatingPointPosition;
        if (left % 32 != 0) {
            for (int i = 0; i < 32 - left % 32; ++i) {
                sd.push_front('0');
            }
        }

        auto right = sd.size() - floatingPointPosition;
        if (right % 32 != 0) {
            for (int i = 0; i < 32 - right % 32; ++i) {
                sd.push_back('0');
            }
        }

        this->_chunks = std::deque<uint32_t>();

        auto leftBorder = sd.rbegin();
        auto rightBorder = sd.rbegin() + 32;

        while (rightBorder < sd.rend()) {
            this->_chunks.push_back(std::stoul(std::string{leftBorder, rightBorder}));
        }

        this->_floatingPointPosition = floatingPointPosition;
    } else {
        this->_floatingPointPosition = 0;

        if (sd.size() % 32 != 0) {
            for (int i = 0; i < 32 - sd.size() % 32; ++i) {
                sd.push_back('0');
            }
        }

        auto leftBorder = sd.rbegin();
        auto rightBorder = sd.rbegin() + 32;

        while (rightBorder < sd.rend()) {
            this->_chunks.push_back(std::stoul(std::string{leftBorder, rightBorder}));
        }
    }
}

BigDecimal &BigDecimal::operator*=(BigDecimal &other) {
    BigDecimal totalResult{};

    auto lhs = this;
    auto rhs = &other;

    // At this point I'm using totalResult as zero
    if (*lhs == totalResult || *rhs == totalResult) {
        *this = totalResult;
        return *this;
    }

    lhs->trim();
    rhs->trim();

    for (int i = 0; i < rhs->size(); ++i) {
        uint32_t overflow = 0;
        BigDecimal tempResult{*lhs};
        for (int j = 0; j < lhs->size(); ++j) {
            u64 t;
            t.value = tempResult._chunks[j];
            tempResult._chunks[i] = t.chunks[0] + overflow;
            overflow = t.chunks[1];
        }

        if (overflow != 0) {
            tempResult._chunks.push_back(overflow);
        }

        for (int j = 0; j < i; ++j) {
            tempResult._chunks.push_front(0);
        }

        totalResult += tempResult;
    }

    totalResult._sign = lhs->sign() * rhs->sign();
    totalResult._floatingPointPosition = lhs->floatingPointPosition() + rhs->floatingPointPosition();

    *this = totalResult;
    return *this;
}

BigDecimal BigDecimal::operator*(BigDecimal &other) {
    auto result = BigDecimal{*this};
    result *= other;
    return result;
}

std::string BigDecimal::toBinary(long double d) {
    auto isNegative = d < 0;
    d = std::abs(d);

    auto integerPart = static_cast<long long>(d);
    auto rationalPart = d - integerPart;

    std::deque<char> x, y;

    // Convert integer part to binary
    while (integerPart > 0) {
        x.push_front(integerPart % 2 ? '1' : '0');
        integerPart /= 2;
    }

    if (x.empty()) {
        x.clear();
        x.push_back('0');
    }

    // Convert rational part to binary
    while (rationalPart > 0) {
        rationalPart *= 2;
        y.push_back(rationalPart >= 1 ? '1' : '0');
        rationalPart -= static_cast<long long>(rationalPart);
    }

    if (y.empty()) {
        y.clear();
        y.push_back('0');
    }

    return (isNegative ? "-" : "")
           + std::string{x.begin(), x.end()}
           + "."
           + std::string{y.begin(), y.end()};
}

BigDecimal operator ""_longnum(long double number) {
    std::basic_string<char> binaryString = BigDecimal::toBinary(number);
    BigDecimal bd{binaryString};

    return bd;
}
