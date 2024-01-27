/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_SCRIPT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_SCRIPT_HPP

#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/enums/forks.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class transaction;

class BC_API script
{
public:
    typedef std::shared_ptr<const script> cptr;

    /// Utilities.
    /// -----------------------------------------------------------------------

    /// Determine if the fork is enabled in the active forks set.
    static constexpr bool is_enabled(uint32_t active_forks, forks fork) NOEXCEPT
    {
        return to_bool(fork & active_forks);
    }

    static inline bool is_push_only(const operations& ops) NOEXCEPT
    {
        const auto push = [](const operation& op) NOEXCEPT
        {
            // !constexpr
            return op.is_push();
        };

        return std::all_of(ops.begin(), ops.end(), push);
    }

    //*************************************************************************
    // CONSENSUS: this pattern is used to activate bip16 validation rules.
    //*************************************************************************
    static inline bool is_relaxed_push(const operations& ops) NOEXCEPT
    {
        const auto push = [&](const operation& op) NOEXCEPT
        {
            // !constexpr
            return op.is_relaxed_push();
        };

        return std::all_of(ops.begin(), ops.end(), push);
    }

    // More efficient [] dereferences are all guarded here.
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)

    //*************************************************************************
    // CONSENSUS: BIP34 requires coinbase input script to begin with one byte
    // that indicates height size. This is inconsistent with an extreme future
    // where the size byte overflows. However satoshi actually requires nominal
    // encoding.
    //*************************************************************************
    static bool is_coinbase_pattern(const operations& ops,
        size_t height) NOEXCEPT;

    //*************************************************************************
    // CONSENSUS: this pattern is used to commit to bip141 witness data.
    //*************************************************************************
    static inline bool is_commitment_pattern(const operations& ops) NOEXCEPT
    {
        constexpr auto header = to_big_endian(chain::witness_head);

        // C++14: remove && ops[1].data().size() >= header.size() guard.
        // Bytes after commitment optional with no consensus meaning (bip141).
        // Commitment not executable so invalid trailing operations are allowed.
        return ops.size() > 1
            && ops[0].code() == opcode::op_return
            && ops[1].code() == opcode::push_size_36
            && ops[1].data().size() >= header.size()
            && std::equal(header.begin(), header.end(), ops[1].data().begin());
    }

    // C++20 constexpr.
    //*************************************************************************
    // CONSENSUS: this pattern is used in bip141 validation rules.
    //*************************************************************************
    static inline bool is_witness_program_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 2
            && ops[0].is_version()
            && ops[1].data().size() >= min_witness_program
            && ops[1].data().size() <= max_witness_program;
    }

    // C++20 constexpr.
    // The satoshi client tests for 83 bytes total. This allows for waste of
    // one byte to represent up to 75 bytes using the push_one_size opcode.
    // It also allows any number of push ops and limits value to 0 and 1 per tx.
    ////static inline bool is_pay_null_data_pattern(const operations& ops)
    ////{
    ////    constexpr auto op_76 = static_cast<uint8_t>(opcode::push_one_size);
    ////
    ////    return ops.size() >= 2
    ////        && ops[0].code() == opcode::return_
    ////        && static_cast<uint8_t>(ops[1].code()) <= op_76
    ////        && ops[1].data().size() <= max_null_data_size;
    ////}

    // C++20 constexpr.
    // Used by neutrino.
    static inline bool is_pay_op_return_pattern(
        const operations& ops) NOEXCEPT
    {
        return !ops.empty()
            && ops[0].code() == opcode::op_return;
    }

    // C++20 constexpr.
    // The satoshi client enables configurable data size for policy.
    static inline bool is_pay_null_data_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 2
            && ops[0].code() == opcode::op_return
            && ops[1].is_minimal_push()
            && ops[1].data().size() <= max_null_data_size;
    }

    // C++20 constexpr.
    // The current 16 (or 20) limit does not affect server indexing because bare
    // multisig is not indexable and p2sh multisig is byte-limited to 15 sigs.
    // The satoshi client policy limit is 3 signatures for bare multisig.
    static inline bool is_pay_multisig_pattern(
        const operations& ops) NOEXCEPT
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

    // C++20 constexpr.
    // The satoshi client considers this non-standard for policy.
    static inline bool is_pay_public_key_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 2
            && is_public_key(ops[0].data())
            && ops[1].code() == opcode::checksig;
    }

    // C++20 constexpr.
    static inline bool is_pay_key_hash_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 5
            && ops[0].code() == opcode::dup
            && ops[1].code() == opcode::hash160
            && ops[2].data().size() == short_hash_size
            && ops[3].code() == opcode::equalverify
            && ops[4].code() == opcode::checksig;
    }

    // C++20 constexpr.
    //*************************************************************************
    // CONSENSUS: this pattern is used to activate bip16 validation rules.
    //*************************************************************************
    static inline bool is_pay_script_hash_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 3
            && ops[0].code() == opcode::hash160
            && ops[1].code() == opcode::push_size_20
            && ops[2].code() == opcode::equal;
    }

    // C++20 constexpr.
    static inline bool is_pay_witness_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 2
            && ops[0].is_version()
            && ops[1].is_push();
    }

    // C++20 constexpr.
    static inline bool is_pay_witness_key_hash_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 2
            && ops[0].code() == opcode::push_size_0
            && ops[1].code() == opcode::push_size_20;
    }

    // C++20 constexpr.
    //*************************************************************************
    // CONSENSUS: this pattern is used to activate bip141 validation rules.
    //*************************************************************************
    static inline bool is_pay_witness_script_hash_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 2
            && ops[0].code() == opcode::push_size_0
            && ops[1].code() == opcode::push_size_32;
    }

    // C++20 constexpr.
    // The first push is based on wacky satoshi op_check_multisig behavior that
    // we must perpetuate, though it's appearance here is policy not consensus.
    // Limiting to push_size_0 removes pattern ambiguity with little downside.
    static inline bool is_sign_multisig_pattern(
        const operations& ops) NOEXCEPT
    {
        const auto endorsement = [](const operation& op) NOEXCEPT
        {
            return is_endorsement(op.data());
        };

        return ops.size() >= 2
            && ops[0].code() == opcode::push_size_0
            && std::all_of(std::next(ops.begin()), ops.end(), endorsement);
    }

    // C++20 constexpr.
    static inline bool is_sign_public_key_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 1
            && is_endorsement(ops[0].data());
    }

    // C++20 constexpr.
    //*************************************************************************
    // CONSENSUS: this pattern is used to activate bip141 validation rules.
    //*************************************************************************
    static inline bool is_sign_key_hash_pattern(
        const operations& ops) NOEXCEPT
    {
        return ops.size() == 2
            && is_endorsement(ops[0].data())
            && is_public_key(ops[1].data());
    }

    BC_POP_WARNING(/*NO_ARRAY_INDEXING*/)

    // C++20 constexpr.
    // Ambiguous with is_sign_key_hash when second/last op is a public key.
    // Ambiguous with is_sign_public_key_pattern when only op is endorsement.
    static inline bool is_sign_script_hash_pattern(
        const operations& ops) NOEXCEPT
    {
        return !ops.empty()
            && is_push_only(ops)
            && !ops.back().data().empty();
    }

    static inline operations to_pay_null_data_pattern(
        const data_slice& data) NOEXCEPT
    {
        return data.size() > max_null_data_size ? operations{} : operations
        {
            { opcode::op_return },
            { to_chunk(data), false }
        };
    }

    static inline operations to_pay_public_key_pattern(
        const data_slice& point) NOEXCEPT
    {
        return !is_public_key(point) ? operations{} : operations
        {
            { to_chunk(point), false },
            { opcode::checksig }
        };
    }

    static inline operations to_pay_key_hash_pattern(
        const short_hash& hash) NOEXCEPT
    {
        return operations
        {
            { opcode::dup },
            { opcode::hash160 },
            { to_chunk(hash), false },
            { opcode::equalverify },
            { opcode::checksig }
        };
    }

    static inline operations to_pay_script_hash_pattern(
        const short_hash& hash) NOEXCEPT
    {
        return operations
        {
            { opcode::hash160 },
            { to_chunk(hash), false },
            { opcode::equal }
        };
    }

    static inline operations to_pay_multisig_pattern(uint8_t signatures,
        const compressed_list& points) NOEXCEPT
    {
        return to_pay_multisig_pattern(signatures,
            to_stack<ec_compressed_size>(points));
    }

    // This supports up to 16 signatures, but check_multisig is limited to 20.
    // The embedded script is limited to 520 bytes, an effective limit of 15
    // for p2sh multisig, which can be as low as 7 with all uncompressed keys.
    static inline operations to_pay_multisig_pattern(uint8_t signatures,
        const data_stack& points) NOEXCEPT
    {
        constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
        constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
        constexpr auto base = sub1(op_81);
        constexpr auto max = op_96 - base;

        const auto m = signatures;
        const auto n = points.size();

        if (m < 1 || m > n || n < 1 || n > max)
            return operations();

        const auto op_m = static_cast<opcode>(m + base);
        const auto op_n = static_cast<opcode>(points.size() + base);

        operations ops;
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

    static inline operations to_pay_witness_pattern(uint8_t version,
        const data_slice& data) NOEXCEPT
    {
        return
        {
            { operation::opcode_from_version(version) },
            { to_chunk(data), false },
        };
    }

    static inline operations to_pay_witness_key_hash_pattern(
        const short_hash& hash) NOEXCEPT
    {
        return
        {
            { opcode::push_size_0 },
            { to_chunk(hash), false },
        };
    }

    static inline operations to_pay_witness_script_hash_pattern(
        const hash_digest& hash) NOEXCEPT
    {
        return
        {
            { opcode::push_size_0 },
            { to_chunk(hash), false }
        };
    }

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default script is an invalid empty script object.
    script() NOEXCEPT;
    virtual ~script() NOEXCEPT;

    /// Metadata is defaulted on copy/assign.
    script(script&& other) NOEXCEPT;
    script(const script& other) NOEXCEPT;
    script(operations&& ops) NOEXCEPT;
    script(const operations& ops) NOEXCEPT;
    script(operations&& ops, bool prefail) NOEXCEPT;
    script(const operations& ops, bool prefail) NOEXCEPT;
    script(const data_slice& data, bool prefix) NOEXCEPT;
    script(stream::in::fast&& stream, bool prefix) NOEXCEPT;
    script(stream::in::fast& stream, bool prefix) NOEXCEPT;
    script(std::istream&& stream, bool prefix) NOEXCEPT;
    script(std::istream& stream, bool prefix) NOEXCEPT;
    script(reader&& source, bool prefix) NOEXCEPT;
    script(reader& source, bool prefix) NOEXCEPT;

    // TODO: move to config serialization wrapper.
    script(const std::string& mnemonic) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    /// Metadata is defaulted on copy/assign.
    script& operator=(script&& other) NOEXCEPT;
    script& operator=(const script& other) NOEXCEPT;

    bool operator==(const script& other) const NOEXCEPT;
    bool operator!=(const script& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data(bool prefix) const NOEXCEPT;
    void to_data(std::ostream& stream, bool prefix) const NOEXCEPT;
    void to_data(writer& sink, bool prefix) const NOEXCEPT;

    // TODO: move to config serialization wrapper.
    std::string to_string(uint32_t active_forks) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    bool is_prefail() const NOEXCEPT;
    const operations& ops() const NOEXCEPT;

    /// Computed properties.
    hash_digest hash() const NOEXCEPT;
    size_t serialized_size(bool prefix) const NOEXCEPT;

    /// Utilities.
    /// -----------------------------------------------------------------------

    /// Pattern optimizations.
    bool is_pay_to_witness(uint32_t forks) const NOEXCEPT;
    bool is_pay_to_script_hash(uint32_t forks) const NOEXCEPT;

    /// Common pattern detection.
    const data_chunk& witness_program() const NOEXCEPT;
    script_version version() const NOEXCEPT;
    script_pattern pattern() const NOEXCEPT;
    script_pattern input_pattern() const NOEXCEPT;
    script_pattern output_pattern() const NOEXCEPT;

    /// Consensus computations.
    size_t signature_operations(bool accurate) const NOEXCEPT;
    bool is_oversized() const NOEXCEPT;
    bool is_unspendable() const NOEXCEPT;

protected:
    script(operations&& ops, bool valid, bool fails) NOEXCEPT;
    script(const operations& ops, bool valid, bool fails) NOEXCEPT;

private:
    // TODO: move to config serialization wrapper.
    static script from_string(const std::string& mnemonic) NOEXCEPT;
    static script from_data(reader& source, bool prefix) NOEXCEPT;
    static size_t op_count(reader& source) NOEXCEPT;

    // Script should be stored as shared.
    operations ops_;

    // TODO: pack these flags.
    bool valid_;
    bool prefail_;
    ////bool roller_{ false };

public:
    using iterator = operations::const_iterator;

    /// Public mutable metadata access, not copied or compared for equality.
    mutable iterator offset;
};

typedef std::vector<script> scripts;

DECLARE_JSON_VALUE_CONVERTORS(script);
DECLARE_JSON_VALUE_CONVERTORS(script::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::script>
{
    size_t operator()(const bc::system::chain::script& value) const NOEXCEPT
    {
        return std::hash<bc::system::data_chunk>{}(value.to_data(false));
    }
};
} // namespace std

#endif
