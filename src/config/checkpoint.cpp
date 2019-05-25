/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/config/checkpoint.hpp>

#include <cstddef>
#include <iostream>
#include <regex>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/formats/base_16.hpp>
#include <bitcoin/system/math/hash.hpp>

namespace libbitcoin {
namespace system {
namespace config {

using namespace boost;
using namespace boost::program_options;

checkpoint::checkpoint()
  : height_(0), hash_(null_hash)
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

size_t checkpoint::height() const
{
    return height_;
}

std::string checkpoint::to_string() const
{
    std::stringstream value;
    value << *this;
    return value.str();
}

config::checkpoint::list checkpoint::sort(const list& checks)
{
    const auto comparator = [](const checkpoint& left, const checkpoint& right)
    {
        return left.height() < right.height();
    };

    auto copy = checks;
    std::sort(copy.begin(), copy.end(), comparator);
    return copy;
}

bool checkpoint::covered(size_t height, const list& checks)
{
    return !checks.empty() && height <= checks.back().height();
}

bool checkpoint::validate(const hash_digest& hash, size_t height,
    const list& checks)
{
    const auto match_invalid = [&height, &hash](const config::checkpoint& item)
    {
        return height == item.height() && hash != item.hash();
    };

    const auto it = std::find_if(checks.begin(), checks.end(), match_invalid);
    return it == checks.end();
}

bool checkpoint::operator==(const checkpoint& other) const
{
    return height_ == other.height_ && hash_ == other.hash_;
}

std::istream& operator>>(std::istream& input, checkpoint& argument)
{
    std::string value;
    input >> value;

    // std::regex requires gcc 4.9, so we are using boost::regex for now.
    static const regex regular("^([0-9a-f]{64})(:([0-9]{1,20}))?$");

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
} // namespace system
} // namespace libbitcoin
