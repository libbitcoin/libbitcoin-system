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
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/elliptic_curve.hpp>
#include <bitcoin/system/utility/serializer.hpp>

namespace libbitcoin {
namespace system {

const ec_scalar ec_scalar::zero(0);

ec_scalar::ec_scalar()
{
    // Avoid initializing secret by default.
    // Helps when you pre-allocate large arrays of values.
}

ec_scalar::ec_scalar(uint64_t value)
{
    *this = value;
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

// Operators.
// ----------------------------------------------------------------------------

ec_scalar& ec_scalar::operator=(uint64_t value)
{
    secret_.reset(new ec_secret);
    BITCOIN_ASSERT(secret_->size() == 32);
    auto serial = bc::system::make_unsafe_serializer(secret_->begin());
    serial.write_8_bytes_big_endian(0);
    serial.write_8_bytes_big_endian(0);
    serial.write_8_bytes_big_endian(0);
    serial.write_8_bytes_big_endian(value);
    return *this;
}

ec_scalar& ec_scalar::operator=(const ec_secret& secret)
{
    secret_.reset(new ec_secret(secret));
    return *this;
}

ec_scalar& ec_scalar::operator=(const ec_scalar& scalar)
{
    if (!scalar)
    {
        secret_.reset();
        return *this;
    }
    secret_.reset(new ec_secret(scalar.secret()));
    return *this;
}

ec_scalar& ec_scalar::operator=(ec_scalar&& scalar)
{
    secret_ = std::move(scalar.secret_);
    return *this;
}

ec_scalar ec_scalar::operator-() const
{
    if (!(*this))
        return *this;

    auto negation = *this;
    BITCOIN_ASSERT(negation.secret_);
    if (!ec_negate(*negation.secret_))
        return {};

    return negation;
}

ec_scalar& ec_scalar::operator+=(const ec_scalar& scalar)
{
    if (!(*this))
        return *this;

    *this = *this + scalar;
    return *this;
}

ec_scalar& ec_scalar::operator-=(const ec_scalar& scalar)
{
    if (!(*this))
        return *this;

    *this = *this - scalar;
    return *this;
}

ec_scalar::operator bool() const
{
    // Caller must make both test against zero when that is required.
    // Overloading bool with valid and non-zero would be very non-intuitive.
    return static_cast<bool>(secret_);
}

ec_scalar::operator const ec_secret&() const
{
    BITCOIN_ASSERT(secret_);
    return *secret_;
}

const ec_secret& ec_scalar::secret() const
{
    BITCOIN_ASSERT(secret_);
    return *secret_;
}

bool operator==(const ec_scalar& left, const ec_scalar& right)
{
    // Compare arrays from left and right in reverse order since
    // scalars are encoded in big endian format.
    // The beginning bytes will be only zeroes for small scalars.
    return left && right &&
        std::equal(left.secret().rbegin(), left.secret().rend(),
            right.secret().rbegin());
}

bool operator!=(const ec_scalar& left, const ec_scalar& right)
{
    return !(left == right);
}

ec_scalar operator+(ec_scalar left, const ec_scalar& right)
{
    if (!left || !right)
        return {};

    BITCOIN_ASSERT(left.secret_);
    BITCOIN_ASSERT(right.secret_);
    if (!ec_add(*left.secret_, *right.secret_))
        return {};

    return left;
}

ec_scalar operator-(ec_scalar left, const ec_scalar& right)
{
    if (!left || !right)
        return {};

    const auto negative_right = -right;
    if (!negative_right)
        return {};

    return left + negative_right;
}

ec_scalar operator*(ec_scalar left, const ec_scalar& right)
{
    if (!left || !right)
        return {};

    BITCOIN_ASSERT(left.secret_);
    BITCOIN_ASSERT(right.secret_);
    if (!ec_multiply(*left.secret_, *right.secret_))
        return {};

    return left;
}

} // namespace system
} // namespace libbitcoin
