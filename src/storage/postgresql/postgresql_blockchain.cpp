#include "postgresql_blockchain.hpp"

#include <bitcoin/constants.hpp>
#include <bitcoin/dialect.hpp>
#include <bitcoin/kernel.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

std::string spaced_bytes(std::string byte_stream)
{
    std::string spaced = "";
    for (auto it = byte_stream.begin(); ; )
    {
        spaced += *it;
        ++it;
        spaced += *it;
        ++it;
        if (it == byte_stream.end())
            break;
        else
            spaced += ' ';
    }
    return spaced;
}

inline data_chunk bytes_from_bytea(std::string byte_stream)
{
    return bytes_from_pretty(spaced_bytes(byte_stream));
}

inline hash_digest hash_from_bytea(std::string byte_stream)
{
    return hash_from_pretty(spaced_bytes(byte_stream));
}

inline uint32_t combine_bits(uint32_t bits_head, uint32_t bits_body)
{
    return bits_body + (bits_head << (3*8));
}

inline uint32_t extract_bits_head(uint32_t bits)
{
    return bits >> (8*3);
}
inline uint32_t extract_bits_body(uint32_t bits)
{
    return bits & 0x00ffffff;
}

pq_organizer::pq_organizer(cppdb::session sql, kernel_ptr kernel)
  : sql_(sql), kernel_(kernel)
{
}

void pq_organizer::delete_chains(size_t left, size_t right)
{
    static cppdb::statement delete_chains = sql_.prepare(
        "DELETE FROM chains \
        WHERE chain_id BETWEEN ? AND ?"
        );
    delete_chains.reset();
    delete_chains.bind(left);
    delete_chains.bind(right);
    delete_chains.exec();

    size_t offset = (right + 1) - left;

    static cppdb::statement adjust_chains = sql_.prepare(
        "UPDATE chains \
        SET chain_id = chain_id - ? \
        WHERE chain_id > ?"
        );
    adjust_chains.reset();
    adjust_chains.bind(offset);
    adjust_chains.bind(right);
    adjust_chains.exec();
}

void pq_organizer::unwind_chain(size_t depth, size_t chain_id)
{
    static cppdb::statement unwind_chain = sql_.prepare(
        "UPDATE chains \
        SET work = work - \
            (SELECT SUM(difficulty(bits_head, bits_body)) \
            FROM blocks \
            WHERE \
                space=0 \
                AND depth >= ? \
                AND span_left <= ? \
                AND span_right >= ?) \
                AND status='valid') \
        WHERE chain_id=?"
        );
    unwind_chain.reset();
    unwind_chain.bind(depth);
    unwind_chain.bind(chain_id);
    unwind_chain.bind(chain_id);
    unwind_chain.bind(chain_id);
    unwind_chain.exec();
}

void pq_organizer::delete_branch(size_t space, size_t depth, 
    size_t span_left, size_t span_right)
{
    static cppdb::statement lonely_child = sql_.prepare(
        "SELECT 1 \
        FROM blocks \
        WHERE \
            space = ? \
            AND depth = ? - 1 \
            AND span_left = ? \
            AND span_right = ? \
        LIMIT 1"
        );
    lonely_child.reset();
    lonely_child.bind(space);
    lonely_child.bind(depth);
    lonely_child.bind(span_left);
    lonely_child.bind(span_right);

    size_t offset = span_right - span_left;

    if (lonely_child.row().empty())
    {
        offset++;
        delete_chains(span_left, span_right);
    }
    else
    {
        delete_chains(span_left + 1, span_right);
        unwind_chain(depth, span_left);
    }
    
    static cppdb::statement delete_branch = sql_.prepare(
        "DELETE FROM blocks \
        WHERE \
            space=? \
            AND depth >= ? \
            AND span_left >= ? \
            AND span_right <= ?"
        );
    delete_branch.reset();
    delete_branch.bind(space);
    delete_branch.bind(depth);
    delete_branch.bind(span_left);
    delete_branch.bind(span_right);
    delete_branch.exec();

    static cppdb::statement adjust_left = sql_.prepare(
        "UPDATE blocks \
        SET span_left = span_left - ? \
        WHERE  \
            space = ? \
            AND span_left > ?"
        );
    adjust_left.reset();
    adjust_left.bind(offset);
    adjust_left.bind(space);
    adjust_left.bind(span_right);
    adjust_left.exec();

    static cppdb::statement adjust_right = sql_.prepare(
        "UPDATE blocks \
        SET span_right = span_right - ? \
        WHERE  \
            space = ? \
            AND span_right >= ?"
        );
    adjust_right.reset();
    adjust_right.bind(offset);
    adjust_right.bind(space);
    adjust_right.bind(span_right);
    adjust_right.exec();
}

void pq_organizer::organize()
{
    static cppdb::statement orphans_statement = sql_.prepare(
        "SELECT \
            block.block_id, \
            block.space, \
            block.depth, \
            parent.block_id \
        FROM \
            blocks block, \
            blocks parent \
        WHERE \
            block.prev_block_hash=parent.block_hash \
            AND block.space > 0 \
            AND block.depth=0"
        );
    orphans_statement.reset();
    cppdb::result orphans_results = orphans_statement.query();
    std::set<size_t> reorganized_blocks;
    while (orphans_results.next())
    {
        size_t child_id = orphans_results.get<size_t>(0),
            child_space = orphans_results.get<size_t>(1),
            child_depth = orphans_results.get<size_t>(2),
            parent_id = orphans_results.get<size_t>(3);
        BITCOIN_ASSERT(child_depth == 0);

        size_t parent_space, parent_depth;
        span parent_span;
        // Parent depth and space can change if it is
        // joined to another branch
        if (!load_position_info(parent_id, 
                parent_space, parent_depth, parent_span))
        {
            // Something went very wrong. Stop.
            return;
        }

        // During this loop the span can be modified so we can't load it above
        span child_span;
        if (!load_span(child_id, child_span))
            return;
        BITCOIN_ASSERT(child_span.left == 0);

        size_t parent_width = 
            get_block_width(parent_space, parent_depth, parent_span);
        size_t child_width = child_span.right - child_span.left + 1;

        size_t new_child_span_left = parent_span.right;
        if (parent_width > 0)
            new_child_span_left++;

        size_t new_child_depth = parent_depth + 1;
        reserve_branch_area(parent_space, parent_width, parent_span, 
            new_child_depth, child_width);
        position_child_branch(child_space, parent_space, new_child_depth, 
            new_child_span_left);

        reorganized_blocks.insert(child_id);
    }
    get_inbetweens(reorganized_blocks);
}

void pq_organizer::get_inbetweens(std::set<size_t> block_ids)
{
    block_ids.insert(recent_blocks_.begin(), recent_blocks_.end());
    recent_blocks_.clear();
    if (block_ids.empty())
        return;

    std::stringstream hack_sql;
    hack_sql <<
        "SELECT DISTINCT ON (process_blocks.space) \
            encode(root_blocks.block_hash, 'hex') \
        FROM \
            blocks process_blocks, \
            blocks root_blocks \
        WHERE \
            process_blocks.space > 0 \
            AND process_blocks.space = root_blocks.space \
            AND root_blocks.depth = 0 \
            AND process_blocks.block_id IN (";
    for (auto it = block_ids.begin(); it != block_ids.end(); ++it)
    {
        if (it != block_ids.begin())
            hack_sql << ", ";
        hack_sql << *it;
    }
    hack_sql << ")";
    cppdb::result block_hashes_result = sql_ << hack_sql.str();
    hash_list block_hashes;
    while (block_hashes_result.next())
        block_hashes.push_back(
            hash_from_bytea(block_hashes_result.get<std::string>(0)));
    if (!block_hashes.empty())
        kernel_->tween_blocks(block_hashes);
}

void pq_organizer::refresh_block(size_t block_id)
{
    recent_blocks_.insert(block_id);
}

bool pq_organizer::load_span(size_t block_id, span& spn)
{
    static cppdb::statement statement = sql_.prepare(
        "SELECT \
            span_left, \
            span_right \
        FROM blocks \
        WHERE block_id=?"
        );
    statement.reset();
    statement.bind(block_id);
    cppdb::result result = statement.row();
    if (result.empty())
    {
        log_fatal() << "load_span() failed for block " << block_id;
        return false;
    }
    spn.left = result.get<size_t>(0);
    spn.right = result.get<size_t>(1);
    BITCOIN_ASSERT(spn.left <= spn.right);
    return true;
}

bool pq_organizer::load_position_info(size_t block_id, 
    size_t& space, size_t& depth, span& spn)
{
    static cppdb::statement statement = sql_.prepare(
        "SELECT \
            space, \
            depth, \
            span_left, \
            span_right \
        FROM blocks \
        WHERE block_id=?"
        );
    statement.reset();
    statement.bind(block_id);
    cppdb::result result = statement.row();
    if (result.empty())
    {
        log_fatal() << "load_parent() failed for block " << block_id;
        return false;
    }
    space = result.get<size_t>(0);
    depth = result.get<size_t>(1);
    spn.left = result.get<size_t>(2);
    spn.right = result.get<size_t>(3);
    BITCOIN_ASSERT(spn.left <= spn.right);
    return true;
}

size_t pq_organizer::get_block_width(
        size_t space, size_t depth, span block_span)
{
    // If parent's span_left < span_right then parent has children
    if (block_span.left < block_span.right)
        return block_span.right - block_span.left + 1;
    static cppdb::statement statement = sql_.prepare(
        "SELECT 1 \
        FROM blocks \
        WHERE \
            space=? \
            AND depth > ? \
            AND span_left >= ? \
            AND span_right <= ? \
        LIMIT 1"
        );
    statement.reset();
    statement.bind(space);
    statement.bind(depth);
    statement.bind(block_span.left);
    statement.bind(block_span.right);
    cppdb::result has_children_result = statement.row();
    if (has_children_result.empty())
    {
        BITCOIN_ASSERT(block_span.left == block_span.left);
        return 0;
    }
    return 1;
}

void pq_organizer::reserve_branch_area(size_t parent_space, 
    size_t parent_width, const span& parent_span, 
        size_t new_child_depth, size_t child_width)
{
    if (parent_width == 0 && child_width == 1)
        // Do nothing
        return;

    // Shift everything to the right
    static cppdb::statement update_right = sql_.prepare(
        "UPDATE blocks \
        SET span_right = span_right + ? \
        WHERE \
            space=? \
            AND span_right > ?"
        );
    update_right.reset();
    update_right.bind(child_width);
    update_right.bind(parent_space);
    update_right.bind(parent_span.right);
    update_right.exec();

    static cppdb::statement update_left = sql_.prepare(
        "UPDATE blocks \
        SET span_left = span_left + ? \
        WHERE \
            space=? \
            AND span_left > ?"
        );
    update_left.reset();
    update_left.bind(child_width);
    update_left.bind(parent_space);
    update_left.bind(parent_span.right);
    update_left.exec();

    // Expand parent's right bracket
    static cppdb::statement update_parents = sql_.prepare(
        "UPDATE blocks \
        SET span_right = span_right + ? \
        WHERE \
            space=? \
            AND depth < ? \
            AND span_right=?"
        );
    update_parents.reset();
    update_parents.bind(child_width);
    update_parents.bind(parent_space);
    update_parents.bind(new_child_depth);
    update_parents.bind(parent_span.right);
    update_parents.exec();

    // Chains only apply to space 0
    if (parent_space != 0)
        return;

    // Fix chain info
    static cppdb::statement update_other_chains = sql_.prepare(
        "UPDATE chains \
        SET chain_id = chain_id + ? \
        WHERE chain_id > ?"
        );
    update_other_chains.reset();
    update_other_chains.bind(child_width);
    update_other_chains.bind(parent_span.right);
    update_other_chains.exec();

    static cppdb::statement tween_chains = sql_.prepare(
        "INSERT INTO chains ( \
            work, \
            chain_id, \
            depth \
        ) SELECT \
            work, \
            chain_id + ?, \
            depth \
        FROM chains \
        WHERE chain_id=?"
        );
    for (size_t sub_chain = parent_width; 
            sub_chain < parent_width + child_width; ++sub_chain)
    {
        tween_chains.reset();
        tween_chains.bind(sub_chain);
        tween_chains.bind(parent_span.left);
        tween_chains.exec();
    }
}

void pq_organizer::position_child_branch(
    size_t old_space, size_t new_space, size_t new_depth, size_t new_span_left)
{
    static cppdb::statement statement = sql_.prepare(
        "UPDATE blocks \
        SET \
            space=?, \
            depth = depth + ?, \
            span_left = span_left + ?, \
            span_right = span_right + ? \
        WHERE space=?"
        );
    statement.reset();
    statement.bind(new_space);
    statement.bind(new_depth);
    statement.bind(new_span_left);
    statement.bind(new_span_left);
    statement.bind(old_space);
    statement.exec();
}

pq_reader::pq_reader(cppdb::session sql)
  : sql_(sql)
{
}

message::transaction_input_list pq_reader::select_inputs(
        size_t transaction_id, std::vector<size_t>& input_ids)
{
    static cppdb::statement statement = sql_.prepare(
        "SELECT \
            input_id, \
            encode(previous_output_hash, 'hex') AS previous_output_hash, \
            previous_output_index, \
            encode(script, 'hex') AS script, \
            sequence \
        FROM inputs \
        WHERE transaction_id=? \
        ORDER BY index_in_parent ASC"
        );
    statement.reset();
    statement.bind(transaction_id);
    cppdb::result result = statement.query();
    message::transaction_input_list inputs;
    while (result.next())
    {
        input_ids.push_back(result.get<size_t>("input_id"));
        message::transaction_input input;
        input.hash = 
            hash_from_bytea(result.get<std::string>("previous_output_hash"));
        input.index = result.get<uint32_t>("previous_output_index");
        data_chunk raw_script = 
            bytes_from_bytea(result.get<std::string>("script"));
        if (previous_output_is_null(input))
            input.input_script = coinbase_script(raw_script);
        else
            input.input_script = parse_script(raw_script);
        input.sequence = result.get<uint32_t>("sequence");
        inputs.push_back(input);
    }
    return inputs;
}
message::transaction_output_list pq_reader::select_outputs(
        size_t transaction_id, std::vector<size_t>& output_ids)
{
    static cppdb::statement statement = sql_.prepare(
        "SELECT \
            output_id, \
            transaction_id, \
            encode(script, 'hex') AS script, \
            sql_to_internal(value) internal_value \
        FROM outputs \
        WHERE transaction_id=? \
        ORDER BY index_in_parent ASC"
        );
    statement.reset();
    statement.bind(transaction_id);
    cppdb::result result = statement.query();
    message::transaction_output_list outputs;
    while (result.next())
    {
        output_ids.push_back(result.get<size_t>("output_id"));
        message::transaction_output output;
        output.value = result.get<uint64_t>("internal_value");
        output.output_script = 
            parse_script(bytes_from_bytea(result.get<std::string>("script")));
        outputs.push_back(output);
    }
    return outputs;
}

message::transaction_list pq_reader::read_transactions(
        cppdb::result result, pq_transaction_info_list& tx_infos)
{
    message::transaction_list transactions;
    while (result.next())
    {
        pq_transaction_info transaction_info;
        message::transaction transaction;
        transaction.version = result.get<uint32_t>("version");
        transaction.locktime = result.get<uint32_t>("locktime");
        size_t transaction_id = result.get<size_t>("transaction_id");
        transaction_info.transaction_id = transaction_id;
        transaction.inputs = 
            select_inputs(transaction_id, transaction_info.input_ids);
        transaction.outputs = 
            select_outputs(transaction_id, transaction_info.output_ids);
        tx_infos.push_back(transaction_info);
        transactions.push_back(transaction);
    }
    return transactions;
}

pq_block pq_reader::read_block(cppdb::result block_result)
{
    pq_block_info block_info;
    block_info.block_id = block_result.get<size_t>("block_id");
    block_info.depth = block_result.get<size_t>("depth");
    block_info.span_left = block_result.get<size_t>("span_left");
    block_info.span_right = block_result.get<size_t>("span_right");

    message::block block;
    block.version = block_result.get<uint32_t>("version");
    block.timestamp = block_result.get<uint32_t>("timest");
    uint32_t bits_head = block_result.get<uint32_t>("bits_head"),
            bits_body = block_result.get<uint32_t>("bits_body");
    block.bits = combine_bits(bits_head, bits_body);
    block.nonce = block_result.get<uint32_t>("nonce");

    block.prev_block = 
            hash_from_bytea(block_result.get<std::string>("prev_block_hash"));
    block.merkle_root = 
            hash_from_bytea(block_result.get<std::string>("merkle"));

    static cppdb::statement transactions_statement = sql_.prepare(
        "SELECT transactions.* \
        FROM \
            transactions_parents, \
            transactions \
        WHERE \
            block_id=? \
            AND transactions.transaction_id=transactions_parents.transaction_id \
        ORDER BY index_in_block ASC"
        );
    transactions_statement.reset();
    transactions_statement.bind(block_info.block_id);
    cppdb::result transactions_result = transactions_statement.query();
    block.transactions = 
        read_transactions(transactions_result, block_info.transactions);

    return std::make_pair(block_info, block);
}

pq_validate_block::pq_validate_block(cppdb::session sql, 
    dialect_ptr dialect, pq_reader_ptr reader, 
    const pq_block_info& block_info, const message::block& current_block)
  : validate_block(dialect, block_info.depth, current_block), 
    sql_(sql), reader_(reader), 
    block_info_(block_info), current_block_(current_block)
{
}

uint32_t pq_validate_block::previous_block_bits()
{
    static cppdb::statement previous = sql_.prepare(
        "SELECT bits_head, bits_body \
        FROM blocks \
        WHERE \
            space = 0 \
            AND depth = ? - 1 \
            AND span_left <= ? \
            AND span_right >= ?"
        );
    previous.reset();
    previous.bind(block_info_.depth);
    previous.bind(block_info_.span_left);
    previous.bind(block_info_.span_right);
    cppdb::result result = previous.row();
    BITCOIN_ASSERT(!result.empty());
    uint32_t bits_head = result.get<uint32_t>("bits_head"),
            bits_body = result.get<uint32_t>("bits_body");
    return combine_bits(bits_head, bits_body);
}

uint64_t pq_validate_block::actual_timespan(const uint64_t interval)
{
    BITCOIN_ASSERT(block_info_.depth >= interval);
    size_t begin_block_depth = block_info_.depth - interval,
        end_block_depth = block_info_.depth - 1;
    static cppdb::statement find_start = sql_.prepare(
        "SELECT EXTRACT(EPOCH FROM \
            end_block.when_created - start_block.when_created) \
        FROM \
            blocks as start_block, \
            blocks as end_block \
        WHERE \
            start_block.space = 0 \
            AND start_block.depth = ? \
            AND start_block.span_left <= ? \
            AND start_block.span_right >= ? \
            \
            AND end_block.space = 0 \
            AND end_block.depth = ? \
            AND end_block.span_left <= ? \
            AND end_block.span_right >= ?"
        );
    find_start.reset();
    find_start.bind(begin_block_depth);
    find_start.bind(block_info_.span_left);
    find_start.bind(block_info_.span_right);
    find_start.bind(end_block_depth);
    find_start.bind(block_info_.span_left);
    find_start.bind(block_info_.span_right);
    cppdb::result result = find_start.row();
    return result.get<uint32_t>(0);
}

uint64_t pq_validate_block::median_time_past()
{
    BITCOIN_ASSERT(block_info_.depth > 0);
    size_t median_offset = 5;
    if (block_info_.depth < 11)
        median_offset = block_info_.depth / 2;

    static cppdb::statement find_median = sql_.prepare(
        "SELECT EXTRACT(EPOCH FROM when_created) \
        FROM blocks \
        WHERE \
            space = 0 \
            AND depth < ? \
            AND depth >= ? - 11 \
            AND span_left <= ? \
            AND span_right >= ? \
        ORDER BY when_created \
        LIMIT 1 \
        OFFSET ?"
        );
    find_median.reset();
    find_median.bind(block_info_.depth);
    find_median.bind(block_info_.depth);
    find_median.bind(block_info_.span_left);
    find_median.bind(block_info_.span_right);
    find_median.bind(median_offset);
    cppdb::result result = find_median.row();
    return result.get<uint32_t>(0);
}

bool pq_validate_block::validate_transaction(
    const message::transaction& tx, size_t index_in_block, 
    uint64_t& value_in)
{
    BITCOIN_ASSERT(block_info_.transactions.size() > index_in_block);
    const pq_transaction_info& tx_info = 
        block_info_.transactions[index_in_block];
    size_t transaction_id = tx_info.transaction_id;

    BITCOIN_ASSERT(!is_coinbase(tx));
    BITCOIN_ASSERT(tx.inputs.size() == tx_info.input_ids.size());
    for (size_t input_index = 0; input_index < tx.inputs.size(); ++input_index)
    {
        size_t input_id = tx_info.input_ids[input_index];
        if (!connect_input(input_id, tx, input_index, value_in))
            return false;
    }
    // select * from inputs as i1, inputs as i2 where i1.input_id=192 and
    // i1.previous_output_hash=i2.previous_output_hash and
    // i1.previous_output_index=i2.previous_output_index and
    // i2.input_id!=i1.input_id;
    return true;
}

bool pq_validate_block::connect_input(
    size_t input_id, const message::transaction& current_tx, 
    size_t input_index, uint64_t& value_in)
{
    BITCOIN_ASSERT(input_index < current_tx.inputs.size());
    const message::transaction_input& input = current_tx.inputs[input_index];
    std::string hash_repr = pretty_hex(input.hash);
    static cppdb::statement find_previous = sql_.prepare(
        "SELECT \
            transactions.transaction_id, \
            coinbase, \
            encode(script, 'hex') AS script, \
            sql_to_internal(value) AS internal_value \
        FROM \
            transactions, \
            outputs \
        WHERE \
            transaction_hash=decode(?, 'hex') \
            AND transactions.transaction_id=outputs.transaction_id \
            AND index_in_parent=?"
        );
    find_previous.reset();
    find_previous.bind(hash_repr);
    find_previous.bind(input.index);
    cppdb::result previous = find_previous.row();
    if (previous.empty())
    {
        log_error(log_domain::validation) 
            << "Couldn't find the previous output " << input.index 
            << " in transaction" << hash_repr;
        return false;
    }
    uint64_t output_value = previous.get<uint64_t>("internal_value");
    if (output_value > max_money())
        return false;
    if (previous.get<std::string>("coinbase") == "t")
    {
        size_t previous_tx_id = previous.get<size_t>("transaction_id");
        // Check whether generated coin has sufficiently matured
        size_t depth_difference =
            previous_block_depth(previous_tx_id) - block_info_.depth;
        if (depth_difference < coinbase_maturity)
            return false;
    }
    std::string raw_script = previous.get<std::string>("script");
    script output_script = parse_script(bytes_from_bytea(raw_script));
    if (!output_script.run(input.input_script, current_tx, input_index))
    {
        log_error(log_domain::validation) << "Script failed evaluation";
        return false;
    }
    if (search_double_spends(input_id, input))
    {
        log_error(log_domain::validation) << "Double spent";
        return false;
    }
    value_in += output_value;
    if (value_in > max_money())
        return false;
    return true;
}

size_t pq_validate_block::previous_block_depth(size_t previous_tx_id)
{
    static cppdb::statement hookup_block = sql_.prepare(
        "SELECT depth \
        FROM \
            transactions_parents, \
            blocks \
        WHERE \
            transaction_id=? \
            AND transactions_parents.block_id=blocks.block_id \
            AND space=0 \
            AND span_left <= ? \
            AND span_right >= ?"
        );
    hookup_block.reset();
    hookup_block.bind(previous_tx_id);
    hookup_block.bind(block_info_.span_left);
    hookup_block.bind(block_info_.span_right);
    cppdb::result result = hookup_block.row();
    BITCOIN_ASSERT(!result.empty());
    return result.get<size_t>(0);
}

bool pq_validate_block::search_double_spends(
    size_t input_id, const message::transaction_input& input)
{
    // What is this input id?
    //   WHERE transaction_id=... AND index_in_parent=...

    // Has this output been already spent by another input?
    std::string hash_repr = pretty_hex(input.hash);
    static cppdb::statement search_spends = sql_.prepare(
        "SELECT 1 \
        FROM inputs \
        WHERE \
            previous_output_hash=decode(?, 'hex') \
            AND previous_output_index=? \
            AND input_id != ?"
        );
    search_spends.reset();
    search_spends.bind(hash_repr);
    search_spends.bind(input.index);
    search_spends.bind(input_id);
    cppdb::result other_spends = search_spends.query();
    if (other_spends.empty())
        return false;
    log_fatal() << "Search other spends in other branches implemented!";
    // Is that input in the same branch as us?
    // - Loop through blocks containing that input
    // - Check if in same branch
    return true;
}

pq_blockchain::pq_blockchain(
        cppdb::session sql, service_ptr service, kernel_ptr kernel)
  : barrier_clearance_level_(500), barrier_timeout_(milliseconds(500)), 
    sql_(sql)
{
    organizer_.reset(new pq_organizer(sql, kernel));
    reader_.reset(new pq_reader(sql));
    timeout_.reset(new deadline_timer(*service));
    reset_state();
}

void pq_blockchain::set_clearance(size_t clearance)
{
    barrier_clearance_level_ = clearance;
}
void pq_blockchain::set_timeout(time_duration timeout)
{
    barrier_timeout_ = timeout;
}

void pq_blockchain::buffer_block(const pq_block& buffer_block)
{
    blocks_buffer_.push_back(buffer_block);
}
void pq_blockchain::raise_barrier()
{
    barrier_level_++;
    if (barrier_level_ > barrier_clearance_level_)
    {
        reset_state();
        start();
    }
    else if (!timer_started_)
    {
        timer_started_ = true;
        timeout_->expires_from_now(barrier_timeout_);
        timeout_->async_wait(std::bind( 
            &pq_blockchain::start_exec, shared_from_this(), _1));
    }
}

void pq_blockchain::reset_state()
{
    timeout_->cancel();
    barrier_level_ = 0;
    timer_started_ = false;
}

void pq_blockchain::start_exec(const boost::system::error_code& ec)
{
    reset_state();
    if (ec == boost::asio::error::operation_aborted)
    {
        return;
    }
    else if (ec)
    {
        log_fatal() << "Blockchain processing: " << ec.message();
        return;
    }
    start();
}

void pq_blockchain::start()
{                     
    organizer_->organize();
    validate();
}

pq_block pq_blockchain::fetch_or_read_block(cppdb::result result)
{
    size_t block_id = result.get<size_t>("block_id");
    // Search our cache...
    for (auto it = blocks_buffer_.begin(); it != blocks_buffer_.end(); ++it)
    {
        pq_block_info& block_info = std::get<0>(*it);
        if (block_info.block_id == block_id)
        {
            block_info.depth = result.get<size_t>("depth");
            block_info.span_left = result.get<size_t>("span_left");
            block_info.span_right = result.get<size_t>("span_right");
            return *it;
        }
    }
    log_info() << "Block not cached in ring buffer.";
    static cppdb::statement statement = sql_.prepare(
        "SELECT \
            block_id, \
            blocks.depth, \
            span_left, \
            span_right, \
            version, \
            bits_head, \
            bits_body, \
            nonce, \
            encode(prev_block_hash, 'hex') AS prev_block_hash, \
            encode(merkle, 'hex') AS merkle, \
            EXTRACT(EPOCH FROM when_created) timest \
        FROM blocks \
        WHERE block_id=?"
        );
    statement.reset();
    statement.bind(block_id);
    result = statement.row();
    // ... else read it from the database
    return reader_->read_block(result);
}

void pq_blockchain::validate()
{
    dialect_.reset(new original_dialect);
    static cppdb::statement statement = sql_.prepare(
        "SELECT DISTINCT ON (block_id) \
            block_id, \
            blocks.depth, \
            span_left, \
            span_right \
        FROM \
            chains, \
            blocks \
        WHERE \
            space=0  \
            AND blocks.depth > chains.depth \
            AND span_left <= chain_id \
            AND span_right >= chain_id \
        ORDER BY block_id, depth ASC"
        );
    statement.reset();
    cppdb::result result = statement.query();
    // foreach invalid block where status = orphan
    // do verification and set status = valid
    while (result.next())
    {
        pq_block block = fetch_or_read_block(result);
        const pq_block_info& block_info = block.first;
        const message::block& current_block = block.second;

        pq_validate_block block_validation(
            sql_, dialect_, reader_, block_info, current_block);

        if (block_validation.validates())
            finalize_status(block_info, current_block);
        else
        {
            log_error() << "Block " << block_info.block_id
                << " failed validation!";
            // TODO: Should delete this branch
            exit(-1);
            break;
        }
    }
    blocks_buffer_.clear();
}

void pq_blockchain::finalize_status(
    const pq_block_info& block_info, const message::block& current_block)
{
    uint32_t bits_head = extract_bits_head(current_block.bits),
        bits_body = extract_bits_body(current_block.bits);
    // TODO: Should be prepared statements. Too lazy ATM
    // TODO: This should be atomic
    sql_ <<
        "UPDATE chains \
        SET \
            work = work + difficulty(?, ?), \
            depth = ? \
        WHERE \
            chain_id >= ? \
            AND chain_id <= ?" 
        << bits_head
        << bits_body
        << block_info.depth
        << block_info.span_left
        << block_info.span_right
        << cppdb::exec;
}

pq_organizer_ptr pq_blockchain::organizer()
{
    return organizer_;
}

pq_reader_ptr pq_blockchain::reader()
{
    return reader_;
}

} // libbitcoin

