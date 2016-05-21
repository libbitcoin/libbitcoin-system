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
#include <bitcoin/bitcoin/utility/string.hpp>

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace libbitcoin {

std::string join(const std::vector<std::string>& words,
    const std::string& delimiter)
{
    return boost::join(words, delimiter);
}

// Note that use of token_compress_on may cause unexpected results when
// working with CSV-style lists that accept empty elements.
std::vector<std::string> split(const std::string& sentence,
    const std::string& delimiter, bool trim)
{
    std::vector<std::string> words;
    const auto compress = boost::token_compress_on;
    const auto delimit = boost::is_any_of(delimiter);

    if (trim)
    {
        const auto trimmed = boost::trim_copy(sentence);
        boost::split(words, trimmed, delimit, compress);
    }
    else
        boost::split(words, sentence, delimit, compress);

    return words;
}

} // namespace libbitcoin
