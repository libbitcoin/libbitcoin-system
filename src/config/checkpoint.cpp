/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
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
#include <bitcoin/bitcoin/config/checkpoint.hpp>

#include <cstddef>
#include <iostream>
#include <regex>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>

namespace libbitcoin {
namespace config {
    
using namespace boost;
using namespace boost::program_options;

checkpoint::checkpoint()
  : height_(0), hash_(bc::null_hash)
{
}

checkpoint::checkpoint(const std::string& value)
  : checkpoint()
{
    std::stringstream(value) >> *this;
}

checkpoint::checkpoint(const checkpoint& other)
  : hash_(other.hash()), height_(other.height())
{
}

// This is intended for static initialization (i.e. of the internal defaults).
checkpoint::checkpoint(const std::string& hash, size_t height)
  : height_(height)
{
    if (!decode_hash(hash_, hash))
    {
        BOOST_THROW_EXCEPTION(invalid_option_value(hash));
    }
}

checkpoint::checkpoint(const hash_digest& hash, size_t height)
    : hash_(hash), height_(height)
{
}

const hash_digest& checkpoint::hash() const
{
    return hash_;
}

const size_t checkpoint::height() const
{
    return height_;
}

std::string checkpoint::to_string() const
{
    std::stringstream value;
    value << *this;
    return value.str();
}

std::istream& operator>>(std::istream& input, checkpoint& argument)
{
    std::string value;
    input >> value;

    // std::regex requires gcc 4.9, so we are using boost::regex for now.
    // We allow 1-10 digits, which is sufficient for 2^32 blocks.
    static const regex regular("^([0-9a-f]{64})(:([0-9]{1,10}))?$");

    sregex_iterator it(value.begin(), value.end(), regular), end;
    if (it == end)
    {
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    const auto& match = *it;
    if (!decode_hash(argument.hash_, match[1]))
    {
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    try
    {
        argument.height_ = lexical_cast<size_t>(match[3]);
    }
    catch (const boost::exception&)
    {
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return input;
}

std::ostream& operator<<(std::ostream& output, const checkpoint& argument)
{
    output << encode_hash(argument.hash()) << ":" << argument.height();
    return output;
}

} // namespace config
} // namespace libbitcoin
