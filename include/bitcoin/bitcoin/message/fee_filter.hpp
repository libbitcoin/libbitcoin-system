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
#ifndef LIBBITCOIN_MESSAGE_FEE_FILTER_HPP
#define LIBBITCOIN_MESSAGE_FEE_FILTER_HPP

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace message {

class BC_API fee_filter
{
public:
    typedef std::shared_ptr<fee_filter> ptr;

    static fee_filter factory_from_data(uint32_t version, const data_chunk& data);
    static fee_filter factory_from_data(uint32_t version, std::istream& stream);
    static fee_filter factory_from_data(uint32_t version, reader& source);
    static uint64_t satoshi_fixed_size(uint32_t version);

    fee_filter();
    fee_filter(uint64_t minimum);

    bool from_data(uint32_t version, const data_chunk& data);
    bool from_data(uint32_t version, std::istream& stream);
    bool from_data(uint32_t version, reader& source);
    data_chunk to_data(uint32_t version) const;
    void to_data(uint32_t version, std::ostream& stream) const;
    void to_data(uint32_t version, writer& sink) const;
    bool is_valid() const;
    void reset();
    uint64_t serialized_size(uint32_t version) const;

    bool operator==(const fee_filter& other) const;
    bool operator!=(const fee_filter& other) const;

    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

    uint64_t minimum_fee;

private:
    bool valid_;
};

} // end message
} // end libbitcoin

#endif
