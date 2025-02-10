#include "BigDecimal.hpp"

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
