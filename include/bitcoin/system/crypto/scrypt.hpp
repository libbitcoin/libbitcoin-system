/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace scrypt {
    
/// [W]ork must be a power of 2 greater than 1.
/// [R]esources and [P]arallelism must satisfy [(R * P) < 2^30].
/// Size must not exceed [sub1(2^32) * 2^5].
/// Memory (stack) required in bytes ~ [add1(W + P) * R * 2^7].

/// False if buffer size > [sub1(2^32) * 2^5].
template<size_t W, size_t R, size_t P,
    bool_if<
        ((W > one) && (power2(floored_log2(W)) == W)) &&
        (safe_multiply(R, P) < power2<uint32_t>(30u))> = true>
inline bool hash(const data_slice& phrase, const data_slice& salt,
    uint8_t* buffer, size_t size) NOEXCEPT;

template<size_t Size, size_t W, size_t R, size_t P,
    if_not_greater<Size,
        sub1(power2<uint64_t>(32u)) * power2<uint64_t>(5u)> = true>
inline data_array<Size> hash(const data_slice& phrase,
    const data_slice& salt) NOEXCEPT
{
    data_array<Size> out{};
    scrypt::hash<W, R, P>(phrase, salt, out.data(), Size);
    return out;
}

} // namespace scrypt
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/crypto/scrypt.ipp>

#endif
