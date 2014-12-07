/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_BASE16_IPP
#define LIBBITCOIN_BASE16_IPP

#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

// Private helper:
bool decode_base16_raw(uint8_t* out, size_t out_size, const char* in);

template<size_t Size>
byte_array<(Size - 1) / 2> base16_literal(const char (&string)[Size])
{
    byte_array<(Size - 1) / 2> out;
    BITCOIN_ASSERT(decode_base16_raw(out.data(), out.size(), string));
    return out;
}

} // libbitcoin

#endif

