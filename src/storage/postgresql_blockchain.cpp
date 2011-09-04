#include "postgresql_blockchain.hpp"

#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

using boost::posix_time::seconds;
using boost::posix_time::minutes;
using boost::posix_time::time_duration;
using std::placeholders::_1;

constexpr size_t barrier_clearance_level = 4;
const time_duration barrier_timeout = seconds(4);

postgresql_organizer::postgresql_organizer(cppdb::session sql)
  : sql_(sql)
{
}

void postgresql_organizer::delete_branch(size_t space, size_t depth, 
    size_t span_left, size_t span_right)
{
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

    size_t offset = (span_right + 1) - span_left;

    static cppdb::statement adjust_left = sql_.prepare(
        "UPDATE blocks \
        SET span_left=span_left-? \
        WHERE  \
            space=? \
            AND span_left > ?"
        );
    adjust_left.reset();
    adjust_left.bind(offset);
    adjust_left.bind(space);
    adjust_left.bind(span_right);
    adjust_left.exec();

    static cppdb::statement adjust_right = sql_.prepare(
        "UPDATE blocks \
        SET span_right=span_right-? \
        WHERE  \
            space=? \
            AND span_right >= ?"
        );
    adjust_right.reset();
    adjust_right.bind(offset);
    adjust_right.bind(space);
    adjust_right.bind(span_right);
    adjust_right.exec();
}

void postgresql_organizer::organize()
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
        reserve_branch_area(parent_space, parent_width, parent_span.right, 
            new_child_depth, child_width);
        position_child_branch(child_space, parent_space, new_child_depth, 
            new_child_span_left);
    }
}

bool postgresql_organizer::load_span(size_t block_id, span& spn)
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

bool postgresql_organizer::load_position_info(size_t block_id, 
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

size_t postgresql_organizer::get_block_width(
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

void postgresql_organizer::reserve_branch_area(size_t parent_space, 
    size_t parent_width, size_t parent_span_right, 
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
    update_right.bind(parent_span_right);
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
    update_left.bind(parent_span_right);
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
    update_parents.bind(parent_span_right);
    update_parents.exec();
}

void postgresql_organizer::position_child_branch(
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

postgresql_reader::postgresql_reader(cppdb::session sql)
  : sql_(sql)
{
}

script postgresql_reader::select_script(size_t script_id)
{
    static cppdb::statement statement = sql_.prepare(
        "SELECT \
            opcode, \
            data \
        FROM operations \
        WHERE script_id=? \
        ORDER BY operation_id ASC"
        );
    statement.reset();
    statement.bind(script_id);
    cppdb::result result = statement.query();
    script scr;
    while (result.next())
    {
        operation op;
        op.code = string_to_opcode(result.get<std::string>("opcode"));
        if (!result.is_null("data"))
            op.data = deserialize_bytes(result.get<std::string>("data"));
        scr.push_operation(op);
    }
    return scr;
}

message::transaction_input_list postgresql_reader::select_inputs(
        size_t transaction_id)
{
    static cppdb::statement statement = sql_.prepare(
        "SELECT * \
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
        message::transaction_input input;
        input.hash = 
            deserialize_hash(result.get<std::string>("previous_output_hash"));
        input.index = result.get<uint32_t>("previous_output_index");
        size_t script_id = result.get<size_t>("script_id");
        input.input_script = select_script(script_id);
        input.sequence = result.get<uint32_t>("sequence");
        inputs.push_back(input);
    }
    return inputs;
}
message::transaction_output_list postgresql_reader::select_outputs(
        size_t transaction_id)
{
    static cppdb::statement statement = sql_.prepare(
        "SELECT \
            *, \
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
        message::transaction_output output;
        output.value = result.get<uint64_t>("internal_value");
        size_t script_id = result.get<size_t>("script_id");
        output.output_script = select_script(script_id);
        outputs.push_back(output);
    }
    return outputs;
}

message::transaction_list postgresql_reader::read_transactions(
        cppdb::result result)
{
    message::transaction_list transactions;
    while (result.next())
    {
        message::transaction transaction;
        transaction.version = result.get<uint32_t>("version");
        transaction.locktime = result.get<uint32_t>("locktime");
        size_t transaction_id = result.get<size_t>("transaction_id");
        transaction.inputs = select_inputs(transaction_id);
        transaction.outputs = select_outputs(transaction_id);
        transactions.push_back(transaction);
    }
    return transactions;
}

message::block postgresql_reader::read_block(cppdb::result block_result)
{
    message::block block;
    size_t block_id = block_result.get<size_t>("block_id");
    block.version = block_result.get<uint32_t>("version");
    block.timestamp = block_result.get<uint32_t>("timest");
    uint32_t bits_head = block_result.get<uint32_t>("bits_head"),
            bits_body = block_result.get<uint32_t>("bits_body");
    block.bits = bits_body + (bits_head << (3*8));
    block.nonce = block_result.get<uint32_t>("nonce");

    block.prev_block = 
            deserialize_hash(block_result.get<std::string>("prev_block_hash"));
    block.merkle_root = 
            deserialize_hash(block_result.get<std::string>("merkle"));

    static cppdb::statement transactions_statement = sql_.prepare(
        "SELECT transactions.* \
        FROM transactions_parents \
        JOIN transactions \
        ON transactions.transaction_id=transactions_parents.transaction_id \
        WHERE block_id=? \
        ORDER BY index_in_block ASC"
        );
    transactions_statement.reset();
    transactions_statement.bind(block_id);
    cppdb::result transactions_result = transactions_statement.query();
    block.transactions = read_transactions(transactions_result);
    return block;
}

postgresql_verifier::postgresql_verifier(cppdb::session sql)
  : postgresql_reader(sql), sql_(sql) 
{
}

void postgresql_verifier::verify()
{
    static cppdb::statement statement = sql_.prepare(
        "SELECT \
            block_id \
        FROM blocks \
        "
        );
    log_fatal() << "Verify unimplemented!";
}

postgresql_blockchain::postgresql_blockchain(
        cppdb::session sql, service_ptr service)
  : postgresql_organizer(sql), postgresql_verifier(sql)
{
    reset_state();
    timeout_.reset(new deadline_timer(*service));
}

void postgresql_blockchain::raise_barrier()
{
    barrier_level_++;
    if (barrier_level_ > barrier_clearance_level)
    {
        timeout_->cancel();
        reset_state();
        start();
    }
    else if (!timer_started_)
    {
        timer_started_ = true;
        timeout_->expires_from_now(barrier_timeout);
        timeout_->async_wait(std::bind( 
            &postgresql_blockchain::start_exec, shared_from_this(), _1));
    }
}

void postgresql_blockchain::reset_state()
{
    barrier_level_ = 0;
    timer_started_ = false;
}

void postgresql_blockchain::start_exec(const boost::system::error_code& ec)
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

void postgresql_blockchain::start()
{
    organize();
    verify();
}

} // libbitcoin

