/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/math/hash_number.hpp>

#include <algorithm>

namespace libbitcoin {

hash_number::hash_number()
{
}

hash_number::hash_number(uint64_t value)
  : hash_(value)
{
}

hash_number::hash_number(const hash_digest& value)
{
    std::copy(value.begin(), value.end(), hash_.begin());
}

bool hash_number::set_compact(uint32_t value)
{
    auto is_negative = true;
    auto is_overflow = true;
    hash_.SetCompact(value, &is_negative, &is_overflow);
    return !is_negative && !is_overflow;
}

uint32_t hash_number::compact() const
{
    return hash_.GetCompact();
}

hash_digest hash_number::hash() const
{
    hash_digest value;
    std::copy(hash_.begin(), hash_.end(), value.begin());
    return value;
}

/// Copy Operators.

hash_number hash_number::operator~() const
{
    hash_number result;
    result.hash_ = ~hash_;
    return result;
}

hash_number hash_number::operator<<(uint32_t value)
{
    return hash_number(*this) <<= value;
}

hash_number hash_number::operator/(const hash_number& value)
{
    return hash_number(*this) /= value;
}

hash_number hash_number::operator+(const hash_number& value)
{
    return hash_number(*this) += value;
}

/// Change Operators.

hash_number& hash_number::operator*=(uint32_t value)
{
    hash_ *= value;
    return *this;
}

hash_number& hash_number::operator/=(uint32_t value)
{
    hash_ /= value;
    return *this;
}

hash_number& hash_number::operator<<=(uint32_t value)
{
    hash_ <<= value;
    return *this;
}

hash_number& hash_number::operator/=(const hash_number& value)
{
    hash_ /= value.hash_;
    return *this;
}

hash_number& hash_number::operator+=(const hash_number& value)
{
    hash_ += value.hash_;
    return *this;
}

/// Comparison Operators.

bool hash_number::operator==(uint64_t value) const
{
    return hash_.EqualTo(value);
}

bool hash_number::operator!=(uint64_t value) const
{
    return !(*this == value);
}

bool hash_number::operator>(const hash_number& value) const
{
    return hash_.CompareTo(value.hash_) > 0;
}

bool hash_number::operator<(const hash_number& value) const
{
    return hash_.CompareTo(value.hash_) < 0;
}

bool hash_number::operator>=(const hash_number& value) const
{
    return hash_.CompareTo(value.hash_) >= 0;
}

bool hash_number::operator<=(const hash_number& value) const
{
    return hash_.CompareTo(value.hash_) <= 0;
}

} // namespace libbitcoin

