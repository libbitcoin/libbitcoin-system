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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_HASH_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_HASH_IPP

#include <algorithm>
#include <cstddef>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {

template <uint32_t Bits, size_t Bytes>
constexpr data_array<Bytes> to_hash(const uintx_t<Bits>& value) NOEXCEPT
{
    return to_little_endian_size<Bytes>(value);
}

template <size_t Bytes, uint32_t Bits>
constexpr uintx_t<Bits> to_uintx(const data_array<Bytes>& hash) NOEXCEPT
{
    return uintx_from_little_endian_array<Bytes>(hash);
}

template<size_t Size>
inline data_array<Size> scrypt(const data_slice& data, const data_slice& salt,
    uint64_t work, uint32_t resources, uint32_t parallelism) NOEXCEPT
{
    return to_array<Size>(scrypt_chunk(data, salt, work, resources,
        parallelism, Size));
}

} // namespace system
} // namespace libbitcoin

#endif
