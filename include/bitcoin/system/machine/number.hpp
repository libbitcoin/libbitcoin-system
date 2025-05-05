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
// data_chunk methods are constexpr but cannot also be INLINE (priority).
template <size_t Size,
    if_not_greater<Size, sizeof(int64_t)> = true>
class integer
{
public:
    typedef signed_type<Size> Integer;

    static inline constexpr bool from_integer(Integer& out, int64_t vary) NOEXCEPT;
    static inline bool from_chunk(Integer& out, const data_chunk& vary) NOEXCEPT;

protected:
    static inline bool strict_zero(const data_chunk& vary) NOEXCEPT;
    static inline bool is_overflow(const data_chunk& vary) NOEXCEPT;
    static inline constexpr bool is_overflow(int64_t value) NOEXCEPT;
};

class BC_API chunk
{
public:
    static inline data_chunk from_bool(bool vary) NOEXCEPT;
    static inline data_chunk from_integer(int64_t vary) NOEXCEPT;
};

class BC_API boolean
{
public:
    template <size_t Size = sizeof(int64_t),
        if_not_greater<Size, sizeof(int64_t)> = true>
    static inline constexpr signed_type<Size> to_integer(bool vary) NOEXCEPT;

    static inline bool from_chunk(const data_chunk& vary) NOEXCEPT;
    static inline bool strict_from_chunk(const data_chunk& vary) NOEXCEPT;
    static inline constexpr bool to_bool(int64_t vary) NOEXCEPT;

protected:
    static inline bool strict_false(const data_chunk& vary) NOEXCEPT;
    static inline constexpr bool is_sign_byte(uint8_t byte) NOEXCEPT;
};

} // namespace number
} // namespace machine
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <size_t Size, if_not_greater<Size, sizeof(int64_t)> If>
#define CLASS integer<Size, If>

#include <bitcoin/system/impl/machine/number.ipp>

#undef CLASS
#undef TEMPLATE

#endif
