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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_OPERATION_IPP
#define LIBBITCOIN_SYSTEM_CHAIN_OPERATION_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/chain/enums/numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/crypto/crypto.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Convert the opcode to the corresponding [1..16] value (or undefined).
constexpr uint8_t operation::opcode_to_positive(opcode code) NOEXCEPT
{
    ////BC_ASSERT(is_positive(code));
    constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<uint8_t>(code) - sub1(op_81);
}

// Compute maximum push data size for the opcode (without script limit).
constexpr size_t operation::opcode_to_maximum_size(opcode code) NOEXCEPT
{
    constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);

    switch (code)
    {
        case opcode::push_one_size:
            return max_uint8;
        case opcode::push_two_size:
            return max_uint16;
        case opcode::push_four_size:
            return max_uint32;
        default:
            const auto byte = static_cast<uint8_t>(code);
            return byte <= op_75 ? byte : zero;
    }
}

// Compute nominal data opcode based on size alone.
// ****************************************************************************
// CONSENSUS: non-minial encoding consensus critical for find_and_delete.
// ****************************************************************************
constexpr opcode operation::opcode_from_size(size_t size) NOEXCEPT
{
    ////BC_ASSERT(size <= max_uint32);
    constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);

    if (size <= op_75)
        return static_cast<opcode>(size);
    else if (size <= max_uint8)
        return opcode::push_one_size;
    else if (size <= max_uint16)
        return opcode::push_two_size;
    else
        return opcode::push_four_size;
}

// Convert the [0..16] value to the corresponding opcode (or undefined).
constexpr opcode operation::opcode_from_version(uint8_t value) NOEXCEPT
{
    ////BC_ASSERT(value <= numbers::positive_16);
    return (value == numbers::number_0) ? opcode::push_size_0 :
        operation::opcode_from_positive(value);
}

// Convert the [1..16] value to the corresponding opcode (or undefined).
constexpr opcode operation::opcode_from_positive(uint8_t value) NOEXCEPT
{
    ////BC_ASSERT(value >= numbers::positive_1);
    ////BC_ASSERT(value <= numbers::positive_16);
    constexpr auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    return static_cast<opcode>(value + sub1(op_81));
}

// Compute the minimal data opcode for a given chunk of data.
// Caller should clear data if converting to non-payload opcode.
constexpr opcode operation::minimal_opcode_from_data(
    const data_chunk& data) NOEXCEPT
{
    const auto size = data.size();

    if (is_one(size))
    {
        const auto value = data.front();

        if (value == numbers::negative_1)
            return opcode::push_negative_1;

        if (value == numbers::number_0)
            return opcode::push_size_0;

        if (value >= numbers::positive_1 &&
            value <= numbers::positive_16)
            return opcode_from_positive(value);
    }

    // Nominal encoding is minimal for multiple bytes and non-numerics.
    return opcode_from_size(size);
}

// Compute the nominal data opcode for a given chunk of data.
// Restricted to sized data, avoids conversion to numeric opcodes.
constexpr opcode operation::nominal_opcode_from_data(
    const data_chunk& data) NOEXCEPT
{
    return opcode_from_size(data.size());
}

// Categories of opcodes.
// ----------------------------------------------------------------------------

// opcode: [0..96].
// ****************************************************************************
// CONSENSUS: this test explicitly includes the satoshi 'reserved' code.
// This affects the operation count in p2sh script evaluation.
// Presumably this was an unintended consequence of range testing enums.
// ****************************************************************************
constexpr bool operation::is_relaxed_push(opcode code) NOEXCEPT
{
    constexpr auto op_96 = opcode::push_positive_16;
    return code <= op_96;
}

// opcode: [0..79, 81..96].
constexpr bool operation::is_push(opcode code) NOEXCEPT
{
    constexpr auto op_80 = opcode::reserved_80;
    return is_relaxed_push(code) && code != op_80;
}

// opcode: [1..78].
constexpr bool operation::is_payload(opcode code) NOEXCEPT
{
    constexpr auto op_1 = opcode::push_size_1;
    constexpr auto op_78 = opcode::push_four_size;
    return code >= op_1 && code <= op_78;
}

// stack: [1..16].
constexpr bool operation::is_positive(opcode code) NOEXCEPT
{
    constexpr auto op_81 = opcode::push_positive_1;
    constexpr auto op_96 = opcode::push_positive_16;
    return code >= op_81 && code <= op_96;
}

// stack: [0, 1..16].
constexpr bool operation::is_nonnegative(opcode code) NOEXCEPT
{
    constexpr auto op_0 = opcode::push_size_0;
    return code == op_0 || is_positive(code);
}

// stack: [-1, 0, 1..16].
constexpr bool operation::is_number(opcode code) NOEXCEPT
{
    constexpr auto op_79 = opcode::push_negative_1;
    return code == op_79 || is_nonnegative(code);
}

// opcode: [122].
constexpr bool operation::is_roller(opcode code) NOEXCEPT
{
    constexpr auto op_122 = opcode::roll;
    return code == op_122;
}

// opcode: [97..255].
constexpr bool operation::is_counted(opcode code) NOEXCEPT
{
    constexpr auto op_97 = opcode::nop;
    return code >= op_97;
}

// This condition is only applicable with bip342 active. bip342: If any opcode
// 80, 98, 126-129, 131-134, 137-138, 141-142, 149-153, 187-254 is encountered,
// validation succeeds. This is most reserved and most invalid, but not all. We
// are not renaming these codes to op_successX as succested by bip342, because:
// (1) they remain unimplemented.
// (2) they are not evaluated during script execution.
// (3) they are only conditionally successful (tapscript).
// (4) renaming them would lose important historical context.
// Regarding op_verif and op_vernotif, it seems that these were not promoted
// because authors failed to recognize that following their demotion from
// conditionals, these codes became unconditionally invalid, just as others.
// Regarding reserved_255, there is no good reason to have left this reserved,
// as op_return already provides that operation, but it's likely the authors
// felt it exhibits special benavior as the highest value code (it does not).
constexpr bool operation::is_success(opcode code) NOEXCEPT
{
    constexpr auto op_187 = opcode::reserved_187;
    constexpr auto op_254 = opcode::reserved_254;

    switch (code)
    {
        case opcode::reserved_80:
        case opcode::op_ver:
        ////case opcode::op_verif:      // stays invalid (why)
        ////case opcode::op_vernotif:   // stays invalid (why)
        ////case opcode::op_return:     // stays reserved
        case opcode::op_cat:
        case opcode::op_substr:
        case opcode::op_left:
        case opcode::op_right:
        case opcode::op_invert:
        case opcode::op_and:
        case opcode::op_or:
        case opcode::op_xor:
        case opcode::reserved_137:
        case opcode::reserved_138:
        case opcode::op_mul2:
        case opcode::op_div2:
        case opcode::op_mul:
        case opcode::op_div:
        case opcode::op_mod:
        case opcode::op_lshift:
        case opcode::op_rshift:
        ////case opcode::reserved_186:  // checksigadd subsumes
        ////case opcode::reserved_255:  // stays reserved (why)
            return true;
        default:
            return code >= op_187 && code <= op_254;
    }
}

// opcode: [101, 102, 126..129, 131..134, 141, 142, 149..153]
// ****************************************************************************
// CONSENSUS: These fail script even if wrapped by a conditional operation.
// ****************************************************************************
constexpr bool operation::is_invalid(opcode code) NOEXCEPT
{
    switch (code)
    {
        // Demoted to invalid by [0.3.6] soft fork.
        case opcode::op_verif:
        case opcode::op_vernotif:

        // Demoted to invalid by [0.3.10] soft fork.
        case opcode::op_cat:
        case opcode::op_substr:
        case opcode::op_left:
        case opcode::op_right:
        case opcode::op_invert:
        case opcode::op_and:
        case opcode::op_or:
        case opcode::op_xor:
        case opcode::op_mul2:
        case opcode::op_div2:
        case opcode::op_mul:
        case opcode::op_div:
        case opcode::op_mod:
        case opcode::op_lshift:
        case opcode::op_rshift:
            return true;
        default:
            return false;
    }
}

// opcode: [99..100, 103..104]
constexpr bool operation::is_conditional(opcode code) NOEXCEPT
{
    switch (code)
    {
        case opcode::if_:
        case opcode::notif:
        case opcode::else_:
        case opcode::endif:
            return true;
        default:
            return false;
    }
}

// opcode: [80, 98, 106, 137..138, 186..255]
// ****************************************************************************
// CONSENSUS: These fail script unless excluded by a conditional operation.
//
// Satoshi tests incorrectly refer to op_ver and op_verif as "reserved".
// Reserved refers to unused but conditionally acceptable codes. When the
// conditional operator skips over them, the script may be valid. On the
// other hand, "disabled" codes are unconditionally invalid - such as
// op_cat. The "disabled" codes are in a group outside of the evaluation
// switch, which makes their unconditional invalidity obvious. The other
// two disabled codes are not so obvious in behavior, and misidentified in
// satoshi test vectors:
//
// These fail because the scripts are unconditional with both reserved and
// disabled codes, yet they are all referred to as reserved.
// { "1", "ver", "op_ver is reserved" }
// { "1", "verif", "op_verif is reserved" }
// { "1", "vernotif", "op_vernotif is reserved" }
// { "1", "reserved", "op_reserved is reserved" }
// { "1", "reserved1", "op_reserved1 is reserved" }
// { "1", "reserved2", "op_reserved2 is reserved" }
//
// These fail because they either execute conditionally invalid codes
// (op_ver) or include unconditionally invalid codes, without execution
// (op_verif, op_vernotif). The comments are correct, contradicting above.
// { "1", "if ver else 1 endif", "ver is reserved" }
// { "0", "if verif else 1 endif", "verif illegal everywhere" }
// { "0", "if else 1 else verif endif", "verif illegal everywhere" }
// { "0", "if vernotif else 1 endif", "vernotif illegal everywhere" }
// { "0", "if else 1 else vernotif endif", "vernotif illegal everywhere" }
//
// These fail regardless of conditional exclusion because they are also
// disabled codes.
// { "'a' 'b'", "cat", "cat disabled" }
// { "'a' 'b' 0", "if cat else 1 endif", "cat disabled" }
// { "'abc' 1 1", "substr", "substr disabled" }
// { "'abc' 1 1 0", "if substr else 1 endif", "substr disabled" }
// { "'abc' 2 0", "if left else 1 endif", "left disabled" }
// { "'abc' 2 0", "if right else 1 endif", "right disabled" }
// { "'abc'", "if invert else 1 endif", "invert disabled" }
// { "1 2 0 if and else 1 endif", "nop", "and disabled" }
// { "1 2 0 if or else 1 endif", "nop", "or disabled" }
// { "1 2 0 if xor else 1 endif", "nop", "xor disabled" }
// { "2 0 if 2mul else 1 endif", "nop", "2mul disabled" }
// { "2 0 if 2div else 1 endif", "nop", "2div disabled" }
// { "2 2 0 if mul else 1 endif", "nop", "mul disabled" }
// { "2 2 0 if div else 1 endif", "nop", "div disabled" }
// { "2 2 0 if mod else 1 endif", "nop", "mod disabled" }
// { "2 2 0 if lshift else 1 endif", "nop", "lshift disabled" }
// { "2 2 0 if rshift else 1 endif", "nop", "rshift disabled" }
//
// The reason op_verif and op_vernotif are unconditionally invalid (and
// therefore behave exactly as "disabled" codes is they are conditionals.
// Note that op_ver is not a conditional, so despite similar name, when
// it was disabled it became a "reserved" code. The former conditonals
// are not excludable by remaining conditions. They pass this condition:
//      else if (fExec || (OP_IF <= opcode && opcode <= OP_ENDIF))
// which evaluates all codes, yet there were removed from the evaluation.
// They were part of this case, but now this break is not hit by them:
//      case OP_IF:
//      case OP_NOTIF: {...}
//          break;
// So they fall through to the conditional default case, just as any
// reserved code would, and halt the evaluation with the bad_opcode error:
//      default:
//          return set_error(serror, SCRIPT_ERR_BAD_OPCODE);
// Yet because they do not bypass any conditional evaluation they hit this
// default unconditionally. So they are "disabled" codes. We use the term
// "invalid" above because of the confusion that can cause. The only truly
// "reserved" codes are op_nop# codes, which were promoted by a hard fork.
// ****************************************************************************
constexpr bool operation::is_reserved(opcode code) NOEXCEPT
{
    constexpr auto op_186 = opcode::reserved_186;

    switch (code)
    {
        // Demoted to reserved by [0.3.6] soft fork.
        case opcode::op_ver:
        case opcode::op_return:

        // Unimplemented.
        case opcode::reserved_80:
        case opcode::reserved_137:
        case opcode::reserved_138:
            return true;
        default:
            return code >= op_186;
    }
}

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
