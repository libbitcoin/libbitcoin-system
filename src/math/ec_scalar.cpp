/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/math/ec_scalar.hpp>

#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

// TODO: implement equality and inequality for ec_scalar and ec_secret.
const ec_scalar ec_scalar::zero = null_hash;

// Null is valid but uninitialized object is invalid.
ec_scalar::ec_scalar()
  : valid_(false), secret_(null_hash)
{
}

ec_scalar::ec_scalar(const ec_secret& secret)
  : valid_(true), secret_(secret)
{
}

ec_scalar::ec_scalar(uint64_t value)
  : valid_(true)
{
    secret_ = null_hash;
    auto serial = make_unsafe_serializer(secret_.end() - 8);
    serial.write_8_bytes_big_endian(value);
}

// Operators.
// ----------------------------------------------------------------------------

ec_scalar ec_scalar::operator-() const
{
    if (!(*this))
        return *this;

    auto negation = *this;
    if (!ec_negate(negation.secret_))
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

ec_scalar& ec_scalar::operator=(const ec_secret& secret)
{
    valid_ = true;
    secret_ = secret;
    return *this;
}

ec_scalar operator+(ec_scalar left, const ec_scalar& right)
{
    if (!left || !right)
        return {};

    if (!ec_add(left.secret_, right.secret_))
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

    if (!ec_multiply(left.secret_, right.secret_))
        return {};

    return left;
}

ec_scalar::operator bool() const
{
    // Caller must make both test against zero when that is required.
    // Overloading bool with valid and non-zero would be very non-intuitive.
    return valid_ /*&& secret_ != ec_scalar::zero*/;
}

ec_scalar::operator const ec_secret&() const
{
    return secret_;
}

const ec_secret& ec_scalar::secret() const
{
    return secret_;
}
ec_secret& ec_scalar::secret()
{
    return secret_;
}

} // namespace libbitcoin
