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

#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

DEFINE_JSON_TO_TAG(output)
{
    return
    {
        value.at("value").as_uint64(),
        value_to<script>(value.at("script"))
    };
}

DEFINE_JSON_FROM_TAG(output)
{
    value =
    {
        { "value", instance.value() },
        { "script", value_from(instance.script()) }
    };
}

DEFINE_JSON_TO_TAG(output::cptr)
{
    return to_shared(tag_invoke(to_tag<output>{}, value));
}

DEFINE_JSON_FROM_TAG(output::cptr)
{
    tag_invoke(from_tag{}, value, *instance);
}

// bitcoind
// ----------------------------------------------------------------------------

DEFINE_JSON_FROM_TAGGED(bitcoind_tag, output)
{
    using namespace boost::json;
    const auto& out = instance.value;
    const auto satoshis = out.value() / to_floating(satoshi_per_bitcoin);
    auto script = value_from(bitcoind(out.script())).as_object();

    // Removed in bitcoind v22.0 (because not useful).
    ////script["reqSigs"] = 1;

    // TODO: use this universal address format when standard not found.
    script["type"] = "nonstandard";
    script["addresses"] = array{ encode_hash(out.script().hash()) };

    value =
    {
        { "value", satoshis},
        { "scriptPubKey", std::move(script) }

        // In what parallel universe does serializing position make sense?
        ////{ "n", injected from loop }
    };
}

DEFINE_JSON_FROM_TAGGED(bitcoind_tag, output_cptrs)
{
    const auto& outs = instance.value;
    value = boost::json::array(outs.size());
    auto& values = value.as_array();
    for (size_t n{}; n < outs.size(); ++n)
    {
        values.at(n) = value_from(bitcoind(*outs.at(n)));
        values.at(n).as_object()["n"] = n;
    }
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
