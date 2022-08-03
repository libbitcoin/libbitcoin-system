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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_NUMBER_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_NUMBER_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {
namespace number {

// Size constraint guards from_little_endian.
template <size_t Size,
    if_not_greater<Size, sizeof(int64_t)> = true>
class integer
{
public:
    typedef signed_type<Size> Integer;

    INLINE static constexpr bool from_integer(Integer& out,
        int64_t vary) NOEXCEPT;
    INLINE static VCONSTEXPR bool from_chunk(Integer& out,
        const data_chunk& vary) NOEXCEPT;

protected:
    INLINE static VCONSTEXPR bool strict_zero(const data_chunk& vary) NOEXCEPT;
    INLINE static VCONSTEXPR bool is_overflow(const data_chunk& vary) NOEXCEPT;
    INLINE static constexpr bool is_overflow(int64_t value) NOEXCEPT;
};

class BC_API chunk
{
public:
    INLINE static VCONSTEXPR data_chunk from_bool(bool vary) NOEXCEPT;
    INLINE static VCONSTEXPR data_chunk from_integer(int64_t vary) NOEXCEPT;
};

class BC_API boolean
{
public:
    template <size_t Size = sizeof(int64_t),
        if_not_greater<Size, sizeof(int64_t)> = true>
    INLINE static constexpr signed_type<Size> to_integer(bool vary) NOEXCEPT;

    INLINE static VCONSTEXPR bool from_chunk(const data_chunk& vary) NOEXCEPT;
    INLINE static VCONSTEXPR bool strict_from_chunk(
        const data_chunk& vary) NOEXCEPT;
    INLINE static constexpr bool to_bool(int64_t vary) NOEXCEPT;

protected:
    INLINE static VCONSTEXPR bool strict_false(const data_chunk& vary) NOEXCEPT;
    INLINE static constexpr bool is_sign_byte(uint8_t byte) NOEXCEPT;
};

} // namespace number
} // namespace machine
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/machine/number.ipp>

#endif
