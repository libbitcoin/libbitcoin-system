/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/chain/json/json.hpp>

#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// native
// ----------------------------------------------------------------------------

DEFINE_JSON_TO_TAG(block)
{
    return
    {
        value_to<header>(value.at("header")),
        value_to<transactions>(value.at("transactions"))
    };
}

DEFINE_JSON_FROM_TAG(block)
{
    value =
    {
        { "header", value_from(instance.header()) },
        { "transactions", value_from(*instance.transactions_ptr()) },
    };
}

DEFINE_JSON_TO_TAG(block::cptr)
{
    return to_shared(tag_invoke(to_tag<block>{}, value));
}

DEFINE_JSON_FROM_TAG(block::cptr)
{
    tag_invoke(from_tag{}, value, *instance);
}

// bitcoind
// ----------------------------------------------------------------------------
// There is no json mapping for a block as hex.

DEFINE_JSON_FROM_TAGGED(bitcoind_tag, block)
{
    const auto& block = instance.value;
    value = value_from(bitcoind(block.header()));
    auto& object = value.as_object();
    object["nTx"] = block.transactions();
    object["strippedsize"] = block.serialized_size(false);
    object["size"] = block.serialized_size(true);
    object["weight"] = block.weight();

    // txs not populated in bitcoind_tag encoding.
    ////object["tx"] = ...;
}

DEFINE_JSON_FROM_TAGGED(bitcoind_hashed_tag, block)
{
    const auto& block = instance.value;
    value = value_from(bitcoind(block));
    value.as_object()["tx"] = value_from(bitcoind_hashed(*block.transactions_ptr()));
}

DEFINE_JSON_FROM_TAGGED(bitcoind_embedded_tag, block)
{
    const auto& block = instance.value;
    value = value_from(bitcoind(block));
    value.as_object()["tx"] = value_from(bitcoind_embedded(*block.transactions_ptr()));
}

DEFINE_JSON_FROM_TAGGED(bitcoind_verbose_tag, block)
{
    const auto& block = instance.value;
    const auto& txs = *block.transactions_ptr();
    value = value_from(bitcoind(block));
    value.as_object()["tx"] = value_from(bitcoind(txs, instance.flags));
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
