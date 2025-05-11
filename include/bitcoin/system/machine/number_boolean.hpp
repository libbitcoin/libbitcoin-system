/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_NUMBER_BOOLEAN_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_NUMBER_BOOLEAN_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {
namespace number {

class BC_API boolean
{
public:
    static constexpr bool from_integer(int64_t vary) NOEXCEPT;
    static constexpr bool from_chunk(const data_chunk& vary) NOEXCEPT;

    /// minimal [bip342], integer must be 0 or 1, chunk must be [] or [0x01].
    static constexpr bool from_integer(bool& value, int64_t vary) NOEXCEPT;
    static constexpr bool from_chunk(bool& value, const data_chunk& vary) NOEXCEPT;

    /// strict [bip147], false must be [].
    static constexpr bool from_chunk_strict(const data_chunk& vary) NOEXCEPT;

protected:
    static constexpr bool strict_false(const data_chunk& vary) NOEXCEPT;
    static constexpr bool is_sign_byte(uint8_t byte) NOEXCEPT;
};

} // namespace number
} // namespace machine
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/machine/number_boolean.ipp>

#endif
