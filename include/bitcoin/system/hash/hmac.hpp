/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_HMAC_HPP
#define LIBBITCOIN_SYSTEM_HASH_HMAC_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/accumulator.hpp>

namespace libbitcoin {
namespace system {

/// hmac accumulator/finalized digest.
/// Algorithm can be any iterated cryptographic hash function exposing:
///     digest_t, block_t, void write(data_slice), and block_t flush().
template <typename Algorithm>
class hmac
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(hmac);
    using digest_t = typename Algorithm::digest_t;

    /// hmac accumulator, not resettable.
    inline hmac(const data_slice& key) NOEXCEPT;

    inline void write(const data_slice& data) NOEXCEPT;
    inline digest_t flush() NOEXCEPT;

    /// finalized authentication code.
    static inline digest_t code(const data_slice& data,
        const data_slice& key) NOEXCEPT;

protected:
    using byte_t = typename Algorithm::byte_t;
    using block_t = typename Algorithm::block_t;

    static CONSTEVAL block_t inner_pad() NOEXCEPT;
    static CONSTEVAL block_t outer_pad() NOEXCEPT;
    static constexpr block_t& xor_n(block_t& pad, const byte_t* from,
        size_t size) NOEXCEPT;

    inline void xor_key(const byte_t* key, size_t size) NOEXCEPT;

private:
    accumulator<Algorithm> inner_{};
    accumulator<Algorithm> outer_{};
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/hash/hmac.ipp>

#endif
