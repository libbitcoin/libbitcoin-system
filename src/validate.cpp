#include <bitcoin/validate.hpp>

#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/block.hpp>
#include <bitcoin/exporter.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/clock.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

constexpr size_t max_block_size = 1000000;
constexpr size_t max_block_script_sig_operations = max_block_size / 50;

validate_transaction::validate_transaction(blockchain_ptr chain,
    exporter_ptr saver, const message::transaction& tx,
    const pool_buffer& pool, io_service::strand& async_strand)
  : chain_(chain), exporter_(saver), tx_(tx), tx_hash_(hash_transaction(tx)),
    pool_(pool), strand_(async_strand)
{
}

void validate_transaction::start(validate_handler handle_validate)
{
    handle_validate_ = handle_validate;
    if (!basic_checks())
    {
        handle_validate_(error::bad_transaction);
        return;
    }

    // Check for duplicates in the blockchain
    chain_->fetch_transaction(tx_hash_,
        strand_.wrap(std::bind(
            &validate_transaction::handle_duplicate_check,
                shared_from_this(), _1)));
}

bool validate_transaction::basic_checks() const
{
    std::error_code ec;
    ec = check_transaction(tx_);
    if (ec)
        return false;

    if (is_coinbase(tx_))
        return false;

    // Ummm...
    //if ((int64)nLockTime > INT_MAX)

    size_t transaction_byte_size = exporter_->save(tx_).size();
    if (number_script_sig_operations(tx_) > (transaction_byte_size / 34) ||
        transaction_byte_size < 100)
    {
        return false;
    }

    if (!is_standard())
        return false;

    // Check for conflicts
    if (exists(tx_hash_))
        return false;
    // Check for blockchain duplicates done next in start() after
    // this function exits.

    return true;
}

bool validate_transaction::is_standard() const
{
    return true;
}

bool validate_transaction::exists(const hash_digest& tx_hash) const
{
    for (const transaction_entry_info& entry: pool_)
        if (entry.hash == tx_hash)
            return true;
    return false;
}

void validate_transaction::handle_duplicate_check(const std::error_code& ec)
{
    if (ec != error::missing_object)
    {
        handle_validate_(error::bad_transaction);
        return;
    }
    // Check for conflicts with memory txs
    for (size_t input_index = 0; input_index < tx_.inputs.size(); 
        ++input_index)
    {
        const message::output_point& previous_output =
            tx_.inputs[input_index].previous_output;
        if (is_spent(previous_output))
        {
            handle_validate_(error::bad_transaction);
            return;
        }
    }
    // Check inputs

    // We already know it is not a coinbase tx

    chain_->fetch_last_depth(strand_.wrap(std::bind(
        &validate_transaction::set_last_depth, shared_from_this(), _1, _2)));
}
 
bool validate_transaction::is_spent(const message::output_point outpoint) const
{
    for (const transaction_entry_info& entry: pool_)
        for (const message::transaction_input current_input: entry.tx.inputs)
            if (current_input.previous_output == outpoint)
                return true;
    return false;
}

void validate_transaction::set_last_depth(
    const std::error_code& ec, size_t last_depth)
{
    if (ec)
    {
        handle_validate_(ec);
        return;
    }
    // Used for checking coinbase maturity
    last_block_depth_ = last_depth;
    value_in_ = 0;
    BITCOIN_ASSERT(tx_.inputs.size() > 0);
    current_input_ = 0;
    fetch_next_previous_transaction();
}

void validate_transaction::fetch_next_previous_transaction()
{
    BITCOIN_ASSERT(current_input_ < tx_.inputs.size());
    chain_->fetch_transaction_index(
        tx_.inputs[current_input_].previous_output.hash,
        strand_.wrap(std::bind(
            &validate_transaction::fetch_input_transaction_index,
                shared_from_this(), _1, _2)));
}

bool validate_transaction::connect_input(
    const message::transaction& tx, size_t current_input,
    const message::transaction& previous_tx, size_t parent_depth,
    size_t last_block_depth, uint64_t& value_in)
{
    const message::transaction_input& input = tx.inputs[current_input];
    const message::output_point& previous_outpoint =
        tx.inputs[current_input].previous_output;
    if (previous_outpoint.index >= previous_tx.outputs.size())
        return false;
    const message::transaction_output& previous_output =
        previous_tx.outputs[previous_outpoint.index];
    uint64_t output_value = previous_output.value;
    if (output_value > max_money())
        return false;
    if (is_coinbase(previous_tx))
    {
        size_t depth_difference = last_block_depth - parent_depth;
        if (depth_difference < coinbase_maturity)
            return false;
    }
    script output_script = previous_output.output_script;
    if (!output_script.run(input.input_script, tx, current_input))
        return false;
    value_in += output_value;
    if (value_in > max_money())
        return false;
    return true;
}

void validate_transaction::fetch_input_transaction_index(
    const std::error_code& ec, size_t parent_depth)
{
    if (ec)
    {
        handle_validate_(error::bad_transaction);
        return;
    }
    // Now fetch actual transaction body
    BITCOIN_ASSERT(current_input_ < tx_.inputs.size());
    chain_->fetch_transaction(
        tx_.inputs[current_input_].previous_output.hash,
        strand_.wrap(std::bind(
            &validate_transaction::fetch_input_transaction,
                shared_from_this(), _1, _2, parent_depth)));
}

void validate_transaction::fetch_input_transaction(const std::error_code& ec,
    const message::transaction& previous_tx, size_t parent_depth)
{
    if (ec)
    {
        handle_validate_(error::bad_transaction);
        return;
    }
    if (!connect_input(tx_, current_input_, previous_tx,
        parent_depth, last_block_depth_, value_in_))
    {
        handle_validate_(error::bad_transaction);
        return;
    }
    // Search for double spends...
    chain_->fetch_spend(tx_.inputs[current_input_].previous_output,
        strand_.wrap(std::bind(
            &validate_transaction::check_double_spend,
                shared_from_this(), _1)));
}

void validate_transaction::check_double_spend(const std::error_code& ec)
{
    if (ec == error::unspent_output)
    {
        // End of connect_input checks
        ++current_input_;
        if (current_input_ == tx_.inputs.size())
            check_fees();
        else
        {
            BITCOIN_ASSERT(current_input_ < tx_.inputs.size());
            // Keep looping
            fetch_next_previous_transaction();
        }
    }
    else
    {
        BITCOIN_ASSERT(!ec || ec != error::missing_object);
        handle_validate_(error::bad_transaction);
    }
}

bool validate_transaction::tally_fees(const message::transaction& tx,
    uint64_t value_in, uint64_t& total_fees)
{
    uint64_t value_out = total_output_value(tx);
    if (value_in < value_out)
        return false;
    uint64_t fee = value_in - value_out;
    total_fees += fee;
    if (total_fees > max_money())
        return false;
    return true;
}

void validate_transaction::check_fees()
{
    uint64_t fee = 0;
    tally_fees(tx_, value_in_, fee);
    // Who cares?
    // Fuck the police
    // Every tx equal!
    handle_validate_(std::error_code());
}

std::error_code validate_transaction::check_transaction(
    const message::transaction& tx)
{
    if (tx.inputs.empty() || tx.outputs.empty())
        return error::empty_transaction;

    // Maybe not needed since we try to serialise block in CheckBlock()
    //if (exporter_->save(tx, false).size() > max_block_size)
    //    return false;

    // Check for negative or overflow output values
    uint64_t total_output_value = 0;
    for (message::transaction_output output: tx.outputs)
    {
        if (output.value > max_money())
            return error::output_value_overflow;
        total_output_value += output.value;
        if (total_output_value > max_money())
            return error::output_value_overflow;
    }

    if (is_coinbase(tx))
    {
        const script& coinbase_script = tx.inputs[0].input_script;
        size_t coinbase_script_size = save_script(coinbase_script).size();
        if (coinbase_script_size < 2 || coinbase_script_size > 100)
            return error::invalid_coinbase_script_size;
    }
    else
    {
        for (message::transaction_input input: tx.inputs)
            if (previous_output_is_null(input.previous_output))
                return error::previous_output_null;
    }

    return std::error_code();
}

inline size_t count_script_sigs(const operation_stack& operations)
{
    size_t total_sigs = 0;
    for (const operation& op: operations)
        if (op.code == opcode::checksig)
            total_sigs++;
        // TODO: Add OP_CHECKSIGVERIFY OP_CHECKMULTISIG OP_CHECKMULTISIGVERIFY
        // ... they need to exist first
    return total_sigs;
}

size_t validate_transaction::number_script_sig_operations(
    const message::transaction& tx)
{
    size_t total_sigs = 0;
    for (message::transaction_input input: tx.inputs)
        total_sigs += count_script_sigs(input.input_script.operations());
    for (message::transaction_output output: tx.outputs)
        total_sigs += count_script_sigs(output.output_script.operations());
    return total_sigs;
}

validate_block::validate_block(exporter_ptr saver, size_t depth,
    const message::block& current_block)
  : exporter_(saver), depth_(depth), current_block_(current_block)
{
    clock_ = std::make_shared<chrono_clock>();
}

std::error_code validate_block::start()
{
    std::error_code ec;
    ec = check_block();
    if (ec)
        return ec;
    if (!accept_block())
        return error::accept_block;
    if (!connect_block())
        return error::connect_block;
    return std::error_code();
}

std::error_code validate_block::check_block()
{
    // CheckBlock()
    // These are checks that are independent of context
    // that can be validated before saving an orphan block
    // ...

    // Size limits
    if (current_block_.transactions.empty() || 
        current_block_.transactions.size() > max_block_size ||
        exporter_->save(current_block_).size() > max_block_size)
    {
        return error::size_limits;
    }

    const hash_digest current_block_hash = hash_block_header(current_block_);
    if (!check_proof_of_work(current_block_hash, current_block_.bits))
        return error::proof_of_work;

    const ptime block_time = 
            boost::posix_time::from_time_t(current_block_.timestamp);
    if (block_time > clock_->time() + hours(2))
        return error::futuristic_timestamp;

    if (!is_coinbase(current_block_.transactions[0]))
        return error::first_not_coinbase;
    for (size_t i = 1; i < current_block_.transactions.size(); ++i)
    {
        const message::transaction& tx = current_block_.transactions[i];
        if (is_coinbase(tx))
            return error::extra_coinbases;
    }

    for (message::transaction tx: current_block_.transactions)
    {
        std::error_code ec = validate_transaction::check_transaction(tx);
        if (ec)
            return ec;
    }

    // Check that it's not full of nonstandard transactions
    if (number_script_sig_operations() > max_block_script_sig_operations)
        return error::too_many_sigs;

    if (current_block_.merkle != 
            generate_merkle_root(current_block_.transactions))
    {
        return error::merkle_mismatch;
    }

    return std::error_code();
}

bool validate_block::check_proof_of_work(hash_digest block_hash, uint32_t bits)
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

size_t validate_block::number_script_sig_operations()
{
    size_t total_sigs = 0;
    for (message::transaction tx: current_block_.transactions)
        total_sigs += validate_transaction::number_script_sig_operations(tx);
    return total_sigs;
}

bool validate_block::accept_block()
{
    if (current_block_.bits != work_required())
        return false;
    if (current_block_.timestamp <= median_time_past())
        return false;
    // Txs should be final when included in a block
    // Do lesser check here since lock_time is currently unused in bitcoin
    for (const message::transaction& tx: current_block_.transactions)
        BITCOIN_ASSERT(tx.locktime == 0);
    if (!passes_checkpoints())
        return false;
    return true;
}

template<typename Value>
inline Value range_constraint(Value value, Value minimum, Value maximum)
{
    if (value < minimum)
        return minimum;
    else if (value > maximum)
        return maximum;
    return value;
}

uint32_t validate_block::work_required()
{
    if (depth_ == 0)
        return max_bits;
    else if (depth_ % readjustment_interval != 0)
        return previous_block_bits();

    uint64_t actual = actual_timespan(readjustment_interval);
    actual = range_constraint(actual, target_timespan / 4, target_timespan * 4);

    big_number retarget;
    retarget.set_compact(previous_block_bits());
    retarget *= actual;
    retarget /= target_timespan;

    if (retarget > max_target())
        retarget = max_target();

    return retarget.compact();
}

bool validate_block::passes_checkpoints()
{
    const hash_digest block_hash = hash_block_header(current_block_);

    if (depth_ == 11111 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x69, 0xe2, 0x44, 0xf7, 
                        0x3d, 0x78, 0xe8, 0xfd, 0x29, 0xba, 0x2f, 0xd2, 
                        0xed, 0x61, 0x8b, 0xd6, 0xfa, 0x2e, 0xe9, 0x25, 
                        0x59, 0xf5, 0x42, 0xfd, 0xb2, 0x6e, 0x7c, 0x1d})
        return false;

    if (depth_ ==  33333 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x2d, 0xd5, 0x58, 0x8a, 
                        0x74, 0x78, 0x4e, 0xaa, 0x7a, 0xb0, 0x50, 0x7a, 
                        0x18, 0xad, 0x16, 0xa2, 0x36, 0xe7, 0xb1, 0xce, 
                        0x69, 0xf0, 0x0d, 0x7d, 0xdf, 0xb5, 0xd0, 0xa6})
        return false;

    if (depth_ ==  68555 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x1b, 0x49, 
                        0x03, 0x55, 0x0a, 0x0b, 0x96, 0xe9, 0xa9, 0x40, 
                        0x5c, 0x8a, 0x95, 0xf3, 0x87, 0x16, 0x2e, 0x49, 
                        0x44, 0xe8, 0xd9, 0xfb, 0xe5, 0x01, 0xcd, 0x6a})
        return false;

    if (depth_ ==  70567 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0x49, 0xb1, 
                        0x4b, 0xcf, 0x27, 0x46, 0x20, 0x68, 0xf1, 0x26, 
                        0x4c, 0x96, 0x1f, 0x11, 0xfa, 0x2e, 0x0e, 0xdd, 
                        0xd2, 0xbe, 0x07, 0x91, 0xe1, 0xd4, 0x12, 0x4a})
        return false;

    if (depth_ ==  74000 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x57, 0x39, 0x93, 
                        0xa3, 0xc9, 0xe4, 0x1c, 0xe3, 0x44, 0x71, 0xc0, 
                        0x79, 0xdc, 0xf5, 0xf5, 0x2a, 0x0e, 0x82, 0x4a, 
                        0x81, 0xe7, 0xf9, 0x53, 0xb8, 0x66, 0x1a, 0x20})
        return false;

    if (depth_ == 105000 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x91, 0xce, 
                        0x28, 0x02, 0x7f, 0xae, 0xa3, 0x20, 0xc8, 0xd2, 
                        0xb0, 0x54, 0xb2, 0xe0, 0xfe, 0x44, 0xa7, 0x73, 
                        0xf3, 0xee, 0xfb, 0x15, 0x1d, 0x6b, 0xdc, 0x97})
        return false;

    if (depth_ == 118000 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x4a, 
                        0x7f, 0x8a, 0x7a, 0x12, 0xdc, 0x90, 0x6d, 0xdb, 
                        0x9e, 0x17, 0xe7, 0x5d, 0x68, 0x4f, 0x15, 0xe0, 
                        0x0f, 0x87, 0x67, 0xf9, 0xe8, 0xf3, 0x65, 0x53})
        return false;

    if (depth_ == 134444 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xb1, 
                        0x2f, 0xfd, 0x4c, 0xd3, 0x15, 0xcd, 0x34, 0xff, 
                        0xd4, 0xa5, 0x94, 0xf4, 0x30, 0xac, 0x81, 0x4c, 
                        0x91, 0x18, 0x4a, 0x0d, 0x42, 0xd2, 0xb0, 0xfe})
        return false;

    return true;
}

bool validate_block::connect_block()
{
    // BIP 30 security fix
    for (const message::transaction& current_tx: current_block_.transactions)
        if (!not_duplicate_or_spent(current_tx))
            return false;

    uint64_t fees = 0;
    for (size_t tx_index = 1; tx_index < current_block_.transactions.size();
            ++tx_index)
    {
        uint64_t value_in = 0;
        const message::transaction& tx = current_block_.transactions[tx_index];
        if (!validate_inputs(tx, tx_index, value_in))
            return false;
        if (!validate_transaction::tally_fees(tx, value_in, fees))
            return false;
    }
    uint64_t coinbase_value = 
        total_output_value(current_block_.transactions[0]);
    if (coinbase_value  > block_value(depth_) + fees)
        return false;
    return true;
}

bool validate_block::not_duplicate_or_spent(const message::transaction& tx)
{
    const hash_digest& tx_hash = hash_transaction(tx);
    // Is there a matching previous tx?
    if (!transaction_exists(tx_hash))
        return true;
    // Then for a duplicate transaction to exist, all its outputs
    // must have been spent.
    for (uint32_t output_index = 0; output_index < tx.outputs.size();
        ++output_index)
    {
        if (!is_output_spent({tx_hash, output_index}))
            return false;
    }
    return true;
}

bool validate_block::validate_inputs(const message::transaction& tx, 
    size_t index_in_parent, uint64_t& value_in)
{
    BITCOIN_ASSERT(!is_coinbase(tx));
    for (size_t input_index = 0; input_index < tx.inputs.size(); ++input_index)
    {
        if (!connect_input(index_in_parent, tx, input_index, value_in))
            return false;
    }
    return true;
}

bool validate_block::connect_input(size_t index_in_parent,
    const message::transaction& current_tx,
    size_t input_index, uint64_t& value_in)
{
    // Lookup previous output
    BITCOIN_ASSERT(input_index < current_tx.inputs.size());
    const message::transaction_input& input = current_tx.inputs[input_index];
    const message::output_point& previous_output = input.previous_output;
    message::transaction previous_tx;
    size_t previous_depth;
    if (!fetch_transaction(previous_tx, previous_depth, previous_output.hash))
        return false;
    const message::transaction_output& previous_tx_out =
        previous_tx.outputs[previous_output.index];
    // Get output amount
    uint64_t output_value = previous_tx_out.value;
    if (output_value > max_money())
        return false;
    // Check coinbase maturity has been reached
    if (is_coinbase(previous_tx))
    {
        uint32_t depth_difference = depth_ - previous_depth;
        if (depth_difference < coinbase_maturity)
            return false;
    }
    // Validate script
    script output_script = previous_tx_out.output_script;
    if (!output_script.run(input.input_script, current_tx, input_index))
    {
        log_error(log_domain::validation) << "Script failed evaluation";
        return false;
    }
    // Search for double spends
    if (is_output_spent(previous_output, index_in_parent, input_index))
        return false;
    // Increase value_in by this output's value
    value_in += output_value;
    if (value_in > max_money())
        return false;
    return true;
}

} // libbitcoin

