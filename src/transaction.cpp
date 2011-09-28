#include <bitcoin/transaction.hpp>

#include <bitcoin/util/serializer.hpp>
#include <bitcoin/util/sha256.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/constants.hpp>

namespace libbitcoin {

typedef std::vector<hash_digest> hash_list;

hash_digest hash_transaction_impl(const message::transaction& transaction, 
        uint32_t* hash_type_code)
{
    serializer key;
    key.write_4_bytes(transaction.version);
    key.write_var_uint(transaction.inputs.size());
    for (message::transaction_input input: transaction.inputs)
    {
        key.write_hash(input.hash);
        key.write_4_bytes(input.index);
        data_chunk raw_script = save_script(input.input_script);
        key.write_var_uint(raw_script.size());
        key.write_data(raw_script);
        key.write_4_bytes(input.sequence);
    }
    key.write_var_uint(transaction.outputs.size());
    for (message::transaction_output output: transaction.outputs)
    {
        key.write_8_bytes(output.value);
        data_chunk raw_script = save_script(output.output_script);
        key.write_var_uint(raw_script.size());
        key.write_data(raw_script);
    }
    key.write_4_bytes(transaction.locktime);
    if (hash_type_code != nullptr)
        key.write_4_bytes(*hash_type_code);
    return generate_sha256_hash(key.get_data());
}

hash_digest hash_transaction(const message::transaction& transaction)
{
    return hash_transaction_impl(transaction, nullptr);
}
hash_digest hash_transaction(const message::transaction& transaction, 
        uint32_t hash_type_code)
{
    return hash_transaction_impl(transaction, &hash_type_code);
}

hash_digest build_merkle_tree(hash_list& merkle)
{
    if (merkle.empty())
        return null_hash;
    else if (merkle.size() == 1)
        return merkle[0];

    while (merkle.size() > 1)
    {
        if (merkle.size()%2 != 0)
            merkle.push_back(merkle.back());

        hash_list new_merkle;
        for (auto it = merkle.begin(); it != merkle.end(); ++it)
        {
            serializer concat;
            concat.write_hash(*it);
            ++it;
            concat.write_hash(*it);
            hash_digest new_root = generate_sha256_hash(concat.get_data());
            new_merkle.push_back(new_root);
        }
        merkle = new_merkle;
    }
    return merkle[0];
}

hash_digest generate_merkle_root(const message::transaction_list& transactions)
{
    hash_list tx_hashes;
    for (message::transaction tx: transactions)
        tx_hashes.push_back(hash_transaction(tx));
    return build_merkle_tree(tx_hashes);
}

std::string string_repr(const message::transaction_input& input)
{
    std::ostringstream ss;
    ss 
        << "\tindex = " << input.index << "\n"
        << "\t" << input.input_script.string_repr() << "\n"
        << "\tsequence = " << input.sequence << "\n";
    return ss.str();
}

std::string string_repr(const message::transaction_output& output)
{
    std::ostringstream ss;
    ss << "\tvalue = " << output.value << "\n"
        << "\t" << output.output_script.string_repr() << "\n";
    return ss.str();
}

std::string string_repr(const message::transaction& transaction)
{
    std::ostringstream ss;
    ss << "Transaction:\n"
        << "\tversion = " << transaction.version << "\n"
        << "\tlocktime = " << transaction.locktime << "\n"
        << "Inputs:\n";
    for (message::transaction_input input: transaction.inputs)
        ss << string_repr(input);
    ss << "Outputs:\n";
    for (message::transaction_output output: transaction.outputs)
        ss << string_repr(output);
    ss << "\n";
    return ss.str();
}

bool previous_output_is_null(const message::transaction_input& input)
{
    return input.index == ~0u && input.hash == null_hash;
}

bool is_coinbase(const message::transaction& tx)
{
    return tx.inputs.size() == 1 && previous_output_is_null(tx.inputs[0]);
}

uint64_t total_value(const message::transaction& tx)
{
    uint64_t total = 0;
    for (const message::transaction_output& output: tx.outputs)
        total += output.value;
    return total;
}

} // libbitcoin

