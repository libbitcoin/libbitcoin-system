#ifndef LIBBITCOIN_SCRIPT_H
#define LIBBITCOIN_SCRIPT_H

#include <vector>

#include <bitcoin/types.hpp>

namespace libbitcoin {

namespace message {
    struct transaction;
}

enum class opcode
{
    raw_data = 0,
    special = 1,
    pushdata1 = 76,
    pushdata2 = 77,
    pushdata4 = 78,
    nop = 97,
    drop = 117,
    dup = 118,
    sha256 = 168,
    hash160 = 169,
    equal = 135,
    equalverify = 136,
    checksig = 172,
    codeseparator,  // Ignored
    bad_operation
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

enum class transaction_type
{
    normal,
    generate,
    other
};

class script
{
public:
    void join(const script& other);
    void push_operation(operation oper);
    bool run(script input_script,
        const message::transaction& parent_tx, uint32_t input_index);

    std::string pretty() const;
    transaction_type type() const;

    const operation_stack& operations() const;

    static hash_digest generate_signature_hash(
        message::transaction parent_tx, uint32_t input_index,
        const script& script_code, uint32_t hash_type);

private:
    typedef std::vector<data_chunk> data_stack;

    bool run(const message::transaction& parent_tx, uint32_t input_index);

    bool op_drop();
    bool op_dup();
    bool op_sha256();
    bool op_hash160();
    bool op_equal();
    bool op_equalverify();
    // op_checksig is a specialised case of op_checksigverify
    bool op_checksig(message::transaction parent_tx, uint32_t input_index);
    bool op_checksigverify(
            message::transaction parent_tx, uint32_t input_index);

    bool run_operation(operation op, 
            const message::transaction& parent_tx, uint32_t input_index);

    bool matches_template(operation_stack templ) const;

    data_chunk pop_stack();

    operation_stack operations_;
    data_stack stack_;
};

std::string opcode_to_string(opcode code);
opcode string_to_opcode(std::string code_repr);

// TODO: Should be inside the exporter imlementation eventually
script coinbase_script(const data_chunk& raw_script);
script parse_script(const data_chunk& raw_script);
data_chunk save_script(const script& scr);

} // libbitcoin

#endif

