#include "BigDecimal.hpp"
#include <algorithm>
#include <bitset>
#include <stdexcept>
#include <vector>

union u64 {
    uint64_t value;
    uint32_t chunks[2];
};

void BigDecimal::trim() {
    if (size() == 0) {
        return;
    }

    while (this->size() > 0 && this->_chunks.back() == 0) {
        this->_chunks.pop_back();
    }

    while (this->size() > 0 && this->_chunks.front() == 0 && this->_floatingPointPosition > 0) {
        this->_chunks.pop_front();
        _floatingPointPosition--;
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
        other.flipSign();
        *lhs -= *rhs;
        other.flipSign();

        return *lhs;
    }

    setNewSize(*lhs, *lhs, *rhs);
    setNewSize(*rhs, *lhs, *rhs);

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

    lhs->trim();
    rhs->trim();

    return *lhs;
}

BigDecimal BigDecimal::operator+(BigDecimal &other) {
    auto lhs = BigDecimal(*this);

    lhs += other;

    return lhs;
}

void BigDecimal::setNewSize(BigDecimal& number, BigDecimal& lhs, BigDecimal& rhs) {
    auto lhsIntPartLen = lhs.size() - lhs.floatingPointPosition();
    auto rhsIntPartLen = rhs.size() - rhs.floatingPointPosition();

    auto intPartLen = number.size() - number.floatingPointPosition();

    auto newIntPartLen = std::max(lhsIntPartLen, rhsIntPartLen);
    auto newFracPartLen =
            std::max(lhs.floatingPointPosition(), rhs.floatingPointPosition());

    auto intPartLenDelta = static_cast<long long>(newIntPartLen) - static_cast<long long>(intPartLen);
    auto fracPartLenDelta = static_cast<long long>(newFracPartLen) - static_cast<long long>(number.floatingPointPosition());

    for (int i = 0; i < intPartLenDelta; i++) {
        number._chunks.push_back(0);
    }
    for (int i = 0; i < fracPartLenDelta; i++) {
        number._chunks.push_front(0);
        number._floatingPointPosition++;
    }
}

BigDecimal &BigDecimal::operator-=(BigDecimal &other) {
    auto lhs = this;
    auto rhs = &other;

    if (lhs->sign() != rhs->sign()) {
        other.flipSign();
        *lhs += *rhs;
        other.flipSign();

        return *lhs;
    }

    // Absolute values are needed
    auto lhsSign = lhs->sign();
    auto rhsSign = rhs->sign();

    lhs->_sign *= lhsSign;
    rhs->_sign *= rhsSign;

    setNewSize(*lhs, *lhs, *rhs);
    setNewSize(*rhs, *lhs, *rhs);

    const bool lrhsSwap = *lhs < *rhs;
    BigDecimal otherCopy{};
    if (lrhsSwap) {
        rhs = lhs;
        otherCopy = other;
        lhs = &otherCopy;
    }

    lhs->_sign *= lhsSign;
    rhs->_sign *= rhsSign;

    std::vector<int64_t> borrowed(lhs->size());

    for (int i = 0; i < lhs->size(); ++i) {
        if (lhs->_chunks[i] + borrowed[i] >= rhs->_chunks[i]) {
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
        this->flipSign();
    }

    lhs->trim();
    rhs->trim();

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

    //region Integer part comparison
    for (auto lhsDigitIter = lhs->chunks().rbegin(), rhsDigitIter = rhs->chunks().rbegin();
         lhs->chunks().rend() - lhsDigitIter > lhs->floatingPointPosition();
         lhsDigitIter++, rhsDigitIter++) {
        if (*lhsDigitIter != *rhsDigitIter) {
            return *lhsDigitIter <=> *rhsDigitIter;
        }
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

    for (auto i = lhs->size(); i > 0; i--) {
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
    if (newPrecision == this->floatingPointPosition()) {
        return;
    }

    if (newPrecision < this->floatingPointPosition()) {
        for (int i = 0; i < floatingPointPosition() - newPrecision; ++i) {
            this->_chunks.pop_front();
        }

        return;
    }

    for (int i = 0; i < newPrecision - this->floatingPointPosition(); ++i) {
        this->_chunks.push_front(0);
    }

    this->_floatingPointPosition = newPrecision;
}

uint32_t strBeToU32(std::string binaryString) {
    std::ranges::reverse(binaryString);
    return std::bitset<32>(binaryString).to_ulong();
}

BigDecimal::BigDecimal(const std::string &s) {
    std::deque<char> sd{s.rbegin(), s.rend()};

    //region Floating point position initialization
    auto fpp = s.find('.');
    if (fpp != std::string::npos) {
        fpp = s.size() - s.find('.') - 1;
        sd.erase(sd.begin() + fpp); // NOLINT(*-narrowing-conversions)

        auto rem = fpp % chunkSize;
        auto padding = rem == 0 ? 0 : chunkSize - rem;

        for (int i = 0; i < padding; ++i) {
            sd.push_front('0');
        }

        fpp += padding;
        this->_floatingPointPosition = fpp / 32;
    } else {
        this->_floatingPointPosition = 0;
    }
    //endregion

    //region Sign initialization
    if (sd.back() == '-') {
        this->_sign = -1;
        sd.pop_back();
    } else {
        this->_sign = 1;
    }
    //endregion

    //region Adding padding
    auto rem = sd.size() % chunkSize;
    auto padding = rem == 0 ? 0 : chunkSize - rem;

    for (int i = 0; i < padding; ++i) {
        sd.push_back('0');
    }
    //endregion

    //region Chunk conversion
    for (int offset = 0; offset < sd.size(); offset += chunkSize) {
        auto left = sd.begin() + offset;
        auto right = left + chunkSize;

        auto chunk = strBeToU32(std::string{left, right});

        this->_chunks.push_back(chunk);
    }
    //endregion
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
            t.value = t.value * rhs->chunks()[i] + overflow;
            tempResult._chunks[j] = t.chunks[0];
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

BigDecimal BigDecimal::operator/(BigDecimal &other) {
    auto lhs = this;
    auto rhs = &other;

    lhs->trim();
    rhs->trim();

    const auto lhsSign = lhs->sign();
    const auto rhsSign = rhs->sign();

    lhs->_sign *= lhs->_sign;
    rhs->_sign *= rhs->_sign;

    auto zero = BigDecimal{};
    if (*rhs == zero) {
        throw std::invalid_argument("Divisor must not be zero");
    }

    BigDecimal remainder{};
    BigDecimal result{};

    const auto numberOfBitsInLhs = lhs->numberOfBits();

    for (int64_t i = numberOfBitsInLhs - 1; i >= 0; --i) {
        const auto bit = lhs->getBit(i);
        remainder.pushBitFront(bit);

        if (remainder >= *rhs) {
            remainder -= *rhs;
            result.pushBitFront(1);
            continue;
        }

        result.pushBitFront(0);
    }

    result.trim();

    auto deltaFpp = (int64_t) lhs->floatingPointPosition() - (int64_t) rhs->floatingPointPosition();
    if (deltaFpp < 0) {
        for (int i = 0; i < std::abs(deltaFpp); ++i) {
            result._chunks.push_front(0);
        }

        deltaFpp = 0;
    }

    result._floatingPointPosition = deltaFpp;
    result._sign = lhsSign * rhsSign;

    rhs->_sign = rhsSign;

    return result;
}

uint32_t BigDecimal::getBit(size_t index) {
    auto chunkIndex = index / chunkSize;
    auto bitIndex = index % 32;

    if (this->chunks().empty()) {
        this->_chunks.push_back(0);
    }

    return ((1 << bitIndex) & this->_chunks[chunkIndex]) >> bitIndex;
}

void BigDecimal::pushBitFront(uint32_t bitValue) {
    *this <<= 1;

    this->_chunks[0] |= (bitValue & this->getBit(1) | bitValue);
}

size_t BigDecimal::numberOfBits() {
    return this->size() * chunkSize;
}

void BigDecimal::setBit(size_t index, uint32_t value) {
    auto chunkIndex = index / chunkSize;
    auto bitIndex = index % 32;

    auto mask = (1 & value) << bitIndex;

    this->_chunks[chunkIndex] |= mask;
}

BigDecimal &BigDecimal::operator/=(BigDecimal &other) {
    auto t = *this / other;
    *this = t;

    return *this;
}

void BigDecimal::flipSign() {
    this->_sign *= -1;
}

const std::deque<uint32_t> &BigDecimal::chunks() {
    return this->_chunks;
}

BigDecimal &BigDecimal::operator<<=(int shift) {
    if (this->size() == 0) {
        this->_chunks.push_back(0);
    }

    auto getOverflow = [=](uint32_t chunk) {
        return chunk >> (chunkSize - shift);
    };

    auto firstOverflow = getOverflow(*chunks().rbegin());
    if (firstOverflow != 0) {
        _chunks.push_back(firstOverflow);
    }

    for (auto i = this->size(); i > 0; --i) {
        auto overflow = getOverflow(chunks()[i - 1]);
        _chunks[i] <<= shift;
        _chunks[i] |= overflow;
    }

    _chunks[0] <<= shift;

    return *this;
}

BigDecimal &BigDecimal::operator=(BigDecimal &other) {
    _chunks = other.chunks();
    _sign = other.sign();
    _floatingPointPosition = other.floatingPointPosition();

    return *this;
}

BigDecimal::BigDecimal(BigDecimal &other) {
    _floatingPointPosition = other._floatingPointPosition;
    _chunks = std::deque<uint32_t> {other._chunks.begin(), other._chunks.end()};
    _sign = other._sign;
}

BigDecimal operator ""_longnum(long double number) {
    std::basic_string<char> binaryString = BigDecimal::toBinary(number);
    BigDecimal bd{binaryString};

    return bd;
}
