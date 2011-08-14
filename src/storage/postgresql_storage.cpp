#include <bitcoin/storage/postgresql_storage.hpp>

#include <bitcoin/block.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

data_chunk deserialize_bytes(std::string byte_stream)
{
    std::stringstream ss;
    ss << std::hex << byte_stream;
    data_chunk stack;
    while (ss.good())
    {
        int val;
        ss >> val;
        stack.push_back(val);
    }
    return stack;
}

hash_digest deserialize_hash(std::string byte_stream)
{
    std::stringstream ss;
    ss << std::hex << byte_stream;
    hash_digest hash;
    for (size_t i = 0; i < hash.size(); ++i)
    {
        int val;
        ss >> val;
        hash[i] = val;
    }
    return hash;
}

postgresql_storage::postgresql_storage(std::string database, 
        std::string user, std::string password)
  : sql_(std::string("postgresql:dbname=") + database + 
        ";user=" + user + ";password=" + password)
{
}

void postgresql_storage::store(message::inv inv,
        store_handler handle_store)
{
    strand()->post(std::bind(
        &postgresql_storage::do_store_inv, shared_from_this(), 
            inv, handle_store));
}
void postgresql_storage::do_store_inv(message::inv inv,
        store_handler handle_store)
{
    cppdb::statement stat = sql_ <<
        "INSERT INTO inventory_requests (type, hash) \
        VALUES (?, ?)";
    for (message::inv_vect ivv: inv.invs)
    {
        stat.reset();
        if (ivv.type == message::inv_type::transaction)
            stat.bind("transaction");
        else if (ivv.type == message::inv_type::block)
            stat.bind("block");
        std::string byte_stream = hexlify(ivv.hash);
        stat.bind(byte_stream);
        stat.exec();
    }
    handle_store(std::error_code());
}

void postgresql_storage::insert(operation operation, size_t script_id)
{
    std::string opcode_repr = opcode_to_string(operation.code);
    cppdb::statement stat = sql_ <<
        "INSERT INTO operations (opcode, script_id, data) \
        VALUES (?, ?, ?)";
    stat.bind(opcode_repr);
    stat.bind(script_id);
    if (operation.data.size() == 0)
    {
        stat.bind_null();
        stat.exec();
    }
    else
    {
        // Scoping rules. String needs to stay around for exec
        std::string byte_stream = hexlify(operation.data);
        stat.bind(byte_stream);
        stat.exec();
    }
}

size_t postgresql_storage::insert_script(operation_stack operations)
{
    cppdb::result result = sql_ <<
        "SELECT nextval('script_sequence')" << cppdb::row;
    size_t script_id = result.get<size_t>(0);
    for (operation operation: operations)
        insert(operation, script_id);
    return script_id;
}

void postgresql_storage::insert(message::transaction_input input,
        size_t transaction_id, size_t index_in_parent)
{
    size_t script_id = insert_script(input.input_script.operations());
    std::string hash = hexlify(input.hash);
    sql_ <<
        "INSERT INTO inputs (input_id, transaction_id, index_in_parent, \
            script_id, previous_output_id, previous_output_hash, \
            previous_output_index, sequence) \
        VALUES (DEFAULT, ?, ?, ?, ?, ?, ?, ?)"
        << transaction_id
        << index_in_parent
        << script_id
        << cppdb::null
        << hash
        << input.index
        << input.sequence
        << cppdb::exec;
}

void postgresql_storage::insert(message::transaction_output output,
        size_t transaction_id, size_t index_in_parent)
{
    size_t script_id = insert_script(output.output_script.operations());
    sql_ <<
        "INSERT INTO outputs (output_id, transaction_id, index_in_parent, \
            script_id, value, output_type, address) \
        VALUES (DEFAULT, ?, ?, ?, internal_to_sql(?), ?, ?)"
        << transaction_id
        << index_in_parent
        << script_id
        << output.value
        << "other"
        << cppdb::null
        << cppdb::exec;
}

size_t postgresql_storage::insert(message::transaction transaction)
{
    hash_digest transaction_hash = hash_transaction(transaction);
    std::string transaction_hash_repr = hexlify(transaction_hash);
    cppdb::result result = sql_ <<
        "INSERT INTO transactions (transaction_id, transaction_hash, \
            version, locktime) \
        VALUES (DEFAULT, ?, ?, ?) \
        RETURNING transaction_id"
        << transaction_hash_repr
        << transaction.version
        << transaction.locktime
        << cppdb::row;
    size_t transaction_id = result.get<size_t>(0);
    for (size_t i = 0; i < transaction.inputs.size(); ++i)
        insert(transaction.inputs[i], transaction_id, i);
    for (size_t i = 0; i < transaction.outputs.size(); ++i)
        insert(transaction.outputs[i], transaction_id, i);
    return transaction_id;
}

void postgresql_storage::store(message::transaction transaction,
        store_handler handle_store)
{
    strand()->post(std::bind(
        &postgresql_storage::do_store_transaction, shared_from_this(),
            transaction, handle_store));
}
void postgresql_storage::do_store_transaction(
        message::transaction transaction, store_handler handle_store)
{
    insert(transaction);
    handle_store(std::error_code());
}

void postgresql_storage::store(message::block block,
        store_handler handle_store)
{
    strand()->post(std::bind(
        &postgresql_storage::do_store_block, shared_from_this(),
            block, handle_store));
}
void postgresql_storage::do_store_block(message::block block,
        store_handler handle_store)
{
    hash_digest block_hash = hash_block_header(block);
    std::string block_hash_repr = hexlify(block_hash),
            prev_block_repr = hexlify(block.prev_block),
            merkle_repr = hexlify(block.merkle_root);

    cppdb::result result = sql_ <<
        "SELECT 1 FROM blocks WHERE block_hash=?"
        << block_hash_repr << cppdb::row;
    if (!result.empty())
        return;

    result = sql_ <<
        "INSERT INTO blocks (block_id, block_hash, version, prev_block_hash, \
            merkle, when_created, bits_head, bits_body, nonce) \
        VALUES (DEFAULT, ?, ?, ?, ?, TO_TIMESTAMP(?), ?, ?, ?) \
        RETURNING block_id"
        << block_hash_repr
        << block.version
        << prev_block_repr
        << merkle_repr
        << block.timestamp
        << (block.bits >> (8*3))
        << (block.bits & 0x00ffffff)
        << block.nonce
        << cppdb::row;
    size_t block_id = result.get<size_t>(0);
    for (size_t i = 0; i < block.transactions.size(); ++i)
    {
        message::transaction transaction = block.transactions[i];
        size_t transaction_id = insert(transaction);
        // Create block <-> txn mapping
        sql_ << "INSERT INTO transactions_parents ( \
                    transaction_id, block_id, index_in_block) \
                VALUES (?, ?, ?)"
            << transaction_id << block_id << i << cppdb::exec;
    }
    handle_store(std::error_code());
}

void postgresql_storage::fetch_inventories(fetch_handler_inventories)
{
}
void postgresql_storage::do_fetch_inventories(fetch_handler_inventories)
{
    // Not implemented
}

script postgresql_storage::select_script(size_t script_id)
{
    script scr;
    cppdb::result result = sql_ <<
        "SELECT \
            opcode, \
            data \
        FROM operations \
        WHERE script_id=? \
        ORDER BY operation_id ASC"
        << script_id;
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

message::transaction_input_list postgresql_storage::select_inputs(
        size_t transaction_id)
{
    message::transaction_input_list inputs;
    cppdb::result result = sql_ <<
        "SELECT * \
        FROM inputs \
        WHERE transaction_id=? \
        ORDER BY index_in_parent ASC"
        << transaction_id;
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
message::transaction_output_list postgresql_storage::select_outputs(
        size_t transaction_id)
{
    message::transaction_output_list outputs;
    cppdb::result result = sql_ <<
        "SELECT \
            *, \
            sql_to_internal(value) internal_value \
        FROM outputs \
        WHERE transaction_id=? \
        ORDER BY index_in_parent ASC"
        << transaction_id;
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

message::transaction_list postgresql_storage::read_transactions(
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

message::block postgresql_storage::read_block(cppdb::result block_result)
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

    cppdb::result transactions_result = sql_ <<
        "SELECT transactions.* \
        FROM transactions_parents \
        JOIN transactions \
        ON transactions.transaction_id=transactions_parents.transaction_id \
        WHERE block_id=? \
        ORDER BY index_in_block ASC"
        << block_id;
    block.transactions = read_transactions(transactions_result);
    return block;
}

void postgresql_storage::fetch_block_by_depth(size_t block_number,
        fetch_handler_block handle_fetch)
{
    strand()->post(std::bind(
        &postgresql_storage::do_fetch_block_by_depth, shared_from_this(),
            block_number, handle_fetch));
}
void postgresql_storage::do_fetch_block_by_depth(size_t block_number,
        fetch_handler_block handle_fetch)
{
    cppdb::result block_result = sql_ <<
        "SELECT \
            *, \
            EXTRACT(EPOCH FROM when_created) timest \
        FROM blocks \
        WHERE \
            depth=? \
            AND span_left=0 \
            AND span_left=0"
        << block_number
        << cppdb::row;
    if (block_result.empty())
    {
        handle_fetch(error::block_doesnt_exist, message::block());
        return;
    }
    message::block block = read_block(block_result);
    handle_fetch(std::error_code(), block);
}

void postgresql_storage::fetch_block_by_hash(hash_digest block_hash, 
        fetch_handler_block handle_fetch)
{
    strand()->post(std::bind(
        &postgresql_storage::do_fetch_block_by_hash, shared_from_this(),
            block_hash, handle_fetch));
}
void postgresql_storage::do_fetch_block_by_hash(hash_digest block_hash, 
        fetch_handler_block handle_fetch)
{
    std::string block_hash_repr = hexlify(block_hash);
    cppdb::result block_result = sql_ <<
        "SELECT \
            *, \
            EXTRACT(EPOCH FROM when_created) timest \
        FROM blocks \
        WHERE \
            block_hash=? \
            AND span_left=0 \
            AND span_left=0"
        << block_hash_repr
        << cppdb::row;
    if (block_result.empty())
    {
        handle_fetch(error::block_doesnt_exist, message::block());
        return;
    }
    message::block block = read_block(block_result);
    handle_fetch(std::error_code(), block);
}

void postgresql_storage::fetch_block_locator(
        fetch_handler_block_locator handle_fetch)
{
    strand()->post(std::bind(
        &postgresql_storage::do_fetch_block_locator, shared_from_this(),
            handle_fetch));
}
void postgresql_storage::do_fetch_block_locator(
        fetch_handler_block_locator handle_fetch)
{
    cppdb::result number_blocks_result = sql_ <<
        "SELECT MAX(depth) \
        FROM blocks \
        WHERE \
            span_left=0 \
            AND span_right=0"
        << cppdb::row;
    if (number_blocks_result.empty())
    {
        handle_fetch(error::block_doesnt_exist, 
                message::block_locator());
        return;
    }
    // Start at max_depth
    int current_depth = number_blocks_result.get<size_t>(0);
    std::vector<size_t> indices;
    // Push last 10 indices first
    for (size_t i = 0; i < 10; ++i, --current_depth)
        indices.push_back(current_depth);
    size_t step = 1;
    while (true)
    {
        current_depth -= step;
        if (current_depth <= 0)
            break;
        indices.push_back(current_depth);
        step *= 2;
    }
    indices.push_back(0);
    // Now actually fetch the hashes for these blocks
    // TODO: UGLY!! Hack around limitation of cppdb!
    std::stringstream hack_sql;
    hack_sql <<
        "SELECT block_hash \
        FROM blocks \
        WHERE \
            span_left=0 \
            AND span_right=0 \
            AND depth IN (";
    for (size_t i = 0; i < indices.size(); ++i)
    {
        if (i != 0)
            hack_sql << ", ";
        hack_sql << indices[i];
    }
    hack_sql << ") ORDER BY depth DESC";
    // ----------------------------------------------
    cppdb::result block_hashes_result = sql_ << hack_sql.str();
    message::block_locator locator;
    while (block_hashes_result.next())
    {
        std::string block_hash_repr = block_hashes_result.get<std::string>(0);
        locator.push_back(deserialize_hash(block_hash_repr));
    }
    handle_fetch(std::error_code(), locator);
}

void postgresql_storage::fetch_output_by_hash(hash_digest transaction_hash, 
        uint32_t index, fetch_handler_output handle_fetch)
{
    strand()->post(std::bind(
        &postgresql_storage::fetch_output_by_hash, shared_from_this(),
            transaction_hash, index, handle_fetch));
}
void postgresql_storage::do_fetch_output_by_hash(hash_digest transaction_hash, 
        uint32_t index, fetch_handler_output handle_fetch)
{
    message::transaction_output output;
    std::string transaction_hash_repr = hexlify(transaction_hash);
    cppdb::result result = sql_ <<
        "SELECT \
            *, \
            sql_to_internal(value) internal_value \
        FROM transactions \
        JOIN outputs \
        ON transaction_id=transaction_id \
        WHERE \
            transaction_hash=? \
            AND index_in_parent=?"
        << transaction_hash_repr
        << index
        << cppdb::row;
    if (result.empty())
    {
        handle_fetch(error::output_doesnt_exist, output);
        return;
    }
    output.value = result.get<uint64_t>("internal_value");
    size_t script_id = result.get<size_t>("script_id");
    output.output_script = select_script(script_id);
    handle_fetch(std::error_code(), output);
}

void postgresql_storage::block_exists_by_hash(hash_digest block_hash,
        exists_handler handle_exists)
{
    strand()->post(std::bind(
        &postgresql_storage::do_block_exists_by_hash, shared_from_this(), 
            block_hash, handle_exists));
}
void postgresql_storage::do_block_exists_by_hash(hash_digest block_hash,
        exists_handler handle_exists)
{
    std::string block_hash_repr = hexlify(block_hash);
    cppdb::result block_result = sql_ <<
        "SELECT 1 \
        FROM blocks \
        WHERE \
            block_hash=? \
            AND span_left=0 \
            AND span_left=0"
        << block_hash_repr
        << cppdb::row;
    if (block_result.empty())
        handle_exists(std::error_code(), false);
    else
        handle_exists(std::error_code(), true);
}

void postgresql_storage::organize_block_chain()
{
    service()->post(std::bind(
        &postgresql_storage::organize_block_chain, shared_from_this()));
}
void postgresql_storage::do_organize_block_chain()
{
    cppdb::result result = sql_ <<
        "SELECT \
            block_id, \
            prev_block_hash \
        FROM blocks \
        WHERE depth IS NULL \
        ORDER BY block_id ASC";
    while (result.next())
    {
        size_t block_id = result.get<size_t>(0);
        std::string prev_block_hash = result.get<std::string>(1);

        cppdb::result parent_result = sql_ <<
            "SELECT\
                block_id, \
                depth, \
                span_left, \
                span_right \
            FROM blocks \
            WHERE \
                block_hash=? \
                AND depth IS NOT NULL"
            << prev_block_hash
            << cppdb::row;
        if (parent_result.empty())
            continue;
        size_t parent_depth = parent_result.get<size_t>(1),
                parent_span_left = parent_result.get<size_t>(2),
                parent_span_right = parent_result.get<size_t>(3);

        // Does this parent have children already?
        cppdb::result has_children_result = sql_ <<
            "SELECT 1 \
            FROM blocks \
            WHERE \
                span_left >= ? \
                AND span_right <= ? \
                AND depth > ? \
            LIMIT 1"
            << parent_span_left
            << parent_span_right
            << parent_depth
            << cppdb::row;
        bool has_children = !has_children_result.empty();

        size_t depth = parent_depth + 1;
        if (has_children)
        {
            // Fork in the blockchain!
            size_t chain_id = parent_span_right;
            cppdb::transaction guard(sql_);
            sql_ <<
                "UPDATE blocks \
                SET span_left=span_left+1 \
                WHERE span_left >= ?"
                << chain_id
                << cppdb::exec;
            sql_ <<
                "UPDATE blocks \
                SET span_right=span_right+1 \
                WHERE span_right >= ?"
                << chain_id
                << cppdb::exec;
            guard.commit();
        }
        else
        {
            BITCOIN_ASSERT(parent_span_left == parent_span_right);
            size_t chain_id = parent_span_left;

            sql_ <<
                "UPDATE blocks \
                SET \
                    depth=?, \
                    span_left=?, \
                    span_right=? \
                WHERE block_id=?"
                << depth
                << chain_id
                << chain_id
                << block_id
                << cppdb::exec;
        }
    }
    matchup_inputs();
}

void postgresql_storage::matchup_inputs()
{
    sql_ <<
        "UPDATE inputs \
        SET previous_output_id=transactions.transaction_id \
        FROM transactions \
        WHERE previous_output_hash=transaction_hash"
        << cppdb::exec;
}

} // libbitcoin

