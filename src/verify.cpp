#include <bitcoin/verify.hpp>

#include <bitcoin/block.hpp>
#include <bitcoin/dialect.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/storage/storage.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/postbind.hpp>
#include <bitcoin/util/clock.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

constexpr size_t max_block_size = 1000000;
constexpr size_t max_block_script_operations = max_block_size / 50;

verify_block::verify_block(dialect_ptr dialect, 
    const message::block& current_block)
  : dialect_(dialect), current_block_(current_block)
{
    clock_.reset(new clock);
}

bool verify_block::check_block()
{
    // CheckBlock()
    // These are checks that are independent of context
    // that can be verified before saving an orphan block
    // ...

    // Size limits
    if (current_block_.transactions.empty() || 
        current_block_.transactions.size() > max_block_size ||
        dialect_->to_network(current_block_, false).size() > max_block_size)
    {
        return false;
    }

    const hash_digest current_block_hash = hash_block_header(current_block_);
    if (!check_proof_of_work(current_block_hash, current_block_.bits))
        return false;

    const ptime block_time = 
            boost::posix_time::from_time_t(current_block_.timestamp);
    if (block_time > clock_->get_time() + hours(2))
        return false;

    if (!is_coinbase(current_block_.transactions[0]))
        return false;
    for (size_t i = 1; i < current_block_.transactions.size(); ++i)
    {
        const message::transaction& tx = current_block_.transactions[i];
        if (is_coinbase(tx))
            return false;
    }

    for (message::transaction tx: current_block_.transactions)
        if (!check_transaction(tx))
            return false;

    // Check that it's not full of nonstandard transactions
    if (number_script_operations() > max_block_script_operations)
        return false;

    if (current_block_.merkle_root != 
            generate_merkle_root(current_block_.transactions))
        return false;

    return true;
}

bool verify_block::check_proof_of_work(hash_digest block_hash, uint32_t bits)
{
    big_number target;
    target.set_compact(bits);

    if (target <= 0 || target > max_target())
        return false;
    
    big_number our_value;
    our_value.set_hash(block_hash);
    if (our_value > target)
        return false;

    return true;
}

bool verify_block::check_transaction(const message::transaction& tx)
{
    if (tx.inputs.empty() || tx.outputs.empty())
        return false;

    // Maybe not needed since we try to serialise block in CheckBlock()
    //if (dialect_->to_network(tx, false).size() > max_block_size)
    //    return false;

    // Check for negative or overflow output values
    uint64_t total_output_value = 0;
    for (message::transaction_output output: tx.outputs)
    {
        if (output.value > max_money())
            return false;
        total_output_value += output.value;
        if (total_output_value > max_money())
            return false;
    }

    if (is_coinbase(tx))
    {
        const script& coinbase_script = tx.inputs[0].input_script;
        size_t coinbase_script_size = save_script(coinbase_script).size();
        if (coinbase_script_size < 2 || coinbase_script_size > 100)
            return false;
    }
    else
    {
        for (message::transaction_input input: tx.inputs)
            if (previous_output_is_null(input))
                return false;
    }

    return true;
}

size_t verify_block::number_script_operations()
{
    size_t total_operations = 0;
    for (message::transaction tx: current_block_.transactions)
    {
        for (message::transaction_input input: tx.inputs)
            total_operations += input.input_script.operations().size();
        for (message::transaction_output output: tx.outputs)
            total_operations += output.output_script.operations().size();
    }
    return total_operations;
}

} // libbitcoin

