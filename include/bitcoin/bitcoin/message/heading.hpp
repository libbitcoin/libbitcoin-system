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
#ifndef LIBBITCOIN_MESSAGE_HEADING_HPP
#define LIBBITCOIN_MESSAGE_HEADING_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <boost/array.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {

class BC_API heading
{
public:
    // Header minus checksum is 4 + 12 + 4 + 4 = 24 bytes
    static BC_CONSTEXPR size_t serialized_size = 24;

    // boost1.54/linux/clang/libstdc++-4.8 error if std::array
    // could not match 'boost::array' against 'std::array'
    typedef boost::array<uint8_t, serialized_size> buffer;

    static heading factory_from_data(const data_chunk& data);
    static heading factory_from_data(std::istream& stream);
    static heading factory_from_data(reader& source);

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);
    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;
    bool is_valid() const;
    void reset();

    uint32_t magic;
    std::string command;
    uint32_t payload_size;
    uint32_t checksum;
};

BC_API bool operator==(const heading& left, const heading& right);
BC_API bool operator!=(const heading& left, const heading& right);

} // namspace message
} // namspace libbitcoin

#endif
