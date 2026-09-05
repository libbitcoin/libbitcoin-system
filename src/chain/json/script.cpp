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

#include <algorithm>
#include <ranges>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
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

// bitcoind classifies any push-only op_return script as null data.
static bool is_null_data(const script& output_script) NOEXCEPT
{
    const auto& ops = output_script.ops();
    const auto pushes = ops | std::views::drop(1);
    const auto is_push = [](const operation& op) NOEXCEPT
    {
        return op.is_relaxed_push();
    };

    return script::is_pay_op_return_pattern(ops) &&
        !output_script.is_underflow() && std::ranges::all_of(pushes, is_push);
}

// bitcoind output script type names.
static std::string to_type(const script& output_script) NOEXCEPT
{
    if (is_null_data(output_script))
        return "nulldata";

    switch (output_script.output_pattern())
    {
        case script_pattern::pay_public_key:
            return "pubkey";
        case script_pattern::pay_key_hash:
            return "pubkeyhash";
        case script_pattern::pay_script_hash:
            return "scripthash";
        case script_pattern::pay_multisig:
            return "multisig";
        case script_pattern::pay_anchor:
            return "anchor";
        case script_pattern::pay_witness_key_hash:
            return "witness_v0_keyhash";
        case script_pattern::pay_witness_script_hash:
            return "witness_v0_scripthash";
        case script_pattern::pay_witness_v1_taproot:
            return "witness_v1_taproot";
        case script_pattern::pay_witness_unknown:
            return "witness_unknown";
        default:
            return "nonstandard";
    }
}

DEFINE_JSON_FROM_TAGGED(bitcoind_tag, script)
{
    const auto& script = instance.value;
    value =
    {
        { "asm", script.to_string(instance.flags, true) },
        { "hex", encode_base16(script.to_data(false)) },
        { "type", to_type(script) }
    };
}

DEFINE_JSON_FROM_TAGGED(bitcoind_signature_tag, script)
{
    const auto& script = instance.value;
    value =
    {
        { "asm", script.to_string(instance.flags, true, true) },
        { "hex", encode_base16(script.to_data(false)) }
    };
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
