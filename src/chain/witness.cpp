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
#include <bitcoin/system/chain/witness.hpp>

#include <algorithm>
#include <istream>
#include <memory>
#include <numeric>
#include <string>
#include <utility>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

using namespace system::machine;
static const script& op_checksig_script()
{
    static const script signature{ { opcode::checksig } };
    return signature;
}

// Constructors.
// ----------------------------------------------------------------------------

witness::witness() NOEXCEPT
  : witness(chunk_cptrs{}, false, zero)
{
}

witness::witness(data_stack&& stack) NOEXCEPT
  : witness(*to_shareds(std::move(stack)), true)
{
    // stack moved so cannot pass serialized_size(stack), order not guaranteed.
}

witness::witness(const data_stack& stack) NOEXCEPT
  : witness(*to_shareds(stack), true)
{
}

witness::witness(chunk_cptrs&& stack) NOEXCEPT
  : witness(std::move(stack), true)
{
    // stack moved so cannot pass serialized_size(stack), order not guaranteed.
}

witness::witness(const chunk_cptrs& stack) NOEXCEPT
  : witness(stack, true, serialized_size(stack, false))
{
}

witness::witness(stream::in::fast&& stream, bool prefix) NOEXCEPT
  : witness(read::bytes::fast(stream), prefix)
{
}

witness::witness(stream::in::fast& stream, bool prefix) NOEXCEPT
  : witness(read::bytes::fast(stream), prefix)
{
}

witness::witness(std::istream&& stream, bool prefix) NOEXCEPT
  : witness(read::bytes::istream(stream), prefix)
{
}

witness::witness(std::istream& stream, bool prefix) NOEXCEPT
  : witness(read::bytes::istream(stream), prefix)
{
}

witness::witness(reader&& source, bool prefix) NOEXCEPT
  : witness(source, prefix)
{
}

witness::witness(reader& source, bool prefix) NOEXCEPT
  : stack_(source.get_arena())
{
    assign_data(source, prefix);
}

witness::witness(const std::string& mnemonic) NOEXCEPT
  : witness(from_string(mnemonic))
{
}

// protected
witness::witness(chunk_cptrs&& stack, bool valid) NOEXCEPT
  : stack_(std::move(stack)), valid_(valid),
    size_(serialized_size(stack_, false))
{
}

// protected
witness::witness(const chunk_cptrs& stack, bool valid) NOEXCEPT
  : stack_(stack), valid_(valid), size_(serialized_size(stack_, false))
{
}

// protected
witness::witness(const chunk_cptrs& stack, bool valid, size_t size) NOEXCEPT
  : stack_(stack), valid_(valid), size_(size)
{
}

// Operators.
// ----------------------------------------------------------------------------

bool witness::operator==(const witness& other) const NOEXCEPT
{
    return deep_equal(stack_, other.stack_);
}

bool witness::operator!=(const witness& other) const NOEXCEPT
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

void witness::skip(reader& source, bool prefix) NOEXCEPT
{
    if (prefix)
    {
        const auto count = source.read_size(max_block_weight);

        for (size_t element = 0; element < count; ++element)
            source.read_bytes(source.read_size(max_block_weight));
    }
    else
    {
        while (!source.is_exhausted())
            source.read_bytes(source.read_size(max_block_weight));
    }
}

// private
void witness::assign_data(reader& source, bool prefix) NOEXCEPT
{
    size_ = zero;
    auto& allocator = source.get_allocator();

    const auto push_witness = [&allocator, &source, this]() NOEXCEPT
    {
        // If read_bytes_raw returns nullptr invalid source is implied.
        const auto size = source.read_size(max_block_weight);
        const auto bytes = source.read_bytes_raw(size);
        if (is_null(bytes))
            return false;

        stack_.emplace_back(POINTER(data_chunk, allocator, bytes));
        size_ = ceilinged_add(size_, element_size(stack_.back()));
        return true;
    };

    if (prefix)
    {
        const auto count = source.read_size(max_block_weight);
        stack_.reserve(count);

        for (size_t element = 0; element < count; ++element)
            if (!push_witness())
                break;
    }
    else
    {
        while (!source.is_exhausted())
            if (!push_witness())
                break;
    }

    valid_ = source;
}

inline bool is_push_token(const std::string& token) NOEXCEPT
{
    return token.size() > one && token.front() == '[' && token.back() == ']';
}

inline std::string remove_token_delimiters(const std::string& token) NOEXCEPT
{
    BC_ASSERT(token.size() > one);
    return std::string(std::next(token.begin()), std::prev(token.end()));
}

// static/private
witness witness::from_string(const std::string& mnemonic) NOEXCEPT
{
    // There is always one data element per non-empty string token.
    auto tokens = split(mnemonic);

    // Split always returns at least one token, and when trimming it will be
    // empty only if there was nothing but whitespace in the mnemonic.
    if (tokens.front().empty())
        tokens.clear();

    data_stack stack(tokens.size());
    auto data = stack.begin();

    // Create data stack from the split tokens.
    for (const auto& token: tokens)
    {
        if (!is_push_token(token) ||
            !decode_base16(*data++, remove_token_delimiters(token)))
            return {};
    }

    return { std::move(stack) };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk witness::to_data(bool prefix) const NOEXCEPT
{
    data_chunk data(serialized_size(prefix));
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out, prefix);
    return data;
}

void witness::to_data(std::ostream& stream, bool prefix) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out, prefix);
}

void witness::to_data(writer& sink, bool prefix) const NOEXCEPT
{
    // Witness prefix is an element count, not byte length (unlike script).
    if (prefix)
        sink.write_variable(stack_.size());

    // Tokens encoded as variable integer prefixed byte array [bip144].
    for (const auto& element: stack_)
    {
        sink.write_variable(element->size());
        sink.write_bytes(*element);
    }
}

std::string witness::to_string() const NOEXCEPT
{
    if (!valid_)
        return "(?)";

    std::string text;
    for (const auto& element: stack_)
        text += "[" + encode_base16(*element) + "] ";

    trim_right(text);
    return text;
}

// Properties.
// ----------------------------------------------------------------------------

bool witness::is_valid() const NOEXCEPT
{
    return valid_;
}

const chunk_cptrs& witness::stack() const NOEXCEPT
{
    return stack_;
}

// static
size_t witness::serialized_size(const chunk_cptrs& stack, bool prefix) NOEXCEPT
{
    const auto size = std::accumulate(stack.begin(), stack.end(), zero,
        [](size_t total, const chunk_cptr& element) NOEXCEPT
        {
            return ceilinged_add(total, element_size(element));
        });

    return prefix ? ceilinged_add(variable_size(stack.size()), size) : size;
}

size_t witness::serialized_size(bool prefix) const NOEXCEPT
{
    // Witness prefix is an element count, not byte length (unlike script).
    // An empty stack is not a valid witnessed tx (no inputs) but a consistent
    // serialization is used independently by database so zero stack allowed.
    return prefix ? ceilinged_add(variable_size(stack_.size()), size_) : size_;
}

// Utilities.
// ----------------------------------------------------------------------------

// This is an internal optimization over using script::to_pay_key_hash_pattern.
inline operations to_pay_key_hash(const chunk_cptr& program) NOEXCEPT
{
    BC_ASSERT(program->size() == short_hash_size);

    return operations
    {
        { opcode::dup },
        { opcode::hash160 },
        { program, true },
        { opcode::equalverify },
        { opcode::checksig }
    };
}

inline const hash_digest& to_array32(const data_chunk& program) NOEXCEPT
{
    BC_ASSERT(program.size() == hash_size);
    return unsafe_array_cast<uint8_t, hash_size>(program.data());
}

inline bool is_valid_control_block(const data_chunk& control) NOEXCEPT
{
    const auto size = control.size();
    constexpr auto maximum = control_block_base + control_block_node *
        control_block_range;

    // Control block must be size 33 + 32m, for integer m [0..128] [bip341].
    return !is_limited(size, control_block_base, maximum)
        && is_zero(floored_modulo(size - control_block_base, control_block_node));
}

// out_script is only useful only for sigop counting.
// program_script is only used to determine witness_program type.
bool witness::extract_sigop_script(script& out_script,
    const script& program_script) const NOEXCEPT
{
    out_script = {};
    switch (program_script.version())
    {
        case script_version::segwit:
        {
            switch (program_script.witness_program()->size())
            {
                // Each p2wkh input is counted as 1 sigop [bip141].
                case short_hash_size:
                    out_script = op_checksig_script();
                    return true;

                // p2wsh sigops are counted as before for p2sh [bip141].
                case hash_size:
                    if (!stack_.empty())
                        out_script = { *stack_.back(), false };

                    return true;

                // Undefined v0 witness script, will not validate.
                default:
                    return true;
            }
        }

        // Sigops in tapscripts do not count towards block limit [bip342].
        case script_version::taproot:
            return true;

        // These versions are reserved for future extensions [bip141].
        case script_version::reserved:
            return true;

        // Return false only if this is not a witness script.
        case script_version::unversioned:
        default:
            return false;
    }
}

// Extract script and initial execution stack.
code witness::extract_script(script::cptr& out_script,
    chunk_cptrs_ptr& out_stack, const script& program_script) const NOEXCEPT
{
    // Copy stack of shared const pointers for use as mutable witness stack.
    out_stack = std::make_shared<chunk_cptrs>(stack_);
    const auto& program = program_script.witness_program();

    switch (program_script.version())
    {
        // All [bip141] comments.
        case script_version::segwit:
        {
            switch (program->size())
            {
                // p2wkh
                // witness stack : <signature> <public-key>
                // input script  : (empty)
                // output script : <0> <20-byte-hash-of-public-key>
                case short_hash_size:
                {
                    // Create a pay-to-key-hash input script from the program.
                    // The hash160 of public key must match program.
                    out_script = to_shared<script>(to_pay_key_hash(program));

                    // Stack must be 2 elements.
                    return out_stack->size() == two ?
                        error::script_success : error::invalid_witness;
                }

                // p2wsh
                // witness stack : <script> [stack-elements]
                // input script  : (empty)
                // output script : <0> <32-byte-hash-of-script>
                case hash_size:
                {
                    // The stack must consist of at least 1 element.
                    if (out_stack->empty())
                        return error::invalid_witness;

                    // Input script is popped from the stack.
                    out_script = to_shared<script>(*pop(*out_stack), false);

                    // Popped script sha256 hash must match program.
                    return to_array32(*program) == out_script->hash() ?
                        error::script_success : error::invalid_witness;
                }

                // If the version byte is 0, but the witness program is neither
                // 20 nor 32 bytes, the script must fail.
                default:
                    return error::invalid_witness;
            }
        }

        // All [bip341] comments.
        case script_version::taproot:
        {
            // input script  : (empty)
            // output script : <1> <32-byte-tweaked-public-key>
            if (program->size() == hash_size)
            {
                auto stack_size = out_stack->size();

                // If at least two elements, discard annex if present.
                if (drop_annex(*out_stack))
                    --stack_size;

                // tapscript (script path spend)
                // witness stack : [annex]<control><script>[stack-elements]
                // input script  : (empty)
                // output script : <1> <32-byte-tweaked-public-key>
                if (stack_size > one)
                {
                    // The last stack element is control block.
                    const auto& control = pop(*out_stack);

                    // Control length must be 33 + 32m, for integer m [0..128].
                    if (!is_valid_control_block(*control))
                        return error::invalid_witness;

                    // The second-to-last stack element is the script.
                    out_script = to_shared<script>(*pop(*out_stack), false);

                    // TODO: DO SOME NASTY SHIT WITH CONTROL(c) AND SCRIPT(s).
                    // TODO: MUST OBTAIN tapleaf_hash for signature hash.
                    // q is referred to as `taproot output key`.
                    // p is referred to as `taproot internal key`.
                    ////Let p = c[1:33] and let P = lift_x(int(p))
                    ////* where lift_x and [:] are defined as in BIP340.
                    ////* Fail if this point is not on the curve.
                    ////Let v = c[0] & 0xfe and call it the leaf version.
                    ////Let k0 = hashTapLeaf(v || compact_size(size of s) || s);
                    ////* also call it the tapleaf_hash.
                    ////For j in [0,1,...,m-1]:
                    ////Let ej = c[33+32j:65+32j].
                    ////Let kj+1 depend on whether kj < ej (lexicographically):
                    ////If kj <  ej: kj+1 = hashTapBranch(kj || ej).
                    ////If kj >= ej: kj+1 = hashTapBranch(ej || kj).
                    ////Let t = hashTapTweak(p || km).
                    ////If t >= 0xFFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE BAAEDCE6
                    ////* AF48A03B BFD25E8C D0364141
                    ////* (order of secp256k1), fail.
                    ////Let Q = P + int(t)G.
                    ////If q != x(Q) or c[0] & 1 != y(Q) mod 2, fail.

                    // Execute script with remaining stack.
                    return error::script_success;
                }

                // taproot (key path spend)
                // witness stack : [annex]<signature>
                // input script  : (empty)
                // output script : <1> <32-byte-tweaked-public-key>
                if (stack_size > zero)
                {
                    // The program is q, a 32 byte bip340 public key.
                    ////const auto& key = to_array32(*program);

                    // Only element is a signature that must be valid for q.
                    ////const auto& sig = out_stack->back();

                    // TODO: DO SOME NASTY SHIT THAT DON'T BELONG HERE.
                    // Validate signature against key (q), using appended
                    // sighash_flags (?) and existing signature_hash function.
                    ////hash = state::signature_hash(script, sighash_flags)
                    ////if (!ecdsa::verify_signature(key, hash, sig))
                    ////    return error::fail;

                    ///////////////////////////////////////////////////////////
                    // TODO: need sentinel to indicate success w/out script ex.
                    ///////////////////////////////////////////////////////////
                    return error::script_success;
                }

                // Fail if witness stack was empty.
                return error::invalid_witness;
            }

            ///////////////////////////////////////////////////////////////////
            // TODO: need sentinel to indicate success w/out script execution.
            ///////////////////////////////////////////////////////////////////
            // Version 1 other than 32 bytes remain unencumbered.
            return error::script_success;
        }

        // These versions are reserved for future extensions [bip141].
        case script_version::reserved:
            return error::script_success;

        // The witness version is undefined.
        case script_version::unversioned:
        default:
            return error::invalid_witness;
    }
}

BC_POP_WARNING()

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

witness tag_invoke(json::value_to_tag<witness>,
    const json::value& value) NOEXCEPT
{
    return witness{ std::string(value.get_string().c_str()) };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const witness& witness) NOEXCEPT
{
    value = witness.to_string();
}

BC_POP_WARNING()

witness::cptr tag_invoke(json::value_to_tag<witness::cptr>,
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<witness>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const witness::cptr& output) NOEXCEPT
{
    tag_invoke(tag, value, *output);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
