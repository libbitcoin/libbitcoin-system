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
#ifndef LIBBITCOIN_SYSTEM_HASH_HKDF_HPP
#define LIBBITCOIN_SYSTEM_HASH_HKDF_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hmac.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Maximum L is [255 * HashLen].
template <typename Algorithm>
constexpr auto hkdf_maximum_size = safe_multiply<size_t>(max_uint8,
    array_count<typename Algorithm::digest_t>);

/// rfc5869 hmac-based extract-and-expand key derivation.
/// Algorithm can be any iterated cryptographic hash function exposing:
///     digest_t, block_t, void write(data_slice), and block_t flush().
template <typename Algorithm>
struct hkdf
{
    static constexpr auto maximum_size = hkdf_maximum_size<Algorithm>;
    using digest_t = typename Algorithm::digest_t;

    /// Extract a pseudorandom key from salt and input keying material.
    static inline digest_t extract(const data_slice& material,
        const data_slice& salt) NOEXCEPT;

    /// Expand the pseudorandom key into output keying material.
    template <size_t Size,
        if_not_greater<Size, hkdf_maximum_size<Algorithm>> = true>
    static inline void expand(data_array<Size>& out, const digest_t& key,
        const data_slice& info) NOEXCEPT;

    /// Extract-and-expand, return by reference.
    template <size_t Size,
        if_not_greater<Size, hkdf_maximum_size<Algorithm>> = true>
    static inline void key(data_array<Size>& out, const data_slice& material,
        const data_slice& salt, const data_slice& info) NOEXCEPT;

    /// Extract-and-expand, return by value.
    template <size_t Size,
        if_not_greater<Size, hkdf_maximum_size<Algorithm>> = true>
    static inline data_array<Size> key(const data_slice& material,
        const data_slice& salt, const data_slice& info) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename Algorithm>
#define CLASS hkdf<Algorithm>

#include <bitcoin/system/impl/hash/hkdf.ipp>

#undef CLASS
#undef TEMPLATE

#endif
