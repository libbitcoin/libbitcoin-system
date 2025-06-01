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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_SCRIPT_PATTERNS_IPP
#define LIBBITCOIN_SYSTEM_CHAIN_SCRIPT_PATTERNS_IPP

#include <algorithm>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// More efficient [] dereferences are all guarded here.
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// static
// ----------------------------------------------------------------------------

constexpr bool script::is_enabled(uint32_t active_flags, flags flag) NOEXCEPT
{
    return to_bool(flag & active_flags);
}

constexpr bool script::is_push_only_pattern(const operations& ops) NOEXCEPT
{
    const auto push = [](const operation& op) NOEXCEPT
    {
        return operation::is_push(op.code());
    };

    return std::all_of(ops.begin(), ops.end(), push);
}

// ****************************************************************************
// CONSENSUS: this pattern is used to activate bip16 validation rules.
// ****************************************************************************
constexpr bool script::is_relaxed_push_pattern(const operations& ops) NOEXCEPT
{
    const auto push = [&](const operation& op) NOEXCEPT
    {
        return operation::is_relaxed_push(op.code());
    };

    return std::all_of(ops.begin(), ops.end(), push);
}

// ****************************************************************************
// CONSENSUS: this pattern is used to commit to bip141 witness data.
// ****************************************************************************
constexpr bool script::is_commitment_pattern(const operations& ops) NOEXCEPT
{
    constexpr auto header = to_big_endian(chain::witness_head);

    // C++14: remove && ops[1].data().size() >= header.size() guard.
    // Bytes after commitment optional with no consensus meaning [bip141].
    // Commitment not executable so invalid trailing operations are allowed.
    return ops.size() > 1
        && ops[0].code() == opcode::op_return
        && ops[1].code() == opcode::push_size_36
        && ops[1].data().size() >= header.size()
        && std::equal(header.begin(), header.end(), ops[1].data().begin());
}

// ****************************************************************************
// CONSENSUS: this pattern is used in bip141 validation rules.
// ****************************************************************************
constexpr bool script::is_witness_program_pattern(
    const operations& ops) NOEXCEPT
{
    return ops.size() == 2
        && ops[0].is_nonnegative()
        && ops[1].is_minimal_push()
        && ops[1].data().size() >= min_witness_program
        && ops[1].data().size() <= max_witness_program;
}

// A witness program used only for policy (not implemented).
constexpr bool script::is_anchor_program_pattern(const operations& ops) NOEXCEPT
{
    return is_witness_program_pattern(ops)
        && ops[1].data().size() == 2
        && ops[1].data()[0] == 0x4e
        && ops[1].data()[1] == 0x73;
}

// The satoshi client now enables configurable data size for policy.
constexpr bool script::is_pay_null_data_pattern(const operations& ops) NOEXCEPT
{
    return ops.size() == 2
        && ops[0].code() == opcode::op_return
        && ops[1].is_minimal_push()
        && ops[1].data().size() <= max_null_data_size;
}

// Used by neutrino.
constexpr bool script::is_pay_op_return_pattern(const operations& ops) NOEXCEPT
{
    return !ops.empty()
        && ops[0].code() == opcode::op_return;
}

// The current 16 (or 20) limit does not affect server indexing because bare
// multisig is not indexable and p2sh multisig is byte-limited to 15 sigs.
// The satoshi client policy limit is 3 signatures for bare multisig.
constexpr bool script::is_pay_multisig_pattern(const operations& ops) NOEXCEPT
{
    constexpr auto op_1 = static_cast<uint8_t>(opcode::push_positive_1);
    constexpr auto op_16 = static_cast<uint8_t>(opcode::push_positive_16);

    const auto op_count = ops.size();

    if (op_count < 4 || ops[op_count - 1].code() != opcode::checkmultisig)
        return false;

    const auto op_m = static_cast<uint8_t>(ops[0].code());
    const auto op_n = static_cast<uint8_t>(ops[op_count - 2].code());

    if (op_m < op_1 || op_m > op_n || op_n < op_1 || op_n > op_16)
        return false;

    const auto number = op_n - op_1 + 1u;
    const auto points = op_count - 3u;

    if (number != points)
        return false;

    for (auto op = std::next(ops.begin());
        op != std::prev(ops.end(), 2); ++op)
    {
        if (!is_public_key(op->data()))
            return false;
    }

    return true;
}

// The satoshi client considers this non-standard for policy.
constexpr bool script::is_pay_public_key_pattern(
    const operations& ops) NOEXCEPT
{
    return ops.size() == 2
        && is_public_key(ops[0].data())
        && ops[1].code() == opcode::checksig;
}

constexpr bool script::is_pay_key_hash_pattern(const operations& ops) NOEXCEPT
{
    return ops.size() == 5
        && ops[0].code() == opcode::dup
        && ops[1].code() == opcode::hash160
        && ops[2].data().size() == short_hash_size
        && ops[3].code() == opcode::equalverify
        && ops[4].code() == opcode::checksig;
}

// ****************************************************************************
// CONSENSUS: this pattern is used to activate bip16 validation rules.
// ****************************************************************************
constexpr bool script::is_pay_script_hash_pattern(
    const operations& ops) NOEXCEPT
{
    return ops.size() == 3
        && ops[0].code() == opcode::hash160
        && ops[1].code() == opcode::push_size_20
        && ops[2].code() == opcode::equal;
}

constexpr bool script::is_pay_witness_pattern(const operations& ops) NOEXCEPT
{
    return ops.size() == 2
        && ops[0].is_nonnegative()
        && ops[1].is_push();
}

constexpr bool script::is_pay_witness_key_hash_pattern(
    const operations& ops) NOEXCEPT
{
    return ops.size() == 2
        && ops[0].code() == opcode::push_size_0
        && ops[1].code() == opcode::push_size_20;
}

// ****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
// ****************************************************************************
constexpr bool script::is_pay_witness_script_hash_pattern(
    const operations& ops) NOEXCEPT
{
    return ops.size() == 2
        && ops[0].code() == opcode::push_size_0
        && ops[1].code() == opcode::push_size_32;
}

// The first push is based on wacky satoshi op_check_multisig behavior that
// we must perpetuate, though this is not used in consensus validation.
constexpr bool script::is_sign_multisig_pattern(const operations& ops) NOEXCEPT
{
    const auto endorsement = [](const operation& op) NOEXCEPT
    {
        return is_endorsement(op.data());
    };

    return ops.size() >= 2
        && ops[0].code() == opcode::push_size_0
        && std::all_of(std::next(ops.begin()), ops.end(), endorsement);
}

constexpr bool script::is_sign_public_key_pattern(
    const operations& ops) NOEXCEPT
{
    return ops.size() == 1
        && is_endorsement(ops[0].data());
}

// ****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
// ****************************************************************************
constexpr bool script::is_sign_key_hash_pattern(const operations& ops) NOEXCEPT
{
    return ops.size() == 2
        && is_endorsement(ops[0].data())
        && is_public_key(ops[1].data());
}


// Ambiguous with is_sign_key_hash when second/last op is a public key.
// Ambiguous with is_sign_public_key_pattern when only op is endorsement.
constexpr bool script::is_sign_script_hash_pattern(
    const operations& ops) NOEXCEPT
{
    return !ops.empty()
        && is_push_only_pattern(ops)
        && !ops.back().data().empty();
}

inline operations script::to_pay_null_data_pattern(
    const data_slice& data) NOEXCEPT
{
    if (data.size() > max_null_data_size)
        return {};
    
    return
    {
        { opcode::op_return },
        { to_chunk(data), false }
    };
}

inline operations script::to_pay_public_key_pattern(
    const data_slice& point) NOEXCEPT
{
    if (!is_public_key(point))
        return {};
    
    return
    {
        { to_chunk(point), false },
        { opcode::checksig }
    };
}

inline operations script::to_pay_key_hash_pattern(
    const short_hash& hash) NOEXCEPT
{
    return to_pay_key_hash_pattern(to_shared(to_chunk(hash)));
}

inline operations script::to_pay_key_hash_pattern(
    const chunk_cptr& hash) NOEXCEPT
{
    BC_ASSERT(hash->size() == short_hash_size);

    return
    {
        { opcode::dup },
        { opcode::hash160 },
        { hash, false },
        { opcode::equalverify },
        { opcode::checksig }
    };
}

inline operations script::to_pay_script_hash_pattern(
    const short_hash& hash) NOEXCEPT
{
    return
    {
        { opcode::hash160 },
        { to_chunk(hash), false },
        { opcode::equal }
    };
}

inline operations script::to_pay_multisig_pattern(uint8_t signatures,
    const compressed_list& points) NOEXCEPT
{
    return to_pay_multisig_pattern(signatures,
        to_stack<ec_compressed_size>(points));
}

// This supports up to 16 signatures, but check_multisig is limited to 20.
// The embedded script is limited to 520 bytes, an effective limit of 15
// for p2sh multisig, which can be as low as 7 with all uncompressed keys.
inline operations script::to_pay_multisig_pattern(uint8_t signatures,
    const data_stack& points) NOEXCEPT
{
    constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    constexpr auto base = sub1(op_81);
    constexpr auto max = op_96 - base;

    const auto m = signatures;
    const auto n = points.size();

    if (m < 1 || m > n || n < 1 || n > max)
        return {};

    const auto op_m = static_cast<opcode>(m + base);
    const auto op_n = static_cast<opcode>(points.size() + base);

    operations ops{};
    ops.reserve(points.size() + 3);
    ops.emplace_back(op_m);

    for (const auto& point: points)
    {
        if (!is_public_key(point))
            return {};

        ops.emplace_back(point, false);
    }

    ops.emplace_back(op_n);
    ops.emplace_back(opcode::checkmultisig);
    return ops;
}

inline operations script::to_pay_witness_pattern(uint8_t version,
    const data_slice& data) NOEXCEPT
{
    return
    {
        { operation::opcode_from_version(version) },
        { to_chunk(data), false },
    };
}

inline operations script::to_pay_witness_key_hash_pattern(
    const short_hash& hash) NOEXCEPT
{
    return
    {
        { opcode::push_size_0 },
        { to_chunk(hash), false },
    };
}

inline operations script::to_pay_witness_script_hash_pattern(
    const hash_digest& hash) NOEXCEPT
{
    return
    {
        { opcode::push_size_0 },
        { to_chunk(hash), false }
    };
}

// private
inline size_t script::op_size(size_t total, const operation& op) NOEXCEPT
{
    return ceilinged_add(total, op.serialized_size());
};

// This is an optimization over using script::pattern.
inline bool script::is_pay_to_witness(uint32_t active_flags) const NOEXCEPT
{
    return is_enabled(active_flags, flags::bip141_rule) &&
        is_witness_program_pattern(ops());
}

// This is an optimization over using script::pattern.
inline bool script::is_pay_to_script_hash(uint32_t active_flags) const NOEXCEPT
{
    return is_enabled(active_flags, flags::bip16_rule) &&
        is_pay_script_hash_pattern(ops());
}

BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
