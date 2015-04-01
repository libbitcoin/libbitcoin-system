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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SATOSHI_PRIMITIVE_GET_DATA_HPP
#define LIBBITCOIN_SATOSHI_PRIMITIVE_GET_DATA_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/inventory_vector.hpp>
#include <bitcoin/bitcoin/primitives/satoshi/inventory.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

class BC_API get_data_type : public inventory_type
{
public:

    static const std::string satoshi_command;

    get_data_type();

    get_data_type(const data_chunk& value);

    template <typename Iterator, bool SafeCheckLast>
    get_data_type(deserializer<Iterator, SafeCheckLast>& deserial);

    template<typename Iterator>
    get_data_type(const Iterator begin, const Iterator end);
};

}

#include <bitcoin/bitcoin/impl/primitives/satoshi/inventory.ipp>

#endif
