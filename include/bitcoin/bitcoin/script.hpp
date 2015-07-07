/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SCRIPT_HPP
#define LIBBITCOIN_SCRIPT_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

struct transaction_type;

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

struct BC_API operation
{
    opcode code;
    data_chunk data;
};

typedef enum sighash_ : uint32_t
{
    all = 1,
    none = 2,
    single = 3,
    anyone_can_pay = 0x80
} sighash;

typedef std::vector<operation> operation_stack;

enum class payment_type
{
    pubkey,             // Pubkey and checksig in output.
    pubkey_hash,        // Payment to hash of pubkey
    script_hash,        // Payment to multisig address
    stealth_info,       // Stealth information (meta-output)
    multisig,           // Multisig
    pubkey_hash_sig,    // Pubkey hash input script.
    script_code_sig,    // BIP16 input script code.
    non_standard
};

class script_type
{
public:
    BC_API void join(const script_type& other);
    BC_API void push_operation(const operation& oper);
    BC_API bool run(const script_type& input_script, 
        const transaction_type& parent_tx, uint32_t input_index,
        bool bip16_enabled=true);

    BC_API payment_type type() const;

    BC_API const operation_stack& operations() const;

    BC_API static hash_digest generate_signature_hash(
        transaction_type parent_tx, uint32_t input_index,
        const script_type& script_code, uint32_t hash_type);

private:
    class conditional_stack
    {
    public:
        bool closed() const;
        bool has_failed_branches() const;

        void clear();
        void open(bool value);
        void else_();
        void close();
    private:
        typedef std::vector<bool> bool_stack;
        bool_stack stack_;
    };

    bool run(const transaction_type& parent_tx, uint32_t input_index);
    bool next_step(operation_stack::iterator it,
        const transaction_type& parent_tx, uint32_t input_index);
    bool increment_op_counter(opcode code);
    bool run_operation(const operation& op,
        const transaction_type& parent_tx, uint32_t input_index);

    bool op_negative_1();
    bool op_x(opcode code);
    bool op_if();
    bool op_notif();
    bool op_else();
    bool op_endif();
    bool op_verify();
    bool op_toaltstack();
    bool op_fromaltstack();
    bool op_2drop();
    bool op_2dup();
    bool op_3dup();
    bool op_2over();
    bool op_2rot();
    bool op_2swap();
    bool op_ifdup();
    bool op_depth();
    bool op_drop();
    bool op_dup();
    bool op_nip();
    bool op_over();
    bool op_pick();
    bool op_roll();
    bool op_rot();
    bool op_swap();
    bool op_tuck();
    bool op_size();
    bool op_equal();
    bool op_equalverify();
    bool op_1add();
    bool op_1sub();
    bool op_negate();
    bool op_abs();
    bool op_not();
    bool op_0notequal();
    bool op_add();
    bool op_sub();
    bool op_booland();
    bool op_boolor();
    bool op_numequal();
    bool op_numequalverify();
    bool op_numnotequal();
    bool op_lessthan();
    bool op_greaterthan();
    bool op_lessthanorequal();
    bool op_greaterthanorequal();
    bool op_min();
    bool op_max();
    bool op_within();
    bool op_ripemd160();
    bool op_sha1();
    bool op_sha256();
    bool op_hash160();
    bool op_hash256();
    // op_checksig is a specialised case of op_checksigverify
    bool op_checksig(
        const transaction_type& parent_tx, uint32_t input_index);
    bool op_checksigverify(
        const transaction_type& parent_tx, uint32_t input_index);
    // multisig variants
    bool read_section(data_stack& section, size_t count);
    bool op_checkmultisig(
        const transaction_type& parent_tx, uint32_t input_index);
    bool op_checkmultisigverify(
        const transaction_type& parent_tx, uint32_t input_index);

    data_chunk pop_stack();

    operation_stack operations_;
    // Used when executing the script
    data_stack stack_, alternate_stack_;
    size_t op_counter_ = 0;
    operation_stack::iterator codehash_begin_;
    conditional_stack conditional_stack_;
};

BC_API opcode data_to_opcode(const data_chunk& data);
BC_API std::string opcode_to_string(opcode code);
BC_API opcode string_to_opcode(const std::string& code_repr);
BC_API std::string pretty(const script_type& script);
BC_API script_type unpretty(const std::string& pretty);
BC_API std::istream& operator>>(
    std::istream& stream, script_type& script);
BC_API std::ostream& operator<<(
    std::ostream& stream, const script_type& script);

BC_API script_type raw_data_script(data_slice raw_script);
BC_API script_type parse_script(data_slice raw_script);
BC_API data_chunk save_script(const script_type& script);
BC_API size_t script_size(const script_type& script);

BC_API bool check_signature(data_slice signature,
    const ec_point& public_key, const script_type& script_code,
    const transaction_type& parent_tx, uint32_t input_index);
BC_API bool create_signature(data_chunk& signature,
    const ec_secret& private_key, const script_type& prevout_script,
    const transaction_type& tx, uint32_t input_index, uint32_t hash_type);
BC_API bool create_signature(data_chunk& signature,
    const ec_secret& private_key, const script_type& prevout_script,
    const transaction_type& tx, uint32_t input_index, uint32_t hash_type,
    const ec_secret& nonce);

} // namespace libbitcoin

#endif

