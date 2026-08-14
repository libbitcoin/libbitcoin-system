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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_POLY1305_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_POLY1305_HPP

#include <span>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Poly1305 one-time authenticator (rfc8439).
/// The key must be used to authenticate no more than one message.
class BC_API poly1305 final
{
public:
    /// Poly1305 secret is always 256 bits.
    static constexpr size_t secret_size = 32;
    typedef data_array<secret_size> secret;

    /// Poly1305 tag is always 128 bits.
    static constexpr size_t tag_size = 16;
    typedef data_array<tag_size> tag;

    /// Poly1305 accumulator, not resettable.
    poly1305(const secret& key) NOEXCEPT;

    void write(std::span<const uint8_t> data) NOEXCEPT;
    void flush(tag& out) NOEXCEPT;

private:
    static constexpr size_t block_size = 16;

    void blocks(const uint8_t* data, size_t blocks, uint32_t hibit) NOEXCEPT;

    std_array<uint32_t, 5> r_{};
    std_array<uint32_t, 5> h_{};
    std_array<uint32_t, 4> pad_{};
    data_array<block_size> buffer_{};
    size_t offset_{};
};

} // namespace system
} // namespace libbitcoin

#endif
