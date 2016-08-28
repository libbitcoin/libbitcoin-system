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
#ifndef LIBBITCOIN_CHAIN_OPCODE_HPP
#define LIBBITCOIN_CHAIN_OPCODE_HPP

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace chain {

enum class opcode : uint8_t
{
    zero = 0,
    special = 1,
    pushdata1 = 76,
    pushdata2 = 77,
    pushdata4 = 78,
    negative_1 = 79,
    // reserved does nothing
    reserved = 80,
    op_1 = 81,
    op_2 = 82,
    op_3 = 83,
    op_4 = 84,
    op_5 = 85,
    op_6 = 86,
    op_7 = 87,
    op_8 = 88,
    op_9 = 89,
    op_10 = 90,
    op_11 = 91,
    op_12 = 92,
    op_13 = 93,
    op_14 = 94,
    op_15 = 95,
    op_16 = 96,
    nop = 97,
    ver = 98,
    if_ = 99,
    notif = 100,
    verif = 101,
    vernotif = 102,
    else_ = 103,
    endif = 104,
    verify = 105,
    return_ = 106,
    toaltstack = 107,
    fromaltstack = 108,
    op_2drop = 109,
    op_2dup = 110,
    op_3dup = 111,
    op_2over = 112,
    op_2rot = 113,
    op_2swap = 114,
    ifdup = 115,
    depth = 116,
    drop = 117,
    dup = 118,
    nip = 119,
    over = 120,
    pick = 121,
    roll = 122,
    rot = 123,
    swap = 124,
    tuck = 125,
    cat = 126,          // disabled
    substr = 127,       // disabled
    left = 128,         // disabled
    right = 129,        // disabled
    size = 130,
    invert = 131,       // disabled
    and_ = 132,         // disabled
    or_ = 133,          // disabled
    xor_ = 134,         // disabled
    equal = 135,
    equalverify = 136,
    reserved1 = 137,
    reserved2 = 138,
    op_1add = 139,
    op_1sub = 140,
    op_2mul = 141,      // disabled
    op_2div = 142,      // disabled
    negate = 143,
    abs = 144,
    not_ = 145,
    op_0notequal = 146,
    add = 147,
    sub = 148,
    mul = 149,          // disabled
    div = 150,          // disabled
    mod = 151,          // disabled
    lshift = 152,       // disabled
    rshift = 153,       // disabled
    booland = 154,
    boolor = 155,
    numequal = 156,
    numequalverify = 157,
    numnotequal = 158,
    lessthan = 159,
    greaterthan = 160,
    lessthanorequal = 161,
    greaterthanorequal = 162,
    min = 163,
    max = 164,
    within = 165,
    ripemd160 = 166,
    sha1 = 167,
    sha256 = 168,
    hash160 = 169,
    hash256 = 170,
    codeseparator = 171,
    checksig = 172,
    checksigverify = 173,
    checkmultisig = 174,
    checkmultisigverify = 175,
    op_nop1 = 176,
    op_nop2 = 177,
    checklocktimeverify = op_nop2,
    op_nop3 = 178,
    op_nop4 = 179,
    op_nop5 = 180,
    op_nop6 = 181,
    op_nop7 = 182,
    op_nop8 = 183,
    op_nop9 = 184,
    op_nop10 = 185,
    bad_operation,
    raw_data
};

enum script_context : uint32_t
{
    none_enabled = 0,

    /// pay-to-script-hash enabled
    bip16_enabled = 1 << 0,

    /// no duplicated unspent transaction ids
    bip30_enabled = 1 << 1,

    /// coinbase must include height
    bip34_enabled = 1 << 2,

    /// strict DER signatures required
    bip66_enabled = 1 << 3,

    /// nop2 becomes check locktime verify
    bip65_enabled = 1 << 4,

    all_enabled = 0xffffffff
};

BC_API std::string opcode_to_string(opcode value, uint32_t flags);
BC_API opcode string_to_opcode(const std::string& value);
BC_API opcode data_to_opcode(const data_chunk& value);

} // namespace chain
} // namespace libbitcoin

#endif
