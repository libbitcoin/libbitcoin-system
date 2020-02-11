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
#ifndef LIBBITCOIN__MESSAGE_INVENTORY_HPP
#define LIBBITCOIN__MESSAGE_INVENTORY_HPP

//#include <cstddef>
//#include <cstdint>
//#include <initializer_list>
//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/inventory.hpp>
#include <bitcoin/math_hash_list.hpp>
#include <bitcoin/message_inventory_list.hpp>
#include <bitcoin/message_inventory_vector_list.hpp>
#include <bitcoin/utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_inventory
{
public:
//    typedef std::shared_ptr<inventory> ptr;
//    typedef std::shared_ptr<const inventory> const_ptr;
    typedef message::inventory_vector::type_id type_id;

    static message_inventory factory(uint32_t version, const utility_data_chunk& data);
//    static message_inventory factory(uint32_t version, std::istream& stream);
//    static message_inventory factory(uint32_t version, reader& source);

    message_inventory();
//    message_inventory(const message_inventory_vector_list& values);
    message_inventory(message_inventory_vector_list&& values);
    message_inventory(const math_hash_list& hashes, type_id type);
//    message_inventory(const std::initializer_list<message_inventory_vector>& values);
    message_inventory(const message_inventory& other);
//    message_inventory(message_inventory&& other);
    virtual ~message_inventory();

    message_inventory_vector_list& inventories();
//    const message_inventory_vector_list& inventories() const;
//    void set_inventories(const message_inventory_vector_list& value);
    void set_inventories(message_inventory_vector_list&& value);

    virtual bool from_data(uint32_t version, const utility_data_chunk& data);
//    virtual bool from_data(uint32_t version, std::istream& stream);
//    virtual bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    void to_hashes(math_hash_list& out, type_id type) const;
    void reduce(message_inventory_vector_list& out, type_id type) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version) const;
    size_t count(type_id type) const;

    // This class is move assignable but not copy assignable.
//    message_inventory& operator=(message_inventory&& other);
    message_inventory& assign(message_inventory&& other);
//    void operator=(const message_inventory&) = delete;

//    bool operator==(const message_inventory& other) const;
    bool eq(const message_inventory& other) const;
//    bool operator!=(const message_inventory& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
//
public:
    message::inventory* getValue() {
        return value_;
    }

    void setValue(message::inventory* value) {
        value_ = value;
    }
private:
    message::inventory* value_;
//    message_inventory_vector::list inventories_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
