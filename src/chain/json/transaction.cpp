/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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

#include <ranges>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

DEFINE_JSON_TO_TAG(transaction)
{
    return
    {
        value.at("version").to_number<uint32_t>(),
        value_to<inputs>(value.at("inputs")),
        value_to<outputs>(value.at("outputs")),
        value.at("locktime").to_number<uint32_t>()
    };
}

DEFINE_JSON_FROM_TAG(transaction)
{
    value =
    {
        // hash is computed property
        { "hash", encode_hash(instance.hash(false)) },
        { "version", instance.version() },
        { "inputs", value_from(*instance.inputs_ptr()) },
        { "outputs", value_from(*instance.outputs_ptr()) },
        { "locktime", instance.locktime() }
    };
}

DEFINE_JSON_TO_TAG(transaction::cptr)
{
    return to_shared(tag_invoke(to_tag<transaction>{}, value));
}

DEFINE_JSON_FROM_TAG(transaction::cptr)
{
    tag_invoke(from_tag{}, value, *instance);
}

// bitcoind
// ----------------------------------------------------------------------------

// There is no non-verbose tx encoding.
DEFINE_JSON_FROM_TAGGED(bitcoind_tag, transaction)
{
    const auto& tx = instance.value;
    value =
    {
        { "hex", value_from(bitcoind_embedded(tx)) },
        { "txid", encode_hash(tx.hash(false)) },
        { "hash", encode_hash(tx.hash(true)) },
        { "size", tx.serialized_size(false) },
        { "vsize", tx.virtual_size() },
        { "weight", tx.weight() },
        { "version", tx.version() },
        { "locktime", tx.locktime() },
        { "vin", value_from(bitcoind(*tx.inputs_ptr())) },
        { "vout", value_from(bitcoind(*tx.outputs_ptr())) }

        ////{ "in_active_chain", false },
        ////{ "blockhash", "" },
        ////{ "confirmations", 0 },
        ////{ "blocktime", 0 },
        ////{ "time", 0 }
    };
}

DEFINE_JSON_FROM_TAGGED(bitcoind_hashed_tag, transaction)
{
    const auto& tx = instance.value;
    value = value_from(encode_hash(tx.hash(false)));
}

DEFINE_JSON_FROM_TAGGED(bitcoind_embedded_tag, transaction)
{
    const auto& tx = instance.value;

    // Streaming hex encode is a material performance optimization.
    ////const auto hash = encode_base16(tx.hash(false));
    std::string hash(two * tx.serialized_size(true), '\0');
    stream::out::fast sink{ hash };
    write::base16::fast writer{ sink };
    tx.to_data(writer, true);
    BC_ASSERT(writer);

    value = value_from(hash);
}

DEFINE_JSON_FROM_TAGGED(bitcoind_verbose_tag, transaction)
{
    const auto& tx = instance.value;
    value = value_from(tx);
}

// The tag wrapper masks the vector, so these must be explicit.
// ----------------------------------------------------------------------------

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

DEFINE_JSON_FROM_TAGGED(bitcoind_tag, transaction_cptrs)
{
    const auto& txs = instance.value;
    value = boost::json::array(txs.size());
    std::ranges::transform(txs, value.as_array().begin(),
        [](const chain::transaction::cptr& tx) NOEXCEPT
        {
            return value_from(bitcoind(*tx));
        });
}

DEFINE_JSON_FROM_TAGGED(bitcoind_hashed_tag, transaction_cptrs)
{
    const auto& txs = instance.value;
    value = boost::json::array(txs.size());
    std::ranges::transform(txs, value.as_array().begin(),
        [](const chain::transaction::cptr& tx) NOEXCEPT
        {
            return value_from(bitcoind_hashed(*tx));
        });
}

DEFINE_JSON_FROM_TAGGED(bitcoind_embedded_tag, transaction_cptrs)
{
    const auto& txs = instance.value;
    value = boost::json::array(txs.size());
    std::ranges::transform(txs, value.as_array().begin(),
        [](const chain::transaction::cptr& tx) NOEXCEPT
        {
            return value_from(bitcoind_embedded(*tx));
        });
}

DEFINE_JSON_FROM_TAGGED(bitcoind_verbose_tag, transaction_cptrs)
{
    const auto& txs = instance.value;
    value = boost::json::array(txs.size());
    std::ranges::transform(txs, value.as_array().begin(),
        [](const chain::transaction::cptr& tx) NOEXCEPT
        {
            return value_from(*tx);
        });
}

BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
