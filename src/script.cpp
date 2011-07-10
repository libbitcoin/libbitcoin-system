#include <bitcoin/script.hpp>

#include <stack>

#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/ripemd.hpp>
#include <bitcoin/util/sha256.hpp>

namespace libbitcoin {

void script::join(script other)
{
    operations_.insert(operations_.end(), 
        other.operations_.begin(), other.operations_.end());
}

void script::push_operation(operation oper)
{
    operations_.push_back(oper);
}

bool script::run(transaction parent_tx)
{
    stack_.clear();
    for (auto it = operations_.cbegin(); it != operations_.cend(); ++it)
    {
        const operation oper = *it;
        logger(LOG_DEBUG) << "Run: " << opcode_to_string(oper.code);
        if (!run_operation(oper, parent_tx))
            return false;
        if (oper.data.size() > 0)
        {
            BITCOIN_ASSERT(oper.code == opcode::special ||
                oper.code == opcode::pushdata1 ||
                oper.code == opcode::pushdata2 ||
                oper.code == opcode::pushdata4);
            stack_.push_back(oper.data);
        }
    }
    if (stack_.size() != 0)
    {
        logger(LOG_ERROR) << "Script left junk on top of the stack";
        return false;
    }
    return true;
}

data_chunk script::pop_stack()
{
    data_chunk value = stack_.back();
    stack_.pop_back();
    return value;
}

bool script::op_dup()
{
    if (stack_.size() < 1)
        return false;
    stack_.push_back(stack_.back());
    return true;
}

bool script::op_hash160()
{
    if (stack_.size() < 1)
        return false;
    data_chunk data = pop_stack();
    short_hash hash = generate_ripemd_hash(data);
    data_chunk raw_hash(hash.begin(), hash.end());
    stack_.push_back(raw_hash);
    return true;
}

bool script::op_equalverify()
{
    if (stack_.size() < 2)
        return false;
    return pop_stack() == pop_stack();
}

bool script::op_checksig(transaction parent_tx)
{
    // Unimplemented
    pop_stack();
    pop_stack();
    return true;
}

bool script::run_operation(operation op, transaction parent_tx)
{
    switch (op.code)
    {
        case opcode::special:
            return true;

        case opcode::pushdata1:
            BITCOIN_ASSERT(op.data.size() == static_cast<size_t>(op.code));
            return true;

        case opcode::pushdata2:
            BITCOIN_ASSERT(op.data.size() == static_cast<size_t>(op.code));
            return true;

        case opcode::pushdata4:
            BITCOIN_ASSERT(op.data.size() == static_cast<size_t>(op.code));
            return true;

        case opcode::dup:
            return op_dup();

        case opcode::hash160:
            return op_hash160();

        case opcode::equalverify:
            return op_equalverify();

        case opcode::checksig:
            return op_checksig(parent_tx);

        default:
            break;
    }
    return false;
}

std::string script::string_repr()
{
    std::ostringstream ss;
    for (auto it = operations_.cbegin(); it != operations_.cend(); ++it)
    {
        const operation op = *it;
        if (op.data.size() == 0)
            ss << opcode_to_string(op.code) << " ";
        else
        {
            ss << "[ ";
            for (auto dit = op.data.cbegin(); dit != op.data.cend(); ++dit)
                ss << std::hex << int(*dit) << " ";
            ss << "] ";
        }
    }
    return ss.str();
}

std::string opcode_to_string(opcode code)
{
    switch (code)
    {
        case opcode::special:
            return "special";
        case opcode::pushdata1:
            return "pushdata1";
        case opcode::pushdata2:
            return "pushdata2";
        case opcode::pushdata4:
            return "pushdata4";
        case opcode::dup:
            return "dup";
        case opcode::hash160:
            return "hash160";
        case opcode::equalverify:
            return "equalverify";
        case opcode::checksig:
            return "checksig";
        default:
        {
            std::ostringstream ss;
            ss << "<none " << static_cast<int>(code) << ">";
            return ss.str();
        }
    }
}

size_t number_of_bytes_from_opcode(opcode code, byte raw_byte)
{
    if (code == opcode::special)
        return raw_byte;
    else if (code == opcode::pushdata1 || code == opcode::pushdata2 
            || code == opcode::pushdata4)
        return static_cast<size_t>(code);
    else
        return 0;
}

script parse_script(data_chunk raw_script)
{
    script script_object;
    for (auto it = raw_script.cbegin(); it != raw_script.cend(); ++it)
    {
        byte raw_byte = *it;
        operation op;
        op.code = static_cast<opcode>(*it);
        if (1 <= raw_byte && raw_byte <= 75)
            op.code = opcode::special;
        size_t read_n_bytes = number_of_bytes_from_opcode(op.code, raw_byte);
        logger(LOG_DEBUG) << "Parsed: " << opcode_to_string(op.code);
        if (read_n_bytes > 0)
            logger(LOG_DEBUG) << "Reading: " << read_n_bytes << " bytes...";

        for (size_t byte_count = 0; byte_count < read_n_bytes; ++byte_count)
        {
            ++it;
            if (it == raw_script.cend())
            {
                logger(LOG_ERROR) << "Premature end of script.";
                return script();
            }
            op.data.push_back(*it);
        }

        script_object.push_operation(op);

    }
    return script_object;
}

} // libbitcoin

