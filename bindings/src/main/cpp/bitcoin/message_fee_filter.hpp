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
#ifndef LIBBITCOIN__MESSAGE_FEE_FILTER_HPP
#define LIBBITCOIN__MESSAGE_FEE_FILTER_HPP

//#include <cstdint>
//#include <istream>
//#include <memory>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/fee_filter.hpp>
#include <bitcoin/utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_fee_filter
{
public:
//    typedef std::shared_ptr<fee_filter> ptr;
//    typedef std::shared_ptr<const fee_filter> const_ptr;

    static message_fee_filter factory(uint32_t version, const utility_data_chunk& data);
//    static message_fee_filter factory(uint32_t version, std::istream& stream);
//    static message_fee_filter factory(uint32_t version, reader& source);
    static size_t satoshi_fixed_size(uint32_t version);

    message_fee_filter();
    message_fee_filter(uint64_t minimum);
    message_fee_filter(const message_fee_filter& other);
//    message_fee_filter(message_fee_filter&& other);

    uint64_t minimum_fee() const;
    void set_minimum_fee(uint64_t value);

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
//    message_fee_filter& operator=(message_fee_filter&& other);
    message_fee_filter& assign(message_fee_filter&& other);
//    void operator=(const message_fee_filter&) = delete;

//    bool operator==(const message_fee_filter& other) const;
    bool eq(const message_fee_filter& other) const;
//    bool operator!=(const message_fee_filter& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;

//protected:
//    message_fee_filter(uint64_t minimum, bool insufficient_version);

public:
    message::fee_filter* getValue() {
        return value_;
    }

    void setValue(message::fee_filter* value) {
        value_ = value;
    }
private:
    message::fee_filter* value_;
//    uint64_t minimum_fee_;
//    bool insufficient_version_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
