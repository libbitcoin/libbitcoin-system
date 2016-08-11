/*
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
#ifndef LIBBITCOIN_MESSAGE_FILTER_LOAD_HPP
#define LIBBITCOIN_MESSAGE_FILTER_LOAD_HPP

#include <istream>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {

class BC_API filter_load
{
public:
    typedef std::shared_ptr<filter_load> ptr;

    static filter_load factory_from_data(uint32_t version,
        const data_chunk& data);
    static filter_load factory_from_data(uint32_t version,
        std::istream& stream);
    static filter_load factory_from_data(uint32_t version,
        reader& source);

    bool from_data(uint32_t version, const data_chunk& data);
    bool from_data(uint32_t version, std::istream& stream);
    bool from_data(uint32_t version, reader& source);
    data_chunk to_data(uint32_t version) const;
    void to_data(uint32_t version, std::ostream& stream) const;
    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t serialized_size(uint32_t version) const;

    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

    data_chunk filter;
    uint32_t hash_functions;
    uint32_t tweak;
    uint8_t flags;
};

BC_API bool operator==(const filter_load& left, const filter_load& right);
BC_API bool operator!=(const filter_load& left, const filter_load& right);

} // end message
} // end libbitcoin

#endif
