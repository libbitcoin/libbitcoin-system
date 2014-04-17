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
#ifndef LIBBITCOIN_SHA_HPP
#define LIBBITCOIN_SHA_HPP

#include <cstdint>
#include <bitcoin/define.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/utility/external/ripemd160.h>
#include <bitcoin/utility/external/sha1.h>
#include <bitcoin/utility/external/sha256.h>

namespace libbitcoin {

//BC_API short_hash generate_ripemd160_hash(const data_chunk& chunk);
//BC_API short_hash generate_sha1_hash(const data_chunk& chunk);
//BC_API hash_digest generate_sha256_hash(const data_chunk& chunk);
BC_API short_hash generate_ripemd160_on_sha256_hash(const data_chunk& chunk);
BC_API hash_digest generate_sha256_on_sha256_hash(const data_chunk& chunk);
BC_API uint32_t generate_sha256_on_sha256_checksum(const data_chunk& chunk);

} // namespace libbitcoin

#endif

