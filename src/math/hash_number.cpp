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

namespace libbitcoin {

hash_number::hash_number()
{
}
hash_number::hash_number(uint64_t value)
  : hash_(value)
{
}
bool hash_number::set_compact(uint32_t compact)
{
    bool is_negative = true, is_overflow = true;
    hash_.SetCompact(compact, &is_negative, &is_overflow);
    return !is_negative && !is_overflow;
}
uint32_t hash_number::compact() const
{
    return hash_.GetCompact();
}
void hash_number::set_hash(const hash_digest& hash)
{
    std::copy(hash.begin(), hash.end(), hash_.begin());
}
hash_digest hash_number::hash() const
{
    hash_digest result;
    std::copy(hash_.begin(), hash_.end(), result.begin());
    return result;
}

const hash_number hash_number::operator~() const
{
    hash_number result;
    result.hash_ = ~hash_;
    return result;
}

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
hash_number& hash_number::operator<<=(uint32_t shift)
{
    hash_ <<= shift;
    return *this;
}
hash_number& hash_number::operator/=(const hash_number& number_b)
{
    hash_ /= number_b.hash_;
    return *this;
}
hash_number& hash_number::operator+=(const hash_number& number_b)
{
    hash_ += number_b.hash_;
    return *this;
}

bool operator>(const hash_number& number_a, const hash_number& number_b)
{
    return number_a.hash_.CompareTo(number_b.hash_) > 0;
}
bool operator<=(const hash_number& number_a, const hash_number& number_b)
{
    return number_a.hash_.CompareTo(number_b.hash_) <= 0;
}
const hash_number operator<<(const hash_number& number_a, int shift)
{
    return hash_number(number_a) <<= shift;
}
const hash_number operator/(
    const hash_number& number_a, const hash_number& number_b)
{
    return hash_number(number_a) /= number_b;
}
const hash_number operator+(
    const hash_number& number_a, const hash_number& number_b)
{
    return hash_number(number_a) += number_b;
}
bool operator==(
    const hash_number& number, uint64_t value)
{
    return number.hash_.EqualTo(value);
}

} // namespace libbitcoin

