#ifndef LIBBITCOIN_SCRIPT_H
#define LIBBITCOIN_SCRIPT_H

#include <vector>

#include <bitcoin/types.hpp>
#include <bitcoin/utility/big_number.hpp>

namespace libbitcoin {

namespace message {
    struct transaction;
}

enum class opcode
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
    toaltstack = 107,
    fromaltstack = 108,
    drop = 117,
    dup = 118,
    over = 120,
    roll = 122,
    size = 130,
    reserved1 = 137,
    reserved2 = 138,
    not_ = 145,
    boolor = 155,
    min = 163,
    sha256 = 168,
    hash160 = 169,
    equal = 135,
    equalverify = 136,
    add = 147,
    greaterthanorequal = 162,
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

struct operation
{
    opcode code;
    data_chunk data;
};

namespace sighash
{
    enum : uint32_t
    {
        all = 1,
        none = 2,
        single = 3,
        anyone_can_pay = 0x80
    };
}

typedef std::vector<operation> operation_stack;

enum class payment_type
{
    pubkey,
    pubkey_hash,
    script_hash,
    multisig,
    non_standard
};

class script
{
public:
    void join(const script& other);
    void push_operation(operation oper);
    bool run(script input_script, const message::transaction& parent_tx,
        uint32_t input_index, bool bip16_enabled=true);

    payment_type type() const;

    const operation_stack& operations() const;

    static hash_digest generate_signature_hash(
        message::transaction parent_tx, uint32_t input_index,
        const script& script_code, uint32_t hash_type);

private:
    typedef std::vector<data_chunk> data_stack;

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

    bool run(const message::transaction& parent_tx, uint32_t input_index);
    bool next_step(operation_stack::iterator it,
        const message::transaction& parent_tx, uint32_t input_index);
    bool run_operation(const operation& op, 
        const message::transaction& parent_tx, uint32_t input_index);

    // Used by add, sub, mul, div, mod, lshift, rshift, booland, boolor,
    // numequal, numequalverify, numnotequal, lessthan, greaterthan,
    // lessthanorequal, greaterthanorequal, min, max
    bool arithmetic_start(big_number& number_a, big_number& number_b);

    bool op_negative_1();
    bool op_x(opcode code);
    bool op_if();
    bool op_notif();
    bool op_else();
    bool op_endif();
    bool op_verify();
    bool op_toaltstack();
    bool op_fromaltstack();
    bool op_drop();
    bool op_dup();
    bool op_over();
    bool op_roll();
    bool op_size();
    bool op_not();
    bool op_boolor();
    bool op_min();
    bool op_sha256();
    bool op_hash160();
    bool op_equal();
    bool op_equalverify();
    bool op_add();
    bool op_greaterthanorequal();
    // op_checksig is a specialised case of op_checksigverify
    bool op_checksig(
        const message::transaction& parent_tx, uint32_t input_index);
    bool op_checksigverify(
        const message::transaction& parent_tx, uint32_t input_index);
    // multisig variants
    bool read_section(data_stack& section);
    bool op_checkmultisig(
        const message::transaction& parent_tx, uint32_t input_index);
    bool op_checkmultisigverify(
        const message::transaction& parent_tx, uint32_t input_index);

    data_chunk pop_stack();

    operation_stack operations_;
    // Used when executing the script
    data_stack stack_, alternate_stack_;
    operation_stack::iterator codehash_begin_;
    conditional_stack conditional_stack_;
};

std::string opcode_to_string(opcode code);
opcode string_to_opcode(const std::string& code_repr);
std::string pretty(const script& source_script);
std::ostream& operator<<(std::ostream& stream, const script& source_script);

script coinbase_script(const data_chunk& raw_script);
script parse_script(const data_chunk& raw_script);
data_chunk save_script(const script& scr);

} // namespace libbitcoin

#endif

