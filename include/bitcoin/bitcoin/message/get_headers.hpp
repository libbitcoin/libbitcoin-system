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
#ifndef LIBBITCOIN_MESSAGE_GET_HEADERS_HPP
#define LIBBITCOIN_MESSAGE_GET_HEADERS_HPP

#include <bitcoin/bitcoin/message/get_blocks.hpp>

namespace libbitcoin {
namespace message {

class BC_API get_headers
  : public get_blocks
{
public:
    static get_headers factory_from_data(const data_chunk& data);
    static get_headers factory_from_data(std::istream& stream);
    static get_headers factory_from_data(reader& source);

    get_headers();
    get_headers(const block_locator start_hashes, const hash_digest hash_stop);

    static const std::string command;
};

} // end message
} // end libbitcoin

#endif
