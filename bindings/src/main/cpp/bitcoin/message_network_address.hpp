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
#ifndef LIBBITCOIN__MESSAGE_NETWORK_ADDRESS_HPP
#define LIBBITCOIN__MESSAGE_NETWORK_ADDRESS_HPP

//#include <cstdint>
//#include <istream>
//#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/message_ip_address.hpp>
#include <bitcoin/utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

//typedef byte_array<16> ip_address;

class BC_API message_network_address
{
public:
//    typedef std::vector<network_address> list;

    static message_network_address factory(uint32_t version, const utility_data_chunk& data,
        bool with_timestamp);
//    static message_network_address factory(uint32_t version, std::istream& stream,
//        bool with_timestamp);
//    static message_network_address factory(uint32_t version, reader& source,
//        bool with_timestamp);
    static size_t satoshi_fixed_size(uint32_t version, bool with_timestamp);

    message_network_address();

    // BC_CONSTCTOR required for declaration of constexpr address types.
//    BC_CONSTCTOR message_network_address(uint32_t timestamp, uint64_t services,
//        const message_ip_address& ip, uint16_t port)
//      : timestamp_(timestamp), services_(services), ip_(ip), port_(port)
//    {
//    }

    message_network_address(const message_network_address&);

    message_network_address(uint32_t timestamp, uint64_t services, message_ip_address&& ip,
        uint16_t port);

//    message_network_address(const message_network_address& other);
    message_network_address(message_network_address&& other);

    // Starting version 31402, addresses are prefixed with a timestamp.
    uint32_t timestamp() const;
    void set_timestamp(uint32_t value);

    uint64_t services() const;
    void set_services(uint64_t value);

    message_ip_address& ip();
//    const message_ip_address& ip() const;
//    void set_ip(const message_ip_address& value);
    void set_ip(message_ip_address&& value);

    uint16_t port() const;
    void set_port(uint16_t value);

    bool from_data(uint32_t version, const utility_data_chunk& data,
        bool with_timestamp);
//    bool from_data(uint32_t version, std::istream& stream,
//        bool with_timestamp);
//    bool from_data(uint32_t version, reader& source, bool with_timestamp);
    utility_data_chunk to_data(uint32_t version, bool with_timestamp) const;
//    void to_data(uint32_t version, std::ostream& stream,
//        bool with_timestamp) const;
//    void to_data(uint32_t version, writer& sink, bool with_timestamp) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version, bool with_timestamp) const;

    message_network_address& operator=(message_network_address&& other);
    message_network_address& operator=(const message_network_address& other);
    message_network_address& assign(const message_network_address& other);

//    bool operator==(const message_network_address& other) const;
    bool eq(const message_network_address& other) const;
//    bool operator!=(const message_network_address& other) const;
    bool ne(const message_network_address& other) const;

public:
    message::network_address getValue() {
		return value;
	}

	void setValue(message::network_address value) {
		this->value = value;
	}
private:
	message::network_address value;

//    uint32_t timestamp_;
//    uint64_t services_;
//    ip_address ip_;
//    uint16_t port_;
};

// version::services::none
//BC_CONSTEXPR uint32_t no_services = 0;
//BC_CONSTEXPR uint32_t no_timestamp = 0;
//BC_CONSTEXPR uint16_t unspecified_ip_port = 0;
//BC_CONSTEXPR message::ip_address unspecified_ip_address
//{
//    {
//        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00
//    }
//};
//
//BC_CONSTEXPR message_ip_address message_unspecified_ip_address {
//	unspecified_ip_address
//};
//
//// Defaults to full node services.
//BC_CONSTEXPR message::network_address unspecified_network_address
//{
//    no_timestamp,
//    no_services,
//    unspecified_ip_address,
//    unspecified_ip_port
//};
//
//BC_CONSTEXPR message_network_address message_unspecified_network_address
//{
//	unspecified_network_address
//};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
