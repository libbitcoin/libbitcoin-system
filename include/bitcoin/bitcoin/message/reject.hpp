/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_MESSAGE_REJECT_HPP
#define LIBBITCOIN_MESSAGE_REJECT_HPP

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {

class BC_API reject
{
public:
    enum class reason_code: uint8_t
    {
        undefined = 0x00,
        malformed = 0x01,
        invalid = 0x10,
        obsolete = 0x11,
        duplicate = 0x12,
        nonstandard = 0x40,
        dust = 0x41,
        insufficient_fee = 0x42,
        checkpoint = 0x43
    };

    typedef std::shared_ptr<reject> ptr;
    typedef std::shared_ptr<const reject> const_ptr;

    static reject factory_from_data(uint32_t version, const data_chunk& data);
    static reject factory_from_data(uint32_t version, std::istream& stream);
    static reject factory_from_data(uint32_t version, reader& source);

    reject();
    reject(reason_code code, const std::string& message,
        const std::string& reason, const hash_digest& data);
    reject(reason_code code, std::string&& message, std::string&& reason,
        hash_digest&& data);
    reject(const reject& other);
    reject(reject&& other);

    reason_code code() const;
    void set_code(reason_code value);

    std::string& message();
    const std::string& message() const;
    void set_message(const std::string& value);
    void set_message(std::string&& value);

    std::string& reason();
    const std::string& reason() const;
    void set_reason(const std::string& value);
    void set_reason(std::string&& value);

    hash_digest& data();
    const hash_digest& data() const;
    void set_data(const hash_digest& value);
    void set_data(hash_digest&& value);

    bool from_data(uint32_t version, const data_chunk& data);
    bool from_data(uint32_t version, std::istream& stream);
    bool from_data(uint32_t version, reader& source);
    data_chunk to_data(uint32_t version) const;
    void to_data(uint32_t version, std::ostream& stream) const;
    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version) const;

    // This class is move assignable but not copy assignable.
    reject& operator=(reject&& other);
    void operator=(const reject&) = delete;

    bool operator==(const reject& other) const;
    bool operator!=(const reject& other) const;

    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

private:
    static reason_code reason_from_byte(uint8_t byte);
    static uint8_t reason_to_byte(reason_code value);

    reason_code code_;
    std::string message_;
    std::string reason_;
    hash_digest data_;
};

} // namespace message
} // namespace libbitcoin

#endif
