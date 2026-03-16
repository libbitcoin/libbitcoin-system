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

#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

DEFINE_JSON_TO_TAG(script)
{
    return script{ value.as_string() };
}

DEFINE_JSON_FROM_TAG(script)
{
    // TODO: how to inject rules.
    value = instance.to_string(flags::all_rules);
}

DEFINE_JSON_TO_TAG(script::cptr)
{
    return to_shared(tag_invoke(to_tag<script>{}, value));
}

DEFINE_JSON_FROM_TAG(script::cptr)
{
    tag_invoke(from_tag{}, value, *instance);
}

// bitcoind
// ----------------------------------------------------------------------------

DEFINE_JSON_FROM_TAGGED(bitcoind_tag, script)
{
    const auto& script = instance.value;
    value =
    {
        // TODO: how to inject rules.
        { "asm", script.to_string(flags::all_rules, true) },
        { "hex", encode_base16(script.to_data(false)) }
    };
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
