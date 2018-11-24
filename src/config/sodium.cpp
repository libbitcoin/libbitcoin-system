/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/config/sodium.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/formats/base_85.hpp>
#include <bitcoin/system/math/hash.hpp>

namespace libbitcoin {
namespace system {
namespace config {

sodium::sodium()
  : value_(null_hash)
{
}

sodium::sodium(const std::string& base85)
{
    std::stringstream(base85) >> *this;
}

sodium::sodium(const hash_digest& value)
  : value_(value)
{
}

sodium::sodium(const sodium& other)
  : sodium(other.value_)
{
}

sodium::operator const hash_digest&() const
{
    return value_;
}

sodium::operator data_slice() const
{
    return value_;
}

sodium::operator bool() const
{
    return value_ != null_hash;
}

std::string sodium::to_string() const
{
    std::stringstream value;
    value << *this;
    return value.str();
}

std::istream& operator>>(std::istream& input, sodium& argument)
{
    std::string base85;
    input >> base85;

    data_chunk out_value;
    if (!decode_base85(out_value, base85) || out_value.size() != hash_size)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(base85));
    }

    std::copy_n(out_value.begin(), hash_size, argument.value_.begin());
    return input;
}

std::ostream& operator<<(std::ostream& output, const sodium& argument)
{
    std::string decoded;

    // Z85 requires four byte alignment (hash_digest is 32).
    /* bool */ encode_base85(decoded, argument.value_);

    output << decoded;
    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
