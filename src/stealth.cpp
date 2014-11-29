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
#include <bitcoin/bitcoin/stealth.hpp>

#include <bitcoin/bitcoin/utility/hash.hpp>

namespace libbitcoin {

binary_type calculate_stealth_prefix(const script_type& stealth_script)
{
    const data_chunk stealth_data = save_script(stealth_script);
    const hash_digest index = bitcoin_hash(stealth_data);
    const size_t bitsize = binary_type::bits_per_block * sizeof(uint32_t);
    return binary_type(bitsize, index);
}

} // namespace libbitcoin

