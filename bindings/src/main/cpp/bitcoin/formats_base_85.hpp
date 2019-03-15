/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__FORMATS_BASE_85_HPP
#define LIBBITCOIN__FORMATS_BASE_85_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <utility_data_slice.hpp>
#include <utility_data_chunk.hpp>
#include <p_std_string.hpp>

namespace libbitcoin {
namespace api {

/**
 * Encode data as base85 (Z85).
 * @return false if the input is not of base85 size (% 4).
 */
BC_API bool encode_base85(p_std_string& out, utility_data_slice in);

/**
 * Attempt to decode base85 (Z85) data.
 * @return false if the input contains non-base85 characters or length (% 5).
 */
BC_API bool decode_base85(utility_data_chunk& out, const p_std_string& in);

} // namespace api
} // namespace libbitcoin

#endif
