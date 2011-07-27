#include <bitcoin/script.hpp>

#include <stack>

#include <bitcoin/messages.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/util/elliptic_curve_key.hpp>
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

operation_stack script::operations() const
{
    return operations_;
}

bool script::run(const message::transaction& parent_tx, uint32_t input_index)
{
    stack_.clear();
    for (const operation oper: operations_)
    {
        log_debug() << "Run: " << opcode_to_string(oper.code);
        if (!run_operation(oper, parent_tx, input_index))
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
        log_error() << "Script left junk on top of the stack";
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

bool script::op_checksig(const message::transaction& parent_tx, 
        uint32_t input_index)
{
    if (stack_.size() < 2)
        return false;
    data_chunk pubkey = pop_stack(), signature = pop_stack();

    script script_code;
    for (operation op: operations_)
    {
        if (op.data == signature || op.code == opcode::codeseparator)
            continue;
        script_code.push_operation(op);
    }

    elliptic_curve_key key;
    key.set_public_key(pubkey);

    uint32_t hash_type = 0;
    hash_type = signature.back();
    signature.pop_back();
    BITCOIN_ASSERT(signature.size() == 70);

    if (hash_type != 1)
    {
        log_error() << "Unimplemented hash_type";
        return false;
    }

    if (input_index >= parent_tx.inputs.size())
    {
        log_fatal() << "script::op_checksig() : input_index " << input_index
                << " is out of range.";
        return false;
    }

    message::transaction tx_tmp = parent_tx;
    // Blank all other inputs' signatures
    for (message::transaction_input& input: tx_tmp.inputs)
        input.input_script = script();
    tx_tmp.inputs[input_index].input_script = script_code;

    hash_digest tx_hash = hash_transaction(tx_tmp, hash_type);
    return key.verify(tx_hash, signature);
}

bool script::run_operation(operation op, 
        const message::transaction& parent_tx, uint32_t input_index)
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
            return op_checksig(parent_tx, input_index);

        default:
            break;
    }
    return false;
}

std::string script::string_repr() const
{
    std::ostringstream ss;
    for (const operation op: operations_)
    {
        if (op.data.size() == 0)
            ss << opcode_to_string(op.code) << " ";
        else
        {
            ss << "[ ";
            for (byte b: op.data)
                ss << std::hex << int(b) << " ";
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
opcode string_to_opcode(std::string code_repr)
{
    if (code_repr == "special")
        return opcode::special;
    else if (code_repr == "pushdata1")
        return opcode::pushdata1;
    else if (code_repr == "pushdata2")
        return opcode::pushdata2;
    else if (code_repr == "pushdata4")
        return opcode::pushdata4;
    else if (code_repr == "dup")
        return opcode::dup;
    else if (code_repr == "hash160")
        return opcode::hash160;
    else if (code_repr == "equalverify")
        return opcode::equalverify;
    else if (code_repr == "checksig")
        return opcode::checksig;
    // ERROR: unknown... 
    return opcode::bad_operation;
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

script parse_script(const data_chunk raw_script)
{
    script script_object;
    for (auto it = raw_script.begin(); it != raw_script.end(); ++it)
    {
        byte raw_byte = *it;
        operation op;
        op.code = static_cast<opcode>(raw_byte);
        if (1 <= raw_byte && raw_byte <= 75)
            op.code = opcode::special;
        size_t read_n_bytes = number_of_bytes_from_opcode(op.code, raw_byte);

        for (size_t byte_count = 0; byte_count < read_n_bytes; ++byte_count)
        {
            ++it;
            if (it == raw_script.cend())
            {
                log_warning() << "Premature end of script.";
                return script();
            }
            op.data.push_back(*it);
        }

        script_object.push_operation(op);

    }
    return script_object;
}

data_chunk save_script(const script scr)
{
    data_chunk raw_script;
    for (operation op: scr.operations())
    {
        byte raw_byte = static_cast<byte>(op.code);
        if (op.code == opcode::special)
            raw_byte = op.data.size();
        raw_script.push_back(raw_byte);
        extend_data(raw_script, op.data);
    }
    return raw_script;
}

} // libbitcoin

