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
#ifndef LIBBITCOIN__MINI_KEYS_HPP
#define LIBBITCOIN__MINI_KEYS_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <math_ec_secret.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

/**
 * Convert Cascasius minikey to secret parameter.
 */
BC_API bool minikey_to_secret(math_ec_secret& out_secret, const std::string& key);

} // namespace api
//} // namespace wallet
} // namespace libbitcoin

#endif
