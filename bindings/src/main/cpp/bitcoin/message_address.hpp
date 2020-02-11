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
#ifndef LIBBITCOIN__MESSAGE_ADDRESS_HPP
#define LIBBITCOIN__MESSAGE_ADDRESS_HPP

#include <istream>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/message_network_address.hpp>
#include <bitcoin/message_network_address_list.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_address
{
public:
//    typedef std::shared_ptr<address> ptr;
//    typedef std::shared_ptr<const address> const_ptr;

    static message_address factory(uint32_t version, const utility_data_chunk& data);
//    static message_address factory(uint32_t version, std::istream& stream);
//    static message_address factory(uint32_t version, reader& source);

    message_address();
    message_address(const message_network_address_list& message_addresses);
    message_address(message_network_address_list&& message_addresses);
    message_address(const message_address& other);
    message_address(message_address&& other);

    message_network_address_list& addresses();
//    const message_network_address_list& addresses() const;
//    void set_addresses(const network_address::list& value);
    void set_addresses(message_network_address_list&& value);

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version) const;

    /// This class is move assignable but not copy assignable.
//    address& operator=(address&& other);
    message_address& assign(message_address&& other);
    //    void operator=(const address&) = delete;
    void operator=(const message_address& other);

//    bool operator==(const address& other) const;
    bool eq(const message_address& other) const;
//    bool operator!=(const address& other) const;
    bool ne(const message_address& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;

    message::address* getValue() {
        return value_;
    }

    void setValue(message::address* value) {
        value_ = value;
    }
private:
    message::address* value_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
