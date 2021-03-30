/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHECKSUM_IPP
#define LIBBITCOIN_SYSTEM_CHECKSUM_IPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/endian.hpp>

namespace libbitcoin {
namespace system {

template <size_t Size>
byte_array<Size> build_checked_array(
    const std::initializer_list<data_slice>& slices)
{
    auto out = build_array<Size>(slices);
    insert_checksum(out);
    return out;
}

template<size_t Size>
void insert_checksum(byte_array<Size>& out)
{
    static_assert(Size >= checksum_size, "insert_checksum out too small");
    data_chunk payload(out.begin(), out.end() - checksum_size);
    const auto checksum = to_little_endian(bitcoin_checksum(payload));
    std::copy_n(checksum.begin(), checksum_size, out.end() - checksum_size);
}

template <size_t Size>
bool unwrap(uint8_t& out_version, byte_array<UNWRAP_SIZE(Size)>& out_payload,
    const byte_array<Size>& wrapped)
{
    uint32_t unused;
    return unwrap(out_version, out_payload, unused, wrapped);
}

template <size_t Size>
bool unwrap(uint8_t& out_version, byte_array<UNWRAP_SIZE(Size)>& out_payload,
    uint32_t& out_checksum, const byte_array<Size>& wrapped)
{
    if (!verify_checksum(wrapped))
        return false;

    out_version = slice<0, 1>(wrapped)[0];
    out_payload = slice<1, Size - checksum_size>(wrapped);
    const auto bytes = slice<Size - checksum_size, Size>(wrapped);
    out_checksum = from_little_endian_unsafe<uint32_t>(bytes.begin());
    return true;
}

// std::array<> is used in place of byte_array<> to enable Size deduction.
template <size_t Size>
byte_array<WRAP_SIZE(Size)> wrap(uint8_t version,
    const byte_array<Size>& payload)
{
    auto out = build_array<WRAP_SIZE(Size)>({ to_array(version), payload });
    insert_checksum(out);
    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
