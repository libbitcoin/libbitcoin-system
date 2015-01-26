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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/config/config.hpp>

#include <istream>
#include <iterator>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/compat.h>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace config {

std::string join(const std::vector<std::string>& words,
    const std::string& delimiter)
{
    return boost::join(words, delimiter);
}

std::string read_stream(std::istream& stream)
{
    BC_SET_BINARY_FILE_MODE(true);
    std::istreambuf_iterator<char> first(stream), last;
    std::string result(first, last);
    BC_SET_BINARY_FILE_MODE(false);
    return result;
}

std::vector<std::string> split(const std::string& sentence,
    const std::string& delimiter)
{
    std::vector<std::string> words;
    boost::split(words, sentence, boost::is_any_of(delimiter),
        boost::token_compress_on);
    return words;
}

void trim(std::string& value)
{
    boost::trim(value);
}

} // namespace config
} // namespace libbitcoin

