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
#ifndef LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_INFLATE_HPP
#define LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_INFLATE_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// rfc-editor.org/rfc/rfc1951.txt

namespace libbitcoin {
namespace system {
namespace secp256k1 {
namespace cuda {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

/// Raw deflate decoder (rfc1951) into output of the exact inflated size.
class inflater
{
public:
    inflater(const data_slice& in, std::string& out) NOEXCEPT
      : in_{ in }, out_{ out }
    {
    }

    /// True if the stream is valid, consumed, and exactly fills the output.
    bool inflate() NOEXCEPT
    {
        auto last = false;
        while (!last)
        {
            uint32_t final{};
            uint32_t type{};
            if (!take(final, one) || !take(type, two))
                return false;

            last = is_one(final);
            switch (type)
            {
                case stored_block:
                {
                    if (!stored())
                        return false;

                    break;
                }
                case fixed_block:
                {
                    if (!fixed())
                        return false;

                    break;
                }
                case dynamic_block:
                {
                    if (!dynamic())
                        return false;

                    break;
                }
                default:
                {
                    return false;
                }
            }
        }

        return position_ == in_.size() && written_ == out_.size();
    }

private:
    static constexpr uint32_t stored_block = 0;
    static constexpr uint32_t fixed_block = 1;
    static constexpr uint32_t dynamic_block = 2;
    static constexpr size_t maximum_bits = 15;
    static constexpr size_t literal_codes = 288;
    static constexpr size_t literal_limit = 286;
    static constexpr size_t distance_codes = 30;
    static constexpr size_t length_codes = 19;
    static constexpr uint16_t end_of_block = 256;
    static constexpr uint16_t first_length = 257;
    static constexpr uint16_t repeat_previous = 16;

    struct huffman
    {
        std_array<uint16_t, add1(maximum_bits)> count{};
        std_array<uint16_t, literal_codes> symbol{};
    };

    // Lengths 257..285 (rfc1951 3.2.5).
    static constexpr std_array<uint16_t, 29> length_base
    {
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51,
        59, 67, 83, 99, 115, 131, 163, 195, 227, 258
    };

    static constexpr std_array<uint8_t, 29> length_extra
    {
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,
        4, 5, 5, 5, 5, 0
    };

    // Distances 0..29 (rfc1951 3.2.5).
    static constexpr std_array<uint16_t, distance_codes> distance_base
    {
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385,
        513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385,
        24577
    };

    static constexpr std_array<uint8_t, distance_codes> distance_extra
    {
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
        10, 11, 11, 12, 12, 13, 13
    };

    // Code length repeat extra bits and minimums (rfc1951 3.2.7).
    static constexpr std_array<uint8_t, 3> repeat_extra{ 2, 3, 7 };
    static constexpr std_array<uint8_t, 3> repeat_base{ 3, 3, 11 };

    // Order of the code length code lengths (rfc1951 3.2.7).
    static constexpr std_array<uint8_t, length_codes> length_order
    {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
    };

    // Fixed literal code lengths by symbol range (rfc1951 3.2.6).
    static constexpr std_array<uint16_t, 4> fixed_ends{ 144, 256, 280, 288 };
    static constexpr std_array<uint8_t, 4> fixed_lengths{ 8, 9, 7, 8 };
    static constexpr uint8_t fixed_distance_length = 5;

    // Bits are taken from the low end of each byte (rfc1951 3.1.1).
    bool take(uint32_t& out, size_t count) NOEXCEPT
    {
        while (bit_count_ < count)
        {
            if (position_ == in_.size())
                return false;

            const auto byte = wide_cast<uint32_t>(in_[position_++]);
            buffer_ = bit_or(buffer_, shift_left(byte, bit_count_));
            bit_count_ += byte_bits;
        }

        out = bit_and(buffer_, unmask_right<uint32_t>(count));
        buffer_ = shift_right(buffer_, count);
        bit_count_ -= count;
        return true;
    }

    // Uncompressed block, byte aligned (rfc1951 3.2.4).
    bool stored() NOEXCEPT
    {
        buffer_ = zero;
        bit_count_ = zero;

        uint32_t length{};
        uint32_t complement{};
        if (!take(length, bits<uint16_t>) ||
            !take(complement, bits<uint16_t>) ||
            length != bit_xor(complement, wide_cast<uint32_t>(max_uint16)))
            return false;

        if (length > in_.size() - position_ || length > out_.size() - written_)
            return false;

        for (; !is_zero(length); --length)
            out_[written_++] = possible_sign_cast<char>(in_[position_++]);

        return true;
    }

    // Canonical code from lengths, false if over-subscribed (rfc1951 3.2.2).
    static bool build(huffman& out, const uint8_t* lengths,
        size_t size) NOEXCEPT
    {
        out = {};
        for (size_t symbol = 0; symbol < size; ++symbol)
            ++out.count[lengths[symbol]];

        out.count[zero] = zero;
        size_t left = one;
        for (size_t length = one; length <= maximum_bits; ++length)
        {
            left = shift_left(left);
            if (out.count[length] > left)
                return false;

            left -= out.count[length];
        }

        std_array<uint16_t, add1(maximum_bits)> offsets{};
        for (size_t length = one; length < maximum_bits; ++length)
            offsets[add1(length)] = offsets[length] + out.count[length];

        for (size_t symbol = 0; symbol < size; ++symbol)
            if (is_nonzero(lengths[symbol]))
                out.symbol[offsets[lengths[symbol]]++] =
                    possible_narrow_cast<uint16_t>(symbol);

        return true;
    }

    // Codes are read most significant bit first (rfc1951 3.1.1).
    bool decode(uint16_t& out, const huffman& code) NOEXCEPT
    {
        size_t value{};
        size_t first{};
        size_t index{};
        for (size_t length = one; length <= maximum_bits; ++length)
        {
            uint32_t bit{};
            if (!take(bit, one))
                return false;

            value = bit_or<size_t>(value, bit);
            const size_t count = code.count[length];
            if (value < first + count)
            {
                out = code.symbol[index + value - first];
                return true;
            }

            index += count;
            first = shift_left(first + count);
            value = shift_left(value);
        }

        return false;
    }

    // Literals and length/distance pairs (rfc1951 3.2.5).
    bool codes(const huffman& literals, const huffman& distances) NOEXCEPT
    {
        while (true)
        {
            uint16_t symbol{};
            if (!decode(symbol, literals))
                return false;

            if (symbol < end_of_block)
            {
                if (written_ == out_.size())
                    return false;

                const auto byte = narrow_cast<uint8_t>(symbol);
                out_[written_++] = possible_sign_cast<char>(byte);
                continue;
            }

            if (symbol == end_of_block)
                return true;

            const size_t index = symbol - first_length;
            if (index >= length_base.size())
                return false;

            uint32_t extra{};
            if (!take(extra, length_extra[index]))
                return false;

            const size_t length = length_base[index] + extra;
            if (!decode(symbol, distances) || symbol >= distance_codes ||
                !take(extra, distance_extra[symbol]))
                return false;

            const size_t distance = distance_base[symbol] + extra;
            if (distance > written_ || length > out_.size() - written_)
                return false;

            for (size_t byte = 0; byte < length; ++byte, ++written_)
                out_[written_] = out_[written_ - distance];
        }
    }

    // Fixed Huffman codes (rfc1951 3.2.6).
    bool fixed() NOEXCEPT
    {
        std_array<uint8_t, literal_codes> lengths{};
        size_t start{};
        for (size_t range = 0; range < fixed_ends.size(); ++range)
        {
            const auto end = fixed_ends[range];
            std::fill(std::next(lengths.begin(), start),
                std::next(lengths.begin(), end), fixed_lengths[range]);
            start = end;
        }

        std_array<uint8_t, distance_codes> distance{};
        distance.fill(fixed_distance_length);

        huffman literals{};
        huffman distances{};
        build(literals, lengths.data(), lengths.size());
        build(distances, distance.data(), distance.size());
        return codes(literals, distances);
    }

    // Dynamic Huffman codes (rfc1951 3.2.7).
    bool dynamic() NOEXCEPT
    {
        uint32_t literal_count{};
        uint32_t distance_count{};
        uint32_t length_count{};
        if (!take(literal_count, 5) || !take(distance_count, 5) ||
            !take(length_count, 4))
            return false;

        literal_count += first_length;
        distance_count += one;
        length_count += 4u;
        if (literal_count > literal_limit || distance_count > distance_codes)
            return false;

        std_array<uint8_t, literal_codes + distance_codes> lengths{};
        for (size_t index = 0; index < length_count; ++index)
        {
            uint32_t length{};
            if (!take(length, 3))
                return false;

            lengths[length_order[index]] = narrow_cast<uint8_t>(length);
        }

        huffman code_lengths{};
        if (!build(code_lengths, lengths.data(), length_codes))
            return false;

        lengths.fill(0_u8);
        const size_t total = literal_count + distance_count;
        for (size_t index = 0; index < total;)
        {
            uint16_t symbol{};
            if (!decode(symbol, code_lengths))
                return false;

            if (symbol < repeat_previous)
            {
                lengths[index++] = narrow_cast<uint8_t>(symbol);
                continue;
            }

            if (symbol == repeat_previous && is_zero(index))
                return false;

            const auto value = (symbol == repeat_previous) ?
                lengths[sub1(index)] : 0_u8;

            const size_t kind = symbol - repeat_previous;
            uint32_t repeat{};
            if (!take(repeat, repeat_extra[kind]))
                return false;

            repeat += repeat_base[kind];
            if (repeat > total - index)
                return false;

            for (; !is_zero(repeat); --repeat)
                lengths[index++] = value;
        }

        if (is_zero(lengths[end_of_block]))
            return false;

        huffman literals{};
        huffman distances{};
        const auto distance_lengths = std::next(lengths.data(), literal_count);
        if (!build(literals, lengths.data(), literal_count) ||
            !build(distances, distance_lengths, distance_count))
            return false;

        return codes(literals, distances);
    }

    const data_slice in_;
    std::string& out_;
    size_t position_{};
    size_t written_{};
    uint32_t buffer_{};
    size_t bit_count_{};
};

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace cuda
} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
