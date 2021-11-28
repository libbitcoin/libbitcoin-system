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
#include <numeric>
#include <sstream>
#include <utility>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/chain/enums/rule_fork.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/enums/sighash_algorithm.hpp>
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

bool script::is_enabled(uint32_t active_forks, rule_fork fork)
{
    return !is_zero(fork & active_forks);
}

// Constructors.
//-----------------------------------------------------------------------------

script::script()
  : script(operation::list{}, true)
{
}

script::script(script&& other)
  : script(std::move(other.ops_), other.valid_)
{
}

script::script(const script& other)
  : script(other.ops_, other.valid_)
{
}

script::script(operation::list&& ops)
  : script(std::move(ops), true)
{
}

script::script(const operation::list& ops)
  : script(ops, true)
{
}

script::script(const data_chunk& encoded, bool prefix)
{
    from_data(encoded, prefix);
}

script::script(std::istream& stream, bool prefix)
{
    from_data(stream, prefix);
}

script::script(reader& source, bool prefix)
{
    from_data(source, prefix);
}

// protected
script::script(operation::list&& ops, bool valid)
  : ops_(std::move(ops)), valid_(true)
{
}

// protected
script::script(const operation::list& ops, bool valid)
  : ops_(ops), valid_(valid)
{
}

// Operators.
//-----------------------------------------------------------------------------

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
    return ops_ == other.ops_;
}

bool script::operator!=(const script& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

bool script::from_data(const data_chunk& encoded, bool prefix)
{
    stream::in::copy istream(encoded);
    return from_data(istream, prefix);
}

bool script::from_data(std::istream& stream, bool prefix)
{
    read::bytes::istream source(stream);
    return from_data(source, prefix);
}

bool script::from_data(reader& source, bool prefix)
{
    reset();

    auto size = zero;
    auto start = zero;

    if (prefix)
    {
        size = source.read_size();
        start = source.get_position();

        // Limit the number of bytes that ops may consume.
        source.set_limit(size);
    }

    operation op;
    while (!source.is_exhausted())
    {
        op.from_data(source);
        ops_.push_back(op);
    }

    if (prefix)
    {
        // Remove the stream limit.
        source.set_limit();

        // Stream was exhausted prior to reaching prefix size.
        if (source.get_position() - start != size)
            source.invalidate();
    }

    if (!source)
        reset();

    valid_ = source;
    return valid_;
}

bool script::from_string(const std::string& mnemonic)
{
    reset();
    valid_ = true;

    // There is always one operation per string token.
    const auto tokens = split(mnemonic);

    // Split always returns at least one token, and when trimming it will be
    // empty only there was nothing but whitespace in the mnemonic.
    if (tokens.front().empty())
        return true;

    // Create an op list from the split tokens, one operation per token.
    ops_.reserve(tokens.size());
    operation op;

    for (const auto& token: tokens)
    {
        valid_ &= op.from_string(token);
        ops_.push_back(op);
    }

    return valid_;
}

// protected
void script::reset()
{
    ops_.clear();
    ops_.shrink_to_fit();
    valid_ = false;
}

bool script::is_valid() const
{
    // Any byte vector is a valid script.
    // This is false only if the byte count did not match the size prefix.
    return valid_;
}

// Serialization.
//-----------------------------------------------------------------------------

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
    DEBUG_ONLY(const auto size = serialized_size(prefix);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    if (prefix)
        sink.write_variable(serialized_size(false));

    for (const auto& op: ops_)
        op.to_data(sink);

    BITCOIN_ASSERT(sink.get_position() - start == size);
}

std::string script::to_string(uint32_t active_forks) const
{
    auto first = true;
    std::ostringstream text;

    for (const auto& op: ops_)
    {
        text << (first ? "" : " ") << op.to_string(active_forks);
        first = false;
    }

    // An invalid operation has a specialized serialization.
    return text.str();
}

// Iteration.
//-----------------------------------------------------------------------------

void script::clear()
{
    reset();
}

bool script::empty() const
{
    return ops_.empty();
}

size_t script::size() const
{
    return ops_.size();
}

const operation& script::front() const
{
    BITCOIN_ASSERT(!empty());
    return ops_.front();
}

const operation& script::back() const
{
    BITCOIN_ASSERT(!empty());
    return ops_.back();
}

operation::iterator script::begin() const
{
    return ops_.begin();
}

operation::iterator script::end() const
{
    return ops_.end();
}

const operation& script::operator[](size_t index) const
{
    BITCOIN_ASSERT(index < size());
    return ops_[index];
}

// Properties.
//-----------------------------------------------------------------------------

size_t script::serialized_size(bool prefix) const
{
    const auto op_size = [](size_t total, const operation& op)
    {
        return total + op.serialized_size();
    };

    auto size =  std::accumulate(ops_.begin(), ops_.end(), zero, op_size);

    if (prefix)
        size += variable_size(size);

    return size;
}

const operation::list& script::operations() const
{
    return ops_;
}

hash_digest script::to_payments_key() const
{
    return sha256_hash(to_data(false));
}

// Signing (unversioned).
//-----------------------------------------------------------------------------

// ****************************************************************************
// CONSENSUS: sighash flags are carried in a single byte but are encoded as 4
// bytes in the signature hash preimage serialization.
// ****************************************************************************
inline hash_digest signature_hash(const transaction& tx, uint8_t flags)
{
    // There is no rational interpretation of a signature hash for a coinbase.
    BITCOIN_ASSERT(!tx.is_coinbase());

    const auto size = tx.serialized_size() + sizeof(uint32_t);
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
inline sighash_algorithm mask_sighash(uint8_t flags)
{
    switch (flags & sighash_algorithm::mask)
    {
        case sighash_algorithm::hash_single:
            return sighash_algorithm::hash_single;
        case sighash_algorithm::hash_none:
            return sighash_algorithm::hash_none;
        default:
            return sighash_algorithm::hash_all;
    }
}

static hash_digest sign_none(const transaction& tx, uint32_t index,
    const script& subscript, uint8_t flags)
{
    input::list ins;
    const auto& inputs = tx.inputs();
    const auto any = !is_zero(flags & sighash_algorithm::anyone_can_pay);
    ins.reserve(any ? one : inputs.size());

    BITCOIN_ASSERT(index < inputs.size());
    const auto& self = inputs[index];

    if (any)
    {
        // Retain only self.
        ins.emplace_back(self.previous_output(), subscript, self.sequence());
    }
    else
    {
        // Erase all input scripts and sequences.
        for (const auto& input: inputs)
            ins.emplace_back(input.previous_output(), script{}, 0);

        // Replace self that is lost in the loop.
        ins[index] = { self.previous_output(), subscript, self.sequence() };
    }

    // Move new inputs to new transaction and drop outputs.
    return signature_hash({ tx.version(), tx.locktime(), std::move(ins), {} },
        flags);
}

static hash_digest sign_single(const transaction& tx, uint32_t index,
    const script& subscript, uint8_t flags)
{
    input::list ins;
    const auto& inputs = tx.inputs();
    const auto any = !is_zero(flags & sighash_algorithm::anyone_can_pay);
    ins.reserve(any ? one : inputs.size());

    BITCOIN_ASSERT(index < inputs.size());
    const auto& self = inputs[index];

    if (any)
    {
        // Retain only self.
        ins.emplace_back(self.previous_output(), subscript, self.sequence());
    }
    else
    {
        // Erase all input scripts and sequences.
        for (const auto& input: inputs)
            ins.emplace_back(input.previous_output(), script{}, 0);

        // Replace self that is lost in the loop.
        ins[index] = { self.previous_output(), subscript, self.sequence() };
    }

    // Trim and clear outputs except that of specified input index.
    const auto& outputs = tx.outputs();
    output::list outs(add1(index));

    BITCOIN_ASSERT(index < outputs.size());
    outs.back() = outputs[index];

    // Move new inputs and new outputs to new transaction.
    return signature_hash({ tx.version(), tx.locktime(), std::move(ins),
        std::move(outs) }, flags);
}

static hash_digest sign_all(const transaction& tx, uint32_t index,
    const script& subscript, uint8_t flags)
{
    input::list ins;
    const auto& inputs = tx.inputs();
    const auto any = !is_zero(flags & sighash_algorithm::anyone_can_pay);
    ins.reserve(any ? one : inputs.size());

    BITCOIN_ASSERT(index < inputs.size());
    const auto& self = inputs[index];

    if (any)
    {
        // Retain only self.
        ins.emplace_back(self.previous_output(), subscript, self.sequence());
    }
    else
    {
        // Erase all input scripts.
        for (const auto& input: inputs)
            ins.emplace_back(input.previous_output(), script{}, input.sequence());

        // Replace self that is lost in the loop.
        ins[index] = { self.previous_output(), subscript, self.sequence() };
    }

    // Move new inputs and copy outputs to new transaction.
    transaction out(tx.version(), tx.locktime(), ins, tx.outputs());
    return signature_hash(out, flags);
}

static bool is_index_overflow(const transaction& tx, uint32_t index,
    sighash_algorithm sighash)
{
    return index >= tx.inputs().size() ||
        (index >= tx.outputs().size() &&
            sighash == sighash_algorithm::hash_single);
}

// private/static
hash_digest script::generate_unversioned_signature_hash(const transaction& tx,
    uint32_t index, const script& subscript, uint8_t flags)
{
    static const auto one_hash = base16_hash(
        "0000000000000000000000000000000000000000000000000000000000000001");

    const auto sighash = mask_sighash(flags);

    //*************************************************************************
    // CONSENSUS: wacky satoshi behavior (continuing with one_hash).
    //*************************************************************************
    if (is_index_overflow(tx, index, sighash))
        return one_hash;

    // The sighash serializations are isolated for clarity and optimization.
    switch (sighash)
    {
        case sighash_algorithm::hash_none:
            return sign_none(tx, index, subscript, flags);
        case sighash_algorithm::hash_single:
            return sign_single(tx, index, subscript, flags);
        default:
        case sighash_algorithm::hash_all:
            return sign_all(tx, index, subscript, flags);
    }
}

// Signing (version 0).
//-----------------------------------------------------------------------------

static size_t version_0_preimage_size(size_t script_size)
{
    return sizeof(uint32_t)
        + hash_size
        + hash_size
        + point::satoshi_fixed_size()
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

    const auto sighash = mask_sighash(flags);
    const auto any = !is_zero(flags & sighash_algorithm::anyone_can_pay);
    const auto single = (sighash == sighash_algorithm::hash_single);
    //// const auto none = (sighash == sighash_algorithm::hash_none);
    const auto all = (sighash == sighash_algorithm::hash_all);

    // Unlike unversioned algorithm this does not allow an invalid input index.
    BITCOIN_ASSERT(index < tx.inputs().size());
    const auto& input = tx.inputs()[index];
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
    input.previous_output().to_data(out);

    // 5. script of the input (with prefix).
    subscript.to_data(out, true);

    // 6. value of the output spent by this input (8).
    out.write_little_endian(value);

    // 7. sequence of the input (4).
    out.write_little_endian(input.sequence());

    // 8. outputs (or output) double hash, or null hash (32).
    out.write_bytes(all ? tx.outputs_hash() :
        (single && index < tx.outputs().size() ?
            bitcoin_hash(tx.outputs()[index].to_data()) : null_hash));

    // 9. transaction locktime (4).
    out.write_little_endian(tx.locktime());

    // 10. hash type of the signature (4 [not 1]).
    out.write_4_bytes_little_endian(flags);

    // TODO: add position tracking to writer (similar to reader).
    ////BITCOIN_ASSERT(out.get_position() == size)
    return bitcoin_hash(data);
}

// Signing (unversioned and version 0).
//-----------------------------------------------------------------------------

// static
hash_digest script::generate_signature_hash(const transaction& tx,
    uint32_t index, const script& subscript, uint64_t value, uint8_t flags,
    script_version version, bool bip143)
{
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
//-----------------------------------------------------------------------------

bool script::is_push_only(const operation::list& ops)
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
bool script::is_relaxed_push(const operation::list& ops)
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
bool script::is_coinbase_pattern(const operation::list& ops, size_t height)
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
////    script compare(operation::list{ { number(height).data(), false } });
////    const auto expected = compare.to_data(false);
////
////    // Require the actual script start with the expected coinbase script.
////    return std::equal(expected.begin(), expected.end(), actual.begin());
////}

//*****************************************************************************
// CONSENSUS: this pattern is used to commit to bip141 witness data.
//*****************************************************************************
bool script::is_commitment_pattern(const operation::list& ops)
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
bool script::is_witness_program_pattern(const operation::list& ops)
{
    return ops.size() == 2
        && ops[0].is_version()
        && ops[1].data().size() >= min_witness_program
        && ops[1].data().size() <= max_witness_program;
}

// The satoshi client tests for 83 bytes total. This allows for the waste of
// one byte to represent up to 75 bytes using the push_one_size opcode.
// It also allows any number of push ops and limits it to 0 value and 1 per tx.
////bool script::is_pay_null_data_pattern(const operation::list& ops)
////{
////    static constexpr auto op_76 = static_cast<uint8_t>(opcode::push_one_size);
////
////    return ops.size() >= 2
////        && ops[0].code() == opcode::return_
////        && static_cast<uint8_t>(ops[1].code()) <= op_76
////        && ops[1].data().size() <= max_null_data_size;
////}

// The satoshi client enables configurable data size for policy.
bool script::is_pay_null_data_pattern(const operation::list& ops)
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
bool script::is_pay_multisig_pattern(const operation::list& ops)
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
bool script::is_pay_public_key_pattern(const operation::list& ops)
{
    return ops.size() == 2
        && is_public_key(ops[0].data())
        && ops[1].code() == opcode::checksig;
}

bool script::is_pay_key_hash_pattern(const operation::list& ops)
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
bool script::is_pay_script_hash_pattern(const operation::list& ops)
{
    return ops.size() == 3
        && ops[0].code() == opcode::hash160
        && ops[1].code() == opcode::push_size_20
        && ops[2].code() == opcode::equal;
}

bool script::is_pay_witness_pattern(const operation::list& ops)
{
    return ops.size() == 2
        && ops[0].is_version()
        && ops[1].is_push();
}

bool script::is_pay_witness_key_hash_pattern(const operation::list& ops)
{
    return ops.size() == 2
        && ops[0].code() == opcode::push_size_0
        && ops[1].code() == opcode::push_size_20;
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
//*****************************************************************************
bool script::is_pay_witness_script_hash_pattern(const operation::list& ops)
{
    return ops.size() == 2
        && ops[0].code() == opcode::push_size_0
        && ops[1].code() == opcode::push_size_32;
}

// The first push is based on wacky satoshi op_check_multisig behavior that
// we must perpetuate, though it's appearance here is policy not consensus.
// Limiting to push_size_0 eliminates pattern ambiguity with little downside.
bool script::is_sign_multisig_pattern(const operation::list& ops)
{
    const auto endorsement = [](const operation& op)
    {
        return is_endorsement(op.data());
    };

    return ops.size() >= 2
        && ops[0].code() == opcode::push_size_0
        && std::all_of(std::next(ops.begin()), ops.end(), endorsement);
}

bool script::is_sign_public_key_pattern(const operation::list& ops)
{
    return ops.size() == 1
        && is_endorsement(ops[0].data());
}

//*****************************************************************************
// CONSENSUS: this pattern is used to activate bip141 validation rules.
//*****************************************************************************
bool script::is_sign_key_hash_pattern(const operation::list& ops)
{
    return ops.size() == 2
        && is_endorsement(ops[0].data())
        && is_public_key(ops[1].data());
}

// Ambiguous with is_sign_key_hash when second/last op is a public key.
// Ambiguous with is_sign_public_key_pattern when only op is an endorsement.
bool script::is_sign_script_hash_pattern(const operation::list& ops)
{
    return !ops.empty()
        && is_push_only(ops)
        && !ops.back().data().empty();
}

operation::list script::to_pay_null_data_pattern(const data_slice& data)
{
    if (data.size() > max_null_data_size)
        return {};

    return operation::list
    {
        { opcode::op_return },
        { to_chunk(data), false }
    };
}

operation::list script::to_pay_public_key_pattern(const data_slice& point)
{
    if (!is_public_key(point))
        return {};

    return operation::list
    {
        { to_chunk(point), false },
        { opcode::checksig }
    };
}

operation::list script::to_pay_key_hash_pattern(const short_hash& hash)
{
    return operation::list
    {
        { opcode::dup },
        { opcode::hash160 },
        { to_chunk(hash), false },
        { opcode::equalverify },
        { opcode::checksig }
    };
}

operation::list script::to_pay_script_hash_pattern(const short_hash& hash)
{
    return operation::list
    {
        { opcode::hash160 },
        { to_chunk(hash), false },
        { opcode::equal }
    };
}

// TODO: limit to 20 for consistency with op_check_multisig.
operation::list script::to_pay_multisig_pattern(uint8_t signatures,
    const compressed_list& points)
{
    return to_pay_multisig_pattern(signatures,
        to_stack<ec_compressed_size>(points));
}

// TODO: expand this to a 20 signature limit.
// This supports up to 16 signatures, however check_multisig is limited to 20.
// The embedded script is limited to 520 bytes, an effective limit of 15 for
// p2sh multisig, which can be as low as 7 when using all uncompressed keys.
operation::list script::to_pay_multisig_pattern(uint8_t signatures,
    const data_stack& points)
{
    static constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    static constexpr auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    static constexpr auto zero = op_81 - 1;
    static constexpr auto max = op_96 - zero;

    const auto m = signatures;
    const auto n = points.size();

    if (m < 1 || m > n || n < 1 || n > max)
        return operation::list();

    const auto op_m = static_cast<opcode>(m + zero);
    const auto op_n = static_cast<opcode>(points.size() + zero);

    operation::list ops;
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

operation::list script::to_pay_witness_pattern(uint8_t version, const data_slice& data)
{
    return
    {
        { operation::opcode_from_version(version) },
        { to_chunk(data), false },
    };
}

operation::list script::to_pay_witness_key_hash_pattern(const short_hash& hash)
{
    return
    {
        { opcode::push_size_0 },
        { to_chunk(hash), false },
    };
}

operation::list script::to_pay_witness_script_hash_pattern(const hash_digest& hash)
{
    return
    {
        { opcode::push_size_0 },
        { to_chunk(hash), false }
    };
}

// Utilities (non-static).
//-----------------------------------------------------------------------------

const data_chunk& script::witness_program() const
{
    static const data_chunk empty;
    return is_witness_program_pattern(ops_) ? ops_[1].data() : empty;
}

script_version script::version() const
{
    if (!is_witness_program_pattern(ops_))
        return script_version::unversioned;

    switch (ops_[0].code())
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
    if (is_pay_key_hash_pattern(ops_))
        return script_pattern::pay_key_hash;

    if (is_pay_script_hash_pattern(ops_))
        return script_pattern::pay_script_hash;

    if (is_pay_null_data_pattern(ops_))
        return script_pattern::pay_null_data;

    if (is_pay_public_key_pattern(ops_))
        return script_pattern::pay_public_key;

    // Limited to 16 signatures though op_check_multisig allows 20.
    if (is_pay_multisig_pattern(ops_))
        return script_pattern::pay_multisig;

    return script_pattern::non_standard;
}

// A sign_key_hash result always implies sign_script_hash as well.
// The bip34 coinbase pattern is not tested here, must test independently.
script_pattern script::input_pattern() const
{
    if (is_sign_key_hash_pattern(ops_))
        return script_pattern::sign_key_hash;

    // This must follow is_sign_key_hash_pattern for ambiguity comment to hold.
    if (is_sign_script_hash_pattern(ops_))
        return script_pattern::sign_script_hash;

    if (is_sign_public_key_pattern(ops_))
        return script_pattern::sign_public_key;

    if (is_sign_multisig_pattern(ops_))
        return script_pattern::sign_multisig;

    return script_pattern::non_standard;
}

bool script::is_pay_to_witness(uint32_t forks) const
{
    // This is used internally as an optimization over using script::pattern.
    return is_enabled(forks, rule_fork::bip141_rule) &&
        is_witness_program_pattern(ops_);
}

bool script::is_pay_to_script_hash(uint32_t forks) const
{
    // This is used internally as an optimization over using script::pattern.
    return is_enabled(forks, rule_fork::bip16_rule) &&
        is_pay_script_hash_pattern(ops_);
}

// Count 1..16 multisig accurately for embedded (bip16) and witness (bip141).
inline size_t multisig_sigops(bool accurate, opcode code)
{
    return accurate && operation::is_positive(code) ?
        operation::opcode_to_positive(code) : multisig_default_sigops;
}

size_t script::sigops(bool accurate) const
{
    size_t total = 0;
    auto preceding = opcode::push_negative_1;

    for (const auto& op: ops_)
    {
        const auto code = op.code();

        if (code == opcode::checksig ||
            code == opcode::checksigverify)
        {
            ++total;
        }
        else if (
            code == opcode::checkmultisig ||
            code == opcode::checkmultisigverify)
        {
            total += multisig_sigops(accurate, preceding);
        }

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
// The criteria below are not be comprehensive but are fast to evaluate.
bool script::is_unspendable() const
{
    if (ops_.empty())
        return false;

    const auto& code = ops_[0].code();

    // There is no condition prior to the first opcode in a script.
    return operation::is_reserved(code) || operation::is_invalid(code);
}

// Validation.
//-----------------------------------------------------------------------------

code script::verify(const transaction& tx, uint32_t index, uint32_t forks,
    const script& prevout_script, uint64_t value)
{
    if (index >= tx.inputs().size())
        return error::inputs_overflow;

    code ec;
    bool witnessed;
    const auto& in = tx.inputs()[index];

    // TODO: Implement original op_codeseparator concatentaion [< 0.3.6].
    // TODO: Implement combined script size limit soft fork (20,000) [0.3.6+].

    // Evaluate input script.
    program input(in.script(), tx, index, forks);
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
        if (!in.script().empty())
            return error::dirty_witness;

        // Validate the native script.
        if ((ec = in.witness().verify(tx, index, forks, prevout_script, value)))
            return ec;
    }

    // p2sh and p2w are mutually exclusive.
    else if (prevout_script.is_pay_to_script_hash(forks))
    {
        if (!is_relaxed_push(in.script().operations()))
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
            if (in.script().size() != 1)
                return error::dirty_witness;

            // Validate the non-native script.
            if ((ec = in.witness().verify(tx, index, forks, embedded_script,
                value)))
                return ec;
        }
    }

    // Witness must be empty if no bip141 or invalid witness program (bip141).
    if (!witnessed && !in.witness().empty())
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
////    const auto& prevout = in.previous_output().metadata.cache;
////    return verify(tx, index, forks, prevout.script(), prevout.value());
////}

} // namespace chain
} // namespace system
} // namespace libbitcoin
