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
#ifndef LIBBITCOIN__MESSAGE_GET_HEADERS_HPP
#define LIBBITCOIN__MESSAGE_GET_HEADERS_HPP

//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/get_headers.hpp>
#include <math_hash_digest.hpp>
#include <math_hash_list.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/message/get_blocks.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_get_headers
//  : public get_blocks
{
public:
//    typedef std::shared_ptr<get_headers> ptr;
//    typedef std::shared_ptr<const get_headers> const_ptr;

    static message_get_headers factory(uint32_t version, const utility_data_chunk& data);
//    static message_get_headers factory(uint32_t version, std::istream& stream);
//    static message_get_headers factory(uint32_t version, reader& source);

    message_get_headers();
//    message_get_headers(const math_hash_list& start, const hash_digest& stop);
    message_get_headers(math_hash_list&& start, math_hash_digest&& stop);
//    message_get_headers(const message_get_headers& other);
    message_get_headers(message_get_headers&& other);
    virtual ~message_get_headers();

    bool from_data(uint32_t version, const utility_data_chunk& data) override;
//    bool from_data(uint32_t version, std::istream& stream) override;
//    bool from_data(uint32_t version, reader& source) override;

    // This class is move assignable but not copy assignable.
//    message_get_headers& operator=(message_get_headers&& other);
    message_get_headers& assign(message_get_headers&& other);
//    void operator=(const message_get_headers&) = delete;

//    bool operator==(const message_get_headers& other) const;
    bool eq(const message_get_headers& other) const;
//    bool operator!=(const message_get_headers& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
public:
    message::get_headers getValue() {
        return value;
    }

    void setValue(message::get_headers value) {
        this->value = value;
    }
private:
    message::get_headers value;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
