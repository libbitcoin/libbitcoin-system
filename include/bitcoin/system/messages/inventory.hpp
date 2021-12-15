/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MESSAGES_INVENTORY_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_INVENTORY_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/inventory_item.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

struct BC_API inventory
{
    typedef std::shared_ptr<const inventory> ptr;
    typedef inventory_item::type_id type_id;

    static const identifier id;
    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

    // TODO: parameterize with witness parameter (once node is ready).
    static inventory factory(hash_list&& hashes, type_id type);
    static inventory factory(const hash_list& hashes, type_id type);

    static inventory deserialize(uint32_t version, reader& source);
    void serialize(uint32_t version, writer& sink) const;
    size_t size(uint32_t version) const;

    inventory_item::list filter(type_id type) const;
    hash_list to_hashes(type_id type) const;
    size_t count(type_id type) const;

    inventory_item::list items;
};

} // namespace messages
} // namespace system
} // namespace libbitcoin

#endif
