#include <bitcoin/storage/postgresql_storage.hpp>

#include <bitcoin/block.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>

#include "postgresql_blockchain.hpp"

namespace libbitcoin {

hash_digest hash_from_bytea(std::string byte_stream);
data_chunk bytes_from_bytea(std::string byte_stream);

uint32_t extract_bits_head(uint32_t bits);
uint32_t extract_bits_body(uint32_t bits);

postgresql_storage::postgresql_storage(kernel_ptr kernel,
    std::string database, std::string user, std::string password)
 : sql_(std::string("postgresql:host=127.0.0.1;dbname=") + database +
        ";user=" + user + ";password=" + password)
{
    blockchain_.reset(new pq_blockchain(sql_, service(), kernel));
    // Organise/validate old blocks in case of unclean shutdown
    strand()->post(std::bind(&pq_blockchain::start, blockchain_));
}

size_t postgresql_storage::insert(const message::transaction_input& input,
    size_t transaction_id, size_t index_in_parent)
{
    std::string hash = pretty_hex(input.previous_output.hash),
        pretty_script = pretty_hex(save_script(input.input_script));
    static cppdb::statement statement = sql_.prepare(
        "INSERT INTO inputs (input_id, transaction_id, index_in_parent, \
            script, previous_output_hash, previous_output_index, sequence) \
        VALUES (DEFAULT, ?, ?, decode(?, 'hex'), decode(?, 'hex'), ?, ?) \
        RETURNING input_id"
        );
    statement.reset();
    statement.bind(transaction_id);
    statement.bind(index_in_parent);
    statement.bind(pretty_script);
    statement.bind(hash);
    statement.bind(input.previous_output.index);
    statement.bind(input.sequence);
    return statement.row().get<size_t>(0);
}

size_t postgresql_storage::insert(const message::transaction_output& output,
        size_t transaction_id, size_t index_in_parent)
{
    std::string pretty_script = pretty_hex(save_script(output.output_script));
    static cppdb::statement statement = sql_.prepare(
        "INSERT INTO outputs ( \
            output_id, transaction_id, index_in_parent, script, value) \
        VALUES (DEFAULT, ?, ?, decode(?, 'hex'), internal_to_sql(?)) \
        RETURNING output_id"
        );
    statement.reset();
    statement.bind(transaction_id);
    statement.bind(index_in_parent);
    statement.bind(pretty_script);
    statement.bind(output.value);
    return statement.row().get<size_t>(0);
}

size_t postgresql_storage::insert(const message::transaction& transaction,
    std::vector<size_t>& input_ids, std::vector<size_t>& output_ids)
{
    hash_digest transaction_hash = hash_transaction(transaction);
    std::string transaction_hash_repr = pretty_hex(transaction_hash);
    // We use special function to insert txs. 
    // Some blocks contain duplicates. See SQL for more details.
    static cppdb::statement statement = sql_.prepare(
        "SELECT insert_transaction(decode(?, 'hex'), ?, ?, ?)"
        );
    statement.reset();
    statement.bind(transaction_hash_repr);
    statement.bind(transaction.version);
    statement.bind(transaction.locktime);
    statement.bind(is_coinbase(transaction));
    cppdb::result result = statement.row();
    size_t transaction_id = result.get<size_t>(0);
    if (transaction_id == 0)
    {
        cppdb::result old_transaction_id = sql_ <<
            "SELECT transaction_id \
            FROM transactions \
            WHERE transaction_hash=decode(?, 'hex')"
            << transaction_hash_repr
            << cppdb::row;
        return old_transaction_id.get<size_t>(0);
    }

    for (size_t i = 0; i < transaction.inputs.size(); ++i)
    {
        size_t input_id = insert(transaction.inputs[i], transaction_id, i);
        input_ids.push_back(input_id);
    }
    for (size_t i = 0; i < transaction.outputs.size(); ++i)
    {
        size_t output_id = insert(transaction.outputs[i], transaction_id, i);
        output_ids.push_back(output_id);
    }
    return transaction_id;
}

void postgresql_storage::store(const message::block& block,
        store_block_handler handle_store)
{
    strand()->post(std::bind(
        &postgresql_storage::do_store_block, shared_from_this(),
            block, handle_store));
}
void postgresql_storage::do_store_block(const message::block& block,
        store_block_handler handle_store)
{
    hash_digest block_hash = hash_block_header(block);
    std::string block_hash_repr = pretty_hex(block_hash),
            prev_block_repr = pretty_hex(block.prev_block),
            merkle_repr = pretty_hex(block.merkle_root);

    static cppdb::statement check_confirmed = sql_.prepare(
        "SELECT 1 \
        FROM \
            blocks, \
            chains \
        WHERE \
            block_id=? \
            AND space=0 \
            AND blocks.depth <= chains.depth \
            AND chain_id >= span_left \
            AND chain_id <= span_right \
        ORDER BY work DESC \
        LIMIT 1"
        );

    cppdb::transaction guard(sql_);
    cppdb::result result = sql_ <<
        "SELECT block_id FROM blocks WHERE block_hash=decode(?, 'hex')"
        << block_hash_repr << cppdb::row;
    if (!result.empty())
    {
        check_confirmed.reset();
        check_confirmed.bind(result.get<size_t>(0));
        cppdb::result is_confirmed = check_confirmed.row();
        if (is_confirmed.empty())
            handle_store(error::object_already_exists, block_status::orphan);
        else
            handle_store(error::object_already_exists, block_status::confirmed);
        return;
    }

    static cppdb::statement statement = sql_.prepare(
        "INSERT INTO blocks( \
            block_id, \
            block_hash, \
            space, \
            depth, \
            span_left, \
            span_right, \
            version, \
            prev_block_hash, \
            merkle, \
            when_created, \
            bits_head, \
            bits_body, \
            nonce \
        ) VALUES ( \
            DEFAULT, \
            decode(?, 'hex'), \
            nextval('blocks_space_sequence'), \
            0, \
            0, \
            0, \
            ?, \
            decode(?, 'hex'), \
            decode(?, 'hex'), \
            TO_TIMESTAMP(?), \
            ?, \
            ?, \
            ? \
        ) \
        RETURNING block_id"
        );

    statement.reset();
    statement.bind(block_hash_repr);
    statement.bind(block.version);
    statement.bind(prev_block_repr);
    statement.bind(merkle_repr);
    statement.bind(block.timestamp);
    uint32_t bits_head = extract_bits_head(block.bits),
        bits_body = extract_bits_body(block.bits);
    statement.bind(bits_head);
    statement.bind(bits_body);
    statement.bind(block.nonce);

    result = statement.row();
    pq_block_info block_info;
    block_info.block_id = result.get<size_t>(0);
    bool buffer_block = true;
    for (size_t i = 0; i < block.transactions.size(); ++i)
    {
        message::transaction transaction = block.transactions[i];
        pq_transaction_info tx_info;
        tx_info.transaction_id =
            insert(transaction, tx_info.input_ids, tx_info.output_ids);
        if (tx_info.input_ids.empty())
        {
            BITCOIN_ASSERT(tx_info.output_ids.empty());
            buffer_block = false;
        }
        // Create block <-> txn mapping
        static cppdb::statement link_txs = sql_.prepare(
            "INSERT INTO transactions_parents ( \
                transaction_id, block_id, index_in_block) \
            VALUES (?, ?, ?)"
            );
        link_txs.reset();
        link_txs.bind(tx_info.transaction_id);
        link_txs.bind(block_info.block_id);
        link_txs.bind(i);
        link_txs.exec();
        block_info.transactions.push_back(tx_info);
    }
    guard.commit();
    blockchain_->start();
    check_confirmed.reset();
    check_confirmed.bind(block_info.block_id);
    cppdb::result is_confirmed = check_confirmed.row();
    if (is_confirmed.empty())
        handle_store(std::error_code(), block_status::orphan);
    else
        handle_store(std::error_code(), block_status::confirmed);
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
        "SELECT depth \
        FROM chains \
        ORDER BY work DESC \
        LIMIT 1"
        << cppdb::row;
    if (number_blocks_result.empty())
    {
        handle_fetch(error::object_doesnt_exist, message::block_locator());
        return;
    }
    // Start at max_depth
    int top_depth = number_blocks_result.get<size_t>(0);
    std::vector<size_t> indices;
    // Push last 10 indices first
    size_t step = 1, start = 0;
    for (int i = top_depth; i > 0; i -= step, ++start)
    {
        if (start >= 10)
            step *= 2;
        indices.push_back(i);
    }
    indices.push_back(0);
    // Now actually fetch the hashes for these blocks
    std::stringstream hack_sql;
    hack_sql <<
        "SELECT encode(block_hash, 'hex') \
        FROM main_chain \
        WHERE depth IN (";
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
        locator.push_back(hash_from_bytea(block_hash_repr));
    }
    BITCOIN_ASSERT(locator.size() == indices.size());
    handle_fetch(std::error_code(), locator);
}

void postgresql_storage::fetch_balance(const short_hash& pubkey_hash,
    fetch_handler_balance handle_fetch)
{
    strand()->post(std::bind(
        &postgresql_storage::do_fetch_balance, shared_from_this(),
            pubkey_hash, handle_fetch));
}
void postgresql_storage::do_fetch_balance(const short_hash& pubkey_hash,
    fetch_handler_balance handle_fetch)
{
    std::string total_script = "76 a9 14 " + pretty_hex(pubkey_hash) + " 88 ac";
    static cppdb::statement statement = sql_.prepare(
        "WITH outs AS ( \
            SELECT \
                transaction_hash,  \
                outputs.index_in_parent,  \
                outputs.value \
            FROM \
            outputs, \
            transactions \
            WHERE  \
                script=decode(?, 'hex') \
                AND outputs.transaction_id=transactions.transaction_id \
            ) \
        SELECT \
            sql_to_internal(SUM(outs.value)) \
        FROM outs \
        LEFT JOIN inputs \
        ON \
            inputs.previous_output_hash=transaction_hash \
            AND inputs.previous_output_index=outs.index_in_parent \
        WHERE inputs IS NULL"
        );
    statement.reset();
    statement.bind(total_script);
    cppdb::result result = statement.row();
    uint64_t value = 0;
    if (!result.is_null(0))
        value = result.get<uint64_t>(0);
    handle_fetch(std::error_code(), value);
}

} // libbitcoin

