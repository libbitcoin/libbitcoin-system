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
#ifndef LIBBITCOIN__MESSAGE_REJECT_HPP
#define LIBBITCOIN__MESSAGE_REJECT_HPP

//#include <cstdint>
//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/reject.hpp>
#include <message_reject_reason_code.hpp>
#include <utility_data_chunk.hpp>
#include <math_hash_digest.hpp>
#include <p_std_string.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_reject
{
public:
//    enum class reason_code: uint8_t
//    {
//        /// The reason code is not defined.
//        undefined = 0x00,
//
//        /// The message was malformed.
//        malformed = 0x01,
//
//        /// In response to block or tx message: invalid (data is hash).
//        invalid = 0x10,
//
//        /// In response to version message: version.
//        /// In respose to block message: version (data is hash).
//        obsolete = 0x11,
//
//        /// In respose to tx message: double spend (data is hash).
//        /// In respose to version message: more than one received.
//        duplicate = 0x12,
//
//        /// In respose to tx message: nonstandard (data is hash).
//        nonstandard = 0x40,
//
//        /// In respose to tx message: dust output(s) (data is hash).
//        dust = 0x41,
//
//        /// In respose to tx message: insufficient fee (data is hash).
//        insufficient_fee = 0x42,
//
//        /// In response to block message: failed checkpoint (data is hash).
//        checkpoint = 0x43
//    };

//    typedef std::shared_ptr<reject> ptr;
//    typedef std::shared_ptr<const reject> const_ptr;

    static message_reject factory(uint32_t version, const utility_data_chunk& data);
//    static reject factory(uint32_t version, std::istream& stream);
//    static reject factory(uint32_t version, reader& source);

    message_reject();

//    message_reject(reason_code code, const std::string& message,
//        const std::string& reason);
    message_reject(message_reject_reason_code code, p_std_string& message, p_std_string& reason);

//    message_reject(message_reject_reason_code code, const std::string& message,
//        const std::string& reason, const math_hash_digest& data);
    message_reject(message_reject_reason_code code, p_std_string& message, p_std_string& reason,
        math_hash_digest&& data);

//    message_reject(const message_reject& other);
    message_reject(message_reject&& other);

    message_reject_reason_code code() const;
    void set_code(message_reject_reason_code value);

    p_std_string message();
//    const std::string& message() const;
//    void set_message(const std::string& value);
    void set_message(p_std_string& value);

    p_std_string reason();
//    const std::string& reason() const;
//    void set_reason(const std::string& value);
    void set_reason(p_std_string& value);

    math_hash_digest& data();
//    const math_hash_digest& data() const;
//    void set_data(const math_hash_digest& value);
    void set_data(math_hash_digest&& value);

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
//    message_reject& operator=(message_reject&& other);
    message_reject& assign(message_reject&& other);
//    void operator=(const message_reject&) = delete;

//    bool operator==(const message_reject& other) const;
    bool eq(const message_reject& other) const;
//    bool operator!=(const message_reject& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
//
public:
    message::reject getValue() {
        return value;
    }

    void setValue(message::reject value) {
        this->value = value;
    }
private:
    message::reject value;
//    static message_reject_reason_code reason_from_byte(uint8_t byte);
//    static uint8_t reason_to_byte(message_reject_reason_code value);
//
//    message_reject_reason_code code_;
//    std::string message_;
//    std::string reason_;
//    math_hash_digest data_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
