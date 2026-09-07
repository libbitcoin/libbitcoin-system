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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_PADDING_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_PADDING_IPP

// 5.1 Specification of pad10*1 (with 6.1 domain separation)
// ============================================================================
// The domain byte follows the message, the final rate byte carries the 1.
// When both land on the same byte (tail is one byte short) they are xored.

namespace libbitcoin {
namespace system {
namespace sha3 {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
constexpr typename CLASS::block_t CLASS::
pad(size_t size, const byte_t* tail) NOEXCEPT
{
    // Caller guarantees size <= space.
    block_t block{};
    std::copy_n(tail, size, block.begin());
    block[size] ^= SHA3::domain;
    block[space] ^= 0x80;
    return block;
}

TEMPLATE
template <size_t Size>
constexpr typename CLASS::block_t CLASS::
pad(const bytes_t<Size>& tail) NOEXCEPT
{
    static_assert(Size <= space);
    return pad(Size, tail.data());
}

} // namespace sha3
} // namespace system
} // namespace libbitcoin

#endif
