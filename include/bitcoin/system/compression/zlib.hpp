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
#ifndef LIBBITCOIN_SYSTEM_COMPRESSION_ZLIB_HPP
#define LIBBITCOIN_SYSTEM_COMPRESSION_ZLIB_HPP

#include <bitcoin/system/compression/deflate.hpp>
#include <bitcoin/system/compression/inflate.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Not thread safe.
/// zlib (rfc1950) encoder of a stream written in parts.
class BC_API zlib_deflater
{
public:
    /// Encode data, appending completed output to out.
    void write(data_chunk& out, const data_slice& data) NOEXCEPT;

    /// Encode the remainder and end the stream, appending to out, after
    /// which another stream may be written.
    void flush(data_chunk& out) NOEXCEPT;

private:
    void start(data_chunk& out) NOEXCEPT;

    deflater deflater_{};
    uint32_t check_{ 1 };
    bool started_{};
};

/// Not thread safe.
/// zlib (rfc1950) decoder of a stream received in parts.
class BC_API zlib_inflater
{
public:
    /// Decode from in, appending to out, until in is consumed, the stream
    /// ends, or out has grown by at least limit (zero is unlimited), setting
    /// the bytes of in consumed. False if the stream is invalid, or has a
    /// preset dictionary.
    bool write(data_chunk& out, size_t& consumed, const data_slice& in,
        size_t limit=zero) NOEXCEPT;

    /// The stream (with its check) has ended.
    bool done() const NOEXCEPT;

private:
    enum class state : uint8_t
    {
        header,
        body,
        trailer,
        done
    };

    inflater inflater_{};
    data_chunk pending_{};
    uint32_t check_{ 1 };
    state state_{ state::header };
};

/// zlib (rfc1950) stream of the deflate of the input.
BC_API data_chunk zlib_deflate(const data_slice& in) NOEXCEPT;

/// Decode of a zlib (rfc1950) stream that is the entire input. False if
/// invalid, preset dictionary, or the output would exceed limit (zero is
/// unlimited).
BC_API bool zlib_inflate(data_chunk& out, const data_slice& in,
    size_t limit=zero) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
