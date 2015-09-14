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
#include <bitcoin/bitcoin/config/ek_private.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>

namespace libbitcoin {
namespace config {

static bool decode_key(wallet::ek_private& key, const std::string& encoded)
{
    return decode_base58(key, encoded) && verify_checksum(key);
}

static std::string encode_key(const wallet::ek_private& key)
{
    return encode_base58(key);
}

ek_private::ek_private()
  : value_()
{
}

ek_private::ek_private(const std::string& base58)
{
    std::stringstream(base58) >> *this;
}

ek_private::ek_private(const wallet::ek_private& value)
  : value_(value)
{
}

ek_private::ek_private(const ek_private& other)
  : ek_private(other.value_)
{
}

wallet::ek_private& ek_private::data()
{
    return value_;
}

ek_private::operator const wallet::ek_private&() const
{
    return value_;
}

std::istream& operator>>(std::istream& input, ek_private& argument)
{
    std::string base58;
    input >> base58;

    if (!decode_key(argument.value_, base58))
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(base58));
    }

    return input;
}

std::ostream& operator<<(std::ostream& output, const ek_private& argument)
{
    output << encode_key(argument.value_);
    return output;
}

} // namespace config
} // namespace libbitcoin
