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

ec_scalar::ec_scalar()
{
}
ec_scalar::ec_scalar(uint64_t value)
{
    *this = value;
}
ec_scalar::ec_scalar(const ec_secret& secret)
{
    *this = secret;
}

void ec_scalar::reset()
{
    valid_ = true;
    std::fill(scalar_.begin(), scalar_.end(), 0);
}

void ec_scalar::invalidate()
{
    valid_ = false;
}

ec_scalar& ec_scalar::operator=(uint64_t value)
{
    valid_ = true;
    reset();
    auto serial = bc::make_unsafe_serializer(scalar_.end() - 8);
    serial.write_8_bytes_big_endian(static_cast<uint64_t>(value));
    return *this;
}
ec_scalar& ec_scalar::operator=(const ec_secret& secret)
{
    valid_ = true;
    scalar_ = secret;
    return *this;
}

bool ec_scalar::is_zero() const
{
    return scalar_ == null_hash;
}
bool ec_scalar::is_valid() const
{
    return valid_;
}
ec_scalar::operator bool() const
{
    return is_valid() && !is_zero();
}

ec_scalar ec_scalar::operator-() const
{
    if (!valid_)
        return *this;
    auto result = *this;
    bool rc = ec_negate(result.scalar_);
    if (!rc)
        result.invalidate();
    return result;
}
ec_scalar& ec_scalar::operator+=(const ec_scalar& rhs)
{
    if (!valid_)
        return *this;
    *this = *this + rhs;
    return *this;
}
ec_scalar& ec_scalar::operator-=(const ec_scalar& rhs)
{
    if (!valid_)
        return *this;
    *this = *this - rhs;
    return *this;
}

ec_scalar operator+(ec_scalar lhs, const ec_scalar& rhs)
{
    if (!lhs.valid_ || !rhs.valid_)
    {
        lhs.invalidate();
        return lhs;
    }
    bool rc = ec_add(lhs.scalar_, rhs.scalar_);
    if (!rc)
        lhs.invalidate();
    return lhs;
}
ec_scalar operator-(ec_scalar lhs, const ec_scalar& rhs)
{
    if (!lhs.valid_ || !rhs.valid_)
    {
        lhs.invalidate();
        return lhs;
    }
    const auto negative_rhs = -rhs;
    if (!negative_rhs.valid_)
        lhs.invalidate();
    return lhs + negative_rhs;
}
ec_scalar operator*(ec_scalar lhs, const ec_scalar& rhs)
{
    if (!lhs.valid_ || !rhs.valid_)
    {
        lhs.invalidate();
        return lhs;
    }
    bool rc = ec_multiply(lhs.scalar_, rhs.scalar_);
    if (!rc)
        lhs.invalidate();
    return lhs;
}

const ec_secret& ec_scalar::secret() const
{
    return scalar_;
}
ec_scalar::operator ec_secret() const
{
    return secret();
}

} // namespace libbitcoin

