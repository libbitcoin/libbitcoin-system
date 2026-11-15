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
#ifndef LIBBITCOIN_SYSTEM_COMPRESSION_INFLATE_HPP
#define LIBBITCOIN_SYSTEM_COMPRESSION_INFLATE_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Not thread safe.
/// Raw deflate (rfc1951) decoder of a stream received in parts. Memory is
/// bounded by the window, independent of the stream size.
class BC_API inflater
{
public:
    /// Code space (rfc1951 3.2.5, 3.2.7).
    static constexpr size_t maximum_bits = 15;
    static constexpr size_t literal_codes = 288;
    static constexpr size_t distance_codes = 30;

    /// Lookup width of the root decoding table.
    static constexpr size_t root_bits = 9;

    /// Canonical code and its root decoding table.
    struct huffman
    {
        std_array<uint16_t, add1(maximum_bits)> count{};
        std_array<uint16_t, literal_codes> symbol{};
        std_array<uint16_t, power2(root_bits)> table{};
    };

    DEFAULT_COPY_MOVE_DESTRUCT(inflater);

    inflater() NOEXCEPT;

    /// Decode from in, appending to out, until in is consumed, the stream
    /// ends, or out has grown by at least limit (zero is unlimited), setting
    /// the bytes of in consumed. False if the stream is invalid.
    bool write(data_chunk& out, size_t& consumed, const data_slice& in,
        size_t limit=zero) NOEXCEPT;

    /// The final block has been decoded.
    bool done() const NOEXCEPT;

private:
    enum class state : uint8_t
    {
        header,
        stored_length,
        stored_copy,
        table,
        code_lengths,
        lengths,
        codes,
        done
    };

    enum class step : uint8_t
    {
        next,
        more,
        invalid
    };

    bool pull() NOEXCEPT;
    void refill() NOEXCEPT;
    void restore() NOEXCEPT;
    void drop(size_t bits) NOEXCEPT;
    void copy(size_t size, size_t distance) NOEXCEPT;
    step header() NOEXCEPT;
    step stored_length() NOEXCEPT;
    step stored_copy() NOEXCEPT;
    step table() NOEXCEPT;
    step code_lengths() NOEXCEPT;
    step lengths() NOEXCEPT;
    step codes() NOEXCEPT;
    bool full() const NOEXCEPT;

    // Window of history (with output of the current write).
    data_chunk window_{};

    // Input of the current write.
    const uint8_t* in_{};
    size_t size_{};
    size_t position_{};
    size_t produced_{};
    size_t limit_{};

    // Bits taken from input and not yet decoded.
    uint64_t buffer_{};
    size_t bit_count_{};

    // Decoding state.
    state state_{ state::header };
    bool last_{};
    size_t remaining_{};
    size_t literal_count_{};
    size_t distance_count_{};
    size_t length_count_{};
    size_t index_{};
    std_array<uint8_t, literal_codes + distance_codes> lengths_{};
    huffman code_lengths_{};
    huffman literals_{};
    huffman distances_{};
};

/// Raw deflate (rfc1951) decode of the entire input. False if invalid, not
/// entirely consumed, or the output would exceed limit (zero is unlimited).
BC_API bool inflate(data_chunk& out, const data_slice& in,
    size_t limit=zero) NOEXCEPT;

/// Raw deflate (rfc1951) decode of the stream that begins the input, setting
/// the byte size of the stream. False if invalid, or the output would exceed
/// limit (zero is unlimited).
BC_API bool inflate(data_chunk& out, size_t& consumed, const data_slice& in,
    size_t limit=zero) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
