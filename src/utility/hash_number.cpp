/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/utility/hash_number.hpp>

namespace libbitcoin {

hash_number::hash_number()
{
}
hash_number::hash_number(const uint64_t value)
  : hash_(value)
{
}
void hash_number::set_compact(uint32_t compact)
{
    hash_.SetCompact(compact);
}
uint32_t hash_number::compact() const
{
    return hash_.GetCompact();
}
void hash_number::set_hash(const hash_digest& hash)
{
    std::copy(hash.begin(), hash.end(), hash_.begin());
}

void hash_number::operator*=(uint32_t value)
{
    hash_ *= value;
}
void hash_number::operator/=(uint32_t value)
{
    hash_ /= value;
}

bool operator>(const hash_number& number_a, const hash_number& number_b)
{
    return number_a.hash_.CompareTo(number_b.hash_) > 0;
}
bool operator<=(const hash_number& number_a, const hash_number& number_b)
{
    return number_a.hash_.CompareTo(number_b.hash_) <= 0;
}

} // namespace libbitcoin

