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
    special = 1,
    pushdata1 = 76,
    pushdata2 = 77,
    pushdata4 = 78,
    dup = 118,
    hash160 = 169,
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

typedef std::vector<operation> operation_stack;

class script
{
public:
    void join(const script& other);
    void push_operation(operation oper);
    bool run(const message::transaction& parent_tx, uint32_t input_index);

    std::string string_repr() const;

    const operation_stack& operations() const;

private:
    typedef std::vector<data_chunk> data_stack;

    bool op_dup();
    bool op_hash160();
    bool op_equalverify();
    bool op_checksig(const message::transaction& parent_tx, 
            uint32_t input_index);

    bool run_operation(operation op, 
            const message::transaction& parent_tx, uint32_t input_index);

    data_chunk pop_stack();

    operation_stack operations_;
    data_stack stack_;
};

std::string opcode_to_string(opcode code);
opcode string_to_opcode(std::string code_repr);

// TODO: Should be inside the dialect imlementation eventually
script parse_script(const data_chunk& raw_script);
data_chunk save_script(const script& scr);

} // libbitcoin

#endif

