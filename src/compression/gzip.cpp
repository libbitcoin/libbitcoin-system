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
#include <bitcoin/system/compression/gzip.hpp>

#include <algorithm>
#include <bitcoin/system/compression/deflate.hpp>
#include <bitcoin/system/compression/inflate.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include "framing.hpp"

// based on:
// rfc-editor.org/rfc/rfc1952.txt

namespace libbitcoin {
namespace system {

// Member identification and compression method (rfc1952 2.3.1).
constexpr uint8_t first_id = 0x1f;
constexpr uint8_t second_id = 0x8b;
constexpr uint8_t deflate_method = 8;

// Flags (rfc1952 2.3.1).
constexpr uint8_t header_crc_flag = 0x02;
constexpr uint8_t extra_flag = 0x04;
constexpr uint8_t name_flag = 0x08;
constexpr uint8_t comment_flag = 0x10;
constexpr uint8_t reserved_flags = 0xe0;

// No flags, no modification time, no extra flags, unknown file system.
constexpr uint8_t no_flags = 0x00;
constexpr uint8_t no_time = 0x00;
constexpr uint8_t no_extra_flags = 0x00;
constexpr uint8_t unknown_system = 0xff;

constexpr size_t header_size = 10;
constexpr size_t trailer_size = 2u * sizeof(uint32_t);

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Member header.
// ----------------------------------------------------------------------------

enum class parsed : uint8_t
{
    found,
    more,
    invalid
};

// Advance past a zero terminated string (rfc1952 2.3.1).
static bool skip_string(size_t& position, const data_chunk& bytes) NOEXCEPT
{
    const auto begin = std::next(bytes.begin(), position);
    const auto it = std::find(begin, bytes.end(), 0_u8);
    if (it == bytes.end())
        return false;

    position += add1(sign_cast<size_t>(std::distance(begin, it)));
    return true;
}

// The size of the member header that begins bytes (rfc1952 2.3.1).
static parsed parse_header(size_t& size, const data_chunk& bytes) NOEXCEPT
{
    if (bytes.size() < header_size)
        return parsed::more;

    const auto flags = bytes[3];
    if (bytes[0] != first_id || bytes[1] != second_id ||
        bytes[2] != deflate_method || is_nonzero(bit_and(flags, reserved_flags)))
        return parsed::invalid;

    auto position = header_size;
    if (is_nonzero(bit_and(flags, extra_flag)))
    {
        if (bytes.size() - position < sizeof(uint16_t))
            return parsed::more;

        const size_t length = unsafe_from_little_endian<uint16_t>(
            std::next(bytes.data(), position));

        position += sizeof(uint16_t);
        if (bytes.size() - position < length)
            return parsed::more;

        position += length;
    }

    if (is_nonzero(bit_and(flags, name_flag)) && !skip_string(position, bytes))
        return parsed::more;

    if (is_nonzero(bit_and(flags, comment_flag)) &&
        !skip_string(position, bytes))
        return parsed::more;

    if (is_nonzero(bit_and(flags, header_crc_flag)))
    {
        if (bytes.size() - position < sizeof(uint16_t))
            return parsed::more;

        const auto begin = bytes.data();
        const auto end = std::next(begin, position);
        const auto check = narrow_cast<uint16_t>(crc32(data_slice(begin, end)));
        if (unsafe_from_little_endian<uint16_t>(end) != check)
            return parsed::invalid;

        position += sizeof(uint16_t);
    }

    size = position;
    return parsed::found;
}

// gzip_deflater
// ----------------------------------------------------------------------------

void gzip_deflater::start(data_chunk& out) NOEXCEPT
{
    if (started_)
        return;

    out.insert(out.end(),
    {
        first_id, second_id, deflate_method, no_flags,
        no_time, no_time, no_time, no_time,
        no_extra_flags, unknown_system
    });

    started_ = true;
}

void gzip_deflater::write(data_chunk& out, const data_slice& data) NOEXCEPT
{
    start(out);
    check_ = crc32(data, check_);
    size_ = possible_narrow_cast<uint32_t>(size_ + data.size());
    deflater_.write(out, data);
}

void gzip_deflater::flush(data_chunk& out) NOEXCEPT
{
    start(out);
    deflater_.flush(out);
    const auto check = to_little_endian(check_);
    const auto size = to_little_endian(size_);
    out.insert(out.end(), check.begin(), check.end());
    out.insert(out.end(), size.begin(), size.end());
    *this = gzip_deflater{};
}

// gzip_inflater
// ----------------------------------------------------------------------------

bool gzip_inflater::done() const NOEXCEPT
{
    return !is_zero(members_) && state_ == state::header && pending_.empty();
}

bool gzip_inflater::write(data_chunk& out, size_t& consumed,
    const data_slice& in, size_t limit) NOEXCEPT
{
    consumed = zero;
    const auto start = out.size();
    while (true)
    {
        switch (state_)
        {
            case state::header:
            {
                if (consumed == in.size())
                    return true;

                size_t size{};
                const auto buffered = pending_.size();
                pending_.insert(pending_.end(), std::next(in.begin(),
                    consumed), in.end());

                const auto result = parse_header(size, pending_);
                if (result == parsed::invalid)
                    return false;

                if (result == parsed::more)
                {
                    consumed = in.size();
                    return true;
                }

                consumed += size - buffered;
                pending_.clear();
                state_ = state::body;
                break;
            }
            case state::body:
            {
                const auto grown = out.size() - start;
                if (!is_zero(limit) && grown >= limit)
                    return true;

                size_t used{};
                const auto begin = out.size();
                const auto rest = data_slice(std::next(in.begin(), consumed),
                    in.end());

                if (!inflater_.write(out, used, rest,
                    remaining_of(limit, grown)))
                    return false;

                consumed += used;
                const auto produced = std::next(out.begin(), begin);
                check_ = crc32(data_slice(produced, out.end()), check_);
                size_ = possible_narrow_cast<uint32_t>(size_ + out.size() -
                    begin);

                if (!inflater_.done())
                {
                    if (used == rest.size())
                        return true;

                    break;
                }

                state_ = state::trailer;
                break;
            }
            case state::trailer:
            {
                if (!gather(pending_, consumed, in, trailer_size))
                    return true;

                const auto data = pending_.data();
                const auto end = std::next(data, sizeof(uint32_t));
                if (unsafe_from_little_endian<uint32_t>(data) != check_ ||
                    unsafe_from_little_endian<uint32_t>(end) != size_)
                    return false;

                ++members_;
                pending_.clear();
                inflater_ = inflater{};
                check_ = zero;
                size_ = zero;
                state_ = state::header;
                break;
            }
        }
    }
}

// One-shot.
// ----------------------------------------------------------------------------

data_chunk gzip_deflate(const data_slice& in) NOEXCEPT
{
    data_chunk out{};
    gzip_deflater encoder{};
    encoder.write(out, in);
    encoder.flush(out);
    return out;
}

bool gzip_inflate(data_chunk& out, const data_slice& in, size_t limit) NOEXCEPT
{
    out.clear();
    size_t consumed{};
    gzip_inflater decoder{};
    if (!decoder.write(out, consumed, in, pause_of(limit)) ||
        !decoder.done() || consumed != in.size())
        return false;

    return is_zero(limit) || out.size() <= limit;
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
