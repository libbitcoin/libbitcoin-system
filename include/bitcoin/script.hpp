#ifndef LIBBITCOIN_SCRIPT_H
#define LIBBITCOIN_SCRIPT_H

#include <deque>

#include <bitcoin/types.hpp>

namespace libbitcoin {

enum class opcode
{
    special = 1,
    pushdata1 = 76,
    pushdata2 = 77,
    pushdata4 = 78,
    dup = 118,
    hash160 = 169,
    equalverify = 136,
    checksig = 172
};

struct operation
{
    opcode code;
    data_chunk data;
};

class script
{
public:
    void push_operation(operation oper);

    std::string string_repr();
private:
    typedef std::deque<operation> operation_stack;
    operation_stack operations_;
};

std::string opcode_to_string(opcode code);
script parse_script(data_chunk raw_script);

} // libbitcoin

#endif

