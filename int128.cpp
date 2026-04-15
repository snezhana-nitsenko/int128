#include "int128.hpp"

#include <ostream>

Int128::Int128() = default;

Int128::Int128(int64_t value)
    : value_(static_cast<storage_type>(value)) {
}

Int128::Int128(std::string_view value)
    : value_(parse(value)) {
}

Int128::operator int64_t() const {
    return static_cast<int64_t>(value_);
}

Int128::operator double() const {
    return static_cast<double>(value_);
}

std::string Int128::str() const {
    if (value_ == 0) {
        return "0";
    }

    const bool negative = value_ < 0;
    unsigned_storage_type magnitude = negative
        ? static_cast<unsigned_storage_type>(-(value_ + 1)) + 1
        : static_cast<unsigned_storage_type>(value_);

    std::string result;
    while (magnitude != 0) {
        const auto digit = static_cast<unsigned>(magnitude % 10);
        result.push_back(static_cast<char>('0' + digit));
        magnitude /= 10;
    }

    if (negative) {
        result.push_back('-');
    }

    return std::string(result.rbegin(), result.rend());
}

Int128& Int128::operator+=(const Int128& other) {
    value_ += other.value_;
    return *this;
}

Int128& Int128::operator-=(const Int128& other) {
    value_ -= other.value_;
    return *this;
}

Int128& Int128::operator*=(const Int128& other) {
    value_ *= other.value_;
    return *this;
}

Int128& Int128::operator/=(const Int128& other) {
    value_ /= other.value_;
    return *this;
}

Int128 Int128::operator-() const {
    return Int128(-value_, raw_tag{});
}

Int128::Int128(storage_type value, raw_tag)
    : value_(value) {
}

Int128::storage_type Int128::parse(std::string_view text) {
    bool negative = false;
    std::size_t index = 0;

    if (!text.empty() && (text.front() == '+' || text.front() == '-')) {
        negative = text.front() == '-';
        index = 1;
    }

    unsigned_storage_type magnitude = 0;
    for (; index < text.size(); ++index) {
        magnitude *= 10;
        magnitude += static_cast<unsigned>(text[index] - '0');
    }

    if (!negative) {
        return Int128(static_cast<storage_type>(magnitude), raw_tag{}).value_;
    }

    constexpr unsigned_storage_type min_magnitude = unsigned_storage_type{1} << 127;
    if (magnitude == min_magnitude) {
        return Int128(static_cast<storage_type>(min_magnitude), raw_tag{}).value_;
    }

    return Int128(-static_cast<storage_type>(magnitude), raw_tag{}).value_;
}

Int128 operator+(Int128 lhs, const Int128& rhs) {
    lhs += rhs;
    return lhs;
}

Int128 operator-(Int128 lhs, const Int128& rhs) {
    lhs -= rhs;
    return lhs;
}

Int128 operator*(Int128 lhs, const Int128& rhs) {
    lhs *= rhs;
    return lhs;
}

Int128 operator/(Int128 lhs, const Int128& rhs) {
    lhs /= rhs;
    return lhs;
}

bool operator==(const Int128& lhs, const Int128& rhs) {
    return lhs.value_ == rhs.value_;
}

bool operator!=(const Int128& lhs, const Int128& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& out, const Int128& value) {
    return out << value.str();
}
