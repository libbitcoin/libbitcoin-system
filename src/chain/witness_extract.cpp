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

#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/machine/machine.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Extract.
// ----------------------------------------------------------------------------

static script checksig_script() NOEXCEPT
{
    static const script cached{ { { opcode::checksig } } };
    return cached;
}

static script::cptr success_script_ptr() NOEXCEPT
{
    constexpr auto op_success = opcode::reserved_80;
    static_assert(operation::is_success(op_success));
    static const auto cached = to_shared<script>({ { op_success } });
    return cached;
}

static script::cptr checksig_script_ptr() NOEXCEPT
{
    static const auto cached = to_shared<script>(checksig_script());
    return cached;
}

// This is an internal optimization over using script::to_pay_key_hash_pattern.
static inline operations to_pay_key_hash(const chunk_cptr& program) NOEXCEPT
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

static inline const hash_digest& to_array32(const data_chunk& program) NOEXCEPT
{
    BC_ASSERT(program.size() == hash_size);
    return unsafe_array_cast<uint8_t, hash_size>(program.data());
}

static bool is_valid_control_block(const data_chunk& control) NOEXCEPT
{
    const auto size = control.size();
    constexpr auto maximum = control_block_base + control_block_node *
        control_block_range;

    // Control block must be size 33 + 32m, for integer m [0..128] [bip341].
    return !is_limited(size, control_block_base, maximum) && is_zero(
        floored_modulo(size - control_block_base, control_block_node));
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
                    out_script = checksig_script();
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

// static/private
inline bool witness::drop_annex(chunk_cptrs& stack) NOEXCEPT
{
    if (is_annex_pattern(stack))
    {
        stack.pop_back();
        return true;
    }

    return false;
}

// Extract script and initial execution stack.
code witness::extract_script(script::cptr& out_script,
    chunk_cptrs_ptr& out_stack, const script& program_script) const NOEXCEPT
{
    // Copy stack of shared const pointers for use as mutable witness stack.
    out_stack = make_shared<chunk_cptrs>(stack_);
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
            // witness stack : [annex]...
            if (program->size() == hash_size)
            {
                auto stack_size = out_stack->size();

                // If at least two elements, discard annex if present.
                if (drop_annex(*out_stack))
                    --stack_size;

                // p2ts (tapscript, script path spend)
                // witness stack : <control> <script> [stack-elements]
                // input script  : (empty)
                // output script : <1> <32-byte-tweaked-public-key>
                if (stack_size > one)
                {
                    // The last stack element is the control block.
                    const auto& control = pop(*out_stack);

                    // Control length must be 33 + 32m, for integer m [0..128].
                    if (!is_valid_control_block(*control))
                        return error::invalid_witness;

                    // The second-to-last stack element is the script.
                    out_script = to_shared<script>(*pop(*out_stack), false);

                    // TODO: DO SOME NASTY SHIT WITH CONTROL AND SCRIPT.
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

                // p2tr (taproot, key path spend)
                // witness stack : <signature>
                // input script  : (empty)
                // output script : <1> <32-byte-tweaked-public-key>
                if (is_one(stack_size))
                {
                    // Stack element is a signature that must be valid for q.
                    // Program is q, a 32 byte bip340 public key, so push it.
                    out_stack->push_back(program);
                    out_script = checksig_script_ptr();

                    // out_stack  : <public-key> <signature>
                    // out_script : <op_checksig>
                    return error::script_success;
                }

                // Fail if witness stack was empty.
                return error::invalid_witness;
            }

            // pay-to-anchor (p2a) programs land here (standardness).
            ////script::is_anchor_program_pattern(program_script.ops())

            // This op_success script succeeds immediately with bip342 active.
            out_script = success_script_ptr();
            out_stack->clear();

            // Version 1 other than 32 bytes remain unencumbered (success).
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

} // namespace chain
} // namespace system
} // namespace libbitcoin
