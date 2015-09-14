/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-explorer.
 *
 * libbitcoin-explorer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CONFIG_EK_PRIVATE_HPP
#define LIBBITCOIN_CONFIG_EK_PRIVATE_HPP

#include <iostream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

namespace libbitcoin {
namespace config {

/**
 * Serialization helper to convert between base58 and encrypted private key.
 */
class BC_API ek_private
{
public:

    /**
     * Default constructor.
     */
    ek_private();

    /**
     * Initialization constructor.
     * @param[in]  base58  The value to initialize with.
     */
    ek_private(const std::string& base58);

    /**
     * Initialization constructor.
     * @param[in]  key  The value to initialize with.
     */
    ek_private(const wallet::ek_private& key);

    /**
     * Copy constructor.
     * @param[in]  other  The object to copy into self on construct.
     */
    ek_private(const ek_private& other);

    /**
     * Return a reference to the data member.
     * @return  A reference to the object's internal data.
     */
    wallet::ek_private& data();

    /**
     * Overload cast to internal type.
     * @return  This object's value cast to internal type.
     */
    operator const wallet::ek_private&() const;

    /**
     * Overload stream in. Throws if input is invalid.
     * @param[in]   input     The input stream to read the value from.
     * @param[out]  argument  The object to receive the read value.
     * @return                The input stream reference.
     */
    friend std::istream& operator>>(std::istream& input,
        ek_private& argument);

    /**
     * Overload stream out.
     * @param[in]   output    The output stream to write the value to.
     * @param[out]  argument  The object from which to obtain the value.
     * @return                The output stream reference.
     */
    friend std::ostream& operator<<(std::ostream& output,
        const ek_private& argument);

private:

    /**
     * The state of this object.
     */
    wallet::ek_private value_;
};

} // namespace config
} // namespace libbitcoin

#endif
