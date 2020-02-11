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
#ifndef LIBBITCOIN__MESSAGE_HEADER_MESSAGE_HPP
#define LIBBITCOIN__MESSAGE_HEADER_MESSAGE_HPP

//#include <cstdint>
//#include <cstddef>
//#include <istream>
//#include <memory>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/header.hpp>
//#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/chain_header.hpp>
#include <bitcoin/utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_header
//  : public chain::header
{
public:
//    typedef std::vector<header> list;
//    typedef std::shared_ptr<header> ptr;
//    typedef std::vector<ptr> ptr_list;
//    typedef std::shared_ptr<const header> const_ptr;
//    typedef std::vector<const_ptr> const_ptr_list;
//    typedef std::shared_ptr<const_ptr_list> const_ptr_list_ptr;
//    typedef std::shared_ptr<const const_ptr_list> const_ptr_list_const_ptr;

    static message_header factory(uint32_t version, const utility_data_chunk& data);
//    static message_header factory(uint32_t version, std::istream& stream);
//    static message_header factory(uint32_t version, reader& source);
    static size_t satoshi_fixed_size(uint32_t version);

    message_header();
//    message_header(uint32_t version, const hash_digest& previous_block_hash,
//        const hash_digest& merkle, uint32_t timestamp, uint32_t bits,
//        uint32_t nonce);
    message_header(uint32_t version, math_hash_digest&& previous_block_hash,
        math_hash_digest&& merkle, uint32_t timestamp, uint32_t bits,
        uint32_t nonce);
//    message_header(const chain::message_header& other);
    message_header(chain_header&& other);
    message_header(const message_header& other);
//    message_header(message_header&& other);

    bool from_data(uint32_t version, const utility_data_chunk& data);
//    bool from_data(uint32_t version, std::istream& stream);
//    bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    void reset();
    size_t serialized_size(uint32_t version) const;

//    message_header& operator=(chain_header&& other);
    message_header& assign(chain_header&& other);

    /// This class is move assignable but not copy assignable.
//    message_header& operator=(message_header&& other);
    message_header& assign(message_header&& other);
//    message_header& operator=(const message_header&) /*= delete*/;

//    bool operator==(const chain_header& other) const;
    bool eq(const chain_header& other) const;
//    bool operator!=(const chain::message_header& other) const;

//    bool operator==(const message_header& other) const;
    bool eq(const message_header& other) const;
//    bool operator!=(const message_header& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
public:
    message::header* getValue() {
        return value_;
    }

    void setValue(message::header* value) {
        value_ = value;
    }
private:
    message::header* value_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
