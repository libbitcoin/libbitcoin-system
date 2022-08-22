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
#ifndef LIBBITCOIN_SYSTEM_HASH_PBKD_HPP
#define LIBBITCOIN_SYSTEM_HASH_PBKD_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Maximum dkLen is [(2^32-1) * hLen].
template <typename Algorithm>
constexpr auto pbkd_maximum_size = safe_multiply<uint64_t>(max_uint32,
    array_count<typename Algorithm::digest_t>);

/// pkcs5 pbkdf2 derivation.
/// Algorithm can be any iterated pseudorandom function exposing:
///     digest_t, block_t, void write(data_slice), and block_t flush().
///     Algorithm::flush() must be non-clearing.
template <typename Algorithm>
struct pbkd
{
    static constexpr auto maximum_size = pbkd_maximum_size<Algorithm>;

    /// Return by reference.
    template <size_t Size,
        if_not_greater<Size, pbkd_maximum_size<Algorithm>> = true>
    static inline void key(data_array<Size>& out, const data_slice& password,
        const data_slice& salt, size_t count) NOEXCEPT;

    /// Return by value.
    template <size_t Size,
        if_not_greater<Size, pbkd_maximum_size<Algorithm>> = true>
    static inline data_array<Size> key(const data_slice& password,
        const data_slice& salt, size_t count) NOEXCEPT;

protected:
    template <size_t Length>
    static constexpr auto xor_n(data_array<Length>& to,
        const data_array<Length>& from) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/hash/pbkd.ipp>

#endif
