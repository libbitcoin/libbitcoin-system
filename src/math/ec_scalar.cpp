/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/math/ec_scalar.hpp>

#include <algorithm>
#include <array>
#include <memory>
#include <utility>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/elliptic_curve.hpp>
#include <bitcoin/system/utility/serializer.hpp>

namespace libbitcoin {
namespace system {

const ec_scalar ec_scalar::zero(0);

ec_scalar::ec_scalar()
{
    // Avoid initializing secret pointer by default.
    // Improves performance when pre-allocating large sets of ec_scalar.
}

ec_scalar::ec_scalar(const ec_secret& secret)
{
    *this = secret;
}

ec_scalar::ec_scalar(const ec_scalar& scalar)
{
    *this = scalar;
}

ec_scalar::ec_scalar(ec_scalar&& scalar)
{
    *this = std::move(scalar);
}

ec_scalar::ec_scalar(uint64_t value)
{
    *this = value;
}


// Operators.
// ----------------------------------------------------------------------------

ec_scalar& ec_scalar::operator=(const ec_secret& secret)
{
    secret_ = std::make_shared<ec_secret>(secret);
    return *this;
}

ec_scalar& ec_scalar::operator=(const ec_scalar& scalar)
{
    if (scalar)
        *this = scalar.secret();
    else
        this->secret_.reset();

    return *this;
}

ec_scalar& ec_scalar::operator=(ec_scalar&& scalar)
{
    secret_ = scalar.secret_;
    scalar.secret_.reset();
    return *this;
}

// Canonical conversion from 64 bit unsigned integer is defined by OpenSSL.
ec_scalar& ec_scalar::operator=(uint64_t value)
{
    static constexpr auto value_size = sizeof(uint64_t);
    static_assert(std::tuple_size<ec_secret>::value >= value_size, "overflow");

    // ec_secret is value-initialized to zero.
    secret_ = std::make_shared<ec_secret>(ec_secret{});
    auto value_iterator = secret_->end() - value_size;

    // Write last value_size bytes with value.
    auto serial = bc::system::make_unsafe_serializer(value_iterator);
    serial.write_8_bytes_big_endian(value);
    return *this;
}

ec_scalar ec_scalar::operator-()
{
    if (!(*this) || !ec_negate(*secret_))
        return {};

    return *this;
}

ec_scalar& ec_scalar::operator+=(const ec_scalar& scalar)
{
    if (!(*this) || !scalar || !ec_add(*secret_, scalar))
        secret_.reset();

    return *this;
}

ec_scalar& ec_scalar::operator-=(const ec_scalar& scalar)
{
    // TODO: any way to avoid copy (cannot change const parameter)?
    auto copy = scalar;

    if (!(*this) || !scalar || !ec_add(*secret_, -copy))
        secret_.reset();

    return *this;
}

ec_scalar& ec_scalar::operator*=(const ec_scalar& scalar)
{
    if (!(*this) || !scalar || !ec_multiply(*secret_, scalar))
        secret_.reset();

    return *this;
}

bool operator==(const ec_scalar& left, const ec_scalar& right)
{
    // Compare arrays from left and right in reverse order since scalars are
    // encoded in big endian format, with leading bytes zero for small scalars.
    return left && right &&
        std::equal(left.secret().rbegin(), left.secret().rend(),
            right.secret().rbegin());
}

bool operator!=(const ec_scalar& left, const ec_scalar& right)
{
    return !(left == right);
}

ec_scalar operator+(const ec_scalar& left, const ec_scalar& right)
{
    if (!left || !right)
        return {};

    // TODO: any way to avoid copy (cannot change const parameter)?
    auto copy = left;

    return copy += right;
}

ec_scalar operator-(const ec_scalar& left, const ec_scalar& right)
{
    if (!left || !right)
        return {};

    // TODO: any way to avoid copies (cannot change const parameters)?
    auto left_copy = left;
    auto right_copy = right;

    return left_copy += -right_copy;
}

ec_scalar operator*(const ec_scalar& left, const ec_scalar& right)
{
    if (!left || !right)
        return {};

    // TODO: any way to avoid copy (cannot change const parameter)?
    auto copy = left;

    return copy *= right;
}

// Cast operators.
// ----------------------------------------------------------------------------

ec_scalar::operator bool() const
{
    // Caller must make both test against zero when that is required.
    // Overloading bool with valid and non-zero would be very non-intuitive.
    return static_cast<bool>(secret_);
}

ec_scalar::operator const ec_secret&() const
{
    // Operation results or the object must be tested by the caller.
    BITCOIN_ASSERT(secret_);
    return *secret_;
}

// Accessors.
// ----------------------------------------------------------------------------

const ec_secret& ec_scalar::secret() const
{
    // Operation results or the object must be tested by the caller.
    BITCOIN_ASSERT(secret_);
    return *secret_;
}

} // namespace system
} // namespace libbitcoin
