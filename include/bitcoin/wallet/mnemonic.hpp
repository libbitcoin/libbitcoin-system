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
#ifndef LIBBITCOIN_MNEMONIC_HPP
#define LIBBITCOIN_MNEMONIC_HPP

#include <string>
#include <vector>
#include <bitcoin/define.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {

typedef std::vector<std::string> string_list;

BC_API string_list encode_mnemonic(const data_chunk& seed);
BC_API const std::string decode_mnemonic(const string_list& words);

} // namespace libbitcoin

#endif

