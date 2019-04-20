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
#ifndef LIBBITCOIN__CONFIG_BASE2_HPP
#define LIBBITCOIN__CONFIG_BASE2_HPP

#include <cstddef>
#include <iostream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/config/base2.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>

namespace libbitcoin {
//namespace config {
namespace api {

/**
 * Serialization helper for base2 encoded data.
 */
class BC_API config_base2
{
public:

    /**
     * Default constructor.
     */
    config_base2();

    /**
     * Initialization constructor.
     * @param[in]  bin  The value to initialize with.
     */
    config_base2(const std::string& binary);

    /**
     * @param[in]  value  The value to initialize with.
     */
    config_base2(const binary& value);

    /**
     * Copy constructor.
     * @param[in]  other  The object to copy into self on construct.
     */
    config_base2(const config_base2& other);

    /**
     * Get number of bits in value.
     */
    size_t size() const;

    /**
     * Overload cast to internal type.
     * @return  This object's value cast to internal type reference.
     */
//    operator const binary&() const;
    const binary& toBinary() const;


    /**
     * Overload stream in. If input is invalid sets no bytes in argument.
     * @param[in]   input     The input stream to read the value from.
     * @param[out]  argument  The object to receive the read value.
     * @return                The input stream reference.
     */
//    friend std::istream& operator>>(std::istream& input, base2& argument);

    /**
     * Overload stream out.
     * @param[in]   output    The output stream to write the value to.
     * @param[out]  argument  The object from which to obtain the value.
     * @return                The output stream reference.
     */
//    friend std::ostream& operator<<(std::ostream& output, const base2& argument);

public:
    config::base2 getValue() {
        return value;
    }

    void setValue(config::base2 value) {
        this->value = value;
    }
private:
    config::base2 value;
//
//    /**
//     * The state of this object.
//     */
//    binary value_;
};

} // namespace api
//} // namespace config
} // namespace libbitcoin

#endif
