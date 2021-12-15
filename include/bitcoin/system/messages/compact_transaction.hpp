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
#ifndef LIBBITCOIN_SYSTEM_MESSAGES_COMPACT_TRANSACTION_HPP
#define LIBBITCOIN_SYSTEM_MESSAGES_COMPACT_TRANSACTION_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

/// This is also known as a "prefilled transaction".
struct BC_API compact_transaction
{
public:
    typedef std::vector<compact_transaction> list;
    typedef std::shared_ptr<const compact_transaction> ptr;

    static compact_transaction deserialize(uint32_t version, reader& source,
        bool witness);
    void serialize(uint32_t version, writer& sink, bool witness) const;
    size_t size(uint32_t version, bool witness) const;

    uint64_t index;
    chain::transaction transaction;
};

} // namespace messages
} // namespace system
} // namespace libbitcoin

#endif
