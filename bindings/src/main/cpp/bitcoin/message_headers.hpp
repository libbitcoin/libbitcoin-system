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
#ifndef LIBBITCOIN__MESSAGE_HEADERS_HPP
#define LIBBITCOIN__MESSAGE_HEADERS_HPP

//#include <cstdint>
//#include <initializer_list>
//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <math_hash_list.hpp>
#include <message_header_list.hpp>
#include <bitcoin/bitcoin/message/headers.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/message/inventory.hpp>
//#include <bitcoin/bitcoin/message/inventory_vector.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_headers
{
public:
//    typedef std::shared_ptr<headers> ptr;
//    typedef std::shared_ptr<const headers> const_ptr;

    static message_headers factory(uint32_t version, const utility_data_chunk& data);
//    static message_headers factory(uint32_t version, std::istream& stream);
//    static message_headers factory(uint32_t version, reader& source);

    message_headers();
//    message_headers(const header::list& values);
    message_headers(message_header_list&& values);
//    message_headers(const std::initializer_list<header>& values);
//    message_headers(const message_headers& other);
    message_headers(message_headers&& other);

    message_header_list& elements();
//    const message_header_list& elements() const;
//    void set_elements(const message_header_list& values);
    void set_elements(message_header_list&& values);

    bool is_sequential() const;
    void to_hashes(math_hash_list& out) const;
//    void to_inventory(inventory_vector::list& out,
//        inventory::type_id type) const;

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version) const;

    // This class is move assignable but not copy assignable.
//    message_headers& operator=(message_headers&& other);
    message_headers& assign(message_headers&& other);
//    void operator=(const message_headers&) = delete;

//    bool operator==(const message_headers& other) const;
    bool eq(const message_headers& other) const;
//    bool operator!=(const message_headers& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
//
public:
    message::headers getValue() {
        return value;
    }

    void setValue(message::headers value) {
        this->value = value;
    }
private:
    message::headers value;
//    header::list elements_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
