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
#ifndef LIBBITCOIN__MESSAGE_NOT_FOUND_HPP
#define LIBBITCOIN__MESSAGE_NOT_FOUND_HPP

//#include <initializer_list>
//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/not_found.hpp>
//#include <bitcoin/bitcoin/constants.hpp>
#include <math_hash_list.hpp>
//#include <bitcoin/bitcoin/message/inventory.hpp>
#include <message_inventory_vector_list.hpp>
#include <message_inventory_vector_type_id.hpp>
#include <utility_data_chunk.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_not_found
//  : public inventory
{
public:
//    typedef std::shared_ptr<message_not_found> ptr;
//    typedef std::shared_ptr<const message_not_found> const_ptr;

    static message_not_found factory(uint32_t version,const utility_data_chunk& data);
//    static message_not_found factory(uint32_t version, std::istream& stream);
//    static message_not_found factory(uint32_t version, reader& source);

    message_not_found();
    virtual ~message_not_found();
//    message_not_found(const inventory_vector::list& values);
    message_not_found(message_inventory_vector_list&& values);
    message_not_found(const math_hash_list& hashes, message_inventory_vector_type_id type);
//    message_not_found(const std::initializer_list<inventory_vector>& values);
//    message_not_found(const message_not_found& other);
    message_not_found(message_not_found&& other);

    bool from_data(uint32_t version, const utility_data_chunk& data) override;
//    bool from_data(uint32_t version, std::istream& stream) override;
//    bool from_data(uint32_t version, reader& source) override;

    // This class is move assignable but not copy assignable.
//    message_not_found& operator=(message_not_found&& other);
    message_not_found& assign(message_not_found&& other);
//    void operator=(const message_not_found&) = delete;

//    bool operator==(const message_not_found& other) const;
    bool eq(const message_not_found& other) const;
//    bool operator!=(const message_not_found& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
public:
    message::not_found getValue() {
        return value;
    }

    void setValue(message::not_found value) {
        this->value = value;
    }
private:
    message::not_found value;

};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
