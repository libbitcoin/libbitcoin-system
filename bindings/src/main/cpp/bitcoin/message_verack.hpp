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
#ifndef LIBBITCOIN__MESSAGE_VERACK_HPP
#define LIBBITCOIN__MESSAGE_VERACK_HPP

//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

// The checksum is ignored by the verack command.
class BC_API message_verack
{
public:
//    typedef std::shared_ptr<verack> ptr;
//    typedef std::shared_ptr<const verack> const_ptr;

    static message_verack factory(uint32_t version, const utility_data_chunk& data);
//    static message_verack factory(uint32_t version, std::istream& stream);
//    static message_verack factory(uint32_t version, reader& source);
    static size_t satoshi_fixed_size(uint32_t version);

    message_verack();

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
public:
    message::verack getValue() {
        return value;
    }

    void setValue(message::verack value) {
        this->value = value;
    }
private:
    message::verack value;

};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
