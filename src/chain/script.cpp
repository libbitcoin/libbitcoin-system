/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <numeric>
#include <sstream>
#include <utility>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/enums/forks.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

using namespace bc::system::machine;

bool script::is_enabled(uint32_t active_forks, forks fork)
{
    return !is_zero(fork & active_forks);
}

// Constructors.
// ----------------------------------------------------------------------------

script::script()
  : script({}, false)
{
}

script::script(script&& other)
  : script(other)
{
}

script::script(const script& other)
  : script(other.ops_, other.valid_)
{
}

script::script(operations&& ops)
  : script(std::move(ops), true)
{
}

script::script(const operations& ops)
  : script(ops, true)
{
}

script::script(const data_slice& data, bool prefix)
  : script(stream::in::copy(data), prefix)
{
}

script::script(std::istream& stream, bool prefix)
  : script(read::bytes::istream(stream), prefix)
{
}

script::script(reader& source, bool prefix)
  : script(from_data(source, prefix))
{
}

script::script(const std::string& mnemonic)
  : script(from_string(mnemonic))
{
}

// protected
script::script(operations&& ops, bool valid)
  : ops_(std::move(ops)), valid_(valid)
{
}

// protected
script::script(const operations& ops, bool valid)
  : ops_(ops), valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

script& script::operator=(script&& other)
{
    ops_ = std::move(other.ops_);
    valid_ = other.valid_;
    return *this;
}

script& script::operator=(const script& other)
{
    ops_ = other.ops_;
    valid_ = other.valid_;
    return *this;
}

bool script::operator==(const script& other) const
{
    return (ops_ == other.ops_);
}

bool script::operator!=(const script& other) const
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
size_t script::op_count(reader& source)
{
    const auto start = source.get_position();
    auto count = zero;

    while (operation::count_op(source))
        ++count;

    // Stream errors ignored, caught in from_data.
    source.set_position(start);
    return count;
}

// static/private
script script::from_data(reader& source, bool prefix)
{
    operation foo(std::string(""));

    auto size = zero;
    auto start = zero;

    if (prefix)
    {
        size = source.read_size();
        start = source.get_position();

        // Limit the number of bytes that ops may consume.
        source.set_limit(size);
    }

    operations ops;
    ops.reserve(op_count(source));

    while (!source.is_exhausted())
        ops.emplace_back(source);

    if (prefix)
    {
        // Remove the stream limit.
        source.set_limit();

        // Stream was exhausted prior to reaching prefix size.
        if (source.get_position() - start != size)
            source.invalidate();
    }

    return { std::move(ops), source };
}

// static/private
script script::from_string(const std::string& mnemonic)
{
    // There is always one operation per non-empty string token.
    auto tokens = split(mnemonic);

    // Split always returns at least one token, and when trimming it will be
    // empty only if there was nothing but whitespace in the mnemonic.
    if (tokens.front().empty())
        tokens.clear();

    operations ops;
    ops.reserve(tokens.size());

    // Create an op list from the split tokens.
    for (const auto& token: tokens)
    {
        ops.emplace_back(token);
        if (!ops.back().is_valid())
            return {};
    }

    return { std::move(ops) };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk script::to_data(bool prefix) const
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size(prefix));
    stream::out::copy ostream(data);
    to_data(ostream, prefix);
    return data;
}

void script::to_data(std::ostream& stream, bool prefix) const
{
    write::bytes::ostream out(stream);
    to_data(out, prefix);
}

void script::to_data(writer& sink, bool prefix) const
{
    DEBUG_ONLY(const auto bytes = serialized_size(prefix);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    if (prefix)
        sink.write_variable(serialized_size(false));

    for (const auto& op: ops())
        op.to_data(sink);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

std::string script::to_string(uint32_t active_forks) const
{
    auto first = true;
    std::ostringstream text;

    for (const auto& op: ops())
    {
        text << (first ? "" : " ") << op.to_string(active_forks);
        first = false;
    }

    // An invalid operation has a specialized serialization.
    return text.str();
}

// Properties.
// ----------------------------------------------------------------------------

bool script::is_valid() const
{
    // Any byte vector is a valid script.
    // This is false only if the byte count did not match the size prefix.
    return valid_;
}

size_t script::serialized_size(bool prefix) const
{
    const auto op_size = [](size_t total, const operation& op)
    {
        return total + op.serialized_size();
    };

    auto size = std::accumulate(ops_.begin(), ops_.end(), zero, op_size);

    if (prefix)
        size += variable_size(size);

    return size;
}

const operations& script::ops() const
{
    return ops_;
}

// Signing (unversioned).
// ----------------------------------------------------------------------------

// ****************************************************************************
// CONSENSUS: sighash flags are carried in a single byte but are encoded as 4
// bytes in the signature hash preimage serialization.
// ****************************************************************************
inline hash_digest signature_hash(const transaction& tx, uint8_t flags)
{
    // There is no rational interpretation of a signature hash for a coinbase.
    BITCOIN_ASSERT(!tx.is_coinbase());

    const auto size = tx.serialized_size(false) + sizeof(uint32_t);
    data_chunk data(no_fill_byte_allocator);
    data.resize(size);
    write::bytes::copy out(data);

    tx.to_data(out, false);
    out.write_4_bytes_little_endian(flags);

    // TODO: add position tracking to writer (similar to reader).
    ////BITCOIN_ASSERT(out.get_position() == size)
    return bitcoin_hash(data);
}

//*****************************************************************************
// CONSENSUS: Due to masking of bits 6/7 (8 is the anyone_can_pay flag),
// there are 4 possible 7 bit values that can set "single" and 4 others that
// can set none, and yet all other values set "all".
//*****************************************************************************
inline coverage mask_sighash(uint8_t flags)
{
    switch (flags & coverage::mask)
    {
        case coverage::hash_single:
            return coverage::hash_single;
        case coverage::hash_none:
            return coverage::hash_none;
        default:
            return coverage::hash_all;
    }
}

static const script empty_script{};
static constexpr uint32_t zero_sequence{ 0 };

static hash_digest sign_single(const transaction& tx, uint32_t index,
    const script& subscript, uint8_t flags)
{
    //*************************************************************************
    // CONSENSUS: wacky satoshi behavior.
    //*************************************************************************
    if (index >= tx.outputs()->size())
        return one_hash;

    const auto& inputs = *tx.inputs();
    const auto& self = inputs[index];
    const auto any = !is_zero(flags & coverage::anyone_can_pay);

    auto ins = std::make_shared<input_ptrs>();
    ins->reserve(any ? one : inputs.size());

    if (any)
    {
        // Retain only the single input.
        ins->emplace_back(self);
    }
    else
    {
        ////auto it = inputs.begin();

        ////for (; *it != self; ++it)
        ////    ins->push_back(to_shared(input{ (*it)->point(), empty_script, zero_sequence }));

        ////// Erase all input scripts and sequences except self.
        ////++it;

        ////for (; it != inputs.end(); ++it)
        ////    ins->push_back(to_shared(input{ (*it)->point(), empty_script, zero_sequence }));
    }

    // Trim and clear outputs except that of the input index (guarded above).
    auto outs = std::make_shared<output_ptrs>();
    outs->reserve(add1(index));

    ////for (size_t out = 0; out < index; ++index)
    ////    outs->push_back(to_shared(output{ output::not_found, empty_script }));

    // Set the output of the input index (guarded above).
    outs->push_back((*tx.outputs())[index]);

    // Move new inputs and trimmed outputs to new transaction.
    return signature_hash({ tx.version(), tx.locktime(), ins, outs }, flags);
}

static hash_digest sign_none(const transaction& tx, uint32_t index,
    const script& subscript, uint8_t flags)
{
    const auto& inputs = *tx.inputs();
    const auto& self = inputs[index];
    const auto any = !is_zero(flags & coverage::anyone_can_pay);

    auto ins = std::make_shared<input_ptrs>();
    ins->reserve(any ? one : inputs.size());

    if (any)
    {
        // Retain only the single input.
        ins->emplace_back(self);
    }
    else
    {
        ////auto it = inputs.begin();

        ////for (; *it != self; ++it)
        ////    ins->push_back(to_shared(input{ (*it)->point(), empty_script, zero_sequence }));

        ////// Erase all input scripts and sequences except self.
        ////++it;

        ////for (; it != inputs.end(); ++it)
        ////    ins->push_back(to_shared(input{ (*it)->point(), empty_script, zero_sequence }));
    }

    // Move new inputs to new transaction and drop outputs.
    return signature_hash({ tx.version(), tx.locktime(), ins, {} }, flags);
}

static hash_digest sign_all(const transaction& tx, uint32_t index,
    const script& subscript, uint8_t flags)
{
    const auto& inputs = *tx.inputs();
    const auto& self = inputs[index];
    const auto any = !is_zero(flags & coverage::anyone_can_pay);

    auto ins = std::make_shared<input_ptrs>();
    ins->reserve(any ? one : inputs.size());

    if (any)
    {
        // Retain only the single input.
        ins->emplace_back(self);
    }
    else
    {
        ////auto& it = inputs.begin();

        ////for (; *it != self; ++it)
        ////    ins->push_back(to_shared(input{ (*it)->point(), empty_script, self->sequence() }));

        ////// Erase all input scripts except self.
        ////++it;

        ////for (; it != inputs.end(); ++it)
        ////    ins->push_back(to_shared(input{ (*it)->point(), empty_script, self->sequence() }));
    }

    // Move new inputs and copy outputs to new transaction.
    return signature_hash({ tx.version(), tx.locktime(), ins, tx.outputs() },
        flags);
}

// private/static
hash_digest script::generate_unversioned_signature_hash(const transaction& tx,
    uint32_t index, const script& subscript, uint8_t flags)
{
    // The sighash serializations are isolated for clarity and optimization.
    switch (mask_sighash(flags))
    {
        case coverage::hash_single:
            return sign_single(tx, index, subscript, flags);
        case coverage::hash_none:
            return sign_none(tx, index, subscript, flags);
        default:
        case coverage::hash_all:
            return sign_all(tx, index, subscript, flags);
    }
}

// Signing (version 0).
// ----------------------------------------------------------------------------

static size_t version_0_preimage_size(size_t script_size)
{
    return sizeof(uint32_t)
        + hash_size
        + hash_size
        + point::serialized_size()
        + script_size
        + sizeof(uint64_t)
        + sizeof(uint32_t)
        + hash_size
        + sizeof(uint32_t)
        + sizeof(uint32_t);
}

// private/static
hash_digest script::generate_version_0_signature_hash(const transaction& tx,
    uint32_t index, const script& subscript, uint64_t value, uint8_t flags,
    bool bip143)
{
    // bip143/v0: the way of serialization is changed.
    if (!bip143)
        return generate_unversioned_signature_hash(tx, index, subscript, flags);

    const auto flag = mask_sighash(flags);
    const auto any = !is_zero(flags & coverage::anyone_can_pay);
    const auto single = (flag == coverage::hash_single);
    //// const auto none = (flag == coverage::hash_none);
    const auto all = (flag == coverage::hash_all);

    const auto& input = (*tx.inputs())[index];
    const auto script_size = subscript.serialized_size(true);
    const auto size = version_0_preimage_size(script_size);

    data_chunk data(no_fill_byte_allocator);
    data.resize(size);
    write::bytes::copy out(data);

    // 1. transaction version (4).
    out.write_little_endian(tx.version());

    // 2. inpoints double sha256 hash (32).
    out.write_bytes(!any ? tx.points_hash() : null_hash);

    // 3. sequences double sha256 hash (32).
    out.write_bytes(!any && all ? tx.sequences_hash() : null_hash);

    // 4. outpoint (32-byte hash + 4-byte little endian).
    input->point().to_data(out);

    // 5. script of the input (with prefix).
    subscript.to_data(out, true);

    // 6. value of the output spent by this input (8).
    out.write_little_endian(value);

    // 7. sequence of the input (4).
    out.write_little_endian(input->sequence());

    // 8. outputs (or output) double hash, or null hash (32).
    out.write_bytes(all ? tx.outputs_hash() :
        (single && index < tx.outputs()->size() ?
            bitcoin_hash((*tx.outputs())[index]->to_data()) : null_hash));

    // 9. transaction locktime (4).
    out.write_little_endian(tx.locktime());

    // 10. hash type of the signature (4 [not 1]).
    out.write_4_bytes_little_endian(flags);

    // TODO: add position tracking to writer (similar to reader).
    ////BITCOIN_ASSERT(out.get_position() == size)
    return bitcoin_hash(data);
}

// Signing (unversioned and version 0).
// ----------------------------------------------------------------------------

// static
hash_digest script::generate_signature_hash(const transaction& tx,
    uint32_t index, const script& subscript, uint64_t value, uint8_t flags,
    script_version version, bool bip143)
{
    // This is merely invalid parameterization, not a consensus value.
    if (index > tx.inputs()->size())
        return {};

    switch (version)
    {
        case script_version::unversioned:
            return generate_unversioned_signature_hash(tx, index, subscript,
                flags);
        case script_version::zero:
            return generate_version_0_signature_hash(tx, index, subscript,
                value, flags, bip143);
        case script_version::reserved:
        default:
            BITCOIN_ASSERT_MSG(false, "invalid script version");
            return {};
    }
}

// static
bool script::check_signature(const ec_signature& signature,
    const data_slice& public_key, const script& subscript,
    const transaction& tx, uint32_t index, uint64_t value, uint8_t flags,
    script_version version, bool bip143)
{
    if (signature.empty() || public_key.empty())
        return false;

    // This always produces a valid signature hash, including one_hash.
    const auto sighash = chain::script::generate_signature_hash(tx,
        index, subscript, value, flags, version, bip143);

    // Validate the EC signature.
    return verify_signature(public_key, sighash, signature);
}

// static
bool script::create_endorsement(endorsement& out, const ec_secret& secret,
    const script& prevout_script, const transaction& tx, uint32_t index,
    uint64_t value, uint8_t flags, script_version version, bool bip143)
{
    out.reserve(max_endorsement_size);

    // This always produces a valid signature hash, including one_hash.
    const auto sighash = chain::script::generate_signature_hash(tx,
        index, prevout_script, value, flags, version, bip143);

    // Create the EC signature and encode as DER.
    ec_signature signature;
    if (!sign(signature, secret, sighash) || !encode_signature(out, signature))
        return false;

    // Add the sighash type to the end of the DER signature -> endorsement.
    out.push_back(flags);
    out.shrink_to_fit();
    return true;
}

// Utilities (static).
// ----------------------------------------------------------------------------

bool script::is_push_only(const operations& ops)
{
    const auto push = [](const operation& op)
    {
        return op.is_push();
    };

    return std::all_of(ops.begin(), ops.end(), push);
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip16 validation rules.
//*****************************************************************************
bool script::is_relaxed_push(const operations& ops)
{
    const auto push = [&](const operation& op)
    {
        return op.is_relaxed_push();
    };

    return std::all_of(ops.begin(), ops.end(), push);
}

//*****************************************************************************
// CONSENSUS: BIP34 requires coinbase input script to begin with one byte that
// indicates the height size. This is inconsistent with an extreme future where
// the size byte overflows. However satoshi actually requires nominal encoding.
//*****************************************************************************
bool script::is_coinbase_pattern(const operations& ops, size_t height)
{
    return !ops.empty()
        && ops[0].is_nominal_push()
        && ops[0].data() == number(height).data();
}

////// This is slightly more efficient because the script does not get parsed,
////// but the static template implementation is more self-explanatory.
////bool script::is_coinbase_pattern(size_t height) const
////{
////    const auto actual = to_data(false);
////
////    // Create the expected script as a non-minimal byte vector.
////    script compare(operations{ { number(height).data(), false } });
////    const auto expected = compare.to_data(false);
////
////    // Require the actual script start with the expected coinbase script.
////    return std::equal(expected.begin(), expected.end(), actual.begin());
////}

//*****************************************************************************
// CONSENSUS: this pattern is used to commit to bip141 witness data.
//*****************************************************************************
bool script::is_commitment_pattern(const operations& ops)
{
    static const auto header = to_big_endian(witness_head);

    // Bytes after commitment are optional with no consensus meaning (bip141).
    // Commitment is not executable so invalid trailing operations are allowed.
    return ops.size() > 1
        && ops[0].code() == opcode::op_return
        && ops[1].code() == opcode::push_size_36
        && std::equal(header.begin(), header.end(), ops[1].data().begin());
}

//*****************************************************************************
// CONSENSUS: this pattern is used in bip141 validation rules.
//*****************************************************************************
bool script::is_witness_program_pattern(const operations& ops)
{
    return ops.size() == 2
        && ops[0].is_version()
        && ops[1].data().size() >= min_witness_program
        && ops[1].data().size() <= max_witness_program;
}

// The satoshi client tests for 83 bytes total. This allows for the waste of
// one byte to represent up to 75 bytes using the push_one_size opcode.
// It also allows any number of push ops and limits it to 0 value and 1 per tx.
////bool script::is_pay_null_data_pattern(const operations& ops)
////{
////    static constexpr auto op_76 = static_cast<uint8_t>(opcode::push_one_size);
////
////    return ops.size() >= 2
////        && ops[0].code() == opcode::return_
////        && static_cast<uint8_t>(ops[1].code()) <= op_76
////        && ops[1].data().size() <= max_null_data_size;
////}

// The satoshi client enables configurable data size for policy.
bool script::is_pay_null_data_pattern(const operations& ops)
{
    return ops.size() == 2
        && ops[0].code() == opcode::op_return
        && ops[1].is_minimal_push()
        && ops[1].data().size() <= max_null_data_size;
}

// TODO: expand this to the 20 signature op_check_multisig limit.
// The current 16 (or 20) limit does not affect server indexing because bare
// multisig is not indexable and p2sh multisig is byte-limited to 15 sigs.
// The satoshi client policy limit is 3 signatures for bare multisig.
bool script::is_pay_multisig_pattern(const operations& ops)
{
    static constexpr auto op_1 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr auto op_16 = static_cast<uint8_t>(opcode::push_positive_16);

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

    for (auto op = std::next(ops.begin()); op != std::prev(ops.end(), 2); ++op)
        if (!is_public_key(op->data()))
            return false;

    return true;
}

// The satoshi client considers this non-standard for policy.
bool script::is_pay_public_key_pattern(const operations& ops)
{
    return ops.size() == 2
        && is_public_key(ops[0].data())
        && ops[1].code() == opcode::checksig;
}

bool script::is_pay_key_hash_pattern(const operations& ops)
{
    return ops.size() == 5
        && ops[0].code() == opcode::dup
        && ops[1].code() == opcode::hash160
        && ops[2].data().size() == short_hash_size
        && ops[3].code() == opcode::equalverify
        && ops[4].code() == opcode::checksig;
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip16 validation rules.
//*****************************************************************************
bool script::is_pay_script_hash_pattern(const operations& ops)
{
    return ops.size() == 3
        && ops[0].code() == opcode::hash160
        && ops[1].code() == opcode::push_size_20
        && ops[2].code() == opcode::equal;
}

bool script::is_pay_witness_pattern(const operations& ops)
{
    return ops.size() == 2
        && ops[0].is_version()
        && ops[1].is_push();
}

bool script::is_pay_witness_key_hash_pattern(const operations& ops)
{
    return ops.size() == 2
        && ops[0].code() == opcode::push_size_0
        && ops[1].code() == opcode::push_size_20;
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
//*****************************************************************************
bool script::is_pay_witness_script_hash_pattern(const operations& ops)
{
    return ops.size() == 2
        && ops[0].code() == opcode::push_size_0
        && ops[1].code() == opcode::push_size_32;
}

// The first push is based on wacky satoshi op_check_multisig behavior that
// we must perpetuate, though it's appearance here is policy not consensus.
// Limiting to push_size_0 eliminates pattern ambiguity with little downside.
bool script::is_sign_multisig_pattern(const operations& ops)
{
    const auto endorsement = [](const operation& op)
    {
        return is_endorsement(op.data());
    };

    return ops.size() >= 2
        && ops[0].code() == opcode::push_size_0
        && std::all_of(std::next(ops.begin()), ops.end(), endorsement);
}

bool script::is_sign_public_key_pattern(const operations& ops)
{
    return ops.size() == 1
        && is_endorsement(ops[0].data());
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
//*****************************************************************************
bool script::is_sign_key_hash_pattern(const operations& ops)
{
    return ops.size() == 2
        && is_endorsement(ops[0].data())
        && is_public_key(ops[1].data());
}

// Ambiguous with is_sign_key_hash when second/last op is a public key.
// Ambiguous with is_sign_public_key_pattern when only op is an endorsement.
bool script::is_sign_script_hash_pattern(const operations& ops)
{
    return !ops.empty()
        && is_push_only(ops)
        && !ops.back().data().empty();
}

operations script::to_pay_null_data_pattern(const data_slice& data)
{
    if (data.size() > max_null_data_size)
        return {};

    return operations
    {
        { opcode::op_return },
        { to_chunk(data), false }
    };
}

operations script::to_pay_public_key_pattern(const data_slice& point)
{
    if (!is_public_key(point))
        return {};

    return operations
    {
        { to_chunk(point), false },
        { opcode::checksig }
    };
}

operations script::to_pay_key_hash_pattern(const short_hash& hash)
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

operations script::to_pay_script_hash_pattern(const short_hash& hash)
{
    return operations
    {
        { opcode::hash160 },
        { to_chunk(hash), false },
        { opcode::equal }
    };
}

// TODO: limit to 20 for consistency with op_check_multisig.
operations script::to_pay_multisig_pattern(uint8_t signatures,
    const compressed_list& points)
{
    return to_pay_multisig_pattern(signatures,
        to_stack<ec_compressed_size>(points));
}

// TODO: expand this to a 20 signature limit.
// This supports up to 16 signatures, however check_multisig is limited to 20.
// The embedded script is limited to 520 bytes, an effective limit of 15 for
// p2sh multisig, which can be as low as 7 when using all uncompressed keys.
operations script::to_pay_multisig_pattern(uint8_t signatures,
    const data_stack& points)
{
    static constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    static constexpr auto zero = op_81 - 1;
    static constexpr auto max = op_96 - zero;

    const auto m = signatures;
    const auto n = points.size();

    if (m < 1 || m > n || n < 1 || n > max)
        return operations();

    const auto op_m = static_cast<opcode>(m + zero);
    const auto op_n = static_cast<opcode>(points.size() + zero);

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

operations script::to_pay_witness_pattern(uint8_t version, const data_slice& data)
{
    return
    {
        { operation::opcode_from_version(version) },
        { to_chunk(data), false },
    };
}

operations script::to_pay_witness_key_hash_pattern(const short_hash& hash)
{
    return
    {
        { opcode::push_size_0 },
        { to_chunk(hash), false },
    };
}

operations script::to_pay_witness_script_hash_pattern(const hash_digest& hash)
{
    return
    {
        { opcode::push_size_0 },
        { to_chunk(hash), false }
    };
}

// Utilities (non-static).
// ----------------------------------------------------------------------------

hash_digest script::to_payments_key() const
{
    return sha256_hash(to_data(false));
}

const data_chunk& script::witness_program() const
{
    static const data_chunk empty;
    return is_witness_program_pattern(ops()) ? ops()[1].data() : empty;
}

script_version script::version() const
{
    if (!is_witness_program_pattern(ops()))
        return script_version::unversioned;

    switch (ops_.front().code())
    {
        case opcode::push_size_0:
            return script_version::zero;
        default:
            return script_version::reserved;
    }
}

// Caller should test for is_sign_script_hash_pattern when sign_key_hash result
// as it is possible for an input script to match both patterns.
script_pattern script::pattern() const
{
    const auto input = output_pattern();
    return input == script_pattern::non_standard ? input_pattern() : input;
}

// Output patterns are mutually and input unambiguous.
// The bip141 coinbase pattern is not tested here, must test independently.
script_pattern script::output_pattern() const
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
script_pattern script::input_pattern() const
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

bool script::is_pay_to_witness(uint32_t forks) const
{
    // This is used internally as an optimization over using script::pattern.
    return is_enabled(forks, forks::bip141_rule) &&
        is_witness_program_pattern(ops());
}

bool script::is_pay_to_script_hash(uint32_t forks) const
{
    // This is used internally as an optimization over using script::pattern.
    return is_enabled(forks, forks::bip16_rule) &&
        is_pay_script_hash_pattern(ops());
}

// Count 1..16 multisig accurately for embedded (bip16) and witness (bip141).
inline size_t multisig_sigops(bool accurate, opcode code)
{
    return accurate && operation::is_positive(code) ?
        operation::opcode_to_positive(code) : multisig_default_sigops;
}

inline bool is_single_sigop(opcode code)
{
    return code == opcode::checksig || code == opcode::checksigverify;
}

inline bool is_multiple_sigop(opcode code)
{
    return code == opcode::checkmultisig || code == opcode::checkmultisigverify;
}

size_t script::sigops(bool accurate) const
{
    auto total = zero;
    auto preceding = opcode::push_negative_1;

    for (const auto& op: ops())
    {
        const auto code = op.code();

        if (is_single_sigop(code))
            total = ceilinged_add(total, one);
        else if (is_multiple_sigop(code))
            total = ceilinged_add(total, multisig_sigops(accurate, preceding));

        preceding = code;
    }

    return total;
}

bool script::is_oversized() const
{
    return serialized_size(false) > max_script_size;
}

// An unspendable script is any that can provably not be spent under any
// circumstance. This allows for exclusion of the output as unspendable.
// The criteria below are not comprehensive but are fast to evaluate.
bool script::is_unspendable() const
{
    if (ops_.empty())
        return false;

    const auto& code = ops_.front().code();

    // There is no condition prior to the first opcode in a script.
    return operation::is_reserved(code) || operation::is_invalid(code);
}

// Validation.
// ----------------------------------------------------------------------------

code script::verify(const transaction& tx, uint32_t index, uint32_t forks,
    const script& prevout_script, uint64_t value)
{
    if (index >= tx.inputs()->size())
        return error::inputs_overflow;

    code ec;
    bool witnessed;
    const auto& in = (*tx.inputs())[index];

    // TODO: Implement original op_codeseparator concatentaion [< 0.3.6].
    // TODO: Implement combined script size limit soft fork (20,000) [0.3.6+].

    // Evaluate input script.
    program input(in->script(), tx, index, forks);
    if ((ec = input.evaluate()))
        return ec;

    // Evaluate output script using stack result from input script.
    program prevout(prevout_script, input);
    if ((ec = prevout.evaluate()))
        return ec;

    // This precludes bare witness programs of -0 (undocumented).
    if (!prevout.stack_result(false))
        return error::stack_false;

    // Triggered by output script push of version and witness program (bip141).
    if ((witnessed = prevout_script.is_pay_to_witness(forks)))
    {
        // The input script must be empty (bip141).
        if (!in->script().ops().empty())
            return error::dirty_witness;

        // Validate the native script.
        if ((ec = in->witness().verify(tx, index, forks, prevout_script, value)))
            return ec;
    }

    // p2sh and p2w are mutually exclusive.
    else if (prevout_script.is_pay_to_script_hash(forks))
    {
        if (!is_relaxed_push(in->script().ops()))
            return error::invalid_script_embed;

        // Embedded script must be at the top of the stack (bip16).
        script embedded_script(input.pop(), false);

        program embedded(embedded_script, std::move(input), true);
        if ((ec = embedded.evaluate()))
            return ec;

        // This precludes embedded witness programs of -0 (undocumented).
        if (!embedded.stack_result(false))
            return error::stack_false;

        // Triggered by embedded push of version and witness program (bip141).
        if ((witnessed = embedded_script.is_pay_to_witness(forks)))
        {
            // The input script must be a push of the embedded_script (bip141).
            if (in->script().ops().size() != one)
                return error::dirty_witness;

            // Validate the non-native script.
            if ((ec = in->witness().verify(tx, index, forks, embedded_script,
                value)))
                return ec;
        }
    }

    // Witness must be empty if no bip141 or invalid witness program (bip141).
    if (!witnessed && !in->witness().stack().empty())
        return error::unexpected_witness;

    return error::success;
}

////code script::verify(const transaction& tx, uint32_t index,
////    uint32_t forks)
////{
////    if (index >= tx.inputs().size())
////        return error::inputs_overflow;
////
////    const auto& in = tx.inputs()[index];
////    const auto prevout = in.prevout;
////    return verify(tx, index, forks, prevout.script(), prevout.value());
////}

} // namespace chain
} // namespace system
} // namespace libbitcoin
