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
#ifndef LIBBITCOIN_SYSTEM_MESSAGES_TRANSACTION_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_TRANSACTION_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

struct BC_API transaction
{
    ////typedef std::shared_ptr<transaction> ptr;
    ////typedef std::shared_ptr<const transaction> const_ptr;
    ////typedef std::vector<ptr> ptr_list;
    ////typedef std::vector<const_ptr> const_ptr_list;
    ////typedef std::shared_ptr<const_ptr_list> const_ptr_list_ptr;
    ////typedef std::shared_ptr<const const_ptr_list> const_ptr_list_const_ptr;
    typedef std::shared_ptr<const transaction> ptr;

    static const identifier id;
    static const std::string command;
    static const uint32_t version_minimum;
    static const uint32_t version_maximum;

    static transaction deserialize(uint32_t version, reader& source,
        bool witness);
    void serialize(uint32_t version, writer& sink, bool witness) const;
    size_t size(uint32_t version, bool witness) const;

    chain::transaction::ptr transaction;
};

} // namespace messages
} // namespace system
} // namespace libbitcoin

#endif
