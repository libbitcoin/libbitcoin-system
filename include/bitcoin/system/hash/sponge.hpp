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
#ifndef LIBBITCOIN_SYSTEM_HASH_SPONGE_HPP
#define LIBBITCOIN_SYSTEM_HASH_SPONGE_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/algorithms.hpp>

namespace libbitcoin {
namespace system {

/// Sponge accumulator, finalized to a digest or squeezed (extendable output).
/// Algorithm is a sha3 algorithm (sha3_256, shake_256, keccak_256).
template <typename Algorithm,
    if_same<typename Algorithm::H::T, sha3::sha3h_t> = true>
class sponge
{
public:
    DEFAULT_COPY_MOVE(sponge);

    using digest_t = typename Algorithm::digest_t;

    /// Sponge accumulator, not writable once output is taken.
    inline sponge() NOEXCEPT;
    inline void write(const_byte_span data) NOEXCEPT;

    /// Squeeze the next digest size of output (the digest when first).
    inline digest_t flush() NOEXCEPT;

    /// Squeeze the next out.size() bytes of output.
    inline void squeeze(byte_span out) NOEXCEPT;

protected:
    using state_t = typename Algorithm::state_t;
    using block_t = typename Algorithm::block_t;
    using iblocks_t = typename Algorithm::iblocks_t;
    static constexpr auto block_bytes = Algorithm::block_bytes;

private:
    state_t state_;
    block_t buffer_{};
    size_t offset_{};
    bool sealed_{};
};

} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename Algorithm, \
    if_same<typename Algorithm::H::T, sha3::sha3h_t> If>
#define CLASS sponge<Algorithm, If>

#include <bitcoin/system/impl/hash/sponge.ipp>

#undef CLASS
#undef TEMPLATE

#endif
