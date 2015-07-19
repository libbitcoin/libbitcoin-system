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
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>

namespace libbitcoin {
namespace config {
    
using namespace boost;
using namespace boost::program_options;

checkpoint_type::checkpoint_type()
  : height_(0), hash_(bc::null_hash)
{
}

checkpoint_type::checkpoint_type(const std::string& value)
  : checkpoint_type()
{
    std::stringstream(value) >> *this;
}

checkpoint_type::checkpoint_type(const checkpoint_type& other)
  : hash_(other.get_hash()), height_(other.get_height())
{
}

const hash_digest& checkpoint_type::get_hash() const
{
    return hash_;
}

const size_t checkpoint_type::get_height() const
{
    return height_;
}

std::istream& operator>>(std::istream& input, checkpoint_type& argument)
{
    std::string value;
    input >> value;

    // std::regex requires gcc 4.9, so we are using boost::regex for now.
    // When matched will always generate 6 tokens, we want 2 and 4.
    // We allow 1-10 digits, which is sufficient for 2^32 blocks.
    const boost::regex regular("([0-9a-f]{64})(:([0-9]{1,10}))?");

    try 
    {
        boost::sregex_iterator it(value.begin(), value.end(), regular), end;

        // No match?
        if (it == end)
        {
            BOOST_THROW_EXCEPTION(invalid_option_value(value));
        }

        // Extract the two tokens of interest.
        boost::smatch match = *it;

        if (!decode_hash(argument.hash_, match[2]))
        {
            BOOST_THROW_EXCEPTION(invalid_option_value(value));
        }

        const std::string number(match[4]);

        // Convert the block height to a number, catching exceptions.
        if (!number.empty())
            argument.height_ = boost::lexical_cast<size_t>(number);
    }
    catch (const boost::exception&)
    {
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }
    catch (const std::exception&)
    {
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return input;
}

std::ostream& operator<<(std::ostream& output, const checkpoint_type& argument)
{
    output << bc::encode_hash(argument.hash_) << ":" << argument.height_;

    return output;
}

} // namespace config
} // namespace libbitcoin
