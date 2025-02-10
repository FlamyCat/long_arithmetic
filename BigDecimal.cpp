#include "BigDecimal.hpp"

union u64 {
    uint64_t number;
    uint32_t buf[2];
};

void BigDecimal::trim() {
    while (this->_chunks.back() == 0) {
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

size_t BigDecimal::intPartLen() {
    return this->size() - floatingPointPosition();
}
