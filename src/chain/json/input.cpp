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
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

DEFINE_JSON_TO_TAG(input)
{
    return
    {
        value_to<point>(value.at("point")),
        value_to<script>(value.at("script")),
        value_to<witness>(value.at("witness")),
        value.at("sequence").to_number<uint32_t>()
    };
}

DEFINE_JSON_FROM_TAG(input)
{
    if (instance.witness().is_valid())
    {
        value =
        {
            { "point", value_from(instance.point()) },
            { "script", value_from(instance.script()) },
            { "witness", value_from(instance.witness()) },
            { "sequence", instance.sequence() }
        };
    }
    else
    {
        value =
        {
            { "point", value_from(instance.point()) },
            { "script", value_from(instance.script()) },
            { "sequence", instance.sequence() }
        };
    }
}

DEFINE_JSON_TO_TAG(input::cptr)
{
    return to_shared(tag_invoke(to_tag<input>{}, value));
}

DEFINE_JSON_FROM_TAG(input::cptr)
{
    tag_invoke(from_tag{}, value, *instance);
}

// bitcoind
// ----------------------------------------------------------------------------

DEFINE_JSON_FROM_TAGGED(bitcoind_tag, input)
{
    const auto& in = instance.value;
    if (in.is_coinbase())
    {
        value =
        {
            { "coinbase", encode_base16(in.script().to_data(false)) },
            { "sequence", in.sequence() }
        };
    }
    else
    {
        value =
        {
            { "txid", encode_hash(in.point().hash()) },
            { "vout", in.point().index() },
            { "scriptSig", value_from(bitcoind(in.script())) },
            { "sequence", in.sequence() }
        };

        if (!in.witness().stack().empty())
            value.as_object()["txinwitness"] = value_from(bitcoind(in.witness()));
    }
}

DEFINE_JSON_FROM_TAGGED(bitcoind_tag, input_cptrs)
{
    const auto& ins = instance.value;
    value = boost::json::array(ins.size());
    std::ranges::transform(ins, value.as_array().begin(),
        [](const chain::input::cptr& in) NOEXCEPT
        {
            return value_from(bitcoind(*in));
        });
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
