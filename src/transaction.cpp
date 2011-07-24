#include <bitcoin/transaction.hpp>

#include <bitcoin/util/serializer.hpp>
#include <bitcoin/util/sha256.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {

hash_digest hash_transaction(net::message::transaction transaction)
{
    serializer key;
    key.write_4_bytes(transaction.version);
    key.write_var_uint(transaction.inputs.size());
    for (net::message::transaction_input input: transaction.inputs)
    {
        key.write_hash(input.hash);
        key.write_4_bytes(input.index);
        data_chunk raw_script = save_script(input.input_script);
        key.write_var_uint(raw_script.size());
        key.write_data(raw_script);
        key.write_4_bytes(input.sequence);
    }
    key.write_var_uint(transaction.outputs.size());
    for (net::message::transaction_output output: transaction.outputs)
    {
        key.write_8_bytes(output.value);
        data_chunk raw_script = save_script(output.output_script);
        key.write_var_uint(raw_script.size());
        key.write_data(raw_script);
    }
    key.write_4_bytes(transaction.locktime);
    return generate_sha256_hash(key.get_data());
}

} // libbitcoin

