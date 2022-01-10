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
#ifndef LIBBITCOIN_SYSTEM_RADIX_BASE_58_IPP
#define LIBBITCOIN_SYSTEM_RADIX_BASE_58_IPP

#include <algorithm>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

template <size_t Size>
bool decode_base58(data_array<Size>& out, const std::string& in) noexcept
{
    data_chunk data;
    if (!decode_base58(data, in) || (data.size() != Size))
        return false;

    std::copy(data.begin(), data.end(), out.begin());
    return true;
}

// TODO: determine if the sizing function is always accurate.
template <size_t Size>
data_array<Size * 733 / 1000> base58_array(const char(&string)[Size]) noexcept
{
    // log(58) / log(256), rounded up.
    data_array<Size * 733 / 1000> out;
    if (!decode_base58(out, string))
        out.fill(0);

    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
