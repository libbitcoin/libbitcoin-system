/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/utility/unicode.hpp>

#include <cstddef>
#include <string>
#include <boost/locale.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

data_chunk to_utf8(int argc, wchar_t* argv[])
{
    // Convert each arg and determine the payload size.
    size_t payload_size = 0;
    std::vector<std::string> collection(argc + 1);
    for (size_t arg = 0; arg < argc; arg++)
    {
        collection[arg] = to_utf8(argv[arg]);
        payload_size += collection[arg].size() + 1;
    }

    // Determine the index size.
    auto index_size = static_cast<size_t>((argc + 1)* sizeof(void*));

    // Allocate the new buffer.
    auto buffer_size = index_size + payload_size;
    data_chunk buffer(buffer_size, 0x00);
    buffer.resize(buffer_size);

    // Set pointers into index and payload buffer sections.
    auto index = reinterpret_cast<char**>(&buffer[0]);
    auto arguments = reinterpret_cast<char*>(&buffer[index_size]);

    // Clone the converted collection into the new narrow argv.
    for (size_t arg = 0; arg < argc; arg++)
    {
        index[arg] = arguments;
        std::copy(collection[arg].begin(), collection[arg].end(), index[arg]);
        arguments += collection[arg].size() + 1;
    }

    return buffer;
}

std::string to_utf8(const std::wstring& wide)
{
    using namespace boost::locale;
    return conv::utf_to_utf<char>(wide, conv::method_type::stop);
}

std::wstring to_utf16(const std::string& narrow)
{
    using namespace boost::locale;
    return conv::utf_to_utf<wchar_t>(narrow, conv::method_type::stop);
}

} // namespace libbitcoin
