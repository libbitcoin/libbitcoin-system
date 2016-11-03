/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/script/operation_stack.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace chain {

bool is_push_only(const operation_stack& ops)
{
    const auto push = [](const operation& op)
    {
        return op.is_push();
    };

    return std::all_of(ops.begin(), ops.end(), push);
}

bool is_null_data_pattern(const operation_stack& ops)
{
    return ops.size() == 2
        && ops[0].code() == opcode::return_
        && ops[1].is_push()
        && ops[1].data().size() <= max_null_data_size;
}

bool is_pay_multisig_pattern(const operation_stack& ops)
{
    static constexpr size_t op_1 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr size_t op_16 = static_cast<uint8_t>(opcode::push_positive_16);

    const auto op_count = ops.size();

    if (op_count < 4 || ops[op_count - 1].code() != opcode::checkmultisig)
        return false;

    const auto op_m = static_cast<uint8_t>(ops[0].code());
    const auto op_n = static_cast<uint8_t>(ops[op_count - 2].code());

    if (op_m < op_1 || op_m > op_n || op_n < op_1 || op_n > op_16)
        return false;

    const auto number = op_n - op_1;
    const auto points = op_count - 3u;

    if (number != points)
        return false;

    for (auto op = ops.begin() + 1; op != ops.end() - 2; ++op)
        if (!is_public_key(op->data()))
            return false;

    return true;
}

bool is_pay_public_key_pattern(const operation_stack& ops)
{
    return ops.size() == 2
        && ops[0].is_push()
        && is_public_key(ops[0].data())
        && ops[1].code() == opcode::checksig;
}

bool is_pay_key_hash_pattern(const operation_stack& ops)
{
    return ops.size() == 5
        && ops[0].code() == opcode::dup
        && ops[1].code() == opcode::hash160
        && ops[2].is_push()
        && ops[2].data().size() == short_hash_size
        && ops[3].code() == opcode::equalverify
        && ops[4].code() == opcode::checksig;
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip16 validation rules.
//*****************************************************************************
bool is_pay_script_hash_pattern(const operation_stack& ops)
{
    return ops.size() == 3
        && ops[0].code() == opcode::hash160
        && ops[1].code() == opcode::push_size_20
        && ops[1].data().size() == short_hash_size
        && ops[2].code() == opcode::equal;
}

bool is_sign_multisig_pattern(const operation_stack& ops)
{
    if (ops.size() < 2 || !is_push_only(ops))
        return false;

    if (ops.front().code() != opcode::push_size_0)
        return false;

    return true;
}

bool is_sign_public_key_pattern(const operation_stack& ops)
{
    return ops.size() == 1 && is_push_only(ops);
}

bool is_sign_key_hash_pattern(const operation_stack& ops)
{
    return ops.size() == 2 && is_push_only(ops) &&
        is_public_key(ops.back().data());
}

bool is_sign_script_hash_pattern(const operation_stack& ops)
{
    if (ops.size() < 2 || !is_push_only(ops))
        return false;

    const auto& redeem_data = ops.back().data();

    if (redeem_data.empty())
        return false;

    script redeem;

    if (!redeem.from_data(redeem_data, false))
        return false;

    // Is the redeem script a standard pay (output) script?
    const auto redeem_script_pattern = redeem.pattern();
    return redeem_script_pattern == script_pattern::pay_multisig
        || redeem_script_pattern == script_pattern::pay_public_key
        || redeem_script_pattern == script_pattern::pay_key_hash
        || redeem_script_pattern == script_pattern::pay_script_hash
        || redeem_script_pattern == script_pattern::null_data;
}
    
operation_stack to_null_data_pattern(data_slice data)
{
    if (data.size() > max_null_data_size)
        return{};

    return operation_stack
    {
        operation{ opcode::return_ },
        operation{ to_chunk(data) }
    };
}

operation_stack to_pay_public_key_pattern(data_slice point)
{
    if (!is_public_key(point))
        return{};

    return operation_stack
    {
        { to_chunk(point) },
        { opcode::checksig }
    };
}

operation_stack to_pay_key_hash_pattern(const short_hash& hash)
{
    return operation_stack
    {
        { opcode::dup },
        { opcode::hash160 },
        { to_chunk(hash) },
        { opcode::equalverify },
        { opcode::checksig }
    };
}

operation_stack to_pay_script_hash_pattern(const short_hash& hash)
{
    return operation_stack
    {
        { opcode::hash160 },
        { to_chunk(hash) },
        { opcode::equal }
    };
}

operation_stack to_pay_multisig_pattern(uint8_t signatures,
    const point_list& points)
{
    const auto conversion = [](const ec_compressed& point)
    {
        return to_chunk(point);
    };

    data_stack chunks(points.size());
    std::transform(points.begin(), points.end(), chunks.begin(), conversion);
    return to_pay_multisig_pattern(signatures, chunks);
}

operation_stack to_pay_multisig_pattern(uint8_t signatures,
    const data_stack& points)
{
    static constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    static constexpr auto zero = op_81 - 1;
    static constexpr auto max = op_96 - zero;

    const auto m = signatures;
    const auto n = points.size();

    if (m < 1 || m > n || n < 1 || n > max)
        return operation_stack();

    const auto op_m = static_cast<opcode>(m + zero);
    const auto op_n = static_cast<opcode>(points.size() + zero);

    operation_stack ops;
    ops.reserve(points.size() + 3);
    ops.push_back({ op_m });

    for (const auto point: points)
    {
        if (!is_public_key(point))
            return{};

        ops.push_back(point);
    }

    ops.push_back({ op_n });
    ops.push_back({ opcode::checkmultisig });
    return ops;
}

} // namespace chain
} // namespace libbitcoin
