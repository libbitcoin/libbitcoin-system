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
#include <bitcoin/system/compression/zlib.hpp>

#include <bitcoin/system/compression/deflate.hpp>
#include <bitcoin/system/compression/inflate.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include "framing.hpp"

// based on:
// rfc-editor.org/rfc/rfc1950.txt

namespace libbitcoin {
namespace system {

// Deflate with a 32K window, default level, no dictionary (rfc1950 2.2).
constexpr uint8_t method_info = 0x78;
constexpr uint8_t level_flags = 0x9c;

constexpr uint8_t deflate_method = 8;
constexpr uint8_t method_mask = 0x0f;
constexpr size_t info_shift = 4;
constexpr uint8_t maximum_info = 7;
constexpr uint8_t dictionary_flag = 0x20;
constexpr size_t check_modulus = 31;
constexpr size_t header_size = 2;
constexpr size_t trailer_size = sizeof(uint32_t);

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// zlib_deflater
// ----------------------------------------------------------------------------

void zlib_deflater::start(data_chunk& out) NOEXCEPT
{
    if (started_)
        return;

    out.push_back(method_info);
    out.push_back(level_flags);
    started_ = true;
}

void zlib_deflater::write(data_chunk& out, const data_slice& data) NOEXCEPT
{
    start(out);
    check_ = adler32(data, check_);
    deflater_.write(out, data);
}

void zlib_deflater::flush(data_chunk& out) NOEXCEPT
{
    start(out);
    deflater_.flush(out);
    const auto check = to_big_endian(check_);
    out.insert(out.end(), check.begin(), check.end());
    *this = zlib_deflater{};
}

// zlib_inflater
// ----------------------------------------------------------------------------

bool zlib_inflater::done() const NOEXCEPT
{
    return state_ == state::done;
}

bool zlib_inflater::write(data_chunk& out, size_t& consumed,
    const data_slice& in, size_t limit) NOEXCEPT
{
    consumed = zero;
    const auto start = out.size();
    while (state_ != state::done)
    {
        switch (state_)
        {
            case state::header:
            {
                if (!gather(pending_, consumed, in, header_size))
                    return true;

                const auto method = pending_.front();
                const auto flags = pending_.back();
                const auto check = bit_or<size_t>(shift_left<size_t>(method,
                    byte_bits), flags);

                if (bit_and(method, method_mask) != deflate_method ||
                    shift_right(method, info_shift) > maximum_info ||
                    is_nonzero(bit_and(flags, dictionary_flag)) ||
                    is_nonzero(check % check_modulus))
                    return false;

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
                check_ = adler32(data_slice(produced, out.end()), check_);
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

                if (unsafe_from_big_endian<uint32_t>(pending_.data()) !=
                    check_)
                    return false;

                pending_.clear();
                state_ = state::done;
                break;
            }
            case state::done:
            default:
                break;
        }
    }

    return true;
}

// One-shot.
// ----------------------------------------------------------------------------

data_chunk zlib_deflate(const data_slice& in) NOEXCEPT
{
    data_chunk out{};
    zlib_deflater encoder{};
    encoder.write(out, in);
    encoder.flush(out);
    return out;
}

bool zlib_inflate(data_chunk& out, const data_slice& in, size_t limit) NOEXCEPT
{
    out.clear();
    size_t consumed{};
    zlib_inflater decoder{};
    if (!decoder.write(out, consumed, in, pause_of(limit)) ||
        !decoder.done() || consumed != in.size())
        return false;

    return is_zero(limit) || out.size() <= limit;
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
