#include <bitcoin/validate.hpp>

#include <set>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/block.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/satoshi_serialize.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

namespace posix_time = boost::posix_time;

constexpr size_t max_block_size = 1000000;
constexpr size_t max_block_script_sig_operations = max_block_size / 50;

validate_transaction::validate_transaction(
    blockchain& chain, const transaction_type& tx,
    const pool_buffer& pool, async_strand& strand)
  : strand_(strand), chain_(chain),
    tx_(tx), tx_hash_(hash_transaction(tx)), pool_(pool)
{
}

void validate_transaction::start(validate_handler handle_validate)
{
    handle_validate_ = handle_validate;
    std::error_code ec = basic_checks();
    if (ec)
    {
        handle_validate_(ec, index_list());
        return;
    }

    // Check for duplicates in the blockchain
    chain_.fetch_transaction(tx_hash_,
        strand_.wrap(
            &validate_transaction::handle_duplicate_check,
                shared_from_this(), _1));
}

std::error_code validate_transaction::basic_checks() const
{
    std::error_code ec;
    ec = check_transaction(tx_);
    if (ec)
        return ec;

    if (is_coinbase(tx_))
        return error::coinbase_transaction;

    // Ummm...
    //if ((int64)nLockTime > INT_MAX)

    if (!is_standard())
        return error::is_not_standard;

    // Check for conflicts
    if (fetch(tx_hash_))
        return error::duplicate;
    // Check for blockchain duplicates done next in start() after
    // this function exits.

    return std::error_code();
}

bool validate_transaction::is_standard() const
{
    return true;
}

const transaction_type* validate_transaction::fetch(
    const hash_digest& tx_hash) const
{
    for (const transaction_entry_info& entry: pool_)
        if (entry.hash == tx_hash)
            return &entry.tx;
    return nullptr;
}

void validate_transaction::handle_duplicate_check(const std::error_code& ec)
{
    if (ec != error::not_found)
    {
        handle_validate_(error::duplicate, index_list());
        return;
    }
    // Check for conflicts with memory txs
    for (size_t input_index = 0; input_index < tx_.inputs.size();
        ++input_index)
    {
        const output_point& previous_output =
            tx_.inputs[input_index].previous_output;
        if (is_spent(previous_output))
        {
            handle_validate_(error::double_spend, index_list());
            return;
        }
    }
    // Check inputs

    // We already know it is not a coinbase tx

    chain_.fetch_last_height(strand_.wrap(
        &validate_transaction::set_last_height, shared_from_this(), _1, _2));
}

bool validate_transaction::is_spent(const output_point outpoint) const
{
    for (const transaction_entry_info& entry: pool_)
        for (const transaction_input_type current_input: entry.tx.inputs)
            if (current_input.previous_output == outpoint)
                return true;
    return false;
}

void validate_transaction::set_last_height(
    const std::error_code& ec, size_t last_height)
{
    if (ec)
    {
        handle_validate_(ec, index_list());
        return;
    }
    // Used for checking coinbase maturity
    last_block_height_ = last_height;
    value_in_ = 0;
    BITCOIN_ASSERT(tx_.inputs.size() > 0);
    current_input_ = 0;
    // Begin looping through the inputs, fetching the previous tx
    next_previous_transaction();
}

void validate_transaction::next_previous_transaction()
{
    BITCOIN_ASSERT(current_input_ < tx_.inputs.size());
    // First we fetch the parent block height for a transaction.
    // Needed for checking the coinbase maturity.
    chain_.fetch_transaction_index(
        tx_.inputs[current_input_].previous_output.hash,
        strand_.wrap(
            &validate_transaction::previous_tx_index,
                shared_from_this(), _1, _2));
}

void validate_transaction::previous_tx_index(
    const std::error_code& ec, size_t parent_height)
{
    if (ec)
    {
        search_pool_previous_tx();
    }
    else
    {
        // Now fetch actual transaction body
        BITCOIN_ASSERT(current_input_ < tx_.inputs.size());
        chain_.fetch_transaction(
            tx_.inputs[current_input_].previous_output.hash,
            strand_.wrap(
                &validate_transaction::handle_previous_tx,
                    shared_from_this(), _1, _2, parent_height));
    }
}

void validate_transaction::search_pool_previous_tx()
{
    const hash_digest& previous_tx_hash =
        tx_.inputs[current_input_].previous_output.hash;
    const transaction_type* previous_tx = fetch(previous_tx_hash);
    if (!previous_tx)
    {
        handle_validate_(error::input_not_found,
            index_list{current_input_});
        return;
    }
    BITCOIN_ASSERT(!is_coinbase(*previous_tx));
    // parent_height ignored here as memory pool transactions can
    // never be a coinbase transaction.
    handle_previous_tx(std::error_code(), *previous_tx, 0);
    unconfirmed_.push_back(current_input_);
}

void validate_transaction::handle_previous_tx(const std::error_code& ec,
    const transaction_type& previous_tx, size_t parent_height)
{
    if (ec)
    {
        handle_validate_(error::input_not_found,
            index_list{current_input_});
        return;
    }
    // Should check for are inputs standard here...
    if (!connect_input(tx_, current_input_, previous_tx,
        parent_height, last_block_height_, value_in_))
    {
        handle_validate_(error::validate_inputs_failed, {current_input_});
        return;
    }
    // Search for double spends...
    chain_.fetch_spend(tx_.inputs[current_input_].previous_output,
        strand_.wrap(
            &validate_transaction::check_double_spend,
                shared_from_this(), _1));
}

bool validate_transaction::connect_input(
    const transaction_type& tx, size_t current_input,
    const transaction_type& previous_tx, size_t parent_height,
    size_t last_block_height, uint64_t& value_in)
{
    const transaction_input_type& input = tx.inputs[current_input];
    const output_point& previous_outpoint =
        tx.inputs[current_input].previous_output;
    if (previous_outpoint.index >= previous_tx.outputs.size())
        return false;
    const transaction_output_type& previous_output =
        previous_tx.outputs[previous_outpoint.index];
    uint64_t output_value = previous_output.value;
    if (output_value > max_money())
        return false;
    if (is_coinbase(previous_tx))
    {
        size_t height_difference = last_block_height - parent_height;
        if (height_difference < coinbase_maturity)
            return false;
    }
    script_type output_script = previous_output.script;
    if (!output_script.run(input.script, tx, current_input))
        return false;
    value_in += output_value;
    if (value_in > max_money())
        return false;
    return true;
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
            next_previous_transaction();
        }
    }
    else
    {
        BITCOIN_ASSERT(!ec || ec != error::unspent_output);
        handle_validate_(error::double_spend, index_list());
    }
}

bool validate_transaction::tally_fees(const transaction_type& tx,
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
    handle_validate_(std::error_code(), unconfirmed_);
}

std::error_code validate_transaction::check_transaction(
    const transaction_type& tx)
{
    if (tx.inputs.empty() || tx.outputs.empty())
        return error::empty_transaction;

    // Maybe not needed since we try to serialise block in CheckBlock()
    //if (exporter_->save(tx, false).size() > max_block_size)
    //    return false;

    // Check for negative or overflow output values
    uint64_t total_output_value = 0;
    for (transaction_output_type output: tx.outputs)
    {
        if (output.value > max_money())
            return error::output_value_overflow;
        total_output_value += output.value;
        if (total_output_value > max_money())
            return error::output_value_overflow;
    }

    if (is_coinbase(tx))
    {
        const script_type& coinbase_script = tx.inputs[0].script;
        size_t coinbase_script_size = save_script(coinbase_script).size();
        if (coinbase_script_size < 2 || coinbase_script_size > 100)
            return error::invalid_coinbase_script_size;
    }
    else
    {
        for (transaction_input_type input: tx.inputs)
            if (previous_output_is_null(input.previous_output))
                return error::previous_output_null;
    }

    return std::error_code();
}

validate_block::validate_block(
    size_t height, const block_type& current_block)
  : height_(height), current_block_(current_block)
{
}

std::error_code validate_block::start()
{
    std::error_code ec;
    ec = check_block();
    if (ec)
        return ec;
    ec = accept_block();
    if (ec)
        return ec;
    ec = connect_block();
    if (ec)
        return ec;
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
        satoshi_raw_size(current_block_) > max_block_size)
    {
        return error::size_limits;
    }

    const block_header_type& blk_header = current_block_.header;
    const hash_digest current_block_hash =
        hash_block_header(blk_header);
    if (!check_proof_of_work(current_block_hash, blk_header.bits))
        return error::proof_of_work;

    const posix_time::ptime block_time =
        posix_time::from_time_t(blk_header.timestamp);
    const posix_time::ptime two_hour_future =
        posix_time::second_clock::universal_time() + posix_time::hours(2);
    if (block_time > two_hour_future)
        return error::futuristic_timestamp;

    if (!is_coinbase(current_block_.transactions[0]))
        return error::first_not_coinbase;
    for (size_t i = 1; i < current_block_.transactions.size(); ++i)
    {
        const transaction_type& tx = current_block_.transactions[i];
        if (is_coinbase(tx))
            return error::extra_coinbases;
    }

    std::set<hash_digest> unique_txs;
    for (transaction_type tx: current_block_.transactions)
    {
        std::error_code ec = validate_transaction::check_transaction(tx);
        if (ec)
            return ec;
        unique_txs.insert(hash_transaction(tx));
    }
    if (unique_txs.size() != current_block_.transactions.size())
        return error::duplicate;

    // Check that it's not full of nonstandard transactions
    if (legacy_sigops_count() > max_block_script_sig_operations)
        return error::too_many_sigs;

    if (blk_header.merkle != generate_merkle_root(current_block_.transactions))
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

inline size_t count_script_sigops(
    const operation_stack& operations, bool accurate)
{
    size_t total_sigs = 0, last_number = 0;
    for (const operation& op: operations)
    {
        if (op.code == opcode::checksig ||
            op.code == opcode::checksigverify)
        {
            total_sigs++;
        }
        else if (op.code == opcode::checkmultisig ||
            op.code == opcode::checkmultisigverify)
        {
            if (accurate && last_number != 0)
                total_sigs += last_number;
            else
                total_sigs += 20;
        }
        uint8_t raw_code = static_cast<uint8_t>(op.code);
        if (static_cast<uint8_t>(opcode::op_1) <= raw_code &&
            static_cast<uint8_t>(opcode::op_16) >= raw_code)
        {
            last_number = raw_code;
        }
    }
    return total_sigs;
}
size_t tx_legacy_sigops_count(const transaction_type& tx)
{
    size_t total_sigs = 0;
    for (transaction_input_type input: tx.inputs)
    {
        const operation_stack& operations = input.script.operations();
        total_sigs += count_script_sigops(operations, false);
    }
    for (transaction_output_type output: tx.outputs)
    {
        const operation_stack& operations = output.script.operations();
        total_sigs += count_script_sigops(operations, false);
    }
    return total_sigs;
}
size_t validate_block::legacy_sigops_count()
{
    size_t total_sigs = 0;
    for (transaction_type tx: current_block_.transactions)
        total_sigs += tx_legacy_sigops_count(tx);
    return total_sigs;
}

std::error_code validate_block::accept_block()
{
    const block_header_type& blk_header = current_block_.header;
    if (blk_header.bits != work_required())
        return error::incorrect_proof_of_work;
    if (blk_header.timestamp <= median_time_past())
        return error::timestamp_too_early;
    // Txs should be final when included in a block
    for (const transaction_type& tx: current_block_.transactions)
        if (!is_final(tx, height_, blk_header.timestamp))
            return error::non_final_transaction;
    if (!passes_checkpoints())
        return error::checkpoints_failed;
    // Reject version=1 blocks after switchover point.
    if (height_ > 237370 && blk_header.version < 2)
        return error::old_version_block;
    // Enforce version=2 rule that coinbase starts with serialized height.
    if (blk_header.version >= 2 && !coinbase_height_match())
        return error::coinbase_height_mismatch;
    return std::error_code();
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
#ifdef ENABLE_TESTNET
    auto last_non_special_bits = [this]
    {
        // Return the last non-special block
        block_header_type previous_block;
        size_t previous_height = height_;
        // Loop backwards until we find a difficulty change point,
        // or we find a block which does not have max_bits (is not special).
        while (true)
        {
            --previous_height;
            previous_block = fetch_block(previous_height);
            if (previous_height % readjustment_interval == 0)
               break;
            if (previous_block.bits != max_bits)
               break;
        }
        return previous_block.bits;
    };
#endif

    if (height_ == 0)
        return max_bits;
    else if (height_ % readjustment_interval != 0)
    {
#ifdef ENABLE_TESTNET
        uint32_t max_time_gap =
            fetch_block(height_ - 1).timestamp + 2 * target_spacing;
        if (current_block_.header.timestamp > max_time_gap)
            return max_bits;
        return last_non_special_bits();
#else
        return previous_block_bits();
#endif
    }

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
    const hash_digest block_hash = hash_block_header(current_block_.header);

#ifdef ENABLE_TESTNET
    if (height_ == 546 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x2a, 0x93, 0x6c, 0xa7,
                        0x63, 0x90, 0x4c, 0x3c, 0x35, 0xfc, 0xe2, 0xf3,
                        0x55, 0x6c, 0x55, 0x9c, 0x02, 0x14, 0x34, 0x5d,
                        0x31, 0xb1, 0xbc, 0xeb, 0xf7, 0x6a, 0xcb, 0x70})
        return false;

    return true;
#else
    if (height_ == 11111 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x69, 0xe2, 0x44, 0xf7,
                        0x3d, 0x78, 0xe8, 0xfd, 0x29, 0xba, 0x2f, 0xd2,
                        0xed, 0x61, 0x8b, 0xd6, 0xfa, 0x2e, 0xe9, 0x25,
                        0x59, 0xf5, 0x42, 0xfd, 0xb2, 0x6e, 0x7c, 0x1d})
        return false;

    if (height_ ==  33333 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x2d, 0xd5, 0x58, 0x8a,
                        0x74, 0x78, 0x4e, 0xaa, 0x7a, 0xb0, 0x50, 0x7a,
                        0x18, 0xad, 0x16, 0xa2, 0x36, 0xe7, 0xb1, 0xce,
                        0x69, 0xf0, 0x0d, 0x7d, 0xdf, 0xb5, 0xd0, 0xa6})
        return false;

    if (height_ ==  68555 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x1b, 0x49,
                        0x03, 0x55, 0x0a, 0x0b, 0x96, 0xe9, 0xa9, 0x40,
                        0x5c, 0x8a, 0x95, 0xf3, 0x87, 0x16, 0x2e, 0x49,
                        0x44, 0xe8, 0xd9, 0xfb, 0xe5, 0x01, 0xcd, 0x6a})
        return false;

    if (height_ ==  70567 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0x49, 0xb1,
                        0x4b, 0xcf, 0x27, 0x46, 0x20, 0x68, 0xf1, 0x26,
                        0x4c, 0x96, 0x1f, 0x11, 0xfa, 0x2e, 0x0e, 0xdd,
                        0xd2, 0xbe, 0x07, 0x91, 0xe1, 0xd4, 0x12, 0x4a})
        return false;

    if (height_ ==  74000 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x57, 0x39, 0x93,
                        0xa3, 0xc9, 0xe4, 0x1c, 0xe3, 0x44, 0x71, 0xc0,
                        0x79, 0xdc, 0xf5, 0xf5, 0x2a, 0x0e, 0x82, 0x4a,
                        0x81, 0xe7, 0xf9, 0x53, 0xb8, 0x66, 0x1a, 0x20})
        return false;

    if (height_ == 105000 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x91, 0xce,
                        0x28, 0x02, 0x7f, 0xae, 0xa3, 0x20, 0xc8, 0xd2,
                        0xb0, 0x54, 0xb2, 0xe0, 0xfe, 0x44, 0xa7, 0x73,
                        0xf3, 0xee, 0xfb, 0x15, 0x1d, 0x6b, 0xdc, 0x97})
        return false;

    if (height_ == 118000 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x4a,
                        0x7f, 0x8a, 0x7a, 0x12, 0xdc, 0x90, 0x6d, 0xdb,
                        0x9e, 0x17, 0xe7, 0x5d, 0x68, 0x4f, 0x15, 0xe0,
                        0x0f, 0x87, 0x67, 0xf9, 0xe8, 0xf3, 0x65, 0x53})
        return false;

    if (height_ == 134444 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xb1,
                        0x2f, 0xfd, 0x4c, 0xd3, 0x15, 0xcd, 0x34, 0xff,
                        0xd4, 0xa5, 0x94, 0xf4, 0x30, 0xac, 0x81, 0x4c,
                        0x91, 0x18, 0x4a, 0x0d, 0x42, 0xd2, 0xb0, 0xfe})
        return false;

    if (height_ == 140700 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3b,
                        0x51, 0x20, 0x28, 0xab, 0xb9, 0x0e, 0x16, 0x26,
                        0xd8, 0xb3, 0x46, 0xfd, 0x0e, 0xd5, 0x98, 0xac,
                        0x0a, 0x3c, 0x37, 0x11, 0x38, 0xdc, 0xe2, 0xbd})
        return false;

    if (height_ == 168000 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x9e,
                        0x61, 0xea, 0x72, 0x01, 0x5e, 0x79, 0x63, 0x2f,
                        0x21, 0x6f, 0xe6, 0xcb, 0x33, 0xd7, 0x89, 0x9a,
                        0xcb, 0x35, 0xb7, 0x5c, 0x83, 0x03, 0xb7, 0x63})
        return false;

    if (height_ == 193000 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9f,
                        0x45, 0x2a, 0x5f, 0x73, 0x40, 0xde, 0x66, 0x82,
                        0xa9, 0x77, 0x38, 0x7c, 0x17, 0x01, 0x0f, 0xf6,
                        0xe6, 0xc3, 0xbd, 0x83, 0xca, 0x8b, 0x13, 0x17})
        return false;

    if (height_ == 210000 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x8b,
                        0x95, 0x34, 0x7e, 0x83, 0x19, 0x2f, 0x69, 0xcf,
                        0x03, 0x66, 0x07, 0x63, 0x36, 0xc6, 0x39, 0xf9,
                        0xb7, 0x22, 0x8e, 0x9b, 0xa1, 0x71, 0x34, 0x2e})
        return false;

    if (height_ == 216116 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xb4,
                        0xf4, 0xb4, 0x33, 0xe8, 0x1e, 0xe4, 0x64, 0x94,
                        0xaf, 0x94, 0x5c, 0xf9, 0x60, 0x14, 0x81, 0x6a,
                        0x4e, 0x23, 0x70, 0xf1, 0x1b, 0x23, 0xdf, 0x4e})
        return false;

    if (height_ == 225430 && block_hash !=
            hash_digest{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc1,
                        0x08, 0x38, 0x43, 0x50, 0xf7, 0x40, 0x90, 0x43,
                        0x3e, 0x7f, 0xcf, 0x79, 0xa6, 0x06, 0xb8, 0xe7,
                        0x97, 0xf0, 0x65, 0xb1, 0x30, 0x57, 0x59, 0x32})
        return false;

    return true;
#endif
}

bool validate_block::coinbase_height_match()
{
    // There are old blocks with version incorrectly set to 2. Ignore them.
    if (height_ < 237370)
        return true;
    // Checks whether the block height is in the coinbase
    // transaction input script.
    // Version 2 blocks and onwards.
    BITCOIN_ASSERT(current_block_.header.version >= 2);
    BITCOIN_ASSERT(current_block_.transactions.size() > 0);
    BITCOIN_ASSERT(current_block_.transactions[0].inputs.size() > 0);
    // First get the serialized coinbase input script as a series of bytes.
    const script_type& coinbase_script =
        current_block_.transactions[0].inputs[0].script;
    const data_chunk raw_coinbase = save_script(coinbase_script);
    // Try to recreate the expected bytes.
    big_number expect_number;
    expect_number.set_int64(height_);
    script_type expect_coinbase;
    expect_coinbase.push_operation({opcode::special, expect_number.data()});
    // Save the expected coinbase script.
    const data_chunk expect = save_script(expect_coinbase);
    // Perform comparison of the first bytes with raw_coinbase.
    BITCOIN_ASSERT(expect.size() <= raw_coinbase.size());
    return std::equal(expect.begin(), expect.end(), raw_coinbase.begin());
}

std::error_code validate_block::connect_block()
{
    // BIP 30 security fix
    if (height_ != 91842 && height_ != 91880)
        for (const auto& current_tx: current_block_.transactions)
            if (!not_duplicate_or_spent(current_tx))
                return error::duplicate_or_spent;

    uint64_t fees = 0;
    size_t total_sigops = 0;
    for (size_t tx_index = 0; tx_index < current_block_.transactions.size();
            ++tx_index)
    {
        uint64_t value_in = 0;
        const transaction_type& tx = current_block_.transactions[tx_index];
        total_sigops += tx_legacy_sigops_count(tx);
        if (total_sigops > max_block_script_sig_operations)
            return error::too_many_sigs;
        // Count sigops for tx 0, but we don't perform
        // the other checks on coinbase tx.
        if (is_coinbase(tx))
            continue;
        if (!validate_inputs(tx, tx_index, value_in, total_sigops))
            return error::validate_inputs_failed;
        if (!validate_transaction::tally_fees(tx, value_in, fees))
            return error::fees_out_of_range;
    }
    uint64_t coinbase_value =
        total_output_value(current_block_.transactions[0]);
    if (coinbase_value  > block_value(height_) + fees)
        return error::coinbase_too_large;
    return std::error_code();
}

bool validate_block::not_duplicate_or_spent(const transaction_type& tx)
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

bool validate_block::validate_inputs(const transaction_type& tx,
    size_t index_in_parent, uint64_t& value_in, size_t& total_sigops)
{
    BITCOIN_ASSERT(!is_coinbase(tx));
    for (size_t input_index = 0; input_index < tx.inputs.size(); ++input_index)
    {
        if (!connect_input(index_in_parent, tx, input_index,
                value_in, total_sigops))
        {
            log_warning(LOG_VALIDATE) << "Validate input "
                << hash_transaction(tx) << ":" << input_index << " failed";
            return false;
        }
    }
    return true;
}

size_t script_hash_signature_operations_count(
    const script_type& output_script, const script_type& input_script)
{
    if (output_script.type() != payment_type::script_hash)
        return 0;
    if (input_script.operations().empty())
        return 0;
    const data_chunk& last_data = input_script.operations().back().data;
    script_type eval_script = parse_script(last_data);
    return count_script_sigops(eval_script.operations(), true);
}

bool validate_block::connect_input(size_t index_in_parent,
    const transaction_type& current_tx,
    size_t input_index, uint64_t& value_in, size_t& total_sigops)
{
    // Lookup previous output
    BITCOIN_ASSERT(input_index < current_tx.inputs.size());
    const transaction_input_type& input = current_tx.inputs[input_index];
    const output_point& previous_output = input.previous_output;
    transaction_type previous_tx;
    size_t previous_height;
    if (!fetch_transaction(previous_tx, previous_height, previous_output.hash))
    {
        log_warning(LOG_VALIDATE) << "Unable to fetch input transaction";
        return false;
    }
    const transaction_output_type& previous_tx_out =
        previous_tx.outputs[previous_output.index];
    // Signature operations count if script_hash payment type.
    try
    {
        total_sigops += script_hash_signature_operations_count(
            previous_tx_out.script, input.script);
    }
    catch (end_of_stream)
    {
        log_warning(LOG_VALIDATE) << "Parsing eval script failed.";
        return false;
    }
    if (total_sigops > max_block_script_sig_operations)
    {
        log_warning(LOG_VALIDATE) << "Total sigops exceeds block maximum";
        return false;
    }
    // Get output amount
    uint64_t output_value = previous_tx_out.value;
    if (output_value > max_money())
    {
        log_warning(LOG_VALIDATE) << "Total sigops exceeds block maximum";
        return false;
    }
    // Check coinbase maturity has been reached
    if (is_coinbase(previous_tx))
    {
        BITCOIN_ASSERT(previous_height <= height_);
        uint32_t height_difference = height_ - previous_height;
        if (height_difference < coinbase_maturity)
        {
            log_warning(LOG_VALIDATE) << "Spends immature coinbase";
            return false;
        }
    }
    // Pay to script hash BIP 16 scripts
    // block 170060 contains an invalid BIP 16 transaction
    // before the switchover date.
    bool bip16_enabled =
        current_block_.header.timestamp >= bip16_switchover_timestamp;
    BITCOIN_ASSERT(!bip16_enabled || height_ >= bip16_switchover_height);
    // Validate script
    script_type output_script = previous_tx_out.script;
    if (!output_script.run(input.script,
            current_tx, input_index, bip16_enabled))
    {
        log_warning(LOG_VALIDATE) << "Input script evaluation failed";
        return false;
    }
    // Search for double spends
    if (is_output_spent(previous_output, index_in_parent, input_index))
    {
        log_warning(LOG_VALIDATE) << "Double spend detected";
        return false;
    }
    // Increase value_in by this output's value
    value_in += output_value;
    if (value_in > max_money())
    {
        log_warning(LOG_VALIDATE) << "Total input money over 21 million";
        return false;
    }
    return true;
}

} // namespace libbitcoin

