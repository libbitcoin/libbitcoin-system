/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_RIPEMD_HPP
#define LIBBITCOIN_RIPEMD_HPP

#include <bitcoin/define.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/utility/ripemd160.hpp>

namespace libbitcoin {

constexpr size_t ripemd_digest_size = RMD160_DIGEST_LENGTH;

BC_API void RIPEMD160(const uint8_t* data, const uint32_t size, uint8_t* hash);
BC_API short_hash generate_ripemd_hash(const data_chunk& chunk);

} // namespace libbitcoin

#endif

