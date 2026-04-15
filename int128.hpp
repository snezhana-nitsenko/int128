#ifndef INT128_HPP
#define INT128_HPP

#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>

class Int128 {
public:
    Int128();
    Int128(int64_t value);
    Int128(std::string_view value);

    explicit operator int64_t() const;
    explicit operator double() const;

    std::string str() const;

    Int128& operator+=(const Int128& other);
    Int128& operator-=(const Int128& other);
    Int128& operator*=(const Int128& other);
    Int128& operator/=(const Int128& other);

    Int128 operator-() const;

    friend Int128 operator+(Int128 lhs, const Int128& rhs);
    friend Int128 operator-(Int128 lhs, const Int128& rhs);
    friend Int128 operator*(Int128 lhs, const Int128& rhs);
    friend Int128 operator/(Int128 lhs, const Int128& rhs);
    friend bool operator==(const Int128& lhs, const Int128& rhs);
    friend bool operator!=(const Int128& lhs, const Int128& rhs);
    friend std::ostream& operator<<(std::ostream& out, const Int128& value);

private:
    using storage_type = __int128_t;
    using unsigned_storage_type = __uint128_t;
    struct raw_tag {};

    Int128(storage_type value, raw_tag);

    static storage_type parse(std::string_view text);

    storage_type value_ = 0;
};

#endif
