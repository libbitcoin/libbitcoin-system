/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/config/btc256.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace config {

btc256::btc256()
  : value_(null_hash)
{
}

btc256::btc256(const std::string& hexcode)
  : btc256()
{
    std::stringstream(hexcode) >> *this;
}

btc256::btc256(const hash_digest& value)
  : value_(value)
{
}

btc256::btc256(const btc256& other)
  : btc256(other.value_)
{
}

std::string btc256::to_string() const
{
    std::stringstream value;
    value << *this;
    return value.str();
}

btc256::operator const hash_digest&() const
{
    return value_;
}

std::istream& operator>>(std::istream& input, btc256& argument)
{
    std::string hexcode;
    input >> hexcode;

    if (!decode_hash(argument.value_, hexcode))
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(hexcode));
    }

    return input;
}

std::ostream& operator<<(std::ostream& output, const btc256& argument)
{
    output << encode_hash(argument.value_);
    return output;
}

} // namespace config
} // namespace libbitcoin
