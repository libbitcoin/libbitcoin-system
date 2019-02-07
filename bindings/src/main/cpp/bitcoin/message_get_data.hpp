/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__MESSAGE_GET_DATA_HPP
#define LIBBITCOIN__MESSAGE_GET_DATA_HPP

//#include <initializer_list>
//#include <memory>
//#include <istream>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/get_data.hpp>
//#include <bitcoin/bitcoin/math/hash.hpp>
//#include <bitcoin/bitcoin/message/inventory.hpp>
//#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <utility_data_chunk.hpp>
#include <message_inventory_vector_list.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_get_data : public message::get_data
//  : public inventory
{
public:
//    typedef std::shared_ptr<message_get_data> ptr;
//    typedef std::shared_ptr<const message_get_data> const_ptr;

    static message_get_data factory(uint32_t version, const utility_data_chunk& data);
//    static message_get_data factory(uint32_t version, std::istream& stream);
//    static message_get_data factory(uint32_t version, reader& source);

    message_get_data();
//    message_get_data(const inventory_vector::list& list);
    message_get_data(message_inventory_vector_list&& list);
//    message_get_data(const hash_list& hashes, type_id type);
//    message_get_data(const std::initializer_list<inventory_vector>& elements);
//    message_get_data(const message_get_data& other);
    message_get_data(message_get_data&& other);
    virtual ~message_get_data();

    bool from_data(uint32_t version, const utility_data_chunk& data) override;
//    bool from_data(uint32_t version, std::istream& stream) override;
//    bool from_data(uint32_t version, reader& source) override;

    /// Convert message types to witness types.
    void to_witness();

    // This class is move assignable but not copy assignable.
//    message_get_data& operator=(message_get_data&& other);
    message_get_data& assign(message_get_data&& other);
//    void operator=(const message_get_data&) = delete;

//    bool operator==(const message_get_data& other) const;
    bool eq(const message_get_data& other) const;
//    bool operator!=(const message_get_data& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
