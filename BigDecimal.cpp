#include <vector>
#include "BigDecimal.hpp"

union u64 {
    uint64_t number;
    uint32_t buf[2];
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

BigDecimal& BigDecimal::operator+=(BigDecimal& other) {
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
        t.number = lhs->_chunks[i];
        t.number += rhs->_chunks[i] + overflow;

        overflow = t.buf[1];
        lhs->_chunks[i] = t.buf[0];
    }

    if (overflow != 0) {
        lhs->_chunks.emplace_back(overflow);
    }

    return *lhs;
}

BigDecimal BigDecimal::operator+(BigDecimal& other) {
    auto lhs = BigDecimal(*this);

    lhs += other;

    return lhs;
}

BigDecimal& BigDecimal::operator-=(BigDecimal& other) {
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

        borrowed[i] += (uint64_t)UINT32_MAX + 1;

        lhs->_chunks[i] += borrowed[i] - rhs->_chunks[i];
    }

    if (lrhsSwap) {
        *this = otherCopy;
    }

    return *this;
}

BigDecimal BigDecimal::operator-(BigDecimal& other) {
    auto lhs = BigDecimal(*this);

    lhs -= other;

    return lhs;
}

std::strong_ordering BigDecimal::operator<=>(BigDecimal& other) {
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

bool BigDecimal::operator<=(BigDecimal& other) {
    auto ordering = *this <=> other;
    return (ordering == std::strong_ordering::equal) || (ordering == std::strong_ordering::less);
}

bool BigDecimal::operator>=(BigDecimal& other) {
    auto ordering = *this <=> other;
    return (ordering == std::strong_ordering::equal) || (ordering == std::strong_ordering::greater);
}

bool BigDecimal::operator<(BigDecimal& other) {
    auto ordering = *this <=> other;
    return ordering == std::strong_ordering::less;
}

bool BigDecimal::operator>(BigDecimal& other) {
    auto ordering = *this <=> other;
    return ordering == std::strong_ordering::greater;
}

bool BigDecimal::operator==(BigDecimal& other) {
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
