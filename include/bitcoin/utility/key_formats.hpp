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
#ifndef LIBBITCOIN_KEY_FORMATS_HPP
#define LIBBITCOIN_KEY_FORMATS_HPP

#include <string>

#include <bitcoin/utility/elliptic_curve_key.hpp>

namespace libbitcoin {

/**
 * Convert a secret parameter to the wallet import format.
 * Returns an empty string on error.
 *
 * @code
 *  std::string wif = secret_to_wif(secret);
 *  if (wif.empty())
 *      // Error...
 * @endcode
 */
std::string secret_to_wif(const secret_parameter& secret);

/**
 * Convert wallet import format key to secret parameter.
 * Returns a nulled secret on error.
 *
 * @code
 *  secret_parameter secret = wif_to_secret(
 *      "5HueCGU8rMjxEXxiPuD5BDku4MkFqeZyd4dZ1jvhTVqvbTLvyTJ");
 *  if (secret == null_hash)
 *      // Error...
 * @endcode
 */
secret_parameter wif_to_secret(const std::string& wif);

/**
 * Convert Cascasius minikey to secret parameter.
 * Returns a nulled secret on error.
 *
 * @code
 *  secret_parameter secret =
 *      minikey_to_secret("S6c56bnXQiBjk9mqSYE7ykVQ7NzrRy");
 *  if (secret == null_hash)
 *      // Error...
 * @endcode
 */
secret_parameter minikey_to_secret(const std::string& minikey);

} // libbitcoin

#endif

