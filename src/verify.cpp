#include <bitcoin/verify.hpp>

#include <bitcoin/block.hpp>
#include <bitcoin/dialect.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/storage/storage.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/clock.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

constexpr size_t max_block_size = 1000000;
constexpr size_t max_block_script_operations = max_block_size / 50;

verify_block::verify_block(storage_ptr storage, dialect_ptr dialect)
  : storage_(storage), dialect_(dialect)
{
}

void verify_block::start(message::block current_block, 
        status_handler handle_status)
{
    handle_status_ = handle_status;
    current_block_ = current_block;
    // Check for duplicate
    current_block_hash_ = hash_block_header(current_block);
    storage_->block_exists_by_hash(current_block_hash_, 
            std::bind(&verify_block::find_duplicate, 
                shared_from_this(), _1, _2));
}

void verify_block::find_duplicate(std::error_code ec, bool block_exists)
{
    goto ignore_duplicate_check;
    if (ec)
    {
        handle_status_(ec, false);
        return;
    }
    else if (!block_exists)
    {
        handle_status_(std::error_code(), false);
        return;
    }
ignore_duplicate_check:
    if (!check_block())
    {
        handle_status_(std::error_code(), false);
        return;
    }

    storage_->fetch_block_by_hash(current_block_.prev_block,
            std::bind(&verify_block::find_previous,
                shared_from_this(), _1, _2));
}

void verify_block::find_previous(std::error_code ec, message::block)
{
    if (ec == error::block_doesnt_exist)
    {
        // TODO not handled for the time being
        handle_status_(std::error_code(), false);
    }
    else if (ec)
        handle_status_(ec, false);

    handle_status_(std::error_code(), true);
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

    if (!check_proof_of_work(current_block_hash_, current_block_.bits))
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
    uint64_t total_output_value;
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

