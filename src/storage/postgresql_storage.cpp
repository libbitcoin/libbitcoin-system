#include <bitcoin/storage/postgresql_storage.hpp>

#include <sstream>
#include <iomanip>

#include <bitcoin/block.hpp>
#include <bitcoin/util/logger.hpp>

namespace libbitcoin {
namespace storage {

template<typename T>
std::string serialize_bytes(T data)
{
    std::stringstream ss;
    ss << std::hex;
    for (int val: data)
        ss << std::setw(2) << std::setfill('0') << val << ' ';
    return ss.str();
}

data_chunk deserialize_bytes(std::string byte_stream)
{
    std::stringstream ss;
    ss << std::hex << byte_stream;
    data_chunk stack;
    for (int val; ss.good(); ss >> val)
        stack.push_back(val);
    return stack;
}

postgresql_storage::postgresql_storage(std::string database, std::string user)
  : sql_(std::string("postgresql:dbname=") + database + ";user=" + user)
{
}

void postgresql_storage::push(net::message::inv inv)
{
    cppdb::statement stat = sql_ <<
        "INSERT INTO inventory_requests (type, hash)"
        "VALUES (?, ?)";
    for (net::message::inv_vect ivv: inv.invs)
    {
        stat.reset();
        if (ivv.type == net::message::inv_type::transaction)
            stat.bind("transaction");
        else if (ivv.type == net::message::inv_type::block)
            stat.bind("block");
        std::string byte_stream = serialize_bytes(ivv.hash);
        stat.bind(byte_stream);
        stat.exec();
    }
}

void postgresql_storage::insert(operation operation, size_t script_id)
{
    cppdb::statement stat = sql_ <<
        "INSERT INTO operations (opcode, data, script_id)"
        "VALUES (?, ?, ?)";
    std::string opcode_repr = opcode_to_string(operation.code);
    stat.bind(opcode_repr);
    if (operation.data.size() == 0)
    {
        stat.bind_null();
    }
    else
    {
        std::string byte_stream = serialize_bytes(operation.data);
        stat.bind(byte_stream);
    }
    stat.bind(script_id);
    stat.exec();
}

size_t postgresql_storage::insert_script(operation_stack operations)
{
    cppdb::result res = sql_ <<
        "SELECT nextval('script_sequence')" << cppdb::row;
    size_t script_id = res.get<size_t>(0);
    for (operation operation: operations)
        insert(operation, script_id);
    return script_id;
}

void postgresql_storage::insert(net::message::transaction_input input,
        size_t transaction_id, size_t index_in_parent)
{
    size_t script_id = insert_script(input.input_script.operations());
    std::string hash = serialize_bytes(input.hash);
    sql_ <<
        "INSERT INTO inputs (input_id, parent_id, index_in_parent, script_id,"
            "previous_output_id, previous_output_hash, sequence)"
        "VALUES (DEFAULT, ?, ?, ?, ?, ?, ?)"
        << transaction_id
        << index_in_parent
        << script_id
        << cppdb::null
        << hash
        << input.sequence
        << cppdb::exec;
}

void postgresql_storage::insert(net::message::transaction_output output,
        size_t transaction_id, size_t index_in_parent)
{
    size_t script_id = insert_script(output.output_script.operations());
    sql_ <<
        "INSERT INTO outputs (output_id, parent_id, index_in_parent, script_id,"
            "value, output_type, address)"
        "VALUES (DEFAULT, ?, ?, ?, internal_to_sql(?), ?, ?)"
        << transaction_id
        << index_in_parent
        << script_id
        << output.value
        << "other"
        << cppdb::null
        << cppdb::exec;
}

size_t postgresql_storage::insert(net::message::transaction transaction)
{
    cppdb::result res = sql_ <<
        "INSERT INTO transactions (transaction_id, transaction_hash, "
            "version, locktime)"
        "VALUES (DEFAULT, ?, ?, ?)"
        "RETURNING transaction_id"
        << "hello" << transaction.version << transaction.locktime << cppdb::row;
    size_t transaction_id = res.get<size_t>(0);
    logger(LOG_DEBUG) << "inserted tx " << transaction_id;
    for (size_t i = 0; i < transaction.inputs.size(); ++i)
        insert(transaction.inputs[i], transaction_id, i);
    for (size_t i = 0; i < transaction.outputs.size(); ++i)
        insert(transaction.outputs[i], transaction_id, i);
    return transaction_id;
}

void postgresql_storage::push(net::message::transaction transaction)
{
    insert(transaction);
}

void postgresql_storage::push(net::message::block block)
{
    hash_digest block_hash = hash_block_header(block);
    std::string block_hash_repr = serialize_bytes(block_hash),
            prev_block_repr = serialize_bytes(block.prev_block),
            merkle_repr = serialize_bytes(block.merkle_root);
    // TODO: fix reverse ordering
    // #1 priority to fix.
    std::stringstream ss;
    ss << std::hex;
    for (int i = block_hash.size() - 1; i >= 0; --i)
        ss << std::setw(2) << std::setfill('0') << (int)block_hash[i] << ' ';
    block_hash_repr = ss.str();

    cppdb::result res = sql_ <<
        "INSERT INTO blocks (block_id, block_hash, version, prev_block_hash,"
            "merkle, when_created, bits_head, bits_body, nonce)"
        "VALUES (DEFAULT, ?, ?, ?, ?, TO_TIMESTAMP(?), ?, ?, ?)"
        "RETURNING block_id"
        << block_hash_repr
        << block.version
        << prev_block_repr
        << merkle_repr
        << block.timestamp
        << (block.bits >> (8*3))
        << (block.bits & 0x00ffffff)
        << block.nonce
        << cppdb::row;
    size_t block_id = res.get<size_t>(0);
    for (size_t i = 0; i < block.transactions.size(); ++i)
    {
        net::message::transaction transaction = block.transactions[i];
        size_t transaction_id = insert(transaction);
        // Create block <-> txn mapping
        sql_ << "INSERT INTO transactions_parents ("
                    "transaction_id, block_id, index_in_block)"
                "VALUES (?, ?, ?)"
            << transaction_id << block_id << i << cppdb::exec;
    }
}

void postgresql_storage::request_inventories(accept_inventories_handler handler)
{
}

} // storage
} // libbitcoin

