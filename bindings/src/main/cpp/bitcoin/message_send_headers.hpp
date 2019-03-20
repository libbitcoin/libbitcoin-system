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
#ifndef LIBBITCOIN__MESSAGE_SEND_HEADERS_HPP
#define LIBBITCOIN__MESSAGE_SEND_HEADERS_HPP

//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/send_headers.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_send_headers
{
public:
//    typedef std::shared_ptr<send_headers> ptr;
//    typedef std::shared_ptr<const send_headers> const_ptr;

    static message_send_headers factory(uint32_t version, const utility_data_chunk& data);
//    static message_send_headers factory(uint32_t version, std::istream& stream);
//    static message_send_headers factory(uint32_t version, reader& source);
    static size_t satoshi_fixed_size(uint32_t version);

    message_send_headers();
//    message_send_headers(const message_send_headers& other);
    message_send_headers(message_send_headers&& other);

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version) const;

public:
    message::send_headers getValue() {
        return value;
    }

    void setValue(message::send_headers value) {
        this->value = value;
    }
//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
//
//protected:
//    message_send_headers(bool insufficient_version);
//
private:
    message::send_headers value;
//    bool insufficient_version_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
