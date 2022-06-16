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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <execution>
#include <iterator>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace machine {
namespace number {

static constexpr uint8_t positive_sign_byte = 0x00;
static constexpr uint8_t negative_sign_byte = bit_left<uint8_t>(0);

template <size_t Size = sizeof(int64_t),
    if_not_greater<Size, sizeof(int64_t)> = true>
class integer
{
public:
    typedef signed_type<Size> Integer;

    static constexpr bool from_int(Integer& out, int64_t vary) noexcept
    {
        out = possible_narrow_cast<Integer>(vary);

        // Disallows integers that exceed overflow constraint.
        return !is_overflow(vary);
    }

    static inline bool from_chunk(Integer& out, const data_chunk& vary) noexcept
    {
        out = 0;

        if (strict_zero(vary))
            return true;

        // Disallows integers that exceed overflow constraint.
        if (vary.size() > Size)
            return false;

        // TODO: optimize endianness conversion (and make constexpr).
        out = from_little_endian<Integer>(vary);

        if (get_left(vary.back()))
            out = -set_right(out, sub1(to_bits(vary.size())), false);

        return true;
    }

protected:
    static constexpr bool strict_zero(const data_chunk& vary) noexcept
    {
        return vary.empty();
    }

    static constexpr bool is_overflow(int64_t value) noexcept
    {
        // Encoding of negative zero constricts the integer domain by one.
        return is_limited(value, bitcoin_min<Size>(), bitcoin_max<Size>());
    }
};

class chunk
{
public:
    static inline data_chunk from_bool(bool vary) noexcept
    {
        return { bc::to_int<uint8_t>(vary) };
    }

    static inline data_chunk from_int(int64_t vary) noexcept
    {
        if (is_zero(vary))
            return {};

        ////const auto size = byte_width(absolute(vary));
        ////data_chunk bytes(size + one, 0x00, no_fill_byte_allocator);

        // TODO: optimize endianness conversion (and make constexpr).
        auto bytes = to_little_endian_chunk(absolute(vary), one);

        const auto hi_bit_set = get_left(bytes.back());

        // C++20: vector construction and push_back/back are constexpr!
        if (hi_bit_set && is_negative(vary))
            bytes.push_back(negative_sign_byte);
        else if (hi_bit_set)
            bytes.push_back(positive_sign_byte);
        else if (is_negative(vary))
            set_left_into(bytes.back());

        ////if (!hi_bit_set)
        ////    bytes.resize(size);

        return bytes;
    }
};

class boolean
{
public:
    static constexpr bool to_bool(int64_t vary) noexcept
    {
        return bc::to_bool(vary);
    }

    template <size_t Size = sizeof(int64_t),
        if_not_greater<Size, sizeof(int64_t)> = true>
    static constexpr signed_type<Size> to_int(bool vary) noexcept
    {
        // This just casts one bit to the integer domain as convenience.
        return bc::to_int<signed_type<Size>>(vary);
    }

    static constexpr bool from_chunk(const data_chunk& vary) noexcept
    {
        // Booleans not overflow constrained, so cannot be converted as int.
        // Any length of bytes is considered valid. Negative zero is false.

        if (strict_false(vary))
            return false;
        if (!is_sign_byte(vary.back()))
            return true;
        return std::any_of(std::execution::par_unseq, vary.begin(),
            std::prev(vary.end()), is_nonzero<uint8_t>);
    }

    static constexpr bool strict_from_chunk(const data_chunk& vary) noexcept
    {
        // A logical zero equates to any +/- sequence of zero bytes up to 520.
        // Strict bool tests for normal false/zero, or a single empty byte.
        return strict_false(vary);
    }

protected:
    static constexpr bool strict_false(const data_chunk& vary) noexcept
    {
        return vary.empty();
    }

    static constexpr bool is_sign_byte(uint8_t byte) noexcept
    {
        return byte == positive_sign_byte || byte == negative_sign_byte;
    }
};

} // namespace number
} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
