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
#ifndef LIBBITCOIN__MESSAGE_GET_BLOCKS_HPP
#define LIBBITCOIN__MESSAGE_GET_BLOCKS_HPP

//#include <istream>
//#include <memory>
//#include <string>
//#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/get_blocks.hpp>
#include <math_hash_digest.hpp>
#include <math_hash_list.hpp>
#include <utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class BC_API message_get_blocks
{
public:
//    typedef std::shared_ptr<get_blocks> ptr;
//    typedef std::shared_ptr<const get_blocks> const_ptr;

    static message_get_blocks factory(uint32_t version, const utility_data_chunk& data);
//    static message_get_blocks factory(uint32_t version, std::istream& stream);
//    static message_get_blocks factory(uint32_t version, reader& source);

    message_get_blocks();
//    message_get_blocks(const math_hash_list& start, const hash_digest& stop);
    message_get_blocks(math_hash_list&& start, math_hash_digest&& stop);
//    message_get_blocks(const message_get_blocks& other);
    message_get_blocks(message_get_blocks&& other);

    math_hash_list& start_hashes();
//    const math_hash_list& start_hashes() const;
//    void set_start_hashes(const math_hash_list& value);
    void set_start_hashes(math_hash_list&& value);

    math_hash_digest& stop_hash();
//    const math_hash_digest& stop_hash() const;
//    void set_stop_hash(const math_hash_digest& value);
    void set_stop_hash(math_hash_digest&& value);

    virtual bool from_data(uint32_t version, const utility_data_chunk& data);
//    virtual bool from_data(uint32_t version, std::istream& stream);
//    virtual bool from_data(uint32_t version, reader& source);
    utility_data_chunk to_data(uint32_t version) const;
//    void to_data(uint32_t version, std::ostream& stream) const;
//    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    size_t serialized_size(uint32_t version) const;

    // This class is move assignable but not copy assignable.
//    message_get_blocks& operator=(message_get_blocks&& other);
    message_get_blocks& assign(message_get_blocks&& other);
//    void operator=(const message_get_blocks&) = delete;

//    bool operator==(const message_get_blocks& other) const;
    bool eq(const message_get_blocks& other) const;
//    bool operator!=(const message_get_blocks& other) const;

//    static const std::string command;
//    static const uint32_t version_minimum;
//    static const uint32_t version_maximum;
//
public:
    message::get_blocks getValue() {
        return value;
    }

    void setValue(message::get_blocks value) {
        this->value = value;
    }
private:
    message::get_blocks value;
//    // 10 sequential hashes, then exponential samples until reaching genesis.
//    math_hash_list start_hashes_;
//    math_hash_digest stop_hash_;
    virtual ~message_get_blocks();
};

} // namespace api
//} // namespace message
} // namespace libbitcoin

#endif
