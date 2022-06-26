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


#include <cstddef>
#include <cstdint>
////#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {
namespace number {

template <size_t Size>
class integer
{
public:
    typedef signed_type<Size> Integer;
    static_assert(Size <= sizeof(int64_t), "type constraint");

    static inline bool from_int(Integer& out, int64_t vary) noexcept;
    static inline bool from_chunk(Integer& out,
        const data_chunk& vary) noexcept;

protected:
    static inline bool strict_zero(const data_chunk& vary) noexcept;
    static inline bool is_overflow(int64_t value) noexcept;
};

class BC_API chunk
{
public:
    static inline data_chunk from_bool(bool vary) noexcept;
    static inline data_chunk from_int(int64_t vary) noexcept;
};

class BC_API boolean
{
public:
    template <size_t Size = sizeof(int64_t)>
    static inline signed_type<Size> to_int(bool vary) noexcept;

    static inline bool from_chunk(const data_chunk& vary) noexcept;
    static inline bool strict_from_chunk(const data_chunk& vary) noexcept;
    static inline bool to_bool(int64_t vary) noexcept;

protected:
    static inline bool strict_false(const data_chunk& vary) noexcept;
    static inline bool is_sign_byte(uint8_t byte) noexcept;
};

} // namespace number
} // namespace machine
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/machine/number.ipp>

#endif
