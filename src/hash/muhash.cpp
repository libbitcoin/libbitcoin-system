/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/hash/muhash.hpp>

#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// The largest 3072 bit prime, the multiplicative group.
static const uintx& modulus() NOEXCEPT
{
    static const uintx prime = (uintx{ 1 } << 3072u) - 1103717u;
    return prime;
}

muhash3072::muhash3072() NOEXCEPT
  : numerator_(1), denominator_(1)
{
}

muhash3072::muhash3072(const data_slice& element) NOEXCEPT
  : numerator_(to_element(element)), denominator_(1)
{
}

// Elements are the chacha20 expansion of the element hash.
uintx muhash3072::to_element(const data_slice& element) NOEXCEPT
{
    data_array<byte_size> bytes{};
    chacha20 cipher{ accumulator<sha256>::hash(element.size(),
        element.data()) };
    cipher.stream(bytes);
    return from_little_endian(to_chunk(bytes)) % modulus();
}

void muhash3072::insert(const data_slice& element) NOEXCEPT
{
    numerator_ = (numerator_ * to_element(element)) % modulus();
}

void muhash3072::remove(const data_slice& element) NOEXCEPT
{
    denominator_ = (denominator_ * to_element(element)) % modulus();
}

hash_digest muhash3072::flush() NOEXCEPT
{
    using namespace boost::multiprecision;
    const auto& prime = modulus();

    // Removals divide out by multiplication with the modular inverse, obtained
    // by exponentiation as the modulus is prime (fermat's little theorem).
    numerator_ = (numerator_ * powm(denominator_, prime - 2u, prime)) % prime;
    denominator_ = 1;
    return sha256_hash(to_little_endian_size<byte_size>(numerator_));
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
