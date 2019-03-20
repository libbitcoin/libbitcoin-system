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
#ifndef LIBBITCOIN__MESSAGE_ALERT_HPP
#define LIBBITCOIN__MESSAGE_ALERT_HPP

//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/alert.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_alert
{
public:
//    typedef std::shared_ptr<alert> ptr;
//    typedef std::shared_ptr<const alert> const_ptr;

    static message_alert factory(uint32_t version, const utility_data_chunk& data);
//    static alert factory(uint32_t version, std::istream& stream);
//    static alert factory(uint32_t version, reader& source);

    message_alert();
//    message_alert(const utility_data_chunk& payload, const utility_data_chunk& signature);
    message_alert(utility_data_chunk&& payload, utility_data_chunk&& signature);
//    message_alert(const message_alert& other);
    message_alert(message_alert&& other);

    utility_data_chunk& payload();
//    const utility_data_chunk& payload() const;
//    void set_payload(const utility_data_chunk& value);
    void set_payload(utility_data_chunk&& value);

    utility_data_chunk& signature();
//    const  utility_data_chunk& signature() const;
//    void set_signature(const utility_data_chunk& value);
    void set_signature(utility_data_chunk&& value);

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
//    alert& operator=(message_alert&& other);
    message_alert& assign(message_alert&& other);
//    void operator=(const message_alert&) = delete;

//    bool operator==(const message_alert& other) const;
    bool eq(const message_alert& other) const;
//    bool operator!=(const message_alert& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
//
public:
    message::alert getValue() {
        return value;
    }

    void setValue(message::alert value) {
        this->value = value;
    }
private:
    message::alert value;
//    utility_data_chunk payload_;
//    utility_data_chunk signature_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
