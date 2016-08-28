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
#ifndef LIBBITCOIN_MESSAGE_GET_DATA_HPP
#define LIBBITCOIN_MESSAGE_GET_DATA_HPP

#include <initializer_list>
#include <memory>
#include <istream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/message/inventory.hpp>
#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace message {

class BC_API get_data
  : public inventory
{
public:
    typedef std::shared_ptr<get_data> ptr;

    static get_data factory_from_data(uint32_t version,
        const data_chunk& data);
    static get_data factory_from_data(uint32_t version, std::istream& stream);
    static get_data factory_from_data(uint32_t version, reader& source);

    get_data();
    get_data(const inventory_vector::list& list);
    get_data(const hash_list& hashes, inventory::type_id type);
    get_data(const std::initializer_list<inventory_vector>& elements);

    bool from_data(uint32_t version, const data_chunk& data) override;
    bool from_data(uint32_t version, std::istream& stream) override;
    bool from_data(uint32_t version, reader& source) override;

    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;
};

} // namespace message
} // namespace libbitcoin

#endif
