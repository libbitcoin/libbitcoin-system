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
#ifndef LIBBITCOIN__MESSAGE_PING_HPP
#define LIBBITCOIN__MESSAGE_PING_HPP

//#include <cstdint>
//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/ping.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_ping : public message::ping
{
public:
//    typedef std::shared_ptr<ping> ptr;
//    typedef std::shared_ptr<const ping> const_ptr;

    static message_ping factory(uint32_t version, const utility_data_chunk& data);
//    static message_ping factory(uint32_t version, std::istream& stream);
//    static message_ping factory(uint32_t version, reader& source);
    static size_t satoshi_fixed_size(uint32_t version);

    message_ping();
    message_ping(uint64_t nonce);
    message_ping(const message_ping& other);

    uint64_t nonce() const;
    void set_nonce(uint64_t value);

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
//    message_ping& operator=(message_ping&& other);
    message_ping& assign(message_ping&& other);
//    void operator=(const message_ping&) = delete;

//    bool operator==(const message_ping& other) const;
    bool eq(const message_ping& other) const;
//    bool operator!=(const message_ping& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
//
//private:
//    uint64_t nonce_;
//    bool nonceless_;
//    bool valid_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
