#include <bitcoin/script.hpp>

#include <stack>

#include <bitcoin/constants.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/data_helpers.hpp>
#include <bitcoin/utility/elliptic_curve_key.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/ripemd.hpp>
#include <bitcoin/utility/sha256.hpp>

namespace libbitcoin {

static const data_chunk stack_true_value{1, 1};
static const data_chunk stack_false_value{0};

void script::join(const script& other)
{
    operations_.insert(operations_.end(),
        other.operations_.begin(), other.operations_.end());
}

void script::push_operation(operation oper)
{
    operations_.push_back(oper);
}

const operation_stack& script::operations() const
{
    return operations_;
}

inline bool cast_to_big_number(const data_chunk& raw_number,
    big_number& result)
{
    // Satoshi bitcoin does it this way.
    // Copy its quack behaviour
    if (raw_number.size() > 4)
        return false;
    big_number mid;
    mid.set_data(raw_number);
    result.set_data(mid.data());
    return true;
}

inline bool cast_to_bool(const data_chunk& values)
{
    for (auto it = values.begin(); it != values.end(); ++it)
    {
        if (*it != 0)
        {
            // Can be negative zero
            if (it == values.end() - 1 && *it == 0x80)
                return false;
            return true;
        }
    }
    return false;
}

bool script::run(script input_script,
    const message::transaction& parent_tx, uint32_t input_index)
{
    stack_.clear();
    input_script.stack_.clear();
    if (!input_script.run(parent_tx, input_index))
        return false;
    stack_ = input_script.stack_;
    if (!run(parent_tx, input_index))
        return false;
    if (stack_.empty())
    {
        log_error() << "Script left no data on the stack";
        return false;
    }
    return cast_to_bool(stack_.back());
}

bool script::run(const message::transaction& parent_tx, uint32_t input_index)
{
    for (const operation oper: operations_)
    {
        //log_debug() << "Run: " << opcode_to_string(oper.code);
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
    return true;
}

data_chunk script::pop_stack()
{
    data_chunk value = stack_.back();
    stack_.pop_back();
    return value;
}

bool script::op_x(opcode code)
{
    uint8_t value_diff =
        static_cast<uint8_t>(code) -
            static_cast<uint8_t>(opcode::op_1) + 1;
    big_number big_repr(value_diff);
    stack_.push_back(big_repr.data());
    return true;
}

bool script::op_drop()
{
    if (stack_.size() < 1)
        return false;
    stack_.pop_back();
    return true;
}

bool script::op_dup()
{
    if (stack_.size() < 1)
        return false;
    stack_.push_back(stack_.back());
    return true;
}

bool script::op_min()
{
    if (stack_.size() < 2)
        return false;
    big_number number_a, number_b;
    if (!cast_to_big_number(pop_stack(), number_a))
        return false;
    if (!cast_to_big_number(pop_stack(), number_b))
        return false;
    if (number_a < number_b)
        stack_.push_back(number_a.data());
    else
        stack_.push_back(number_b.data());
    return true;
}

bool script::op_sha256()
{
    if (stack_.size() < 1)
        return false;
    data_chunk data = pop_stack();
    data_chunk hash(sha256_length);
    SHA256(data.data(), data.size(), hash.data());
    stack_.push_back(hash);
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

bool script::op_equal()
{
    if (stack_.size() < 2)
        return false;
    if (pop_stack() == pop_stack())
        stack_.push_back(stack_true_value);
    else
        stack_.push_back(stack_false_value);
    return true;
}

bool script::op_equalverify()
{
    if (stack_.size() < 2)
        return false;
    return pop_stack() == pop_stack();
}

inline void nullify_input_sequences(
    message::transaction_input_list& inputs, uint32_t except_input)
{
    for (size_t i = 0; i < inputs.size(); ++i)
        if (i != except_input)
            inputs[i].sequence = 0;
}

hash_digest script::generate_signature_hash(
    message::transaction parent_tx, uint32_t input_index,
    const script& script_code, uint32_t hash_type)
{
    BITCOIN_ASSERT(input_index < parent_tx.inputs.size());

    if ((hash_type & 0x1f) == sighash::none)
    {
        parent_tx.outputs.clear();
        nullify_input_sequences(parent_tx.inputs, input_index);
    }
    else if ((hash_type & 0x1f) == sighash::single)
    {
        uint32_t output_index = input_index;
        if (output_index >= parent_tx.outputs.size())
        {
            log_error() << "sighash::single the output_index is out of range";
            return null_hash;
        }
        parent_tx.outputs.resize(output_index + 1);
        for (message::transaction_output& output: parent_tx.outputs)
        {
            output.value = ~0;
            output.output_script = script();
        }
        nullify_input_sequences(parent_tx.inputs, input_index);
    }

    if (hash_type & sighash::anyone_can_pay)
    {
        parent_tx.inputs[0] = parent_tx.inputs[input_index];
        parent_tx.inputs.resize(1);
    }

    if (input_index >= parent_tx.inputs.size())
    {
        log_fatal() << "script::op_checksig() : input_index " << input_index
                << " is out of range.";
        return null_hash;
    }

    message::transaction tx_tmp = parent_tx;
    // Blank all other inputs' signatures
    for (message::transaction_input& input: tx_tmp.inputs)
        input.input_script = script();
    tx_tmp.inputs[input_index].input_script = script_code;

    return hash_transaction(tx_tmp, hash_type);
}

bool script::op_checksig(message::transaction parent_tx, uint32_t input_index)
{
    if (op_checksigverify(parent_tx, input_index))
        stack_.push_back(stack_true_value);
    else
        stack_.push_back(stack_false_value);
    return true;
}

bool script::op_checksigverify(
    message::transaction parent_tx, uint32_t input_index)
{
    if (stack_.size() < 2)
        return false;
    data_chunk pubkey = pop_stack(), signature = pop_stack();

    elliptic_curve_key key;
    key.set_public_key(pubkey);

    uint32_t hash_type = 0;
    hash_type = signature.back();
    signature.pop_back();

    script script_code;
    for (operation op: operations_)
    {
        if (op.data == signature || op.code == opcode::codeseparator)
            continue;
        script_code.push_operation(op);
    }

    hash_digest tx_hash =
        generate_signature_hash(
            parent_tx, input_index, script_code, hash_type);
    if (tx_hash == null_hash)
        return false;
    return key.verify(tx_hash, signature);
}

bool script::run_operation(operation op, 
        const message::transaction& parent_tx, uint32_t input_index)
{
    switch (op.code)
    {
        case opcode::raw_data:
            return false;

        case opcode::special:
        case opcode::pushdata1:
        case opcode::pushdata2:
        case opcode::pushdata4:
            return true;

        case opcode::op_1:
        case opcode::op_2:
        case opcode::op_3:
        case opcode::op_4:
        case opcode::op_5:
        case opcode::op_6:
        case opcode::op_7:
        case opcode::op_8:
        case opcode::op_9:
        case opcode::op_10:
        case opcode::op_11:
        case opcode::op_12:
        case opcode::op_13:
        case opcode::op_14:
        case opcode::op_15:
        case opcode::op_16:
            return op_x(op.code);

        case opcode::nop:
            return true;

        case opcode::drop:
            return op_drop();

        case opcode::dup:
            return op_dup();

        case opcode::min:
            return op_min();

        case opcode::sha256:
            return op_sha256();

        case opcode::hash160:
            return op_hash160();

        case opcode::equal:
            return op_equal();

        case opcode::equalverify:
            return op_equalverify();

        case opcode::checksig:
            return op_checksig(parent_tx, input_index);

        default:
            log_fatal() << "Unimplemented operation <none " 
                << static_cast<int>(op.code) << ">";
            break;
    }
    return false;
}

bool is_pubkey_type(const operation_stack& ops)
{
    return ops.size() == 2 &&
        ops[0].code == opcode::special &&
        ops[1].code == opcode::checksig;
}
bool is_pubkey_hash_type(const operation_stack& ops)
{
    return ops.size() == 5 &&
        ops[0].code == opcode::dup &&
        ops[1].code == opcode::hash160 &&
        ops[2].code == opcode::special &&
        ops[3].code == opcode::equalverify &&
        ops[4].code == opcode::checksig;
}
bool is_script_hash_type(const operation_stack& ops)
{
    return false;
}
bool is_multisig_type(const operation_stack& ops)
{
    return false;
}

payment_type script::type() const
{
    if (is_pubkey_type(operations_))
        return payment_type::pubkey;
    if (is_pubkey_hash_type(operations_))
        return payment_type::pubkey_hash;
    if (is_script_hash_type(operations_))
        return payment_type::script_hash;
    if (is_multisig_type(operations_))
        return payment_type::multisig;
    return payment_type::non_standard;
}

bool script::matches_template(operation_stack templ) const
{
    return templ.size() == 0;
}

std::string script::pretty() const
{
    std::ostringstream ss;
    for (auto it = operations_.begin(); it != operations_.end(); ++it)
    {
        if (it != operations_.begin())
            ss << " ";
        const operation& op = *it;
        if (op.data.empty())
            ss << opcode_to_string(op.code);
        else
            ss << "[ " << pretty_hex(op.data) << " ]";
    }
    return ss.str();
}

std::string opcode_to_string(opcode code)
{
    switch (code)
    {
        case opcode::raw_data:
            return "raw_data";
        case opcode::special:
            return "special";
        case opcode::pushdata1:
            return "pushdata1";
        case opcode::pushdata2:
            return "pushdata2";
        case opcode::pushdata4:
            return "pushdata4";
        case opcode::op_1:
            return "1";
        case opcode::op_2:
            return "2";
        case opcode::op_3:
            return "3";
        case opcode::op_4:
            return "4";
        case opcode::op_5:
            return "5";
        case opcode::op_6:
            return "6";
        case opcode::op_7:
            return "7";
        case opcode::op_8:
            return "8";
        case opcode::op_9:
            return "9";
        case opcode::op_10:
            return "10";
        case opcode::op_11:
            return "11";
        case opcode::op_12:
            return "12";
        case opcode::op_13:
            return "13";
        case opcode::op_14:
            return "14";
        case opcode::op_15:
            return "15";
        case opcode::op_16:
            return "16";
        case opcode::nop:
            return "nop";
        case opcode::drop:
            return "drop";
        case opcode::dup:
            return "dup";
        case opcode::min:
            return "min";
        case opcode::sha256:
            return "sha256";
        case opcode::hash160:
            return "hash160";
        case opcode::equal:
            return "equal";
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
    if (code_repr == "raw_data")
        return opcode::raw_data;
    else if (code_repr == "special")
        return opcode::special;
    else if (code_repr == "pushdata1")
        return opcode::pushdata1;
    else if (code_repr == "pushdata2")
        return opcode::pushdata2;
    else if (code_repr == "pushdata4")
        return opcode::pushdata4;
    else if (code_repr == "1")
        return opcode::op_1;
    else if (code_repr == "2")
        return opcode::op_2;
    else if (code_repr == "3")
        return opcode::op_3;
    else if (code_repr == "4")
        return opcode::op_4;
    else if (code_repr == "5")
        return opcode::op_5;
    else if (code_repr == "6")
        return opcode::op_6;
    else if (code_repr == "7")
        return opcode::op_7;
    else if (code_repr == "8")
        return opcode::op_8;
    else if (code_repr == "9")
        return opcode::op_9;
    else if (code_repr == "10")
        return opcode::op_10;
    else if (code_repr == "11")
        return opcode::op_11;
    else if (code_repr == "12")
        return opcode::op_12;
    else if (code_repr == "13")
        return opcode::op_13;
    else if (code_repr == "14")
        return opcode::op_14;
    else if (code_repr == "15")
        return opcode::op_15;
    else if (code_repr == "16")
        return opcode::op_16;
    else if (code_repr == "nop")
        return opcode::nop;
    else if (code_repr == "drop")
        return opcode::drop;
    else if (code_repr == "dup")
        return opcode::dup;
    else if (code_repr == "min")
        return opcode::min;
    else if (code_repr == "sha256")
        return opcode::sha256;
    else if (code_repr == "hash160")
        return opcode::hash160;
    else if (code_repr == "equal")
        return opcode::equal;
    else if (code_repr == "equalverify")
        return opcode::equalverify;
    else if (code_repr == "checksig")
        return opcode::checksig;
    // ERROR: unknown... 
    return opcode::bad_operation;
}

// Read next n bytes while advancing iterator
// Used for seeing length of data to push to stack with pushdata2/4
template <typename Iterator>
inline data_chunk read_back_from_iterator(Iterator& it, size_t total)
{
    data_chunk number_bytes;
    for (size_t i = 0; i < total; ++i)
    {
        ++it;
        number_bytes.push_back(*it);
    }
    std::reverse(std::begin(number_bytes), std::end(number_bytes));
    return number_bytes;
}

template <typename Iterator>
size_t number_of_bytes_from_opcode(opcode code, byte raw_byte, Iterator& it)
{
    switch (code)
    {
        case opcode::special:
            return raw_byte;
        case opcode::pushdata1:
            ++it;
            return static_cast<uint8_t>(*it);
        case opcode::pushdata2:
            return cast_chunk<uint16_t>(read_back_from_iterator(it, 2));
        case opcode::pushdata4:
            return cast_chunk<uint32_t>(read_back_from_iterator(it, 4));
        default:
            return 0;
    }
}

script coinbase_script(const data_chunk& raw_script)
{
    script script_object;
    operation op;
    op.code = opcode::raw_data;
    op.data = raw_script;
    script_object.push_operation(op);
    return script_object;
}

script parse_script(const data_chunk& raw_script)
{
    script script_object;
    for (auto it = raw_script.begin(); it != raw_script.end(); ++it)
    {
        byte raw_byte = *it;
        operation op;
        op.code = static_cast<opcode>(raw_byte);
        // raw_byte is unsigned so it's always >= 0
        if (raw_byte <= 75)
            op.code = opcode::special;
        size_t read_n_bytes = 
            number_of_bytes_from_opcode(op.code, raw_byte, it);

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

inline data_chunk operation_metadata(const opcode code, size_t data_size)
{
    switch (code)
    {
        case opcode::pushdata1:
            return uncast_type<uint8_t>(data_size);
        case opcode::pushdata2:
            return uncast_type<uint16_t>(data_size);
        case opcode::pushdata4:
            return uncast_type<uint32_t>(data_size);
        default:
            return data_chunk();
    }
}
data_chunk save_script(const script& scr)
{
    const operation_stack& operations = scr.operations();
    if (operations.empty())
        return data_chunk();
    else if (operations[0].code == opcode::raw_data)
        return operations[0].data;
    data_chunk raw_script;
    for (const operation& op: scr.operations())
    {
        byte raw_byte = static_cast<byte>(op.code);
        if (op.code == opcode::special)
            raw_byte = op.data.size();
        raw_script.push_back(raw_byte);
        extend_data(raw_script, operation_metadata(op.code, op.data.size()));
        extend_data(raw_script, op.data);
    }
    return raw_script;
}

} // libbitcoin

