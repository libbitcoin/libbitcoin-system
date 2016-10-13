/**
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
#ifndef LIBBITCOIN_MESSAGE_HEADERS_HPP
#define LIBBITCOIN_MESSAGE_HEADERS_HPP

#include <cstdint>
#include <initializer_list>
#include <istream>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/message/header_message.hpp>
#include <bitcoin/bitcoin/message/inventory.hpp>
#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {

class BC_API headers
{
public:
    typedef std::shared_ptr<headers> ptr;
    typedef std::shared_ptr<const headers> const_ptr;

    static headers factory_from_data(uint32_t version, const data_chunk& data);
    static headers factory_from_data(uint32_t version, std::istream& stream);
    static headers factory_from_data(uint32_t version, reader& source);

    headers();
    headers(const header_message::list& values);
    headers(header_message::list&& values);
    headers(const std::initializer_list<header_message>& values);
    headers(const headers& other);
    headers(headers&& other);

    header_message::list& elements();
    const header_message::list& elements() const;
    void set_elements(const header_message::list& values);
    void set_elements(header_message::list&& values);

    void to_hashes(hash_list& out) const;
    void to_inventory(inventory_vector::list& out,
        inventory::type_id type) const;

    bool from_data(uint32_t version, const data_chunk& data);
    bool from_data(uint32_t version, std::istream& stream);
    bool from_data(uint32_t version, reader& source);
    data_chunk to_data(uint32_t version) const;
    void to_data(uint32_t version, std::ostream& stream) const;
    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t serialized_size(uint32_t version) const;

    // This class is move assignable but not copy assignable.
    headers& operator=(headers&& other);
    void operator=(const headers&) = delete;

    bool operator==(const headers& other) const;
    bool operator!=(const headers& other) const;

    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

private:
    header_message::list elements_;
};

} // namespace message
} // namespace libbitcoin

#endif
