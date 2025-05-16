/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/chain/script.hpp>

#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Extraction.
// ----------------------------------------------------------------------------

const chunk_cptr& script::witness_program() const NOEXCEPT
{
    static const auto empty = to_shared<const data_chunk>();
    return is_witness_program_pattern(ops()) ? ops().at(1).data_ptr() : empty;
}

script_version script::version() const NOEXCEPT
{
    if (!is_witness_program_pattern(ops()))
        return script_version::unversioned;

    switch (ops_.front().code())
    {
        case opcode::push_size_0:
            return script_version::segwit;
        case opcode::push_size_1:
            return script_version::taproot;
        default:
            return script_version::reserved;
    }
}

// Caller should test for is_sign_script_hash_pattern when sign_key_hash result
// as it is possible for an input script to match both patterns.
script_pattern script::pattern() const NOEXCEPT
{
    const auto input = output_pattern();
    return input == script_pattern::non_standard ? input_pattern() : input;
}

// Output patterns are mutually and input unambiguous.
// The bip141 coinbase pattern is not tested here, must test independently.
script_pattern script::output_pattern() const NOEXCEPT
{
    if (is_pay_key_hash_pattern(ops()))
        return script_pattern::pay_key_hash;

    if (is_pay_script_hash_pattern(ops()))
        return script_pattern::pay_script_hash;

    if (is_pay_null_data_pattern(ops()))
        return script_pattern::pay_null_data;

    if (is_pay_public_key_pattern(ops()))
        return script_pattern::pay_public_key;

    // Limited to 16 signatures though op_check_multisig allows 20.
    if (is_pay_multisig_pattern(ops()))
        return script_pattern::pay_multisig;

    return script_pattern::non_standard;
}

// A sign_key_hash result always implies sign_script_hash as well.
// The bip34 coinbase pattern is not tested here, must test independently.
script_pattern script::input_pattern() const NOEXCEPT
{
    if (is_sign_key_hash_pattern(ops()))
        return script_pattern::sign_key_hash;

    // This must follow is_sign_key_hash_pattern for ambiguity comment to hold.
    if (is_sign_script_hash_pattern(ops()))
        return script_pattern::sign_script_hash;

    if (is_sign_public_key_pattern(ops()))
        return script_pattern::sign_public_key;

    if (is_sign_multisig_pattern(ops()))
        return script_pattern::sign_multisig;

    return script_pattern::non_standard;
}

// prevout_script is only used to determine is_pay_script_hash_pattern.
bool script::extract_sigop_script(script& embedded,
    const chain::script& prevout_script) const NOEXCEPT
{
    // There are no embedded sigops when the prevout script is not p2sh.
    if (!is_pay_script_hash_pattern(prevout_script.ops()))
        return false;

    // There are no embedded sigops when the input script is not push only.
    if (ops().empty() || !is_relaxed_push_pattern(ops()))
        return false;

    // Parse the embedded script from the last input script item (data).
    // This cannot fail because there is no prefix to invalidate the length.
    embedded = { ops().back().data(), false };
    return true;
}

// Count 1..16 multisig accurately for embedded [bip16] and witness [bip141].
constexpr size_t multisig_sigops(bool accurate, opcode code) NOEXCEPT
{
    return accurate && operation::is_positive(code) ?
        operation::opcode_to_positive(code) : multisig_default_sigops;
}

constexpr bool is_single_sigop(opcode code) NOEXCEPT
{
    return code == opcode::checksig || code == opcode::checksigverify;
}

constexpr bool is_multiple_sigop(opcode code) NOEXCEPT
{
    return code == opcode::checkmultisig || code == opcode::checkmultisigverify;
}

// TODO: compute in or at script evaluation and add coinbase input scripts?
// TODO: this would avoid second deserialization of script for sigop counting.
size_t script::signature_operations(bool accurate) const NOEXCEPT
{
    size_t total{};
    auto last = opcode::push_negative_1;

    for (const auto& op: ops())
    {
        const auto code = op.code();

        if (is_single_sigop(code))
        {
            total = ceilinged_add(total, one);
        }
        else if (is_multiple_sigop(code))
        {
            total = ceilinged_add(total, multisig_sigops(accurate, last));
        }

        last = code;
    }

    return total;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
