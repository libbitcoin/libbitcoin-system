#include <bitcoin/transaction.hpp>

#include <bitcoin/types.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/satoshi_serialize.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/utility/sha256.hpp>
#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

typedef std::vector<hash_digest> hash_list;

hash_digest hash_transaction_impl(const message::transaction& tx, 
    uint32_t* hash_type_code)
{
    data_chunk serialized_tx(satoshi_raw_size(tx));
    satoshi_save(tx, serialized_tx.begin());
    if (hash_type_code != nullptr)
        extend_data(serialized_tx, uncast_type(*hash_type_code));
    return generate_sha256_hash(serialized_tx);
}

hash_digest hash_transaction(const message::transaction& tx)
{
    return hash_transaction_impl(tx, nullptr);
}
hash_digest hash_transaction(const message::transaction& tx, 
    uint32_t hash_type_code)
{
    return hash_transaction_impl(tx, &hash_type_code);
}

hash_digest build_merkle_tree(hash_list& merkle)
{
    if (merkle.empty())
        return null_hash;
    else if (merkle.size() == 1)
        return merkle[0];

    while (merkle.size() > 1)
    {
        if (merkle.size() % 2 != 0)
            merkle.push_back(merkle.back());

        hash_list new_merkle;
        for (auto it = merkle.begin(); it != merkle.end(); it += 2)
        {
            serializer concat;
            concat.write_hash(*it);
            concat.write_hash(*(it + 1));
            hash_digest new_root = generate_sha256_hash(concat.data());
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

std::string pretty(const message::transaction_input& input)
{
    std::ostringstream ss;
    ss << "\thash = " << input.previous_output.hash << "\n"
        << "\tindex = " << input.previous_output.index << "\n"
        << "\t" << input.input_script << "\n"
        << "\tsequence = " << input.sequence << "\n";
    return ss.str();
}

std::string pretty(const message::transaction_output& output)
{
    std::ostringstream ss;
    ss << "\tvalue = " << output.value << "\n"
        << "\t" << output.output_script << "\n";
    return ss.str();
}

std::string pretty(const message::transaction& tx)
{
    std::ostringstream ss;
    ss << "Transaction:\n"
        << "\tversion = " << tx.version << "\n"
        << "\tlocktime = " << tx.locktime << "\n"
        << "Inputs:\n";
    for (message::transaction_input input: tx.inputs)
        ss << pretty(input);
    ss << "Outputs:\n";
    for (message::transaction_output output: tx.outputs)
        ss << pretty(output);
    ss << "\n";
    return ss.str();
}

bool previous_output_is_null(const message::output_point& previous_output)
{
    return previous_output.index == std::numeric_limits<uint32_t>::max() &&
        previous_output.hash == null_hash;
}

bool is_coinbase(const message::transaction& tx)
{
    return tx.inputs.size() == 1 && 
        previous_output_is_null(tx.inputs[0].previous_output);
}

uint64_t total_output_value(const message::transaction& tx)
{
    uint64_t total = 0;
    for (const message::transaction_output& output: tx.outputs)
        total += output.value;
    return total;
}

bool operator==(const message::output_point& output_a,
    const message::output_point& output_b)
{
    return output_a.hash == output_b.hash && output_a.index == output_b.index;
}

} // namespace libbitcoin

