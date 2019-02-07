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
#ifndef LIBBITCOIN__CONFIG_SCRIPT_HPP
#define LIBBITCOIN__CONFIG_SCRIPT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <bitcoin/chain_script.hpp>
#include <bitcoin/bitcoin/config/script.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/string_vector.hpp>
#include <bitcoin/utility_data_chunk.hpp>

namespace libbitcoin {
namespace config {
namespace api {

/**
 * Serialization helper to convert between base16/raw script and script_type.
 */
class BC_API config_script : public config::script
{
public:

    /**
     * Default constructor.
     */
    config_script();

    /**
     * Initialization constructor.
     * @param[in]  mnemonic  The value to initialize with.
     */
    config_script(const std::string& mnemonic);

    /**
     * Initialization constructor.
     * @param[in]  value  The value to initialize with.
     */
    config_script(const libbitcoin::chain::api::chain_script& value);

    /**
     * Initialization constructor.
     * @param[in]  value  The value to initialize with.
     */
    config_script(const libbitcoin::api::utility_data_chunk& value);

    /**
     * Initialization constructor.
     * @param[in]  tokens  The mnemonic tokens to initialize with.
     */
    config_script(const libbitcoin::api::string_vector& tokens);

    /**
     * Copy constructor.
     * @param[in]  other  The object to copy into self on construct.
     */
    config_script(const config_script& other);

    /**
     * Serialize the script to bytes according to the wire protocol.
     * @return  The byte serialized copy of the script.
     */
    libbitcoin::api::utility_data_chunk to_data() const;

    /**
     * Return a pretty-printed copy of the script.
     * @param[in]  flags  The rule fork flags to use.
     * @return            A mnemonic-printed copy of the internal script.
     */
    std::string to_string(uint32_t flags=machine::rule_fork::all_rules) const;

    /**
     * Overload cast to internal type.
     * @return  This object's value cast to internal type.
     */
//    operator const chain::script&() const;
    const libbitcoin::chain::api::chain_script& to_script() const;

    /**
     * Overload stream in. Throws if input is invalid.
     * @param[in]   input     The input stream to read the value from.
     * @param[out]  argument  The object to receive the read value.
     * @return                The input stream reference.
     */
//    friend std::istream& operator>>(std::istream& input, script& argument);

    /**
     * Overload stream out.
     * @param[in]   output    The output stream to write the value to.
     * @param[out]  argument  The object from which to obtain the value.
     * @return                The output stream reference.
     */
//    friend std::ostream& operator<<(std::ostream& output, const script& argument);

//private:
//
//    /**
//     * The state of this object.
//     */
//    chain::script value_;
};

} // namespace api
} // namespace config
} // namespace libbitcoin

#endif
